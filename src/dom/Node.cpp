#include "StarFishConfig.h"
#include "Node.h"

#include "Document.h"
#include "DocumentType.h"
#include "Element.h"
#include "Traverse.h"
#include "NodeList.h"

namespace StarFish {

NodeList* Node::childNodes()
{
    STARFISH_ASSERT(m_document);
    NodeList* list = new NodeList(m_document->scriptBindingInstance(), this, [&](Node* node) {
        return node->parentNode() == this? true: false;
    });
    return list;
}

Node* Node::cloneNode(bool deep)
{
    Node* newNode = clone();
    STARFISH_ASSERT(newNode);
    if(deep) {
        for(Node* child=firstChild(); child; child=child->nextSibling()) {
            Node* newChild = child->clone();
            STARFISH_ASSERT(newChild);
            newNode->appendChild(newChild);
        }
    }
    return newNode;
}

bool Node::isEqualNode(Node* other)
{
    if(other == nullptr) {
        return false;
    }
    if(this == other) {
        return true;
    }
    if(nodeType() != other->nodeType()) {
        return false;
    }

    switch(nodeType()) {
        case DOCUMENT_TYPE_NODE: {
            DocumentType* thisNode = asDocumentType();
            DocumentType* otherNode = other->asDocumentType();
            if(!(thisNode->nodeName()->equals(otherNode->nodeName()) &&
                 thisNode->publicId()->equals(otherNode->publicId()) &&
                 thisNode->systemId()->equals(otherNode->systemId()))) {
                return false;
            }
        }
        case ELEMENT_NODE: {
            Element* thisNode = asElement();
            Element* otherNode = other->asElement();
            if(!(thisNode->namespaceUri()->equals(otherNode->namespaceUri()) &&
                 thisNode->namespacePrefix()->equals(otherNode->namespacePrefix()) &&
                 thisNode->localName()->equals(otherNode->localName()) &&
                 true /* FIXME: impl 4.8.1 NamedNodeMap */  )) {
                 return false;
            }
        }
        case PROCESSING_INSTRUCTION_NODE:
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        case TEXT_NODE:
        case COMMENT_NODE:
            if(!nodeValue()->equals(other->nodeValue())) {
                return false;
            }
        default: {
            // for any other node, do nothing
        }
    }

    // FIXME: impl 4.8.1 NamedNodeMap to iterate attr
    STARFISH_RELEASE_ASSERT_NOT_REACHED();
    return true;
}

Element* Node::firstElementChild()
{
    Node* ret = Traverse::firstChild(this, [](Node* child) {
        if (child->isElement())
            return true;
        else
            return false;
    });
#ifndef NDEBUG
    // if debug mode, we can run-time type check
    if (ret)
        return ret->asElement();
    else
        return nullptr;
#else
    return ret->asElement();
#endif
}

Element* Node::lastElementChild()
{
    Node* ret = Traverse::lastChild(this, [](Node* child) {
        if (child->isElement())
            return true;
        else
            return false;
    });
#ifndef NDEBUG
    // if debug mode, we can run-time type check
    if (ret)
        return ret->asElement();
    else
        return nullptr;
#else
    return ret->asElement();
#endif
}

Element* Node::nextElementSibling()
{
    Node* ret = Traverse::nextSibling(this, [](Node* sibling) {
        if (sibling->isElement())
            return true;
        else
            return false;
    });
#ifndef NDEBUG
    // if debug mode, we can run-time type check
    if (ret)
        return ret->asElement();
    else
        return nullptr;
#else
    return ret->asElement();
#endif
}

Element* Node::previousElementSibling()
{
    Node* ret = Traverse::previousSibling(this, [](Node* sibling) {
        if (sibling->isElement())
            return true;
        else
            return false;
    });
#ifndef NDEBUG
    // if debug mode, we can run-time type check
    if (ret)
        return ret->asElement();
    else
        return nullptr;
#else
    return ret->asElement();
#endif
}

unsigned long Node::childElementCount()
{
    return Traverse::childCount(this, [](Node* child) {
        if (child->isElement())
            return true;
        else
            return false;
    });
}

bool isPreceding(const Node* node, const Node* isPrec, const Node* refNode)
{
    if(node == isPrec) {
        return true;
    } else if (node == refNode) {
        return false;
    }

    for(Node* child = node->firstChild(); child != nullptr; child = child->nextSibling()) {
        if(isPreceding(child, isPrec, refNode)) {
            return true;
        }
    }
    return false;
}

unsigned short Node::compareDocumentPosition(const Node* other)
{
    // spec does not say what to do when other is nullptr
    if(!other) {
        return DOCUMENT_POSITION_DISCONNECTED;
    }
    if(this == other) {
        return 0;
    }
    STARFISH_ASSERT(!isDocument());
    if(ownerDocument() != other->ownerDocument()) {
        return DOCUMENT_POSITION_DISCONNECTED +
               DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC +
               DOCUMENT_POSITION_PRECEDING;
    }

    for(Node* p = parentNode(); p != nullptr; p = p->parentNode()) {
        if(p == other) {
            return DOCUMENT_POSITION_CONTAINS +
                   DOCUMENT_POSITION_PRECEDING;
        }
    }

    for(Node* p = other->parentNode(); p != nullptr; p = p->parentNode()) {
        if(p == this) {
            return DOCUMENT_POSITION_CONTAINED_BY +
                   DOCUMENT_POSITION_FOLLOWING;
        }
    }

    Node* root = ownerDocument();
    if(isPreceding(root, other, this)) {
        return DOCUMENT_POSITION_PRECEDING;
    } else {
        return DOCUMENT_POSITION_FOLLOWING;
    }
}

String* Node::lookupNamespacePrefix(String* namespaceUri, Element* element)
{
    if(namespaceUri == nullptr) {
        return String::emptyString;
    }
    if(element->lookupNamespaceURI(prefix())->equals(namespaceUri)) {
        return prefix();
    }
    // Impl here
    return String::emptyString;
}

String* Node::lookupPrefix(String* namespaceUri)
{
    if(!namespaceUri) {
        return String::emptyString;
    }
    if(namespaceUri->equals(String::emptyString)) {
        return String::emptyString;
    }

    switch(nodeType()) {
        case ELEMENT_NODE:
            return lookupNamespacePrefix(namespaceUri, asElement());
        case DOCUMENT_NODE: {
            Element* documentElement = asDocument()->documentElement();
            if(documentElement) {
                return documentElement->lookupPrefix(namespaceUri);
            } else {
                return String::emptyString;
            }
        }
        case DOCUMENT_TYPE_NODE:
        case DOCUMENT_FRAGMENT_NODE:
            return String::emptyString;
        default: {
            Node* parent = parentNode();
            if(parent) {
                parent->lookupPrefix(namespaceUri);
            } else {
                return String::emptyString;
            }
        }
    }
    return String::emptyString;
}

HTMLCollection* Node::children()
{
    if (!m_rareNodeMembers) {
        m_rareNodeMembers = new RareNodeMembers();
    } else if (!m_rareNodeMembers->m_children)
        return m_rareNodeMembers->m_children;

    auto filter = [&](Node* node) {
        if (node->parentNode() == this && node->isElement())
            return true;
        return false;
    };
    m_rareNodeMembers->m_children = new HTMLCollection(m_document->scriptBindingInstance(), this, filter);
    return m_rareNodeMembers->m_children;
}

DOMTokenList* Node::classList()
{
    if (isElement()) {
        if (!m_rareNodeMembers) {
            m_rareNodeMembers = new RareNodeMembers();
        } else if (!m_rareNodeMembers->m_domTokenList)
            return m_rareNodeMembers->m_domTokenList;


        m_rareNodeMembers->m_domTokenList = new DOMTokenList(m_document->scriptBindingInstance(), asElement(),
                document()->window()->starFish()->staticStrings()->m_class);
        return m_rareNodeMembers->m_domTokenList;
    }
    return nullptr;
}

NamedNodeMap* Node::attributes()
{
    if (isElement()) {
        if (!m_rareNodeMembers) {
            m_rareNodeMembers = new RareNodeMembers();
        } else if (!m_rareNodeMembers->m_namedNodeMap)
            return m_rareNodeMembers->m_namedNodeMap;


        m_rareNodeMembers->m_namedNodeMap = new NamedNodeMap(m_document->scriptBindingInstance(), asElement());
        return m_rareNodeMembers->m_namedNodeMap;
    }
    return nullptr;
}

void Node::dumpStyle()
{
    dump();
    printf(", style: { ");

    // display
    if (m_style->display() == InlineDisplayValue) {
        printf("display: inline, ");
    } else if (m_style->display() == BlockDisplayValue) {
        printf("display: block, ");
    } else if (m_style->display() == InlineBlockDisplayValue) {
        printf("display: inline-block, ");
    } else if (m_style->display() == NoneDisplayValue) {
        printf("display: none, ");
    }

    // position
    if (m_style->position() == StaticPositionValue) {
        printf("position: static, ");
    } else if (m_style->position() == RelativePositionValue) {
        printf("position: relative, ");
    } else if (m_style->position() == AbsolutePositionValue) {
        printf("position: absolute, ");
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

    // direction
    if (m_style->direction() == DirectionValue::LtrDirectionValue)
        printf("direction: ltr, ");
    else
        printf("direction: rtl, ");

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
    if (m_style->bgSizeType() == BackgroundSizeType::Cover) {
        printf("background-size: cover, ");
    } else if (m_style->bgSizeType() == BackgroundSizeType::Contain) {
        printf("background-size: contain, ");
    } else if (m_style->bgSizeType() == BackgroundSizeType::SizeValue) {
        printf("background-size: (%s, %s),", m_style->bgSizeValue()->width().dumpString()->utf8Data(),
                                             m_style->bgSizeValue()->height().dumpString()->utf8Data());
    }

    // bottom
    if (m_style->bottom().isFixed()) {
        printf("bottom: %f, ", m_style->bottom().fixed());
    } else if (m_style->bottom().isPercent()) {
        printf("bottom: %f, ", m_style->bottom().percent());
    } else if (m_style->bottom().isAuto()) {
        printf("bottom: auto, ");
    }

    // left
    if (m_style->left().isFixed()) {
        printf("left: %f, ", m_style->left().fixed());
    } else if (m_style->left().isPercent()) {
        printf("left: %f, ", m_style->left().percent());
    } else if (m_style->left().isAuto()) {
        printf("left: auto, ");
    }

    // border-image-repeat
    if (m_style->borderImageRepeatX() == BorderImageRepeatValue::StretchValue) {
        printf("border-image-repeat: (stretch ");
    } else if (m_style->borderImageRepeatX() == BorderImageRepeatValue::RepeatValue) {
        printf("border-image-repeat: (repeat ");
    } else if (m_style->borderImageRepeatX() == BorderImageRepeatValue::RoundValue) {
        printf("border-image-repeat: (round ");
    } else if (m_style->borderImageRepeatX() == BorderImageRepeatValue::SpaceValue) {
        printf("border-image-repeat: (space ");
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }
    if (m_style->borderImageRepeatY() == BorderImageRepeatValue::StretchValue) {
        printf("stretch), ");
    } else if (m_style->borderImageRepeatY() == BorderImageRepeatValue::RepeatValue) {
        printf("repeat), ");
    } else if (m_style->borderImageRepeatY() == BorderImageRepeatValue::RoundValue) {
        printf("round), ");
    } else if (m_style->borderImageRepeatY() == BorderImageRepeatValue::SpaceValue) {
        printf("space), ");
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }

    // border-color
    printf("border-top-color: (%d,%d,%d,%d), ", m_style->borderTopColor().r(), m_style->borderTopColor().g(), m_style->borderTopColor().b(), m_style->borderTopColor().a());
    printf("border-right-color: (%d,%d,%d,%d), ", m_style->borderRightColor().r(), m_style->borderRightColor().g(), m_style->borderRightColor().b(), m_style->borderRightColor().a());
    printf("border-bottom-color: (%d,%d,%d,%d), ", m_style->borderBottomColor().r(), m_style->borderBottomColor().g(), m_style->borderBottomColor().b(), m_style->borderBottomColor().a());
    printf("border-left-color: (%d,%d,%d,%d), ", m_style->borderLeftColor().r(), m_style->borderLeftColor().g(), m_style->borderLeftColor().b(), m_style->borderLeftColor().a());

    // border-image-slice
    LengthBox l = m_style->borderImageSlices();
    if (l.top().isPercent()) printf("border-image-slice: (%.1fp, ", l.top().percent());
    else printf("border-image-slice: (%.1f, ", l.top().fixed());
    if (l.right().isPercent()) printf("%.1fp, ", l.right().percent());
    else printf("%.1f, ", l.right().fixed());
    if (l.bottom().isPercent()) printf("%.1fp, ", l.bottom().percent());
    else printf("%.1f, ", l.bottom().fixed());
    if (l.left().isPercent()) printf("%.1fp, ", l.left().percent());
    else printf("%.1f, ", l.left().fixed());
    printf("%d), ", m_style->borderImageSliceFill());

    // border-image-source
    if(!m_style->borderImageSource()->equals(String::emptyString))
        printf("border-image-source: %s, ", m_style->borderImageSource()->utf8Data());

    // border-style
    printf("border-style(t,r,b,l): (");
    if (m_style->borderTopStyle() == BorderStyleValue::BNone) {
        printf("none,");
    } else if (m_style->borderTopStyle() == BorderStyleValue::BSolid) {
        printf("solid,");
    }
    if (m_style->borderRightStyle() == BorderStyleValue::BNone) {
        printf("none,");
    } else if (m_style->borderRightStyle() == BorderStyleValue::BSolid) {
        printf("solid,");
    }
    if (m_style->borderBottomStyle() == BorderStyleValue::BNone) {
        printf("none,");
    } else if (m_style->borderBottomStyle() == BorderStyleValue::BSolid) {
        printf("solid,");
    }
    if (m_style->borderLeftStyle() == BorderStyleValue::BNone) {
        printf("none), ");
    } else if (m_style->borderLeftStyle() == BorderStyleValue::BSolid) {
        printf("solid), ");
    }

    // border-width
    printf("border-width(t,r,b,l): (%.0f,%.0f,%.0f,%.0f), ", m_style->borderTopWidth().fixed(), m_style->borderRightWidth().fixed(),
            m_style->borderBottomWidth().fixed(), m_style->borderLeftWidth().fixed());

    // background-repeat-x
    if(m_style->backgroundRepeatX() == BackgroundRepeatValue::RepeatRepeatValue)
        printf("background-repeat-x: repeat, ");
    else
        printf("background-repeat-x: no-repeat, ");

    // background-repeat-y
    if(m_style->backgroundRepeatY() == BackgroundRepeatValue::RepeatRepeatValue)
        printf("background-repeat-y: repeat, ");
    else
        printf("background-repeat-y: no-repeat, ");

    // margin-bottom
    if (m_style->marginBottom().isFixed()) {
        printf("margin-bottom: %f, ", m_style->marginBottom().fixed());
    } else if (m_style->marginBottom().isPercent()) {
        printf("margin-bottom: %f, ", m_style->marginBottom().percent());
    } else if (m_style->marginBottom().isAuto()) {
        printf("margin-bottom: auto, ");
    }

    // margin-left
    if (m_style->marginLeft().isFixed()) {
        printf("margin-left: %f, ", m_style->marginLeft().fixed());
    } else if (m_style->marginLeft().isPercent()) {
        printf("margin-left: %f, ", m_style->marginLeft().percent());
    } else if (m_style->marginLeft().isAuto()) {
        printf("margin-left: auto, ");
    }

    // margin-right
    if (m_style->marginRight().isFixed()) {
        printf("margin-right: %f, ", m_style->marginRight().fixed());
    } else if (m_style->marginRight().isPercent()) {
        printf("margin-right: %f, ", m_style->marginRight().percent());
    } else if (m_style->marginRight().isAuto()) {
        printf("margin-right: auto, ");
    }

    // padding-bottom
    if (m_style->paddingBottom().isFixed()) {
        printf("padding-bottom: %f, ", m_style->paddingBottom().fixed());
    } else if (m_style->paddingBottom().isPercent()) {
        printf("padding-bottom: %f, ", m_style->paddingBottom().percent());
    } else if (m_style->paddingBottom().isAuto()) {
        printf("padding-bottom: auto, ");
    } else {
        printf("padding-bottom: not computed yet,");
    }

    // opacity
    printf("opacity: %.1f, ", m_style->opacity());

    // overflow-x
    if(m_style->overflowX() == OverflowValue::Visible)
        printf("overflow-x: visible, ");
    else
        printf("overflow-x: hidden, ");

    // overflow-y
    if(m_style->overflowY() == OverflowValue::Visible)
        printf("overflow-y: visible, ");
    else
        printf("overflow-y: hidden, ");

    printf("}");
}

}
