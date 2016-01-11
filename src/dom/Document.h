#ifndef __StarFishDocument__
#define __StarFishDocument__

#include "dom/Node.h"
#include "platform/window/Window.h"

namespace StarFish {

class Window;

class Document : public Node {
protected:
    Document(Window* window, ScriptBindingInstance* scriptBindingInstance)
        : Node(this, scriptBindingInstance)
    {
        m_window = window;
        m_scriptBindingInstance = scriptBindingInstance;
    }
public:

    virtual bool isDocument()
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
    m_document->window()->setNeedsRendering();
}

}

#endif
