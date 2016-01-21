#ifndef __StarFishNode__
#define __StarFishNode__

#include "util/String.h"
#include "style/Unit.h"
#include "style/Length.h"
#include "style/ComputedStyle.h"
#include "platform/canvas/Canvas.h"
#include "platform/canvas/image/ImageData.h"
#include "dom/EventTarget.h"
#include "dom/HTMLCollection.h"

namespace StarFish {

class CharacterData;
class Element;
class Frame;
class NodeList;

class RareNodeMembers : public gc {
public:
    RareNodeMembers():m_children(nullptr) {}
    HTMLCollection* m_children;
};

class Node : public EventTarget<ScriptWrappable> {
protected:
    Node(Document* document, ScriptBindingInstance* instance)
    {
        m_document = document;
        initScriptWrappable(this, instance);
        m_nextSibling = nullptr;
        m_previousSibling = nullptr;
        m_parentNode = nullptr;
        m_firstChild = nullptr;
        m_lastChild = nullptr;
        m_state = NodeStateNormal;
        m_needsStyleRecalc = true;
        m_style = nullptr;
        m_frame = nullptr;
        m_baseUri = String::emptyString; // need to set by the parser
        m_rareNodeMembers = nullptr;
    }

    Node(Document* document)
    {
        m_document = document;
        initScriptWrappable(this);
        m_nextSibling = nullptr;
        m_previousSibling = nullptr;
        m_parentNode = nullptr;
        m_firstChild = nullptr;
        m_lastChild = nullptr;
        m_state = NodeStateNormal;
        m_needsStyleRecalc = true;
        m_style = nullptr;
        m_frame = nullptr;
        m_baseUri = String::emptyString; // need to set by the parser
        m_rareNodeMembers = nullptr;
    }
public:
    virtual ~Node()
    {

    }

    /* 4.4 Interface Node */

    enum NodeType {
        ELEMENT_NODE = 1,
        ATTRIBUTE_NODE = 2,        // historical
        TEXT_NODE = 3,
        CDATA_SECTION_NODE = 4,    // historical
        ENTITY_REFERENCE_NODE = 5, // historical
        ENTITY_NODE = 6,           // historical
        PROCESSING_INSTRUCTION_NODE = 7,
        COMMENT_NODE = 8,
        DOCUMENT_NODE = 9,
        DOCUMENT_TYPE_NODE = 10,
        DOCUMENT_FRAGMENT_NODE = 11,
        NOTATION_NODE = 12,         // historical
    };

    enum DocumentPosition {
        DOCUMENT_POSITION_DISCONNECTED = 0x01,
        DOCUMENT_POSITION_PRECEDING = 0x02,
        DOCUMENT_POSITION_FOLLOWING = 0x04,
        DOCUMENT_POSITION_CONTAINS = 0x08,
        DOCUMENT_POSITION_CONTAINED_BY = 0x10,
        DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC = 0x20,
    };

    virtual NodeType nodeType() = 0;

    virtual String* nodeName() = 0;

    String* baseURI()
    {
        return m_baseUri;
    }

    virtual Document* ownerDocument()
    {
        if(isDocument()) {
            return nullptr;
        } else {
            return m_document;
        }
    }

    Node* parentNode()
    {
        return m_parentNode;
    }

    virtual Element* parentElement() = 0;

    virtual String* localName()
    {
        return nullptr;
    }

    bool hasChildNodes()
    {
        return firstChild();
    }

    virtual NodeList* childNodes();

    Node* firstChild()
    {
        return m_firstChild;
    }

    Node* lastChild()
    {
        return m_lastChild;
    }

    Node* previousSibling()
    {
        return m_previousSibling;
    }

    Node* nextSibling()
    {
        return m_nextSibling;
    }

    virtual String* nodeValue() = 0;

    virtual String* textContent() = 0;

    virtual bool isEqualNode(Node* other);

    virtual Node* cloneNode(bool deep);

    virtual unsigned short compareDocumentPosition(Node* other);

    virtual bool contains(Node* other)
    {
        if(other == nullptr) {
            return false;
        }
        if(this == other) {
            return true;
        }
        for(Node* child = firstChild(); child != nullptr; child = child->nextSibling()) {
            if(child->contains(other)) {
                return true;
            }
        }
        return false;
    }

    virtual String* lookupPrefix(String* namespaceUri);

    virtual String* lookupNamespaceURI(String* prefix)
    {
        if(prefix == nullptr) {
            return nullptr;
        }
        if(prefix->equals(String::emptyString)) {
            return nullptr;
        }
        // Impl here
        return nullptr;
    }
    virtual bool isDefaultNamespace(String* namespace_)
    {
        // Impl here
        return false;
    }

