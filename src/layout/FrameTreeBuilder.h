#ifndef __StarFishFrameTreeBuilder__
#define __StarFishFrameTreeBuilder__

namespace StarFish {

class Node;
class Document;
class Element;

class FrameTreeBuilder {
public:
    static void buildFrameTree(Document* document);
    static void clearTree(Node* current);
#ifdef STARFISH_ENABLE_TEST
    // debug function
    static void dumpFrameTree(Document* document);
#endif
};

}

#endif
