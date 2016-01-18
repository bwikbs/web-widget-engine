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
        setStyle(style);
    }

public:

    virtual bool isDocument()
    {
        return true;
    }

    virtual NodeType nodeType() {
        return DOCUMENT_NODE;
    }

    virtual String* nodeName()
    {
        return String::createASCIIString("#document");
    }

    Window* window()
    {
        return m_window;
    }

    ScriptBindingInstance* scriptBindingInstance()
    {
        return m_scriptBindingInstance;
    }

    virtual String* localName();
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
