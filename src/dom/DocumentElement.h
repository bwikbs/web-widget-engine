#ifndef __StarFishDocumentElement__
#define __StarFishDocumentElement__

#include "dom/Element.h"

namespace StarFish {

class Window;

class DocumentElement : public Element {
public:
    DocumentElement(Window* window, ScriptBindingInstance* scriptBindingInstance)
        : Element(this, scriptBindingInstance)
    {
        m_window = window;
        m_scriptBindingInstance = scriptBindingInstance;
    }

    virtual bool isDocumentElement()
    {
        return true;
    }

    Window* window()
    {
        return m_window;
    }

    ScriptBindingInstance* scriptBindingInstance()
    {
        return m_scriptBindingInstance;
    }

protected:
    Window* m_window;
    ScriptBindingInstance* m_scriptBindingInstance;
};

void Node::setNeedsRendering()
{
    m_documentElement->window()->setNeedsRendering();
}

}

#endif
