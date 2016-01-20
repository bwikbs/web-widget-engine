#include "StarFishConfig.h"
#include "NodeList.h"
#include "dom/Document.h"
#include "dom/Traverse.h"

namespace StarFish {

unsigned long NodeList::length() const
{
    std::vector<Node*, gc_allocator<Node*>> collection;
    Traverse::getherDescendant(&collection, m_root, m_filter);
    return collection.size();
}

Node* NodeList::item(unsigned long index)
{
    std::vector<Node*, gc_allocator<Node*>> collection;
    Traverse::getherDescendant(&collection, m_root, m_filter);
    if (index < collection.size()) {
        return collection.at(index);
    }
    return nullptr;
}

}
