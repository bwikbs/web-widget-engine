#ifndef __StarFishNodeList__
#define __StarFishNodeList__

#include "dom/EventTarget.h"

namespace StarFish {

class Node;
class NodeList : public EventTarget<ScriptWrappable> {
public:
    NodeList(ScriptBindingInstance* instance)
    {
        initScriptWrappable(this, instance);
    }
    unsigned long length() const;
    Node* item(unsigned long index);
    std::vector<Node*, gc_allocator<Node*>>* collection() { return &m_collection; }
private:
    std::vector<Node*, gc_allocator<Node*>> m_collection;
};

}

#endif
