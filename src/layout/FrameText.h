#ifndef __StarFishFrameText__
#define __StarFishFrameText__

#include "layout/Frame.h"

namespace StarFish {

class FrameText : public Frame {
public:
    FrameText (Node* node, ComputedStyle* style)
        : Frame(node, style)
    {
    }
};

}

#endif
