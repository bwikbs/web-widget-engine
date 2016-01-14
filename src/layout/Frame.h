#ifndef __StarFishFrame__
#define __StarFishFrame__

#include "style/ComputedStyle.h"

namespace StarFish {

class Node;

class Frame : public gc {
public:
    Frame(Node* node, ComputedStyle* style)
        : m_node(node)
        , m_style(style)
    {

    }

    ComputedStyle* style()
    {
        return m_style;
    }

protected:
    Node* m_node;
    ComputedStyle* m_style;
};

}

#endif
