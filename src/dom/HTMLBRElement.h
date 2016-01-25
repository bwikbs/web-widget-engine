#ifndef __StarFishHTMLBRElement__
#define __StarFishHTMLBRElement__

#include "dom/HTMLElement.h"

namespace StarFish {

class HTMLBRElement : public HTMLElement {
public:
    HTMLBRElement(Document* document)
        : HTMLElement(document)
    {
        initScriptWrappable(this);
    }

    virtual String* localName()
    {
        return document()->window()->starFish()->staticStrings()->m_brLocalName;
    }

    virtual bool isHTMLBRElement() const
    {
        return true;
    }

protected:
};

}

#endif
