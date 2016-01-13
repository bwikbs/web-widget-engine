#ifndef __StarFishLayoutObject__
#define __StarFishLayoutObject__

#include "style/ComputedStyle.h"

namespace StarFish {

class Node;

class LayoutObject : public gc {
public:
    LayoutObject(Node* node, ComputedStyle* style)
        : m_node(node)
        , m_style(style)
    {

    }

protected:
    Node* m_node;
    ComputedStyle* m_style;
};

}

#endif
