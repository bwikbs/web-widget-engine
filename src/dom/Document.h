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
    /* 4.5. Interface Document */
    virtual NodeType nodeType()
    {
        return DOCUMENT_NODE;
    }

    virtual String* nodeName()
    {
        return String::createASCIIString("#document");
    }

    virtual String* localName();

    virtual String* nodeValue()
    {
        return nullptr;
    }

    virtual String* textContent()
    {
        return nullptr;
    }

    Element* documentElement()
    {
        if(firstChild() == nullptr) {
            return nullptr;
        } else {
            STARFISH_ASSERT(firstChild()->localName()->equals("html"));
            return firstChild()->asElement();
        }
    }

    /* Other methods */

    virtual bool isDocument()
    {
        return true;
    }

    virtual Element* parentElement()
    {
        return nullptr;
    }

    Window* window()
    {
        return m_window;
    }

    ScriptBindingInstance* scriptBindingInstance()
    {
        return m_scriptBindingInstance;
    }

    Element* getElementById(String* id);
    Element* createElement(String* localName);
    Text*    createTextNode(String* data);
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
