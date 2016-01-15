#ifndef __StarFishFrameTreeBuilder__
#define __StarFishFrameTreeBuilder__

namespace StarFish {

class Document;

class FrameTreeBuilder {
public:
    static void buildFrameTree(Document* document);
    // debug function
    static void dumpFrameTree(Document* document);
};

}

#endif
