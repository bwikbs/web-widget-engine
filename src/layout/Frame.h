#ifndef __StarFishFrame__
#define __StarFishFrame__

#include "style/Unit.h"
#include "style/ComputedStyle.h"

namespace StarFish {

class Node;

class Frame : public gc {
public:
    Frame(Node* node, ComputedStyle* style)
        : m_node(node)
        , m_style(style)
    {
        m_firstChild = m_lastChild = m_next = m_previous = m_parent = nullptr;
    }

    virtual ~Frame()
    {

    }

    ComputedStyle* style()
    {
        return m_style;
    }

    void appendChild(Frame* frm)
    {

    }

protected:
    Node* m_node;
    ComputedStyle* m_style;
    Frame* m_parent;
    Frame* m_previous;
    Frame* m_next;
    Frame* m_firstChild;
    Frame* m_lastChild;
};

}

#endif
