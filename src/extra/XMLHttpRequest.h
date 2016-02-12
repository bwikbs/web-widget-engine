#ifndef __StarFishXHRElement__
#define __StarFishXHRElement__

#include "dom/binding/ScriptWrappable.h"
#include "dom/EventTarget.h"
#include "platform/canvas/Canvas.h"
#include <curl/curl.h>

#define MINIMAL_PROGRESS_INTERVAL     0.1

namespace StarFish {

class XMLHttpRequestEventTarget : public EventTarget<ScriptWrappable>{

public:
    XMLHttpRequestEventTarget()
    : m_onloadstart(escargot::ESValue::ESNull),
      m_onprogress(escargot::ESValue::ESNull),
      m_onabort(escargot::ESValue::ESNull),
      m_onerror(escargot::ESValue::ESNull),
      m_onload(escargot::ESValue::ESNull),
      m_ontimeout(escargot::ESValue::ESNull),
      m_onloadend(escargot::ESValue::ESNull),
      m_onreadystatechange(escargot::ESValue::ESNull)
    {}
    bool setHandler(String* keyName,escargot::ESValue handler){
        if(keyName->equals("loadstart")||keyName->equals("onloadstart")){
            m_onloadstart = handler;
        }else if(keyName->equals("progress")||keyName->equals("onprogress")){
            m_onprogress = handler;
        }else if(keyName->equals("error")||keyName->equals("onerror")){
            m_onerror = handler;
        }else if(keyName->equals("abort")||keyName->equals("onabort")){
            m_onabort = handler;
        }else if(keyName->equals("timeout")||keyName->equals("ontimeout")){
            m_ontimeout = handler;
        }else if(keyName->equals("load")||keyName->equals("onload")){
            m_onload = handler;
        }else if(keyName->equals("loadend")||keyName->equals("onloadend")){
            m_onloadend = handler;
        }else if(keyName->equals("readystatechange")||keyName->equals("onreadystatechange")){
            m_onreadystatechange = handler;
        }else{
            return false;
        }
        return true;
    }

    escargot::ESValue getHandler(String* keyName){

        if(keyName->equals("loadstart")||keyName->equals("onloadstart")){
            if(m_onloadstart!=escargot::ESValue::ESNull)
                return m_onloadstart;
        }else if(keyName->equals("progress")||keyName->equals("onprogress")){
            if(m_onprogress!=escargot::ESValue::ESNull)
                return m_onprogress;
        }else if(keyName->equals("error")||keyName->equals("onerror")){
            if(m_onerror!=escargot::ESValue::ESNull)
                return m_onerror;
        }else if(keyName->equals("abort")||keyName->equals("onabort")){
            if(m_onabort!=escargot::ESValue::ESNull)
                return m_onabort;
        }else if(keyName->equals("timeout")||keyName->equals("ontimeout")){
            if(m_ontimeout!=escargot::ESValue::ESNull)
                return m_ontimeout;
        }else if(keyName->equals("load")||keyName->equals("onload")){
            if(m_onload!=escargot::ESValue::ESNull)
                return m_onload;
        }else if(keyName->equals("loadend")||keyName->equals("onloadend")){
            if(m_onloadend!=escargot::ESValue::ESNull)
                return m_onloadend;
        }else if(keyName->equals("readystatechange")||keyName->equals("onreadystatechange")){
            if(m_onreadystatechange!=escargot::ESValue::ESNull)
                return m_onreadystatechange;
        }
        return escargot::ESValue::ESNull;
    }

private:
    escargot::ESValue m_onloadstart;
    escargot::ESValue m_onprogress;
    escargot::ESValue m_onabort;
    escargot::ESValue m_onerror;
    escargot::ESValue m_onload;
    escargot::ESValue m_ontimeout;
    escargot::ESValue m_onloadend;

    //FIXME: mh.byun
    escargot::ESValue m_onreadystatechange;

};


class XMLHttpRequest : public XMLHttpRequestEventTarget{
public:
    enum METHOD_TYPE{
        POST_METHOD,
        GET_METHOD,
        UNKNOWN_METHOD
    };

    enum RESPONSE_TYPE{
        TEXT_RESPONSE,
        ARRAY_BUFFER_RESPONSE,
        BLOB_RESPONSE,
        DOCUMENT_RESPONSE,
        JSON_RESPONSE,
        DEFAULT_RESPONSE
    };

    enum READY_STATE{
        UNSENT,
        OPENED,
        HEADERS_RECEIVED,
        LOADING,
        DONE
    };

    struct Buffer {
        char *memory;
        size_t size;
    };

    struct HeaderBuffer {
        char *memory;
        char *contentType;
        size_t size;
    };

    struct ProgressData {
      double lastruntime;
      CURL *curl;
      XMLHttpRequest* obj;
      int32_t loaded;
      int32_t total;
    };

    XMLHttpRequest();

    RESPONSE_TYPE getResponseType(){
        return m_response_type;
    }

    String* getResponseTypeStr();
    int getReadyState(){
        return m_ready_state;
    }

