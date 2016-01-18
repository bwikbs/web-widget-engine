#ifndef __StarFishHTMLBodyElement__
#define __StarFishHTMLBodyElement__

#include "dom/HTMLElement.h"

namespace StarFish {

class HTMLBodyElement : public HTMLElement {
public:
    HTMLBodyElement(Document* document)
        : HTMLElement(document)
    {
        initScriptWrappable(this);
    }

    virtual String* localName()
    {
        return document()->window()->starFish()->staticStrings()->m_bodyLocalName;
    }

    virtual bool isHTMLBodyElement()
    {
        return true;
    }

protected:
};

}

#endif
