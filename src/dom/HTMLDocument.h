
#ifndef __StarFishHTMLDocument__
#define __StarFishHTMLDocument__

#include "Document.h"

namespace StarFish {

class Window;

class HTMLDocument : public Document {
public:
    HTMLDocument(Window* window, ScriptBindingInstance* scriptBindingInstance, ComputedStyle* style)
        : Document(window, scriptBindingInstance, style)
    {
        initScriptWrappable(this);
    }

protected:
};

}

#endif
