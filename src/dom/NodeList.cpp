#include "StarFishConfig.h"
#include "NodeList.h"

namespace StarFish {

unsigned long NodeList::length() const
{
    return m_collection.size();
}

Node* NodeList::item(unsigned long index)
{
    if (index < m_collection.size()) {
        return m_collection.at(index);
    }
    return nullptr;
}

}
