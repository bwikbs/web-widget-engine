#ifndef __StarFishFrame__
#define __StarFishFrame__

#include "dom/DOM.h"

#include "style/Unit.h"
#include "style/ComputedStyle.h"

namespace StarFish
{

class Node;
class FrameText;
class FrameBox;
class FrameBlockBox;
class FrameReplaced;

enum PaintingStage
{
    PaintingStackingContext,
    PaintingNormalFlowBlock, // the in-flow, non-inline-level, non-positioned descendants.
    PaintingNonPositionedFloats, // the non-positioned floats.
    PaintingNormalFlowInline, // the in-flow, inline-level, non-positioned descendants, including inline tables and inline blocks.
    PaintingPositionedElements, // the child stacking contexts with stack level 0 and the positioned descendants with stack level 0.
};

enum HitTestStage
{
    HitTestStackingContext, HitTestNormalFlowBlock, HitTestNonPositionedFloats, HitTestNormalFlowInline, HitTestPositionedElements,
};

class LayoutContext
{
public:
    float parentContentWidth(Frame* currentFrame);
};

class Frame: public gc
{
public:
    Frame(Node* node, ComputedStyle* style) :
            m_node(node), m_style(style)
    {
        m_firstChild = m_lastChild = m_next = m_previous = m_parent = nullptr;
        m_flags.m_needsLayout = true;

        bool isRootElement = node && node->isElement() && node->asElement()->isHTMLElement() && node->asElement()->asHTMLElement()->isHTMLHtmlElement();
        // TODO add condition
        m_flags.m_isEstablishesBlockFormattingContext = isRootElement;
        // TODO add condition
        m_flags.m_isEstablishesStackingContext = isRootElement;
        // TODO add condition
        m_flags.m_isPositionedElement = false;
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

    virtual bool isFrameReplaced()
    {
        return false;
    }

    FrameText* asFrameText()
    {
        STARFISH_ASSERT(isFrameText());
        return (FrameText*) this;
    }

    FrameBox* asFrameBox()
    {
        STARFISH_ASSERT(isFrameBox());
        return (FrameBox*) this;
    }

    FrameReplaced* asFrameReplaced()
    {
        STARFISH_ASSERT(isFrameReplaced());
        return (FrameReplaced*) this;
    }

    FrameBlockBox* asFrameBlockBox()
    {
        STARFISH_ASSERT(isFrameBlockBox());
        return (FrameBlockBox*) this;
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

    Frame* previous()
    {
        return m_previous;
    }

    Frame* firstChild()
    {
        return m_firstChild;
    }

    Frame* lastChild()
    {
        return m_lastChild;
    }

    void setNext(Frame* f)
    {
        m_next = f;
    }

    void appendChild(Frame* newChild)
    {
        STARFISH_ASSERT(newChild->parent() == nullptr);

        newChild->setParent(this);
        Frame* lChild = lastChild();

        if (lChild) {
            newChild->m_previous = lChild;
            lChild->m_next = newChild;
        } else {
            m_firstChild = newChild;
        }

        m_lastChild = newChild;
    }

    void removeChild(Frame* oldChild)
    {
        STARFISH_ASSERT(oldChild);
        STARFISH_ASSERT(oldChild->parent() == this);

        if (oldChild->m_previous)
            oldChild->m_previous->m_next = oldChild->next();
        if (oldChild->m_next)
            oldChild->m_next->m_previous = oldChild->previous();

        if (m_firstChild == oldChild)
            m_firstChild = oldChild->next();
        if (m_lastChild == oldChild)
            m_lastChild = oldChild->previous();

        oldChild->m_previous = nullptr;
        oldChild->m_next = nullptr;
        oldChild->setParent(nullptr);
    }

    virtual void dump()
    {

    }

    virtual const char* name()
    {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }

    virtual void layout(LayoutContext& ctx)
    {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }

    virtual void paint(Canvas* canvas, PaintingStage = PaintingStackingContext)
    {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }

    virtual Frame* hitTest(float x, float y, HitTestStage stage = HitTestStackingContext)
    {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }

    bool isEstablishesBlockFormattingContext()
    {
        return m_flags.m_isEstablishesBlockFormattingContext;
    }

    bool isEstablishesStackingContext()
    {
        return m_flags.m_isEstablishesStackingContext;
    }

    bool isPositionedElement()
    {
        return m_flags.m_isPositionedElement;
    }

protected:
    struct
    {
        bool m_needsLayout :1;

        // https://www.w3.org/TR/CSS21/visuren.html#block-formatting
        // Floats, absolutely positioned elements, block containers (such as inline-blocks, table-cells, and table-captions) that are not block boxes, and block boxes with 'overflow' other than 'visible' (except when that value has been propagated to the viewport) establish new block formatting contexts for their contents.
        bool m_isEstablishesBlockFormattingContext :1;

        // https://www.w3.org/TR/CSS21/visuren.html#propdef-z-index
        // Other stacking contexts are generated by any positioned element (including relatively positioned elements) having a computed value of 'z-index' other than 'auto'. Stacking contexts are not necessarily related to containing blocks. In future levels of CSS, other properties may introduce stacking contexts, for example 'opacity' [CSS3COLOR].
        bool m_isEstablishesStackingContext :1;

        // https://www.w3.org/TR/CSS21/visuren.html#positioning-scheme
        // 9.3.2
        // An element is said to be positioned if its 'position' property has a value other than 'static'. Positioned elements generate positioned boxes, laid out according to four properties:
        bool m_isPositionedElement;
    } m_flags;

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
