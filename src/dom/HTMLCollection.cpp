#include "StarFishConfig.h"
#include "HTMLCollection.h"
#include "dom/Document.h"
#include "dom/Element.h"
#include "dom/Traverse.h"

namespace StarFish {

unsigned long HTMLCollection::length() const
{
    std::vector<Node*, gc_allocator<Node*>> collection;
    Traverse::getherDescendant(&collection, m_root, m_filter);
    return collection.size();
}

Element* HTMLCollection::item(unsigned long index)
{
    std::vector<Node*, gc_allocator<Node*>> collection;
    Traverse::getherDescendant(&collection, m_root, m_filter);
    if (index < collection.size()) {
        return collection.at(index)->asElement();
    }
    return nullptr;
}

Element* HTMLCollection::namedItem(String* key)
{
    std::vector<Node*, gc_allocator<Node*>> collection;
    Traverse::getherDescendant(&collection, m_root, m_filter);
    for (unsigned i = 0; i < collection.size(); i++) {
        Element* elem = collection.at(i)->asElement();
        if (elem->id()->equals(key))
            return elem;
    }
    return nullptr;
}

}
