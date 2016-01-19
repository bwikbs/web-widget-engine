#include "StarFishConfig.h"
#include "HTMLCollection.h"
#include "dom/Document.h"
#include "dom/Element.h"

namespace StarFish {

unsigned long HTMLCollection::length() const
{
    unsigned long count = 0;
    Element* child = m_parent->firstElementChild();
    while (child) {
        count++;
        child = ((Node*) child)->nextElementSibling();
    }
    return count;
}

Element* HTMLCollection::item(unsigned long index)
{
    Element* child = m_parent->firstElementChild();
    unsigned long count = index;
    while (count > 0) {
        count--;
        child = ((Node*) child)->nextElementSibling();
        if (!child)
            return nullptr;
    }
    return child;
}

Element* HTMLCollection::namedItem(String* key)
{
    Element* child = m_parent->firstElementChild();
    while (child) {
        if (child->id()->equals(key))
            return child;
        child = ((Node*) child)->nextElementSibling();
    }
    return nullptr;
}

}
