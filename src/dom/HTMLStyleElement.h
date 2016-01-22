#ifndef __StarFishHTMLStyleElement__
#define __StarFishHTMLStyleElement__

#include "dom/HTMLElement.h"

namespace StarFish {

class HTMLStyleElement : public HTMLElement {
public:
    HTMLStyleElement(Document* document)
        : HTMLElement(document)
    {
        initScriptWrappable(this);
    }

    virtual bool isHTMLStyleElement() const
    {
        return true;
    }

    virtual String* localName()
    {
        return document()->window()->starFish()->staticStrings()->m_styleLocalName;
    }

protected:
};

}

#endif
