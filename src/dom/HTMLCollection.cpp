#include "StarFishConfig.h"
#include "HTMLCollection.h"
#include "dom/Document.h"
#include "dom/Element.h"

namespace StarFish {

unsigned long HTMLCollection::length() const
{
    return m_collection.size();
}

Element* HTMLCollection::item(unsigned long index)
{
    if (index < m_collection.size()) {
        return m_collection.at(index)->asElement();
    }
    return nullptr;
}

Element* HTMLCollection::namedItem(String* key)
{
    for (unsigned i = 0; i < m_collection.size(); i++) {
        Element* elem = m_collection.at(i)->asElement();
        if (elem->id()->equals(key))
            return elem;
    }
    return nullptr;
}

}
