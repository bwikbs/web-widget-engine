
#ifndef __StarFishHTMLDocumentHTMLDocument__
#define __StarFishHTMLDocument__

#include "Document.h"

namespace StarFish {

class Window;

class HTMLDocument : public DocumentElement {
public:
    HTMLDocument(Window* window, ScriptBindingInstance* scriptBindingInstance)
        : DocumentElement(this, scriptBindingInstance)
    {
        initScriptWrappable(this);
    }

protected:
};

}

#endif
