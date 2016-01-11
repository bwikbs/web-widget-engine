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

protected:
};

}

#endif
