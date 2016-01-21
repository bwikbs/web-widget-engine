#include "StarFishConfig.h"
#include "FrameBlockBox.h"
#include "FrameText.h"

namespace StarFish {

void FrameBlockBox::layoutBlock(LayoutContext& ctx)
{
    // TODO Determine horizontal margins of this object.
    float parentContentWidth = ctx.parentContentWidth(this);

    // https://www.w3.org/TR/CSS2/visudet.html#the-width-property
    if (m_style->width().isAuto()) {
        setContentWidth(parentContentWidth);
    } else if (m_style->width().isFixed()) {
        setContentWidth(m_style->width().fixed());
    } else if (m_style->width().isPercent()) {
        setContentWidth(parentContentWidth * m_style->width().percent());
    }

    float normalFlowHeight = 0;
    Frame* child = firstChild();
    while (child) {
        // Lay out the child
        child->layout(ctx);

        // TODO Place the child.
        child->asFrameBox()->setX(0);
        child->asFrameBox()->setY(normalFlowHeight);

        normalFlowHeight += child->asFrameBox()->height();

        child = child->next();
    }

    // Now the intrinsic height of the object is known because the children are placed
    // TODO Determine the final height
    setContentHeight(normalFlowHeight);
}

bool isInlineBox(Frame* f)
{
    return f->isFrameText() || f->isFrameReplaced();
}

void collectInlineBox(std::vector<Frame*>& result, Frame* current)
{
    if (isInlineBox(current)) {
        result.push_back(current);
    }

    Frame* f = current->firstChild();
    while (f) {
        collectInlineBox(result, f);
        f = f->next();
    }
}

void FrameBlockBox::layoutInline(LayoutContext& ctx)
{
    m_lineBoxes.clear();
    // m_lineBoxes.shrink_to_fit();

    float parentContentWidth = m_parent->asFrameBlockBox()->contentWidth();
    setWidth(parentContentWidth);

    m_lineBoxes.push_back(LineBox());

    // we dont need gc_allocator here. because frame-tree-item is referenced by its parent
    std::vector<Frame*> result;
    collectInlineBox(result, this);
    float nowLineWidth = 0;
    size_t nowLine = 0;
    for (unsigned i = 0; i < result.size(); i ++) {
        Frame* f = result[i];
        if (f->isFrameText()) {
            String* txt = f->asFrameText()->text();
            //fast path
            if (f == lastChild()) {
                if (txt->containsOnlyWhitespace()) {
                    f = f->next();
                    continue;
                }
            }
            unsigned offset = 0;
            while(true) {
                if(offset>=txt->length())
                    break;
                bool isWhiteSpace = false;
                if(String::isSpaceOrNewline(txt->charAt(offset))) {
                    isWhiteSpace = true;
                }

                // find next space
                unsigned nextOffset = offset+1;
                if(isWhiteSpace) {
                    while(nextOffset < txt->length() && String::isSpaceOrNewline((*txt)[nextOffset])) {
                        nextOffset++;
                    }
                }
                else {
                    while(nextOffset < txt->length() && !String::isSpaceOrNewline((*txt)[nextOffset])) {
                        nextOffset++;
                    }
                }

                if(nowLineWidth == 0 && isWhiteSpace) {
                    offset = nextOffset;
                    continue;
                }

                float textWidth;
                String* resultString;
                if(isWhiteSpace) {
                    resultString = String::spaceString;
                    textWidth = f->style()->font()->measureText(String::spaceString);
                }
                else {
                    String* ss = txt->substring(offset,nextOffset-offset);
                    resultString = ss;
                    textWidth = f->style()->font()->measureText(ss);
                }

                if(textWidth <= parentContentWidth - nowLineWidth || nowLineWidth == 0) {
                    nowLineWidth += textWidth;
                } else {
                    // try this at nextline
                    m_lineBoxes.push_back(LineBox());
                    nowLine++;
                    nowLineWidth = 0;
                    continue;
                }

                m_lineBoxes[nowLine].m_boxes.push_back(new InlineTextBox(f->node(), f->style(), resultString));
                m_lineBoxes[nowLine].m_boxes.back()->setWidth(textWidth);
                m_lineBoxes[nowLine].m_boxes.back()->setHeight(f->style()->font()->metrics().m_fontHeight);
                offset = nextOffset;
            }
        } else if (f->isFrameReplaced()) {
            FrameReplaced* r = f->asFrameReplaced();
            r->layout(ctx);

            insertReplacedBox:
            if (r->width() < (parentContentWidth - nowLineWidth) || nowLineWidth == 0) {
                m_lineBoxes[nowLine].m_boxes.push_back(new InlineReplacedBox(f->node(), f->style(), r));
                m_lineBoxes[nowLine].m_boxes.back()->setWidth(r->width());
                m_lineBoxes[nowLine].m_boxes.back()->setHeight(r->height());
                nowLineWidth += r->width();
            } else {
                m_lineBoxes.push_back(LineBox());
                nowLine++;
                nowLineWidth = 0;
                goto insertReplacedBox;
            }
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    }

    // position each line
    float contentHeight = 0;
    for (size_t i = 0; i < m_lineBoxes.size(); i ++) {
        LineBox& b = m_lineBoxes[i];

        b.m_frameRect.setX(0);
        b.m_frameRect.setY(contentHeight);

        // align boxes
        float x = 0;
        for (size_t j = 0; j < b.m_boxes.size(); j ++) {
            b.m_boxes[j]->setX(x);
            x += b.m_boxes[j]->width();
        }

        // text align
        if (style()->textAlign() == TextAlignValue::LeftTextAlignValue) {
        } else if (style()->textAlign() == TextAlignValue::RightTextAlignValue) {
            float xx = 0;
            for (size_t j = 0; j < b.m_boxes.size(); j ++) {
                InlineBox* box = b.m_boxes[b.m_boxes.size() -1 - j];
                box->setX(x - xx);
                xx += box->width();
            }
        } else {
            STARFISH_ASSERT(style()->textAlign() == TextAlignValue::CenterTextAlignValue);
            float diff = (parentContentWidth - x) / 2;
            for (size_t j = 0; j < b.m_boxes.size(); j ++) {
                InlineBox* box = b.m_boxes[j];
                box->setX(box->x() + diff);
            }
        }

        // TODO vertical align(middle, top, bottom, middle)
        float maxAscender = 0;
        float maxDecender = 0;
        for (size_t j = 0; j < b.m_boxes.size(); j ++) {
            InlineBox* ib = b.m_boxes[j];
            if (ib->isInlineTextBox()) {
                maxAscender = std::max(ib->asInlineTextBox()->style()->font()->metrics().m_ascender, maxAscender);
                maxDecender = std::min(ib->asInlineTextBox()->style()->font()->metrics().m_descender, maxDecender);
            } else if (ib->isInlineReplacedBox()) {
                maxAscender = std::max(b.m_boxes[j]->height(), maxAscender);
            }
        }

        float height = maxAscender - maxDecender;
        for (size_t j = 0; j < b.m_boxes.size(); j ++) {
            InlineBox* ib = b.m_boxes[j];
            if (ib->isInlineTextBox()) {
                ib->setY(height + maxDecender - ib->height() - ib->asInlineTextBox()->style()->font()->metrics().m_descender);
            } else if (ib->isInlineReplacedBox()) {
                ib->setY(height + maxDecender - ib->height());
            }
        }

        b.m_frameRect.setWidth(parentContentWidth);
        b.m_frameRect.setHeight(height);
        contentHeight += b.m_frameRect.height();
    }


    // compute object height
    setHeight(contentHeight);
}

void FrameBlockBox::paint(Canvas* canvas, PaintingStage stage)
{
    if (isEstablishesStackingContext()) {
        ASSERT(stage == PaintingStackingContext);
        // Within each stacking context, the following layers are painted in back-to-front order:

        // the background and borders of the element forming the stacking context.
        paintBackgroundAndBorders(canvas);

        // TODO the child stacking contexts with negative stack levels (most negative first).

        // the in-flow, non-inline-level, non-positioned descendants.
        ASSERT(hasBlockFlow());
        Frame* child = firstChild();
        while (child) {
            canvas->save();
            canvas->translate(child->asFrameBox()->x(), child->asFrameBox()->y());
            child->paint(canvas, PaintingNormalFlowBlock);
            canvas->restore();
            child = child->next();
        }

        // TODO the non-positioned floats.
        /*
        child = firstChild();
        while (child) {
            canvas->save();
            canvas->translate(child->asFrameBox()->x(), child->asFrameBox()->y());
            child->paint(canvas, PaintingNonPositionedFloats);
            canvas->restore();
            child = child->next();
        }
        */

        // the in-flow, inline-level, non-positioned descendants, including inline tables and inline blocks.
        child = firstChild();
        while (child) {
            canvas->save();
            canvas->translate(child->asFrameBox()->x(), child->asFrameBox()->y());
            child->paint(canvas, PaintingNormalFlowInline);
            canvas->restore();
            child = child->next();
        }

        // TODO the child stacking contexts with stack level 0 and the positioned descendants with stack level 0.
        /*
        child = firstChild();
        while (child) {
            canvas->save();
            canvas->translate(child->asFrameBox()->x(), child->asFrameBox()->y());
            child->paint(canvas, PaintingPositionedElements);
            canvas->restore();
            child = child->next();
        }
        */

        // TODO the child stacking contexts with positive stack levels (least positive first).
    } else {
        if (stage == PaintingNormalFlowBlock) {
            if (hasBlockFlow()) {
                paintBackgroundAndBorders(canvas);
                Frame* child = firstChild();
                while (child) {
                    canvas->save();
                    canvas->translate(child->asFrameBox()->x(), child->asFrameBox()->y());
                    child->paint(canvas, stage);
                    canvas->restore();
                    child = child->next();
                }
            } else {
                paintBackgroundAndBorders(canvas);
                for (size_t i = 0; i < m_lineBoxes.size(); i ++) {
                    canvas->save();
                    LineBox& b = m_lineBoxes[i];
                    canvas->translate(b.m_frameRect.x(), b.m_frameRect.y());
                    for (size_t j = 0; j < b.m_boxes.size(); j ++) {
                        canvas->save();
                        canvas->translate(b.m_boxes[j]->x(), b.m_boxes[j]->y());
                        b.m_boxes[j]->paint(canvas, stage);
                        canvas->restore();
                    }
                    canvas->restore();
                }
            }
        } else if (stage == PaintingNormalFlowInline) {
            if (!hasBlockFlow()) {
                for (size_t i = 0; i < m_lineBoxes.size(); i ++) {
                    canvas->save();
                    LineBox& b = m_lineBoxes[i];
                    canvas->translate(b.m_frameRect.x(), b.m_frameRect.y());
                    for (size_t j = 0; j < b.m_boxes.size(); j ++) {
                        canvas->save();
                        canvas->translate(b.m_boxes[j]->x(), b.m_boxes[j]->y());
                        b.m_boxes[j]->paint(canvas, stage);
                        canvas->restore();
                    }
                    canvas->restore();
                }
            } else {
                Frame* child = firstChild();
                while (child) {
                    canvas->save();
                    canvas->translate(child->asFrameBox()->x(), child->asFrameBox()->y());
                    child->paint(canvas, stage);
                    canvas->restore();
                    child = child->next();
                }
            }
        }
    }
}

Frame* FrameBlockBox::hitTest(float x, float y,HitTestStage stage)
{
    if (isEstablishesStackingContext()) {
        ASSERT(stage == HitTestStackingContext);
        Frame* result = nullptr;
        Frame* child;
        // TODO the child stacking contexts with positive stack levels (least positive first).

        // TODO the child stacking contexts with stack level 0 and the positioned descendants with stack level 0.

        // the in-flow, inline-level, non-positioned descendants, including inline tables and inline blocks.
        child = lastChild();
        while (child) {
            float cx = x - child->asFrameBox()->x();
            float cy = y - child->asFrameBox()->y();
            result = child->hitTest(cx ,cy , HitTestNormalFlowInline);
            if (result)
                return result;
            child = child->previous();
        }

        // TODO the non-positioned floats.

        // the in-flow, non-inline-level, non-positioned descendants.
        ASSERT(hasBlockFlow());
        child = lastChild();
        while (child) {
            float cx = x - child->asFrameBox()->x();
            float cy = y - child->asFrameBox()->y();
            result = child->hitTest(cx, cy, HitTestNormalFlowBlock);
            if (result)
                return result;
            child = child->previous();
        }

        // the background and borders of the element forming the stacking context.
        result = FrameBox::hitTest(x, y, stage);
        if (result)
            return result;

        // TODO the child stacking contexts with negative stack levels (most negative first).
        return nullptr;
    } else {
        if (stage == HitTestNormalFlowBlock) {
            if (hasBlockFlow()) {
                Frame* result = nullptr;
                Frame* child = lastChild();
                while (child) {
                    float cx = x - child->asFrameBox()->x();
                    float cy = y - child->asFrameBox()->y();
                    result = child->hitTest(cx ,cy , stage);
                    if (result)
                        return result;
                    child = child->previous();
                }

                return FrameBox::hitTest(x, y, stage);
            } else {
                return FrameBox::hitTest(x, y, stage);
            }
        } else if (stage == HitTestNormalFlowInline) {
            if (!hasBlockFlow()) {
                Frame* result = nullptr;
                for (size_t i = 0; i < m_lineBoxes.size(); i ++) {
                    LineBox& b = m_lineBoxes[i];
                    float cx = x - b.m_frameRect.x();
                    float cy = y - b.m_frameRect.y();

                    for (size_t j = 0; j < b.m_boxes.size(); j ++) {
                        float cx2 = cx - b.m_boxes[j]->x();
                        float cy2 = cy - b.m_boxes[j]->y();
                        result = b.m_boxes[j]->hitTest(cx2, cy2, stage);
                        if (result)
                            return result;
                    }
                }

                return nullptr;
            } else {
                Frame* result = nullptr;
                Frame* child = lastChild();
                while (child) {
                    float cx = x - child->asFrameBox()->x();
                    float cy = y - child->asFrameBox()->y();
                    result = child->hitTest(cx ,cy , stage);
                    if (result)
                        return result;
                    child = child->previous();
                }
                return nullptr;
            }
        }
    }
    return nullptr;
}

}
