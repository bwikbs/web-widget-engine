#ifndef __StarFishFrame__
#define __StarFishFrame__

#include "dom/DOM.h"

#include "style/Unit.h"
#include "style/ComputedStyle.h"

namespace StarFish {

class Node;
class FrameText;
class FrameBox;
class FrameBlockBox;

class Frame : public gc {
public:
    Frame(Node* node, ComputedStyle* style)
        : m_node(node)
        , m_style(style)
    {
        m_firstChild = m_next = m_parent = nullptr;
    }

    virtual ~Frame()
    {

    }

    virtual bool isFrameBox()
    {
        return false;
    }

    virtual bool isFrameBlockBox()
    {
        return false;
    }

    virtual bool isFrameText()
    {
        return false;
    }

    FrameText* asFrameText()
    {
        STARFISH_ASSERT(isFrameText());
        return (FrameText*)this;
    }

    FrameBox* asFrameBox()
    {
        STARFISH_ASSERT(isFrameBox());
        return (FrameBox*)this;
    }

    FrameBlockBox* asFrameBlockBox()
    {
        STARFISH_ASSERT(isFrameBlockBox());
        return (FrameBlockBox*)this;
    }

    ComputedStyle* style()
    {
        return m_style;
    }

    Node* node()
    {
        return m_node;
    }

    void setParent(Frame* f)
    {
        m_parent = f;
    }

    Frame* parent()
    {
        return m_parent;
    }

    Frame* next()
    {
        return m_next;
    }

    Frame* firstChild()
    {
        return m_firstChild;
    }

    Frame* lastChild()
    {
        // TODO implement lastChild member
        Frame* f = m_firstChild;
        while (f && f->next()) {
            f = f->next();
        }

        return f;
    }

    void setNext(Frame* f)
    {
        m_next = f;
    }

    void appendChild(Frame* child)
    {
        STARFISH_ASSERT(child->parent() == nullptr);
        if (m_firstChild) {
            Frame* frame = m_firstChild;
            while (frame->next() != nullptr) {
                frame = frame->next();
            }
            STARFISH_ASSERT(frame->next() == nullptr);
            frame->setNext(child);
        } else {
            m_firstChild = child;
        }
        child->setParent(this);
    }

    void removeChild(Frame* child)
    {
        STARFISH_ASSERT(child);
        STARFISH_ASSERT(child->parent() == this);
        Frame* prevFrame = nullptr;
        Frame* frame = m_firstChild;
        while (frame != child) {
            prevFrame = frame;
            frame = frame->next();
        }

        STARFISH_ASSERT(frame == child);
        frame->setParent(nullptr);
        if (prevFrame)
            prevFrame->setNext(frame->next());
        else
            m_firstChild = frame->next();

        frame->setNext(nullptr);
    }

    void replaceChildWith(Frame* now, Frame* after)
    {
        if (m_firstChild == now) {
            after->setNext(m_firstChild->next());
            after->setParent(this);
            m_firstChild->setParent(nullptr);
            m_firstChild->setNext(nullptr);
            m_firstChild = after;
            return ;
        }

        Frame* frameBefore = m_firstChild;
        Frame* frame = m_firstChild->next();
        while (frame->next() != now) {
            frameBefore = frame;
            frame = frame->next();
        }
        STARFISH_ASSERT(frame);

        after->setNext(frame->next());
        after->setParent(this);
        frame->setParent(nullptr);
        frame->setNext(nullptr);
        frameBefore->setNext(after);
    }

    virtual void dump()
    {

    }

    virtual const char* name() = 0;

protected:
    Node* m_node;
    ComputedStyle* m_style;
    Frame* m_parent;
    // Frame* m_previous;
    Frame* m_next;
    Frame* m_firstChild;
    // Frame* m_lastChild;
};

}

#endif
