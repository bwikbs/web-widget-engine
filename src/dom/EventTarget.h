#ifndef __StarFishEventTarget__
#define __StarFishEventTarget__

#include "dom/binding/ScriptWrappable.h"

namespace StarFish {

class Window;
class Node;

// TODO
// addEventListener, removeEventListener..
// implement event listeners storage

template<typename T>
class EventTarget : public T {
protected:
    EventTarget()
    {
    }

    virtual ~EventTarget()
    {

    }

public:
    virtual bool isWindow()
    {
        return false;
    }

    Window* asWindow()
    {
        STARFISH_ASSERT(isWindow());
        return (Window*)this;
    }

    virtual bool isNode() const
    {
        return false;
    }

    Node* asNode()
    {
        STARFISH_ASSERT(isNode());
        return (Node*)this;
    }
};

}

#endif
