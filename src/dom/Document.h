#ifndef __StarFishDocument__
#define __StarFishDocument__

#include "dom/Node.h"
#include "platform/window/Window.h"
#include "loader/ResourceLoader.h"

namespace StarFish {

class Window;
class Attribute;
class NetworkRequest;
class DocumentBuilder;

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
    friend class DOMImplementation;
#endif
    friend class ActiveNetworkRequestTracker;
protected:
    Document(Window* window, ScriptBindingInstance* scriptBindingInstance, const URL& url);
public:
    enum CompatibilityMode { QuirksMode, LimitedQuirksMode, NoQuirksMode };
    void setCompatibilityMode(CompatibilityMode m) { m_compatibilityMode = m; }
    CompatibilityMode compatibilityMode() const { return m_compatibilityMode; }
    bool inQuirksMode() const { return m_compatibilityMode == QuirksMode; }
    bool inLimitedQuirksMode() const { return m_compatibilityMode == LimitedQuirksMode; }
    bool inNoQuirksMode() const { return m_compatibilityMode == NoQuirksMode; }

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

    DocumentFragment* createDocumentFragment();
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

    ResourceLoader* resourceLoader()
    {
        return &m_resourceLoader;
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

    bool inParsing()
    {
        return m_inParsing;
    }

    void setInParsing(bool b)
    {
        m_inParsing = b;
    }

    const URL& documentURI()
    {
        return m_documentURI;
    }

    void open();
    // method for script element
    void resumeDocumentParsing();
    void notifyDomContentLoaded();
    void close();

    DocumentBuilder* documentBuilder()
    {
        return m_documentBuilder;
    }

    virtual void didNodeInserted(Node* parent, Node* newChild);
    virtual void didNodeRemoved(Node* parent, Node* oldChild);

    HTMLCollection* namedAccess(String* name);
    void invalidNamedAccessCacheIfNeeded();

    std::vector<Element*, gc_allocator<Element*>>& elementExecutionStackForAttributeStringEventFunctionObject()
    {
        return m_elementExecutionStackForAttributeStringEventFunctionObject;
    }

protected:
    bool m_inParsing : 1;
    CompatibilityMode m_compatibilityMode;
    URL m_documentURI;
    ResourceLoader m_resourceLoader;
    StyleResolver m_styleResolver;
    DocumentBuilder* m_documentBuilder;
    Window* m_window;
    ScriptBindingInstance* m_scriptBindingInstance;
    PageVisibilityState m_pageVisibilityState;
    size_t m_domVersion;
    std::vector<NetworkRequest*, gc_allocator<NetworkRequest*>> m_activeNetworkRequests;
    ActiveHTMLCollectionList m_namedAccessActiveHTMLCollectionList;
    std::vector<Element*, gc_allocator<Element*>> m_elementExecutionStackForAttributeStringEventFunctionObject;
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