    Node* appendChild(Node* child)
    {
        STARFISH_ASSERT(child);
        STARFISH_ASSERT(child->parentNode() == nullptr);
        STARFISH_ASSERT(child->nextSibling() == nullptr);
        STARFISH_ASSERT(child->previousSibling() == nullptr);

        if(m_lastChild) {
            child->setPreviousSibling(m_lastChild);
            m_lastChild->setNextSibling(child);
        } else {
            m_firstChild = child;
        }
        m_lastChild = child;
        child->setParentNode(this);
        setNeedsStyleRecalc();
        return child;
    }

    Node* insertBefore(Node* child, Node* childRef = nullptr)
    {
        STARFISH_ASSERT(child);

        if(!childRef) {
            appendChild(child);
        }

        STARFISH_ASSERT(childRef->parentNode() == this);
        STARFISH_ASSERT(child->parentNode() == nullptr);
        STARFISH_ASSERT(child->nextSibling() == nullptr);
        STARFISH_ASSERT(child->previousSibling() == nullptr);

        Node* prev = childRef->previousSibling();
        childRef->setPreviousSibling(child);
        STARFISH_ASSERT(m_lastChild != prev);
        if(prev) {
            STARFISH_ASSERT(m_firstChild != childRef);
            prev->setNextSibling(child);
        } else {
            STARFISH_ASSERT(m_firstChild == childRef);
            m_firstChild = child;
        }

        child->setParentNode(this);
        child->setPreviousSibling(prev);
        child->setNextSibling(childRef);
        setNeedsStyleRecalc();
        return child;
    }

    Node* removeChild(Node* child)
    {
        STARFISH_ASSERT(child);
        STARFISH_ASSERT(child->parentNode() == this);

        Node* prevChild = child->previousSibling();
        Node* nextChild = child->nextSibling();

        if(nextChild) {
            nextChild->setPreviousSibling(prevChild);
        }
        if(prevChild) {
            prevChild->setNextSibling(nextChild);
        }
        if(m_firstChild == child) {
            m_firstChild = nextChild;
        }
        if(m_lastChild == child) {
            m_lastChild = prevChild;
        }

        child->setPreviousSibling(nullptr);
        child->setNextSibling(nullptr);
        child->setParentNode(nullptr);
        setNeedsStyleRecalc();
        return child;
    }

    void remove() {
        if (m_parentNode)
            m_parentNode->removeChild(this);
    }

    Node* replaceChild(Node* child, Node* childToRemove)
    {
        STARFISH_ASSERT(child);
        STARFISH_ASSERT(childToRemove);
        STARFISH_ASSERT(childToRemove->parentNode() == this);

        setNeedsStyleRecalc();
        insertBefore(child, childToRemove);
        Node* n = removeChild(childToRemove);
        setNeedsStyleRecalc();
        return n;
    }

    template <typename T>
    Node* childMatchedBy(Node* parent, T fn)
    {
        Node* child = parent->firstChild();
        while (child) {
            if (fn(child)) {
                return child;
            }
            Node* matchedDescendant = childMatchedBy(child, fn);
            if (matchedDescendant)
                return matchedDescendant;
            child = child->nextSibling();
        }
        return nullptr;
    }

    /* Other methods (not in Node Interface) */
    enum NodeState {
        NodeStateNormal,
        NodeStateActive,
    };

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

    virtual bool isDocumentType()
    {
        return false;
    }

    virtual bool isComment()
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

    DocumentType* asDocumentType()
    {
        STARFISH_ASSERT(isDocumentType());
        return (DocumentType*)this;
    }

    void setFirstChild(Node* s)
    {
        m_firstChild = s;
    }

    void setNextSibling(Node* s)
    {
        m_nextSibling = s;
    }

    void setPreviousSibling(Node* s)
    {
        m_previousSibling = s;
    }

    void setParentNode(Node* s)
    {
        m_parentNode = s;
    }

    Document* document()
    {
        return m_document;
    }

    virtual Node* clone() = 0;

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

    virtual void setStyle(ComputedStyle* style)
    {
        m_style = style;
    }

    virtual ComputedStyle* style()
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

    virtual void dump()
    {
        printf("[%s] ", localName()->utf8Data());
    }

