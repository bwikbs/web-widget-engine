#include "StarFishConfig.h"
#include "XMLHttpRequest.h"

#include <curl/curl.h>
#include <future>
#include <Elementary.h>

namespace StarFish {

XMLHttpRequest::XMLHttpRequest()
{
    //FIXME: temp soluation
    set(escargot::ESString::create("responseText"),escargot::ESValue(escargot::ESValue::ESNull));
    set(escargot::ESString::create("response"),escargot::ESValue(escargot::ESValue::ESNull));

    //init
    initScriptWrappable(this);
    m_method = UNKNOWN_METHOD;
    m_response_type = DEFAULT_RESPONSE;
    m_url = nullptr;
}

void XMLHttpRequest::send(String* body)
{
      escargot::ESObject* obj = (escargot::ESObject*)this;
      const char* url = m_url->utf8Data();

      // invoke loadstart Event.
      callEventHandler(String::fromUTF8("loadstart"),true);

      auto task  = [](escargot::ESObject* obj,const char* url, XMLHttpRequest::METHOD_TYPE methodType, const char* body)->bool{

      Buffer header;
      header.memory=NULL;
      header.size=0;

      Buffer buffer;
      buffer.memory=NULL;
      buffer.size=0;

      CURL* curl = curl_easy_init();
      CURLcode res;

      if (!curl) return false;

      curl_easy_setopt(curl, CURLOPT_URL,url );
      //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
      curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void*) &header);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &buffer);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

      if(methodType==POST_METHOD){
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
      }else if(methodType==GET_METHOD){

      }else{
        printf("XMLHttpRequest UnSupportted method!!  \n");

        //invoke loadend event
        ((XMLHttpRequest*)obj)->callEventHandler(String::fromUTF8("loadend"),false);
        return false;
      }

      res = curl_easy_perform(curl);

      long res_code = 0;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res_code);
      if (!((res_code == 200 || res_code == 201) && res != CURLE_ABORTED_BY_CALLBACK))
      {
          printf("!!! Response code: %ld\n", res_code);

          //invoke error event
          ((XMLHttpRequest*)obj)->callEventHandler(String::fromUTF8("error"),false);

          //invoke loadend event
          ((XMLHttpRequest*)obj)->callEventHandler(String::fromUTF8("loadend"),false);
          return false;
      }
      curl_easy_cleanup(curl);

      if(buffer.size==0)
          return false;

      struct Pass {
          escargot::ESObject* obj;
          char* buf;
          char* header;

      };

      Pass* pass = new Pass;
      pass->buf = buffer.memory;
      pass->header = header.memory;
      pass->obj = obj;

      ecore_thread_main_loop_begin();
      ecore_idler_add([](void *data)->Eina_Bool{
          Pass* pass = (Pass*)data;

          escargot::ESObject* this_obj = pass->obj;
          escargot::ESVMInstance* instance = escargot::ESVMInstance::currentInstance();
          printf("HEADER : %s\n",pass->header);

          switch(((XMLHttpRequest*)this_obj)->getResponseType()){
            case JSON_RESPONSE:
              {
                escargot::ESValue ret;
                escargot::ESValue json_arg[1] = {escargot::ESValue(escargot::ESString::create(pass->buf))};
                escargot::ESValue json_parse_fn = instance->globalObject()->json()->get(escargot::ESValue(escargot::ESString::create("parse")));
                ret = ((XMLHttpRequest*)this_obj)->callJSFunction(instance, json_parse_fn, instance->globalObject()->json(), json_arg, 1);
                this_obj->set(escargot::ESString::create("response"),ret);
              }
              break;

            case TEXT_RESPONSE:
            default:
              this_obj->set(escargot::ESString::create("response"),escargot::ESValue(escargot::ESString::create(pass->buf)));
              this_obj->set(escargot::ESString::create("responseText"),escargot::ESValue(escargot::ESString::create(pass->buf)));

          }
          //invoke load event
          ((XMLHttpRequest*)this_obj)->callEventHandler(String::fromUTF8("load"),true);

          //invoke loadend event
          ((XMLHttpRequest*)this_obj)->callEventHandler(String::fromUTF8("loadend"),true);
          return ECORE_CALLBACK_CANCEL;
      }, pass);
      ecore_thread_main_loop_end();
      return true;
  };

  //std::future<bool> runTask = std::async(std::launch::async, task);

  std::thread(task,obj,url,m_method,body->utf8Data()).detach();
}

