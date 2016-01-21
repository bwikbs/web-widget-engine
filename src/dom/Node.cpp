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

Node* Node::cloneNode(bool deep = false)
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

bool isPreceding(Node* node, Node* isPrec, Node* refNode)
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

unsigned short Node::compareDocumentPosition(Node* other)
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
        return nullptr;
    }
    if(element->lookupNamespaceURI(prefix())->equals(namespaceUri)) {
        return prefix();
    }
    // Impl here
    return nullptr;
}

String* Node::lookupPrefix(String* namespaceUri)
{
    if(!namespaceUri) {
        return nullptr;
    }
    if(namespaceUri->equals(String::emptyString)) {
        return nullptr;
    }

    switch(nodeType()) {
        case ELEMENT_NODE:
            return lookupNamespacePrefix(namespaceUri, asElement());
        case DOCUMENT_NODE: {
            Element* documentElement = asDocument()->documentElement();
            if(documentElement) {
                return documentElement->lookupPrefix(namespaceUri);
            } else {
                return nullptr;
            }
        }
        case DOCUMENT_TYPE_NODE:
        case DOCUMENT_FRAGMENT_NODE:
            return nullptr;
        default: {
            Node* parent = parentNode();
            if(parent) {
                parent->lookupPrefix(namespaceUri);
            } else {
                return nullptr;
            }
        }
    }
    return nullptr;
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

}
