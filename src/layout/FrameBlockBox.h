#ifndef __StarFishBlockBox__
#define __StarFishBlockBox__

#include "layout/FrameBox.h"

namespace StarFish {

class FrameBlockBox : public FrameBox {
public:
    FrameBlockBox(Node* node, ComputedStyle* style)
        : FrameBox(node, style)
    {

    }
};

}

#endif