void XMLHttpRequest::setResponseType(const char* responseType)
{
    if(responseType){
        std::string data = responseType;
        std::transform(data.begin(), data.end(), data.begin(), ::tolower);
        String* lowerName = String::fromUTF8(data.c_str());

        if(lowerName->equals("text")){
            m_response_type = TEXT_RESPONSE;
        }else if(lowerName->equals("arraybuffer")){
            m_response_type = ARRAY_BUFFER_RESPONSE;
        }else if(lowerName->equals("blob")){
            m_response_type = BLOB_RESPONSE;
        }else if(lowerName->equals("document")){
            m_response_type = DOCUMENT_RESPONSE;
        }else if(lowerName->equals("json")){
            m_response_type = JSON_RESPONSE;
        }
    }
}


void XMLHttpRequest::setOpen(const char* method,String* url)
{
    if(method){
        std::string data = method;
        std::transform(data.begin(), data.end(), data.begin(), ::tolower);
        String* lowerName = String::fromUTF8(data.c_str());

        if(lowerName->equals("get")){
            m_method = GET_METHOD;
        }else if(lowerName->equals("post")){
            m_method = POST_METHOD;
        }
    }
    m_url = url;
}

String* XMLHttpRequest::getResponseTypeStr(){
    switch(m_response_type){
        case TEXT_RESPONSE:
            return String::fromUTF8("text");

        case ARRAY_BUFFER_RESPONSE:
            return String::fromUTF8("arraybuffer");

        case BLOB_RESPONSE:
            return String::fromUTF8("blob");

        case DOCUMENT_RESPONSE:
            return String::fromUTF8("document");

        case JSON_RESPONSE:
            return String::fromUTF8("json");

        case DEFAULT_RESPONSE:
            return String::emptyString;

    }
    return String::emptyString;
}

void XMLHttpRequest::callEventHandler(String* eventName,bool isMainThread){
    if(isMainThread){
      escargot::ESVMInstance* instance = escargot::ESVMInstance::currentInstance();
      escargot::ESValue fn = getHandler(eventName);
      if(fn!=escargot::ESValue::ESNull)
        callJSFunction(instance, fn, this, NULL, 0);
    }else{

      struct Pass {
          escargot::ESObject* obj;
          String* buf;
      };

      Pass* pass = new Pass;
      pass->buf = eventName;
      pass->obj = this;

      ecore_thread_main_loop_begin();
      ecore_idler_add([](void *data)->Eina_Bool{
          Pass* pass = (Pass*)data;
          escargot::ESObject* this_obj = pass->obj;
          escargot::ESVMInstance* instance = escargot::ESVMInstance::currentInstance();
          escargot::ESValue fn = ((XMLHttpRequest*)this_obj)->getHandler(pass->buf);
          if(fn!=escargot::ESValue::ESNull)
            callJSFunction(instance, fn, this_obj, NULL, 0);
          return ECORE_CALLBACK_CANCEL;
      }, pass);
      ecore_thread_main_loop_end();
    }

}

// void XMLHttpRequest::callReadystatechangeHandler(escargot::ESVMInstance* instance){
//     escargot::ESValue fn = getHandler(String::fromUTF8("readystatechange"));
//     if(fn!=escargot::ESValue::ESNull)
//       callJSFunction(instance, fn, this, NULL, 0);
// }

}
