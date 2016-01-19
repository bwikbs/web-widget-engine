#ifndef __StarFishHTMLCollection__
#define __StarFishHTMLCollection__

#include "dom/EventTarget.h"

namespace StarFish {

class Node;
class Element;
class HTMLCollection : public EventTarget<ScriptWrappable> {
public:
    HTMLCollection(Node* parent, ScriptBindingInstance* instance) : m_parent(parent) {
        initScriptWrappable(this, instance);
    }
    unsigned long length() const;
    Element* item(unsigned long index);
    Element* namedItem(String* name);
private:
    Node* m_parent;
};

}

#endif
