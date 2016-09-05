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

#ifndef __StarFishHTMLMetaElement__
#define __StarFishHTMLMetaElement__

#include "dom/HTMLElement.h"

namespace StarFish {

class HTMLMetaElement : public HTMLElement {
public:
    HTMLMetaElement(Document* document)
        : HTMLElement(document)
        , m_name(String::emptyString)
        , m_content(String::emptyString)
#ifdef STARFISH_TIZEN
        , m_tizenWidgetTransparentBackground(false)
#endif
    {
    }

    virtual void initScriptObject(ScriptBindingInstance* instance)
    {
        initScriptWrappable(this);
    }

    /* 4.4 Interface Node */

    virtual String* localName()
    {
        return document()->window()->starFish()->staticStrings()->m_metaTagName.localName();
    }

    virtual QualifiedName name()
    {
        return document()->window()->starFish()->staticStrings()->m_metaTagName;
    }

    /* Other methods (not in DOM API) */

    virtual bool isHTMLMetaElement() const
    {
        return true;
    }

    virtual void didAttributeChanged(QualifiedName name, String* old, String* value, bool attributeCreated, bool attributeRemoved);
    virtual void didNodeInsertedToDocumenTree();
    virtual void didNodeRemovedFromDocumenTree();
    void checkPlatformFlags();
protected:
    String* m_name;
    String* m_content;

#ifdef STARFISH_TIZEN
    bool m_tizenWidgetTransparentBackground;
#endif
};

}

#endif
