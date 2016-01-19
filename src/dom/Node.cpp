#include "StarFishConfig.h"
#include "Node.h"

#include "Document.h"
#include "Element.h"
#include "Traverse.h"

namespace StarFish {
Element* Node::firstElementChild()
{
    return (Element*) Traverse::firstChild(this, [](Node* child) {
        if (child->isElement())
            return true;
        else
            return false;
    });
}

Element* Node::lastElementChild()
{
    return nullptr;
}

unsigned long Node::childElementCount()
{
    return 0;
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
}

}
