#ifndef __StarFishHTMLCollection__
#define __StarFishHTMLCollection__

#include "dom/EventTarget.h"

namespace StarFish {

class Node;
class Element;
class HTMLCollection : public EventTarget<ScriptWrappable> {
public:
    HTMLCollection(ScriptBindingInstance* instance)
    {
        initScriptWrappable(this, instance);
    }
    unsigned long length() const;
    Element* item(unsigned long index);
    Element* namedItem(String* name);
    std::vector<Node*, gc_allocator<Node*>>* collection() { return &m_collection; }
private:
    std::vector<Node*, gc_allocator<Node*>> m_collection;
};

}

#endif
