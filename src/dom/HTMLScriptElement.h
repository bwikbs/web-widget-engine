#ifndef __StarFishHTMLScriptElement__
#define __StarFishHTMLScriptElement__

#include "dom/HTMLElement.h"

namespace StarFish {

class HTMLScriptElement : public HTMLElement {
public:
    HTMLScriptElement(Document* document)
        : HTMLElement(document)
    {
        initScriptWrappable(this);
    }

    virtual bool isHTMLScriptElement() const
    {
        return true;
    }

    virtual String* localName()
    {
        return document()->window()->starFish()->staticStrings()->m_scriptLocalName;
    }

protected:
    // String* m_text;
};

}

#endif
