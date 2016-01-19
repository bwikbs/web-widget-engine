#include "StarFishConfig.h"
#include "Node.h"

#include "Document.h"
#include "Element.h"
#include "Traverse.h"

namespace StarFish {

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

unsigned long Node::childElementCount()
{
    return Traverse::childCount(this, [](Node* child) {
        if (child->isElement())
            return true;
        else
            return false;
    });
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

String* Node::lookupPrefix(String* namespaceUri = nullptr)
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

}
