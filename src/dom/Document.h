#ifndef __StarFishDocument__
#define __StarFishDocument__

#include "dom/Node.h"
#include "platform/window/Window.h"

namespace StarFish {

class Window;
class Attribute;
#ifdef STARFISH_EXP
class DOMImplementation;
#endif

/* Page Visibility */
enum PageVisibilityState {
    PageVisibilityStateHidden,
    PageVisibilityStateVisible,
    PageVisibilityStatePrerender,
    PageVisibilityStateUnloaded
};

class Document : public Node {
#ifdef STARFISH_EXP
friend DOMImplementation;
#endif
protected:
    Document(Window* window, ScriptBindingInstance* scriptBindingInstance);
public:
    /* 4.2.2. Interface NonElementParentNode */
    Element* getElementById(String* id);

    /* 4.5. Interface Document */
    DocumentType* docType()
    {
        Node* node = getDocTypeChild();
        if (node != nullptr) {
            return node->asDocumentType();
        }
        return nullptr;
    }

    Element* createElement(QualifiedName localName);
    Text* createTextNode(String* data);
    Comment* createComment(String* data);
    // Moved to Node as it is common to Document and Element
    // HTMLCollection* getElementsByTagName(String* qualifiedName);
    // HTMLCollection* getElementsByClassName(String* classNames);
    Attr* createAttribute(QualifiedName localName);

#ifdef STARFISH_EXP
    DOMImplementation* domImplementation()
    {
        return m_domImplementation;
    }
#endif

    /* Other methods */
    virtual NodeType nodeType()
    {
        return DOCUMENT_NODE;
    }

    virtual String* nodeName();
    virtual String* localName();
    virtual String* textContent()
    {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
        return nullptr;
    }
    virtual void didNodeInserted(Node* parent, Node* newChild)
    {
        Node::didNodeInserted(parent, newChild);
        updateDOMVersion();
    }
    virtual void didNodeRemoved(Node* parent, Node* oldChild)
    {
        Node::didNodeRemoved(parent, oldChild);
        updateDOMVersion();
    }

    Element* documentElement();

    virtual bool isDocument() const
    {
        return true;
    }

    virtual Node* clone();

    Window* window()
    {
        return m_window;
    }

    StyleResolver* styleResolver()
    {
        return &m_styleResolver;
    }

    ScriptBindingInstance* scriptBindingInstance()
    {
        return m_scriptBindingInstance;
    }

    HTMLHtmlElement* rootElement();
    HTMLHeadElement* headElement();
    HTMLBodyElement* bodyElement();

    /* Page Visibility */
    bool hidden() const;
    String* visibilityState();
    void setVisibleState(PageVisibilityState visibilityState);
    void visibilityStateChanged();

    void updateDOMVersion()
    {
        m_domVersion++;
    }

protected:
    StyleResolver m_styleResolver;
    Window* m_window;
    ScriptBindingInstance* m_scriptBindingInstance;
    PageVisibilityState m_pageVisibilityState;
    size_t m_domVersion;

#ifdef STARFISH_EXP
private:
    DOMImplementation* m_domImplementation;
#endif
};

void Node::setNeedsStyleRecalc()
{
    if (!m_needsStyleRecalc) {
        m_needsStyleRecalc = true;

        Node* node = parentNode();
        while (node && !node->childNeedsStyleRecalc()) {
            node->setChildNeedsStyleRecalc();
            node = node->parentNode();
        }
    }
    m_document->window()->setNeedsStyleRecalc();
}

void Node::setNeedsLayout()
{
    m_document->window()->setNeedsLayout();
}

void Node::setNeedsPainting()
{
    m_document->window()->setNeedsPainting();
}

void Node::setNeedsComposite()
{
    m_document->window()->setNeedsComposite();
}


}

#endif
