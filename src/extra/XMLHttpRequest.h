#ifndef __StarFishXHRElement__
#define __StarFishXHRElement__

#include <curl/curl.h>
#include "dom/binding/ScriptWrappable.h"

namespace StarFish {

class XMLHttpRequest : public ScriptWrappable {
public:
    struct Buffer {
        char *memory;
        size_t size;
    };

    XMLHttpRequest()
    {
        //FIXME: temp soluation
        set(escargot::ESString::create("responseText"),escargot::ESValue(escargot::ESValue::ESNull));
        initScriptWrappable(this);

        m_method = nullptr;
        m_url = nullptr;
    }

    void setOpen(String* method,String* url)
    {
        m_method = method;
        m_url = url;
    }

    void send(){

        //Buffer header;
        Buffer buffer;
        buffer.memory=NULL;
        buffer.size=0;

        CURL* curl = curl_easy_init();
        CURLcode res;

        if (!curl) return ;
        curl_easy_setopt(curl, CURLOPT_URL, m_url->utf8Data());
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        //curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void*) &header);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &buffer);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

        res = curl_easy_perform(curl);

        long res_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res_code);
        if (!((res_code == 200 || res_code == 201) && res != CURLE_ABORTED_BY_CALLBACK))
        {
            printf("!!! Response code: %ld\n", res_code);
        }

        set(escargot::ESString::create("responseText"),escargot::ESValue(escargot::ESString::create(buffer.memory)));
        curl_easy_cleanup(curl);

        //fire load event
        escargot::ESObject* obj = (escargot::ESObject*)this;
        escargot::ESValue fn = load_eventhandler;
        escargot::ESVMInstance* instance = escargot::ESVMInstance::currentInstance();

        std::jmp_buf tryPosition;
        if (setjmp(instance->registerTryPos(&tryPosition)) == 0) {
            escargot::ESFunctionObject::call(instance, fn, obj, NULL, 0, false);
            instance->unregisterTryPos(&tryPosition);
        } else {
            escargot::ESValue err = instance->getCatchedError();
            printf("Uncaught %s\n", err.toString()->utf8Data());
        }

    }
    void addEventListener(String* eventName,escargot::ESValue handler){
        if(eventName->equals("load")){
            load_eventhandler = handler;
        }
    }

    // String* getResponse()
    //     return m_response;
    // }
    static void* CURL_realloc(void *ptr, size_t size)
    {
        /* There might be a realloc() out there that doesn't like reallocing
         *      NULL pointers, so we take care of it here */
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

    virtual void paint(Canvas* canvas){}

protected:
    String* m_url;
    String* m_method;
    // String* m_response;
    escargot::ESValue load_eventhandler;

};


}

#endif
