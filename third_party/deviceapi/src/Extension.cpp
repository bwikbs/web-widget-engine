// Copyright 2014 Samsung Electronics Co, Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "Extension.h"

#include <dlfcn.h>
#include <dlog.h>

#include "ExtensionAdapter.h"
#include "StarFishConfig.h"
#include "Escargot.h"
#include "TizenDeviceAPILoaderForEscargot.h"
#include "platform/window/Window.h"
#include "dom/binding/ScriptBindingInstance.h"

namespace wrt {
namespace xwalk {

Extension::Extension(const std::string& path, RuntimeVariableProvider* provider)
  : initialized_(false),
    library_path_(path),
    xw_extension_(0),
    use_trampoline_(true),
    created_instance_callback_(NULL),
    destroyed_instance_callback_(NULL),
    shutdown_callback_(NULL),
    handle_msg_callback_(NULL),
    handle_sync_msg_callback_(NULL),
    rv_provider_(provider){
}

Extension::Extension(const std::string& path,
                     const std::string& name,
                     const std::vector<std::string>& entry_points,
                     RuntimeVariableProvider* provider)
  : initialized_(false),
    library_path_(path),
    xw_extension_(0),
    name_(name),
    entry_points_(entry_points),
    use_trampoline_(true),
    created_instance_callback_(NULL),
    destroyed_instance_callback_(NULL),
    shutdown_callback_(NULL),
    handle_msg_callback_(NULL),
    handle_sync_msg_callback_(NULL),
    rv_provider_(provider){
}


Extension::~Extension() {
  if (!initialized_)
    return;

  if (shutdown_callback_)
    shutdown_callback_(xw_extension_);
  ExtensionAdapter::GetInstance()->UnregisterExtension(this);
}

bool Extension::Initialize() {
  if (initialized_)
    return true;

  DEVICEAPI_LOG_INFO("========== << Initialize >> ENTER ==========");
  DEVICEAPI_SLOG_INFO("Extension Module library : [%s]", library_path_.c_str());

  void* handle = dlopen(library_path_.c_str(), RTLD_LAZY);
  if (!handle) {
    const char* error = (const char*)dlerror();
    DEVICEAPI_LOG_ERROR("Error loading extension '%s'. Reason: %s", library_path_.c_str(), (error != NULL ? error : "unknown"));
    return false;
  }

  XW_Initialize_Func initialize = reinterpret_cast<XW_Initialize_Func>(
      dlsym(handle, "XW_Initialize"));
  if (!initialize) {
    DEVICEAPI_LOG_ERROR("Error loading extension");
    DEVICEAPI_SLOG_ERROR("[%s] couldn't get XW_Initialize function", library_path_.c_str());
    dlclose(handle);
    return false;
  }

  ExtensionAdapter* adapter = ExtensionAdapter::GetInstance();
  xw_extension_ = adapter->GetNextXWExtension();
  adapter->RegisterExtension(this);

  int ret = initialize(xw_extension_, ExtensionAdapter::GetInterface);
  if (ret != XW_OK) {
    DEVICEAPI_LOG_ERROR("Error loading extension");
    DEVICEAPI_SLOG_ERROR("[%s] XW_Initialize function returned error value.", library_path_.c_str());
    dlclose(handle);
    return false;
  }

  initialized_ = true;
  DEVICEAPI_LOG_INFO("========== << Initialize >> END ==========");
  return true;
}

ExtensionInstance* Extension::CreateInstance() {
  Initialize();
  ExtensionAdapter* adapter = ExtensionAdapter::GetInstance();
  XW_Instance xw_instance = adapter->GetNextXWInstance();
  return new ExtensionInstance(this, xw_instance);
}

void Extension::GetRuntimeVariable(const char* key, char* value, size_t value_len) {
#if 0
  if( rv_provider_ ){
      std::string ret = rv_provider_->GetRuntimeVariable(key);
      strncpy(value, ret.c_str(), value_len);
  }
#else
  DEVICEAPI_LOG_INFO("GETRUNTIMEVAR: not implemented");
  STARFISH_ASSERT_NOT_REACHED();
#endif
}
int Extension::CheckAPIAccessControl(const char* /*api_name*/) {
  // TODO
  return XW_OK;
}

int Extension::RegisterPermissions(const char* /*perm_table*/) {
  // TODO
  return XW_OK;
}

ExtensionInstance::ExtensionInstance(Extension* extension, XW_Instance xw_instance)
  : extension_(extension),
    xw_instance_(xw_instance),
    instance_data_(NULL),
    post_message_listener_(NULL),
    post_data_listener_(NULL) {
  DEVICEAPI_LOG_INFO("Enter");
  ExtensionAdapter::GetInstance()->RegisterInstance(this);
  XW_CreatedInstanceCallback callback = extension_->created_instance_callback_;
  if (callback)
    callback(xw_instance_);
}

ExtensionInstance::~ExtensionInstance() {
  DEVICEAPI_LOG_INFO("Enter");
  XW_DestroyedInstanceCallback callback = extension_->destroyed_instance_callback_;
  if (callback)
    callback(xw_instance_);
  ExtensionAdapter::GetInstance()->UnregisterInstance(this);
}

void ExtensionInstance::HandleMessage(const std::string& msg) {
  XW_HandleMessageCallback callback = extension_->handle_msg_callback_;
  if (callback)
    callback(xw_instance_, msg.c_str());
}

void ExtensionInstance::HandleSyncMessage(const std::string& msg) {
  XW_HandleSyncMessageCallback callback = extension_->handle_sync_msg_callback_;
  if (callback) {
    sync_reply_msg_.clear();
    callback(xw_instance_, msg.c_str());
  }
}

void ExtensionInstance::PostMessage(const std::string& msg) {
  if (post_message_listener_) {
    post_message_listener_->PostMessageToJS(msg);
  }
}

void ExtensionInstance::SyncReply(const std::string& reply) {
  sync_reply_msg_ = reply;
}

void ExtensionInstance::HandleData(const std::string& msg, uint8_t* buffer, size_t len) {
  XW_HandleDataCallback callback = extension_->handle_data_callback_;
  if (callback)
    callback(xw_instance_, msg.c_str(), buffer, len);
}

void ExtensionInstance::HandleSyncData(const std::string& msg, uint8_t* buffer, size_t len) {
  XW_HandleDataCallback callback = extension_->handle_sync_data_callback_;
  if (callback) {
    sync_reply_msg_.clear();
    sync_reply_buffer_len_ = 0;
    sync_reply_buffer_ = NULL;
    // sync_reply_buffer_ will be freed by XWalkExtensionModule
    callback(xw_instance_, msg.c_str(), buffer, len);
  }
}

void ExtensionInstance::PostData(const std::string& msg, uint8_t* buffer, size_t len) {
  if (post_data_listener_) {
    post_data_listener_->PostDataToJS(msg, buffer, len);
  }
}

void ExtensionInstance::SyncDataReply(const std::string& reply, uint8_t* buffer, size_t len) {
  sync_reply_msg_ = reply;
  sync_reply_buffer_ = buffer;
  sync_reply_buffer_len_ = len;
}

} // namespace xwalk
} // namespace wrt

