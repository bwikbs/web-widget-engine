#ifndef __StarFishStackingContext__
#define __StarFishStackingContext__

#include "platform/canvas/Canvas.h"

namespace StarFish {
class Node;
class FrameBox;
class StackingContext;
class Canvas;
class CanvasSurface;
class Frame;

class StackingContextChild : public std::vector<StackingContext*, gc_allocator<StackingContext*> > , public gc {
};

class StackingContext : public gc {
public:
    StackingContext(FrameBox* owner, StackingContext* parent);
    ~StackingContext();

    void clearChildContexts()
    {
        m_childContexts.clear();
    }

    const std::map<int32_t, StackingContextChild*, std::less<int32_t>, gc_allocator<std::pair<uint32_t, StackingContextChild*> > >& childContexts()
    {
        return m_childContexts;
    }

    FrameBox* owner()
    {
        return m_owner;
    }

    bool needsOwnBuffer()
    {
        return m_needsOwnBuffer;
    }

    void clearOwnBuffer()
    {
        delete m_buffer;
        m_buffer = nullptr;
    }

    bool computeStackingContextProperties(bool forceNeedsBuffer = false);

    void paintStackingContext(Canvas* canvas);
    void compositeStackingContext(Canvas* canvas);
    Frame* hitTestStackingContext(LayoutUnit x, LayoutUnit y);
protected:
    bool m_needsOwnBuffer;
    FrameBox* m_owner;
    StackingContext* m_parent;
    CanvasSurface* m_buffer;

    SkMatrix m_matrix;

    std::map<int32_t, StackingContextChild*, std::less<int32_t>, gc_allocator<std::pair<uint32_t, StackingContextChild*> > > m_childContexts;
};
}

#endif
