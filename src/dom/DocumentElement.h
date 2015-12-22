#ifndef __StarFishDocumentElement__
#define __StarFishDocumentElement__

#include "dom/Element.h"

namespace StarFish {

class Window;
class DocumentElement : public Element {
public:
    DocumentElement(Window* window)
        : Element(this)
    {
        m_window = window;
    }

    virtual bool isDocumentElement()
    {
        return true;
    }

    Window* window()
    {
        return m_window;
    }

protected:
    Window* m_window;
};

void Node::setNeedsRendering()
{
    m_documentElement->window()->setNeedsRendering();
}

}

#endif