    void setTimeout(uint32_t timeout){
        m_timeout = timeout;
    }
    uint32_t getTimeout(){
        return m_timeout;
    }
    void setResponseType(const char* responseType);
    void setOpen(const char* method,String* url);
    void send(String* body);
    void callEventHandler(String* eventName,bool isMainThread,uint32_t loaded,uint32_t total);

    void setResponseHeader(String* responseHeader){
        m_response_header = responseHeader;
    }

    String* getAllResponseHeadersStr(){
        if(m_response_header!=nullptr)
            return m_response_header;
        return String::emptyString;
    }

    void abort(){
        m_abort_flag=true;
    }

    bool checkAbort(){
        if(m_abort_flag){
            m_abort_flag=false;
            return true;
        }
        return false;
    }

    void setScriptBindingInstance(ScriptBindingInstance* instance){
        m_bindingInstance = instance;
    }

    ScriptBindingInstance* striptBindingInstance(){
        return m_bindingInstance;
    }

    static escargot::ESValue callJSFunction(escargot::ESVMInstance* instance, const escargot::ESValue& callee, const escargot::ESValue& receiver, escargot::ESValue arguments[], const size_t& argumentCount){
        escargot::ESValue result;
        std::jmp_buf tryPosition;
        if (setjmp(instance->registerTryPos(&tryPosition)) == 0) {
            result = escargot::ESFunctionObject::call(instance, callee, receiver, arguments, argumentCount, false);
            instance->unregisterTryPos(&tryPosition);
        } else {
            result = instance->getCatchedError();
            printf("Uncaught %s\n", result.toString()->utf8Data());
        }
        return result;
    }

    // FIXME:mh.byun (FIX!!! memory leak)
    static void* CURL_realloc(void *ptr, size_t size)
    {
        if(ptr)
            return realloc(ptr, size);
        else
            return malloc(size);
    }

    static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
    {
        size_t realsize = size * nmemb;
        struct Buffer *mem = (struct Buffer *)data;

        mem->memory = (char *)
            CURL_realloc(mem->memory, mem->size + realsize + 1);
        if (mem->memory) {
            memcpy(&(mem->memory[mem->size]), ptr, realsize);
            mem->size += realsize;
            mem->memory[mem->size] = 0;
        }
        return realsize;
    }

    static size_t WriteHeaderCallback(void *ptr, size_t size, size_t nmemb, void *data)
    {
        size_t realsize = size * nmemb;
        struct HeaderBuffer *mem = (struct HeaderBuffer *)data;

        mem->memory = (char *)
            CURL_realloc(mem->memory, mem->size + realsize + 1);
        if (mem->memory) {
            memcpy(&(mem->memory[mem->size]), ptr, realsize);
            mem->size += realsize;
            mem->memory[mem->size] = 0;
        }

        if (strncmp((char *)(ptr), "Content-Type:", 13) == 0) {
            mem->contentType = (char *)CURL_realloc(mem->contentType, realsize + 1 - 13);
            sscanf ((char *)(ptr), "Content-Type: %s",mem->contentType);
        }
        return realsize;
    }
    static int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
    {
        ProgressData* p_data = (ProgressData *)clientp;
        CURL *curl = p_data->curl;
        XMLHttpRequest* this_obj = p_data->obj;
        double curtime = 0;

        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &curtime);

        if((curtime - p_data->lastruntime) >= MINIMAL_PROGRESS_INTERVAL) {
          p_data->lastruntime = curtime;

          p_data->loaded = static_cast<uint32_t>(dlnow);
          p_data->total = static_cast<uint32_t>(dltotal);
          this_obj->callEventHandler(String::fromUTF8("progress"),false,p_data->loaded,p_data->total);

          // printf("TOTAL TIME: %f \r\n", curtime);
          // printf("UP: %" CURL_FORMAT_CURL_OFF_T " of %" CURL_FORMAT_CURL_OFF_T
          //         "  DOWN: %" CURL_FORMAT_CURL_OFF_T " of %" CURL_FORMAT_CURL_OFF_T
          //         "\r\n",
          //         ulnow, ultotal, dlnow, dltotal);
        }

        //check abort
        if(this_obj->checkAbort())
            return 1;

        return 0;

    }

    // static int progress_callback_old(void *p,
    //                           double dltotal, double dlnow,
    //                           double ultotal, double ulnow)
    // {
    //     printf("test??\n");
    //   return progress_callback(p,
    //                   (curl_off_t)dltotal,
    //                   (curl_off_t)dlnow,
    //                   (curl_off_t)ultotal,
    //                   (curl_off_t)ulnow);
    // }

    virtual void paint(Canvas* canvas){}

protected:
    String* m_url;
    String* m_response_header;
    METHOD_TYPE m_method;
    RESPONSE_TYPE m_response_type;
    READY_STATE m_ready_state;
    uint32_t m_timeout;
    bool m_abort_flag;
    ScriptBindingInstance* m_bindingInstance;
};


}

#endif
