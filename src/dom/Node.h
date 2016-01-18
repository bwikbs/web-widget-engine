#ifndef __StarFishNode__
#define __StarFishNode__

#include "util/String.h"
#include "style/Unit.h"
#include "style/Length.h"
#include "style/ComputedStyle.h"
#include "platform/canvas/Canvas.h"
#include "platform/canvas/image/ImageData.h"
#include "dom/EventTarget.h"

namespace StarFish {

class CharacterData;
class Element;
class Frame;

class Node : public EventTarget<ScriptWrappable> {
protected:
    Node(Document* document, ScriptBindingInstance* instance)
    {
        m_document = document;
        initScriptWrappable(this, instance);
        m_nextSibling = nullptr;
        m_parentNode = nullptr;
        m_firstChild = nullptr;
        m_state = NodeStateNormal;
        m_needsStyleRecalc = true;
        m_style = nullptr;
        m_frame = nullptr;
    }

    Node(Document* document)
    {
        m_document = document;
        initScriptWrappable(this);
        m_nextSibling = nullptr;
        m_parentNode = nullptr;
        m_firstChild = nullptr;
        m_state = NodeStateNormal;
        m_needsStyleRecalc = true;
        m_style = nullptr;
        m_frame = nullptr;
    }
public:
    enum NodeState {
        NodeStateNormal,
        NodeStateActive,
    };

    enum NodeType {
        ELEMENT_NODE = 1,
        ATTRIBUTE_NODE,        // historical
        TEXT_NODE,
        CDATA_SECTION_NODE,    // historical
        ENTITY_REFERENCE_NODE, // historical
        ENTITY_NODE,           // historical
        PROCESSING_INSTRUCTION_NODE,
        COMMENT_NODE,
        DOCUMENT_NODE,
        DOCUMENT_TYPE_NODE,
        DOCUMENT_FRAGMENT_NODE,
        NOTATION_NODE,         // historical
    };

    virtual ~Node()
    {

    }

    virtual NodeType nodeType() = 0;

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
        STARFISH_ASSERT(isCharacterData());
        return (CharacterData*)this;
    }

    Element* asElement()
    {
        STARFISH_ASSERT(isElement());
        return (Element*)this;
    }

    Document* asDocument()
    {
        STARFISH_ASSERT(isDocument());
        return (Document*)this;
    }

    virtual String* localName()
    {
        return String::emptyString;
    }

    bool hasChildNodes() {
        return firstChild()? true: false;
    }

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

    Document* document()
    {
        return m_document;
    }

    void setState(NodeState state)
    {
        m_state = state;
        setNeedsStyleRecalc();
    }

    NodeState state()
    {
        return m_state;
    }

    inline void setNeedsStyleRecalc()
    {
        m_needsStyleRecalc = true;
        setNeedsRendering();
    }

    void setStyle(ComputedStyle* style)
    {
        m_style = style;
    }

    ComputedStyle* style()
    {
        return m_style;
    }

    void setFrame(Frame* frame)
    {
        m_frame = frame;
    }

    Frame* frame()
    {
        return m_frame;
    }

private:
    inline void setNeedsRendering();

protected:
    bool m_needsStyleRecalc;

    Node* m_nextSibling;
    // Node* m_previousSibling;
    Node* m_firstChild;
    // Node* m_lastChild;
    Node* m_parentNode;
    Document* m_document;
    NodeState m_state;

    ComputedStyle* m_style;
    Frame* m_frame;
};

}

#endif
