#ifndef __StarFishHTMLElement__
#define __StarFishHTMLElement__

#include "dom/Element.h"

namespace StarFish {

class HTMLElement : public Element {
public:
    HTMLElement(Document* document)
        : Element(document)
    {
        initScriptWrappable(this);
        m_id = String::emptyString;
        m_className = String::emptyString;
    }

protected:
    String* m_id;
    String* m_className;
};

}

#endif
