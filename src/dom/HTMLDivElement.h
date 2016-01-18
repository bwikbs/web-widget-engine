#ifndef __StarFishHTMLDivElement__
#define __StarFishHTMLDivElement__

#include "dom/HTMLElement.h"

namespace StarFish {

class HTMLDivElement : public HTMLElement {
public:
    HTMLDivElement(Document* document)
        : HTMLElement(document)
    {
        initScriptWrappable(this);
    }

    virtual String* localName()
    {
        return document()->window()->starFish()->staticStrings()->m_divLocalName;
    }

    virtual bool isHTMLDivElement()
    {
        return true;
    }

protected:
};

}

#endif