    virtual void dumpStyle()
    {
        dump();
        printf(", style: { ");

        // display
        if (m_style->display() == InlineDisplayValue) {
            printf("display: inline, ");
        } else if (m_style->display() == BlockDisplayValue) {
            printf("display: block, ");
        } else if (m_style->display() == NoneDisplayValue) {
            printf("display: none, ");
        }

        // width
        if (m_style->width().isFixed()) {
            printf("width: %f, ", m_style->width().fixed());
        } else if (m_style->width().isPercent()) {
            printf("width: %f, ", m_style->width().percent());
        } else if (m_style->width().isAuto()) {
            printf("width: auto, ");
        }

        // height
        if (m_style->height().isFixed()) {
            printf("height: %f, ", m_style->height().fixed());
        } else if (m_style->height().isPercent()) {
            printf("height: %f, ", m_style->height().percent());
        } else if (m_style->height().isAuto()) {
            printf("height: auto, ");
        }

        // font-size
        printf("font-size: %.0f, ", m_style->font()->size());

        // color
        printf("color: (%d,%d,%d,%d), ", m_style->color().r(), m_style->color().g(), m_style->color().b(), m_style->color().a());

        // background-color
        printf("background-color: (%d,%d,%d,%d), ", m_style->bgColor().r(), m_style->bgColor().g(), m_style->bgColor().b(), m_style->bgColor().a());

        // background-image
        if (m_style->bgImage()->length() == 0) {
            printf("background-image: none, ");
        } else {
            printf("background-image: %s, ", m_style->bgImage()->utf8Data());
        }

        // background-size
        if (m_style->background()->sizeType() == BackgroundSizeType::Cover) {
            printf("background-size: cover, ");
        } else if (m_style->background()->sizeType() == BackgroundSizeType::Contain) {
            printf("background-size: contain, ");
        } else if (m_style->background()->sizeType() == BackgroundSizeType::SizeValue) {
            printf("background-size: (%s, %s),", m_style->background()->sizeValue()->width().dumpString()->utf8Data(),
                                                m_style->background()->sizeValue()->height().dumpString()->utf8Data());
        }

        // bottom
        if (m_style->bottom().isFixed()) {
            printf("bottom: %f, ", m_style->bottom().fixed());
        } else if (m_style->bottom().isPercent()) {
            printf("bottom: %f, ", m_style->bottom().percent());
        } else if (m_style->bottom().isAuto()) {
            printf("bottom: auto, ");
        }

        // border-image-repeat
        if (m_style->borderImageRepeat()->m_XAxis == BorderImageRepeatValue::StretchValue) {
            printf("border-image-repeat: {stretch ");
        } else if (m_style->borderImageRepeat()->m_XAxis == BorderImageRepeatValue::RepeatValue) {
            printf("border-image-repeat: {repeat ");
        } else if (m_style->borderImageRepeat()->m_XAxis == BorderImageRepeatValue::RoundValue) {
            printf("border-image-repeat: {round ");
        } else if (m_style->borderImageRepeat()->m_XAxis == BorderImageRepeatValue::SpaceValue) {
            printf("border-image-repeat: {space ");
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
        if (m_style->borderImageRepeat()->m_YAxis == BorderImageRepeatValue::StretchValue) {
            printf("stretch}, ");
        } else if (m_style->borderImageRepeat()->m_YAxis == BorderImageRepeatValue::RepeatValue) {
            printf("repeat}, ");
        } else if (m_style->borderImageRepeat()->m_YAxis == BorderImageRepeatValue::RoundValue) {
            printf("round}, ");
        } else if (m_style->borderImageRepeat()->m_YAxis == BorderImageRepeatValue::SpaceValue) {
            printf("space}, ");
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }

        // border-image-source
        if(m_style->borderImageSource() != 0)
            printf("border-image-source: %s, ", m_style->borderImageSource()->utf8Data());

        // background-repeat-x
        printf("background-repeat-x: %d, ", m_style->backgroundRepeatX());
        // background-repeat-y
        printf("background-repeat-y: %d, ", m_style->backgroundRepeatY());

        // margin-bottom
        if (m_style->marginBottom().isFixed()) {
            printf("margin-bottom: %f, ", m_style->marginBottom().fixed());
        } else if (m_style->marginBottom().isPercent()) {
            printf("margin-bottom: %f, ", m_style->marginBottom().percent());
        } else if (m_style->marginBottom().isAuto()) {
            printf("margin-bottom: auto, ");
        }

        // opacity
        printf("opacity: %f, ", m_style->opacity());

        printf("}");
    }

    Element* firstElementChild();
    Element* lastElementChild();
    unsigned long childElementCount();

    HTMLCollection* children();

    Element* nextElementSibling();
    Element* previousElementSibling();

private:
    inline void setNeedsRendering();

    String* lookupNamespacePrefix(String* namespaceUri, Element* element);
    virtual String* prefix()
    {
        // For nodes other than elements and attributes, the prefix is always null
        return nullptr;
    }

protected:
    bool m_needsStyleRecalc;

    Node* m_nextSibling;
    Node* m_previousSibling;
    Node* m_firstChild;
    Node* m_lastChild;
    Node* m_parentNode;
    Document* m_document;
    NodeState m_state;

    String* m_baseUri;

    ComputedStyle* m_style;
    Frame* m_frame;
    RareNodeMembers* m_rareNodeMembers;
};

}

#endif