namespace DeviceAPI {

ESPostListener::ESPostListener(escargot::ESVMInstance* instance, escargot::ESFunctionObject* listener)
    : instance_(instance)
    , listener_(listener)
{
    DEVICEAPI_LOG_INFO("Enter");
    GC_add_roots(&listener_, &listener_ + sizeof(escargot::ESFunctionObject*));
}

ESPostListener::~ESPostListener()
{
    DEVICEAPI_LOG_INFO("Enter");
    finalize();
}

void ESPostListener::finalize()
{
    DEVICEAPI_LOG_INFO("Enter");
    GC_remove_roots(&listener_, &listener_ + sizeof(escargot::ESFunctionObject*));
    listener_ = nullptr;
    instance_ = nullptr;
}

void ESPostMessageListener::PostMessageToJS(const std::string& msg)
{
    DEVICEAPI_LOG_INFO("ESPostMessageListener::PostMessageToJS (msg %s listener %p instance %p)", msg.c_str(), listener_, instance_);

    ExtensionManagerInstance* extensionManagerInstance = ExtensionManagerInstance::get(instance_);
    if (!extensionManagerInstance)
        return;

    StarFish::Window* wnd = (StarFish::Window*)instance_->globalObject()->extraPointerData();
    StarFish::StarFishEnterer e(wnd->starFish());

    std::jmp_buf tryPosition;
    if (setjmp(instance_->registerTryPos(&tryPosition)) == 0) {
        escargot::ESValue arguments[] = {escargot::ESString::create(msg.c_str())};
        escargot::ESFunctionObject::call(instance_, listener_, escargot::ESValue(), arguments, 1, false);
        instance_->unregisterTryPos(&tryPosition);
    } else {
        escargot::ESValue err = instance_->getCatchedError();
        DEVICEAPI_LOG_ERROR("Uncaught %s\n", err.toString()->utf8Data());
    }
}

void ESPostDataListener::PostDataToJS(const std::string& msg, uint8_t* buffer, size_t len)
{
    DEVICEAPI_LOG_INFO("ESPostDataListener::PostDataToJS (%s, %u)", msg.c_str(), len);

    ExtensionManagerInstance* extensionManagerInstance = ExtensionManagerInstance::get(instance_);
    if (!extensionManagerInstance)
        return;

    StarFish::Window* wnd = (StarFish::Window*)instance_->globalObject()->extraPointerData();
    StarFish::StarFishEnterer e(wnd->starFish());

    std::jmp_buf tryPosition;
    if (setjmp(instance_->registerTryPos(&tryPosition)) == 0) {
#if 0
        escargot::ESValue arguments[] = {escargot::ESString::create(msg.c_str())};
        escargot::ESFunctionObject::call(instance_, listener_, escargot::ESValue(), arguments, 1, false);
#else
        DEVICEAPI_LOG_ERROR("NOT IMPLEMENTED");
        STARFISH_ASSERT_NOT_REACHED();
        instance_->unregisterTryPos(&tryPosition);
#endif
    } else {
        escargot::ESValue err = instance_->getCatchedError();
        DEVICEAPI_LOG_ERROR("Uncaught %s\n", err.toString()->utf8Data());
    }
}

} // namespace DeviceAPI
