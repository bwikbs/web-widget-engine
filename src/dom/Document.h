#ifndef __StarFishDocument__
#define __StarFishDocument__

#include "dom/Node.h"
#include "platform/window/Window.h"

namespace StarFish {

class Window;

class Document : public Node {
protected:
    Document(Window* window, ScriptBindingInstance* scriptBindingInstance, ComputedStyle* style)
        : Node(this, scriptBindingInstance)
    {
        m_window = window;
        m_scriptBindingInstance = scriptBindingInstance;
        setDocumentStyle(style);
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

    void setDocumentStyle(ComputedStyle* s);

    ComputedStyle* documentStyle()
    {
        return m_documentStyle;
    }

    virtual String* localName();
protected:
    Window* m_window;
    ScriptBindingInstance* m_scriptBindingInstance;
    ComputedStyle* m_documentStyle;
};

void Node::setNeedsRendering()
{
    m_document->window()->setNeedsRendering();
}

}

#endif
