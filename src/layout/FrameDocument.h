#ifndef __StarFishFrameDocument__
#define __StarFishFrameDocument__

#include "layout/FrameBlockBox.h"

namespace StarFish {

class FrameDocument : public FrameBlockBox {
public:
    FrameDocument(Node* node, ComputedStyle* style)
        : FrameBlockBox(node, style)
    {

    }

    virtual const char* name()
    {
        return "FrameDocument";
    }
};

}

#endif
