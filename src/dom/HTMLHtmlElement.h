#ifndef __StarFishHTMLHtmlElement__
#define __StarFishHTMLHtmlElement__

#include "dom/HTMLElement.h"

namespace StarFish {

class HTMLHtmlElement : public HTMLElement {
public:
    HTMLHtmlElement(Document* document)
        : HTMLElement(document)
    {
        initScriptWrappable(this);
    }

    /* 4.4 Interface Node */

    virtual String* localName()
    {
        return document()->window()->starFish()->staticStrings()->m_htmlLocalName;
    }

    /* Other methods (not in DOM API) */

    virtual bool isHTMLHtmlElement() const
    {
        return true;
    }

protected:
};

}

#endif
