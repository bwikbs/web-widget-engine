#ifndef __StarFishNode__
#define __StarFishNode__

#include "util/String.h"
#include "util/Unit.h"
#include "style/Length.h"
#include "style/Drawable.h"
#include "dom/EventTarget.h"

namespace StarFish {

class Element;

class Node : public ScriptWrappable {
protected:
    Node(Document* documentElement, ScriptBindingInstance* instance)
    {
        m_documentElement = documentElement;
        initScriptWrappable(this, instance);
        m_nextSibling = nullptr;
        m_parentElement = nullptr;
        m_id = nullptr;
        m_state = NodeStateNormal;
    }

    Node(Document* documentElement)
    {
        m_documentElement = documentElement;
        initScriptWrappable(this);
        m_nextSibling = nullptr;
        m_parentElement = nullptr;
        m_id = nullptr;
        m_state = NodeStateNormal;
    }
public:
    enum NodeState {
        NodeStateNormal,
        NodeStateActive,
    };

    virtual ~Node()
    {

    }

    virtual bool isNode()
    {
        return true;
    }

    virtual bool isElement()
    {
        return false;
    }

    virtual bool isDocumentElement()
    {
        return false;
    }

    Node* nextSibling()
    {
        return m_nextSibling;
    }

    void setNextSibling(Node* s)
    {
        m_nextSibling = s;
    }

    Element* parentElement()
    {
        return m_parentElement;
    }

    void setParentElement(Element* s)
    {
        m_parentElement = s;
    }

    Element* asElement()
    {
        STARFISH_ASSERT(isElement());
        return (Element*)this;
    }

    DocumentElement* documentElement()
    {
        return m_documentElement;
    }

    void setId(String* id)
    {
        m_id = id;
        setNeedsRendering();
    }

    String* id()
    {
        return m_id;
    }

    void setState(NodeState state)
    {
        m_state = state;
        setNeedsRendering();
    }

    NodeState state()
    {
        return m_state;
    }

    virtual void computeStyle()
    {
    }

    virtual void computeLayout()
    {

    }

    inline void setNeedsRendering();

    virtual void paint(Canvas* canvas)
    {
    }

    virtual Node* hitTest(float x, float y)
    {
        return NULL;
    }

    virtual bool onTouchEvent(Window::TouchEventKind kind, float x, float y);

protected:
    Node* m_nextSibling;
    // Node* m_previousSibling;
    // Node* m_firstChild;
    // Node* m_lastChild;
    Node* m_parentNode;
    DocumentElement* m_document;
    String* m_id;
    NodeState m_state;
};

}

#endif
