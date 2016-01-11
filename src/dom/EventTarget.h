#ifndef __StarFishEventTarget__
#define __StarFishEventTarget__

#include "dom/binding/ScriptWrappable.h"

namespace StarFish {

class Window;
class Node;

template<typename T = ScriptWrappable>
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

    virtual bool isNode()
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
