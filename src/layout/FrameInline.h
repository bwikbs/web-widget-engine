#ifndef __StarFishFrameInline__
#define __StarFishFrameInline__

#include "layout/Frame.h"

namespace StarFish {

class FrameInline : public Frame {
public:
    FrameInline(Node* node, ComputedStyle* style)
        : Frame(node, style)
    {

    }

    virtual bool isFrameInline()
    {
        return true;
    }

    virtual const char* name()
    {
        return "FrameInline";
    }

    virtual void dump()
    {
        Frame::dump();
    }

protected:
};

}

#endif
