/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef __StarFishURL__
#define __StarFishURL__

#include "util/String.h"
#include "dom/binding/ScriptWrappable.h"

namespace StarFish {

class Blob;

class URL : public ScriptWrappable {
    URL(String* baseURL, String* url);
public:
    static String* parseURLString(String* baseURL, String* url);
    static String* createObjectURL(Blob* blob);
    static URL* createURL(String* baseURL, String* url)
    {
        return new URL(baseURL, url);
    }

    virtual void initScriptObject(ScriptBindingInstance* instance)
    {
        initScriptWrappable(this, instance);
    }

    String* baseURI() const;
    bool isFileURL() const
    {
        return m_urlString->startsWith("file://");
    }

    bool isDataURL() const
    {
        return m_urlString->startsWith("data:");
    }

    bool isBlobURL() const
    {
        return m_urlString->startsWith("blob:");
    }

    String* string() const
    {
        return m_string;
    }

    String* urlString() const
    {
        return m_urlString;
    }

    // http://foo.com/asdf?asdf=1 -> http://foo.com/asdf
    String* urlStringWithoutSearchPart() const;
    bool operator==(const URL& other) const
    {
        return other.urlString()->equals(m_urlString);
    }

    String* origin();
    String* getHref();
    String* getProtocol();
    String* getUsername();
    String* getPassword();
    String* getHost();
    String* getHostname();
    String* getPort();
    String* getPathname();
    String* getSearch();
    String* getHash();

protected:
    String* m_string;
    String* m_urlString;

    int m_protocolEnd;
    int m_userStart;
    int m_userEnd;
    int m_passwordEnd;
    int m_hostEnd;
    int m_portEnd;
    int m_pathEnd;
    int m_queryEnd;
    int m_fragmentEnd;
};
}

#endif
