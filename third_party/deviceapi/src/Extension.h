// Copyright 2014 Samsung Electronics Co, Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WRT_SERVICE_NODE_EXTENSION_H_
#define WRT_SERVICE_NODE_EXTENSION_H_

#include <string>
#include <vector>

#include "XW_Extension.h"
#include "XW_Extension_SyncMessage.h"
#include "XW_Extension_Data.h"

namespace wrt {
class RuntimeVariableProvider;

namespace xwalk {

class ExtensionAdapter;
class ExtensionInstance;

class Extension {
 public:
  Extension(const std::string& path, RuntimeVariableProvider* provider);
  Extension(const std::string& path,
            const std::string& name,
            const std::vector<std::string>& entry_points,
            RuntimeVariableProvider* provider);

  virtual ~Extension();

  bool Initialize();
  ExtensionInstance* CreateInstance();

  XW_Extension xw_extension() {
    return xw_extension_;
  }

  std::string name() {
    return name_;
  }

  std::string javascript_api() {
    Initialize();
    return javascript_api_;
  }

  std::vector<std::string>& entry_points() {
    return entry_points_;
  }

  bool use_trampoline() {
    return use_trampoline_;
  }

  void set_name(const std::string& name) {
    name_ = name;
  }

  void set_javascript_api(const std::string& javascript_api) {
    javascript_api_ = javascript_api;
  }

  void set_use_trampoline(bool use_trampoline) {
    use_trampoline_ = use_trampoline;
  }

 private:
  friend class ExtensionAdapter;
  friend class ExtensionInstance;

  void GetRuntimeVariable(const char* key, char* value, size_t value_len);
  int CheckAPIAccessControl(const char* api_name);
  int RegisterPermissions(const char* perm_table);

  bool initialized_;
  std::string library_path_;

  XW_Extension xw_extension_;
  std::string name_;
  std::string javascript_api_;
  std::vector<std::string> entry_points_;
  bool use_trampoline_;

  XW_CreatedInstanceCallback created_instance_callback_;
  XW_DestroyedInstanceCallback destroyed_instance_callback_;
  XW_ShutdownCallback shutdown_callback_;
  XW_HandleMessageCallback handle_msg_callback_;
  XW_HandleSyncMessageCallback handle_sync_msg_callback_;
  XW_HandleDataCallback handle_data_callback_;
  XW_HandleDataCallback handle_sync_data_callback_;
  RuntimeVariableProvider* rv_provider_;
};

class PostMessageListener {
 public:
  virtual void PostMessageToJS(const std::string& msg) = 0;
};

class PostDataListener {
 public:
  virtual void PostDataToJS(const std::string& msg, uint8_t* buffer, size_t len) = 0;
};

class ExtensionInstance {
 public:
  ExtensionInstance(Extension* extension, XW_Instance xw_instance);
  virtual ~ExtensionInstance();

  void HandleMessage(const std::string& msg);
  void HandleSyncMessage(const std::string& msg);

  void HandleData(const std::string& msg, uint8_t* buffer, size_t len);
  void HandleSyncData(const std::string& msg, uint8_t* buffer, size_t len);

  XW_Instance xw_instance() {
    return xw_instance_;
  }

  std::string sync_replay_msg() {
    return sync_reply_msg_;
  }

  std::string sync_data_reply_msg(uint8_t** buffer, size_t *len) {
    *buffer = sync_reply_buffer_;
    *len = sync_reply_buffer_len_;
    return sync_reply_msg_;
  }

  void set_post_message_listener(PostMessageListener* listener) {
    post_message_listener_ = listener;
  }

  void set_post_data_listener(PostDataListener* listener) {
    post_data_listener_ = listener;
  }

 private:
  friend class ExtensionAdapter;

  void PostMessage(const std::string& msg);
  void SyncReply(const std::string& reply);

  void PostData(const std::string& msg, uint8_t* buffer, size_t len);
  void SyncDataReply(const std::string& reply, uint8_t* buffer, size_t len);

  Extension* extension_;
  XW_Instance xw_instance_;
  void* instance_data_;
  std::string sync_reply_msg_;
  uint8_t* sync_reply_buffer_;
  size_t sync_reply_buffer_len_;

  PostMessageListener* post_message_listener_;
  PostDataListener* post_data_listener_;
};

} // namespace xwalk
} // namespace wrt

namespace escargot {
class ESFunctionObject;
class ESVMInstance;
}

namespace DeviceAPI {

class ESPostListener {
public:
    virtual ~ESPostListener();
    void finalize();

protected:
    ESPostListener(escargot::ESVMInstance* instance, escargot::ESFunctionObject* listener);

    escargot::ESVMInstance* instance_;
    escargot::ESFunctionObject* listener_;
};

class ESPostMessageListener : public wrt::xwalk::PostMessageListener, public ESPostListener {
public:
    static ESPostMessageListener* create(escargot::ESVMInstance* instance, escargot::ESFunctionObject* listener)
    {
        return new ESPostMessageListener(instance, listener);
    }
    void PostMessageToJS(const std::string& msg);

private:
    ESPostMessageListener(escargot::ESVMInstance* instance, escargot::ESFunctionObject* listener)
        : ESPostListener(instance, listener) { }
};

class ESPostDataListener : public wrt::xwalk::PostDataListener, public ESPostListener {
public:
    static ESPostDataListener* create(escargot::ESVMInstance* instance, escargot::ESFunctionObject* listener)
    {
        return new ESPostDataListener(instance, listener);
    }
    void PostDataToJS(const std::string& msg, uint8_t* buffer, size_t len);

private:
    ESPostDataListener(escargot::ESVMInstance* instance, escargot::ESFunctionObject* listener)
        : ESPostListener(instance, listener) { }
};

} // namespace DeviceAPI

#endif // WRT_SERVICE_NODE_EXTENSION_H_