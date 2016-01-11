#ifndef __StarFishHTMLHeadElement__
#define __StarFishHTMLHeadElement__

#include "dom/HTMLElement.h"

namespace StarFish {

class HTMLHeadElement : public HTMLElement {
public:
    HTMLHeadElement(Document* document)
        : HTMLElement(document)
    {
        initScriptWrappable(this);
    }

protected:
};

}

#endif
