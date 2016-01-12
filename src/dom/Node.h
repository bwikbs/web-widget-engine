#ifndef __StarFishNode__
#define __StarFishNode__

#include "util/String.h"
#include "util/Unit.h"
#include "style/Length.h"
#include "style/Drawable.h"
#include "platform/canvas/Canvas.h"
#include "dom/EventTarget.h"

namespace StarFish {

class Element;
class CharacterData;

class Node : public EventTarget<ScriptWrappable> {
protected:
    Node(Document* document, ScriptBindingInstance* instance)
    {
        m_document = document;
        initScriptWrappable(this, instance);
        m_nextSibling = nullptr;
        m_parentNode = nullptr;
        m_firstChild = nullptr;
        // m_id = nullptr;
        m_state = NodeStateNormal;
    }

    Node(Document* document)
    {
        m_document = document;
        initScriptWrappable(this);
        m_nextSibling = nullptr;
        m_parentNode = nullptr;
        m_firstChild = nullptr;
        // m_id = nullptr;
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

    virtual bool isDocument()
    {
        return false;
    }

    virtual bool isCharacterData()
    {
        return false;
    }

    CharacterData* asCharacterData()
    {
        return (CharacterData*)this;
    }

    /*Element* getElementById(String* id)
    {
        if (!id)
            return nullptr;

        Node* node = m_firstChild;
        while (node) {
            if (node->isElement()) {
                if (node->asElement()->id() && node->asElement()->id()->equals(id)) {
                    return node->asElement();
                }
                Element* ret = node->asElement()->getElementById(id);
                if (ret)
                    return ret;
            }
            node = node->nextSibling();
        }

        return nullptr;
    }*/

    Node* firstChild()
    {
        return m_firstChild;
    }

    void setFirstChild(Node* s)
    {
        m_firstChild = s;
    }

    void appendChild(Node* child)
    {
        STARFISH_ASSERT(child->parentNode() == nullptr);
        if (m_firstChild) {
            Node* node = m_firstChild;
            while (node->nextSibling() != nullptr) {
                node = node->nextSibling();
            }
            STARFISH_ASSERT(node->nextSibling() == nullptr);
            node->setNextSibling(child);
        } else {
            m_firstChild = child;
        }
        child->setParentNode(this);
        setNeedsRendering();
    }

    void removeChild(Node* child)
    {
        STARFISH_ASSERT(child);
        STARFISH_ASSERT(child->parentNode() == this);
        Node* prevNode = nullptr;
        Node* node = m_firstChild;
        while (node != child) {
            prevNode = node;
            node = node->nextSibling();
        }

        STARFISH_ASSERT(node == child);
        node->setParentNode(nullptr);
        prevNode->setNextSibling(node->nextSibling());
        node->setNextSibling(nullptr);
        setNeedsRendering();
    }


    Node* nextSibling()
    {
        return m_nextSibling;
    }

    void setNextSibling(Node* s)
    {
        m_nextSibling = s;
    }

    Node* parentNode()
    {
        return m_parentNode;
    }

    void setParentNode(Node* s)
    {
        m_parentNode = s;
    }

    Element* asElement()
    {
        STARFISH_ASSERT(isElement());
        return (Element*)this;
    }

    Document* document()
    {
        return m_document;
    }
/*
    void setId(String* id)
    {
        m_id = id;
        setNeedsRendering();
    }

    String* id()
    {
        return m_id;
    }
*/
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

protected:
    Node* m_nextSibling;
    // Node* m_previousSibling;
    Node* m_firstChild;
    // Node* m_lastChild;
    Node* m_parentNode;
    Document* m_document;
    // String* m_id; -> htmlelement
    NodeState m_state;
};

}

#endif
