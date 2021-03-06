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

#ifndef __StarFishHTMLBodyElement__
#define __StarFishHTMLBodyElement__

#include "dom/HTMLElement.h"

namespace StarFish {

class HTMLBodyElement : public HTMLElement {
public:
    HTMLBodyElement(Document* document)
        : HTMLElement(document)
    {
    }

    virtual void initScriptObject(ScriptBindingInstance* instance)
    {
        initScriptWrappable(this);
    }

    /* 4.4 Interface Node */

    virtual String* localName()
    {
        return document()->window()->starFish()->staticStrings()->m_bodyTagName.localName();
    }

    virtual QualifiedName name()
    {
        return document()->window()->starFish()->staticStrings()->m_bodyTagName;
    }

    /* Other methods (not in DOM API) */

    virtual bool isHTMLBodyElement() const
    {
        return true;
    }

    virtual void didComputedStyleChanged(ComputedStyle* oldStyle, ComputedStyle* newStyle)
    {
        HTMLElement::didComputedStyleChanged(oldStyle, newStyle);
        if (!newStyle->backgroundColor().isTransparent() || !newStyle->backgroundImage()->equals(String::emptyString)) {
            document()->window()->m_hasBodyElementBackground = true;
        } else {
            document()->window()->m_hasBodyElementBackground = false;
        }

        if (oldStyle && oldStyle->overflow() != newStyle->overflow()) {
            document()->setNeedsFrameTreeBuild();
        }
    }

    virtual void didAttributeChanged(QualifiedName name, String* old, String* value, bool attributeCreated, bool attributeRemoved);
protected:
};

}

#endif
