#ifndef __StarFishDocument__
#define __StarFishDocument__

#include "dom/Node.h"
#include "platform/window/Window.h"

namespace StarFish {

class Window;
class Attribute;

class Document : public Node {
protected:
    Document(Window* window, ScriptBindingInstance* scriptBindingInstance, ComputedStyle* style);
public:
    /* 4.2.2. Interface NonElementParentNode */
    Element* getElementById(String* id);

    /* 4.5. Interface Document */
    Element* createElement(QualifiedName localName);
    Text*    createTextNode(String* data);
    Comment* createComment(String* data);
    HTMLCollection* getElementsByTagName(String* qualifiedName);
    HTMLCollection* getElementsByClassName(String* classNames);
    Attr* createAttribute(QualifiedName localName);

    /* Other methods */
    virtual NodeType nodeType()
    {
        return DOCUMENT_NODE;
    }

    virtual String* nodeName();

    virtual String* localName();

    virtual String* nodeValue()
    {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
        return nullptr;
    }

    virtual String* textContent()
    {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
        return nullptr;
    }

    Element* documentElement()
    {
        if(firstChild() == nullptr) {
            return nullptr;
        } else {
            STARFISH_ASSERT(firstChild()->localName()->equals(window()->starFish()->staticStrings()->m_htmlLocalName));
            return firstChild()->asElement();
        }
    }

    virtual bool isDocument() const
    {
        return true;
    }

    virtual Element* parentElement()
    {
        return nullptr;
    }

    virtual Node* clone();

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
