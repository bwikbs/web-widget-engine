#ifndef __StarFishBlockBox__
#define __StarFishBlockBox__

#include "layout/FrameBox.h"

namespace StarFish {

class FrameDocument : public FrameBox {
public:
    FrameDocument(Node* node, ComputedStyle* style)
        : FrameBox(node, style)
    {

    }
};

}

#endif
