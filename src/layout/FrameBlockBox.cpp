#include "StarFishConfig.h"
#include "FrameBlockBox.h"
#include "FrameText.h"

namespace StarFish {

void FrameBlockBox::layout(LayoutContext& ctx)
{
    if (isNormalFlow()) {
        float parentContentWidth = ctx.parentContentWidth(this);
        // TODO Determine horizontal margins of this object.

        // https://www.w3.org/TR/CSS2/visudet.html#the-width-property
        if (m_style->width().isAuto()) {
            if (m_flags.m_shouldComputePreferredWidth) {
                ComputePreferredWidthContext p(ctx, parentContentWidth);
                computePreferredWidth(p);
                setContentWidth(p.result());
            } else {
                setContentWidth(parentContentWidth);
            }
        } else if (m_style->width().isFixed()) {
            setContentWidth(m_style->width().fixed());
        } else if (m_style->width().isPercent()) {
            setContentWidth(parentContentWidth * m_style->width().percent());
        }
    } else {
        STARFISH_ASSERT(node() != nullptr);
        FrameBox* cb = ctx.containingBlock(this)->asFrameBox();
        FrameBox* parent = m_parent->asFrameBox();
        auto absLoc = parent->absolutePoint(cb);
        float absX = absLoc.x() + cb->borderLeft();
        auto setAbsX = [&](float x) {
            setX(x - absX);
        };

        auto getPreferredWidth = [&](float parentWidth) -> float {
            STARFISH_ASSERT(style()->width().isAuto());
            ComputePreferredWidthContext p(ctx, parentWidth);
            computePreferredWidth(p);
            return p.result();
        };

        // TODO Determine horizontal margins of this object.

        // 10.3.7 Absolutely positioned, non-replaced elements
        // The constraint that determines the used values for these elements is:
        // 'left' + 'margin-left' + 'border-left-width' + 'padding-left' + 'width' + 'padding-right' + 'border-right-width' + 'margin-right' + 'right' = width of containing block

        // Then, if the 'direction' property of the element establishing the static-position containing block is 'ltr' set 'left' to the static position and apply rule number three below;
        // otherwise, set 'right' to the static position and apply rule number one below.

        Length marginLeft = style()->marginLeft();
        Length marginRight = style()->marginRight();
        Length left = style()->left();
        Length right = style()->right();
        Length width = style()->width();

        float parentWidth = cb->contentWidth() + cb->paddingWidth();

        if (left.isAuto() && width.isAuto() && right.isAuto()) {
            // If all three of 'left', 'width', and 'right' are 'auto':
        } else if (!left.isAuto() && !width.isAuto() && !right.isAuto()) {
            // If none of the three is 'auto':

            // If both 'margin-left' and 'margin-right' are 'auto',
            if (marginLeft.isAuto() && marginRight.isAuto()) {
                // solve the equation under the extra constraint that the two margins get equal values,
                // unless this would make them negative, in which case when direction of the containing block is 'ltr' ('rtl'),
                // set 'margin-left' ('margin-right') to zero and solve for 'margin-right' ('margin-left').
                // TODO direction == rtl
                if (style()->direction() == DirectionValue::LtrDirectionValue) {
                    float computedLeft = left.specifiedValue(parentWidth);
                    setAbsX(computedLeft);
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }

            } else if (style()->direction() == DirectionValue::LtrDirectionValue) {
                // If one of 'margin-left' or 'margin-right' is 'auto', solve the equation for that value.
                // If the values are over-constrained, ignore the value for 'left'
                // (in case the 'direction' property of the containing block is 'rtl') or 'right' (in case 'direction' is 'ltr') and solve for that value.
                if (!marginLeft.isAuto()) {
                    if (style()->direction() == DirectionValue::LtrDirectionValue) {
                        float computedLeft = left.specifiedValue(parentWidth);
                        computedLeft += marginLeft.specifiedValue(parentWidth);
                        setAbsX(computedLeft);
                    }
                }
            } else {
                STARFISH_ASSERT(style()->direction() == DirectionValue::RtlDirectionValue);
                STARFISH_RELEASE_ASSERT_NOT_REACHED();
            }
        } else {
            // Otherwise, set 'auto' values for 'margin-left' and 'margin-right' to 0, and pick the one of the following six rules that applies.
            if (marginLeft.isAuto())
                marginLeft = Length(Length::Fixed, 0);
            if (marginRight.isAuto())
                marginRight = Length(Length::Fixed, 0);

            // TODO add margin-left, margin-right
            if (left.isAuto() && width.isAuto() && !right.isAuto()) {
                // 'left' and 'width' are 'auto' and 'right' is not 'auto', then the width is shrink-to-fit. Then solve for 'left'
                float w = getPreferredWidth(parentWidth);
                width = Length(Length::Fixed, w);
                setAbsX(parentWidth - right.specifiedValue(parentWidth) - w - paddingWidth() - borderWidth());
            } else if(left.isAuto() && right.isAuto() && !width.isAuto()) {
                // 'left' and 'right' are 'auto' and 'width' is not 'auto',
                // then if the 'direction' property of the element establishing the static-position containing block is 'ltr' set 'left' to the static position,
                // otherwise set 'right' to the static position. Then solve for 'left' (if 'direction is 'rtl') or 'right' (if 'direction' is 'ltr').
            } else if(width.isAuto() && right.isAuto() && !left.isAuto()) {
                // 'width' and 'right' are 'auto' and 'left' is not 'auto', then the width is shrink-to-fit . Then solve for 'right'
                float w = getPreferredWidth(parentWidth);
                width = Length(Length::Fixed, w);
                setAbsX(left.specifiedValue(parentWidth));
            } else if(left.isAuto() && !width.isAuto() && !right.isAuto()) {
                // 'left' is 'auto', 'width' and 'right' are not 'auto', then solve for 'left'
                float w = width.specifiedValue(parentWidth);
                setAbsX(parentWidth - right.specifiedValue(parentWidth) - w - paddingWidth() - borderWidth());
            } else if(width.isAuto() && !left.isAuto() && !right.isAuto()) {
                // 'width' is 'auto', 'left' and 'right' are not 'auto', then solve for 'width'
                float l = left.specifiedValue(parentWidth);
                float r = right.specifiedValue(parentWidth);
                float w = l - r + parentWidth;
                w = w - paddingWidth() - borderWidth();
                width = Length(Length::Fixed, w);
            } else {
                // 'right' is 'auto', 'left' and 'width' are not 'auto', then solve for 'right'
                STARFISH_ASSERT(right.isAuto() && !left.isAuto() && !width.isAuto());
                setAbsX(left.specifiedValue(parentWidth));
            }
        }

        if (width.isAuto()) {
            if (m_flags.m_shouldComputePreferredWidth) {
                ComputePreferredWidthContext p(ctx, parentWidth);
                computePreferredWidth(p);
                setContentWidth(p.result());
            } else {
                setContentWidth(parentWidth);
            }
        } else if (width.isFixed()) {
            setContentWidth(width.fixed());
        } else if (width.isPercent()) {
            setContentWidth(parentWidth * width.percent());
        }
    }

    float contentHeight;
    if (hasBlockFlow()) {
        contentHeight = layoutBlock(ctx);
    } else {
        contentHeight = layoutInline(ctx);
    }

    // Now the intrinsic height of the object is known because the children are placed

    // Determine the final height
    if (isNormalFlow()) {
        if (style()->height().isAuto()) {
            setContentHeight(contentHeight);
        } else if (style()->height().isFixed()) {
            setContentHeight(style()->height().fixed());
        } else {
            if (ctx.parentHasFixedHeight(this)) {
                setContentHeight(style()->height().percent() * ctx.parentFixedHeight(this));
            } else {
                setContentHeight(contentHeight);
            }
        }
    } else {
        FrameBox* cb = ctx.containingBlock(this)->asFrameBox();
        FrameBox* parent = m_parent->asFrameBox();
        auto absLoc = parent->absolutePoint(cb);
        float absY = absLoc.y() + cb->borderTop();
        auto setAbsY = [&](float y) {
            setY(y - absY);
        };
        float parentHeight = cb->contentHeight() + cb->paddingHeight();

        Length marginTop = style()->marginTop();
        Length marginBottom = style()->marginBottom();
        Length top = style()->top();
        Length bottom = style()->bottom();
        Length height = style()->height();

        // 10.6.4 Absolutely positioned, non-replaced elements

        // For absolutely positioned elements, the used values of the vertical dimensions must satisfy this constraint:
        // 'top' + 'margin-top' + 'border-top-width' + 'padding-top' + 'height' + 'padding-bottom' + 'border-bottom-width' + 'margin-bottom' + 'bottom' = height of containing block

        if (top.isAuto() && height.isAuto() && bottom.isAuto()) {
            // If all three of 'top', 'height', and 'bottom' are auto, set 'top' to the static position and apply rule number three below.
            // TODO add margin-top, margin-bottom
        } else if(!top.isAuto() && !height.isAuto() && !bottom.isAuto()) {
            // If none of the three are 'auto': If both 'margin-top' and 'margin-bottom' are 'auto',
            // solve the equation under the extra constraint that the two margins get equal values.
            // If one of 'margin-top' or 'margin-bottom' is 'auto', solve the equation for that value.
            // If the values are over-constrained, ignore the value for 'bottom' and solve for that value.
            // TODO add margin-top, margin-bottom
            setAbsY(top.specifiedValue(parentHeight));
        } else if(top.isAuto() && height.isAuto() && !bottom.isAuto()) {
            // 'top' and 'height' are 'auto' and 'bottom' is not 'auto', then the height is based on the content per 10.6.7
            // set 'auto' values for 'margin-top' and 'margin-bottom' to 0, and solve for 'top'
            if (marginTop.isAuto())
                marginTop = Length(Length::Fixed, 0);
            if (marginBottom.isAuto())
                marginBottom = Length(Length::Fixed, 0);
            setContentHeight(contentHeight);
            // TODO add margin-top, margin-bottom
            setAbsY(parentHeight - contentHeight - paddingHeight() - borderHeight() - bottom.specifiedValue(parentHeight));
        } else if (top.isAuto() && bottom.isAuto() && !height.isAuto()) {
            // 'top' and 'bottom' are 'auto' and 'height' is not 'auto', then set 'top' to the static position
            // set 'auto' values for 'margin-top' and 'margin-bottom' to 0, and solve for 'bottom'
            if (marginTop.isAuto())
                marginTop = Length(Length::Fixed, 0);
            if (marginBottom.isAuto())
                marginBottom = Length(Length::Fixed, 0);
        } else if (height.isAuto() && bottom.isAuto() && !top.isAuto()) {
            // 'height' and 'bottom' are 'auto' and 'top' is not 'auto', then the height is based on the content per 10.6.7,
            // set 'auto' values for 'margin-top' and 'margin-bottom' to 0, and solve for 'bottom'
            if (marginTop.isAuto())
                marginTop = Length(Length::Fixed, 0);
            if (marginBottom.isAuto())
                marginBottom = Length(Length::Fixed, 0);
            // TODO add margin-top, margin-bottom
            setY(top.specifiedValue(parentHeight));
        } else if (top.isAuto() && !height.isAuto() && !bottom.isAuto()) {
            // 'top' is 'auto', 'height' and 'bottom' are not 'auto', then set 'auto' values for 'margin-top' and 'margin-bottom' to 0, and solve for 'top'
            if (marginTop.isAuto())
                marginTop = Length(Length::Fixed, 0);
            if (marginBottom.isAuto())
                marginBottom = Length(Length::Fixed, 0);

            // TODO add margin-top, margin-bottom
            setAbsY(parentHeight - height.specifiedValue(parentHeight) - paddingHeight() - borderHeight() - bottom.specifiedValue(parentHeight));
        } else if(height.isAuto() && !top.isAuto() && !bottom.isAuto()) {
            // 'height' is 'auto', 'top' and 'bottom' are not 'auto', then 'auto' values for 'margin-top' and 'margin-bottom' are set to 0 and solve for 'height'
            if (marginTop.isAuto())
                marginTop = Length(Length::Fixed, 0);
            if (marginBottom.isAuto())
                marginBottom = Length(Length::Fixed, 0);

            float t = top.specifiedValue(parentHeight);
            float b = bottom.specifiedValue(parentHeight);
            float h = t - b + parentHeight;
            h = h - paddingHeight() - borderHeight();
            height = Length(Length::Fixed, h);
            // TODO add margin-top, margin-bottom
            setAbsY(t);
        } else {
            // 'bottom' is 'auto', 'top' and 'height' are not 'auto', then set 'auto' values for 'margin-top' and 'margin-bottom' to 0 and solve for 'bottom'
            STARFISH_ASSERT(bottom.isAuto() && !top.isAuto() && !height.isAuto());
            if (marginTop.isAuto())
                marginTop = Length(Length::Fixed, 0);
            if (marginBottom.isAuto())
                marginBottom = Length(Length::Fixed, 0);

            setAbsY(top.specifiedValue(parentHeight));
        }

        if (height.isAuto()) {
            setContentHeight(contentHeight);
        } else {
            setContentHeight(height.specifiedValue(parentHeight));
        }
    }

    // layout absolute positioned blocks
    ctx.layoutRegisteredAbsolutePositionedFrames(this, [&](const std::vector<Frame*>& frames) {
        for (size_t i = 0; i < frames.size(); i ++) {
            Frame* f = frames[i];
            f->layout(ctx);
        }
    });
}

float FrameBlockBox::layoutBlock(LayoutContext& ctx)
{
    float normalFlowHeight = 0;
    Frame* child = firstChild();
    while (child) {
        // Lay out the child
        if (child->isNormalFlow())
            child->layout(ctx);

        // TODO Place the child.
        child->asFrameBox()->setX(0);
        child->asFrameBox()->setY(normalFlowHeight);

        if (child->isNormalFlow())
            normalFlowHeight += child->asFrameBox()->height();
        else
            ctx.registerAbsolutePositionedFrames(child);

        child = child->next();
    }

    return normalFlowHeight;
}

bool isInlineBox(Frame* f)
{
    return f->isFrameText() || f->isFrameReplaced() || f->isFrameBlockBox() || f->isFrameLineBreak();
}

// NOTE this function collect only normal-flow child!
void collectInlineBox(std::vector<Frame*>& result, Frame* current, Frame* top)
{
    if (current != top) {
        if (isInlineBox(current)) {
            result.push_back(current);
            return;
        }
    }

    Frame* f = current->firstChild();
    while (f) {
        collectInlineBox(result, f, top);
        f = f->next();
    }
}

template <typename fn>
void textDividerForLayout(String* txt, fn f)
{
    // TODO consider white-space
    // TODO consider letter-spacing
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

        f(offset, nextOffset, isWhiteSpace);
        offset = nextOffset;
    }
}

class LineFormattingContext {
public:
    LineFormattingContext(FrameBlockBox& block, LayoutContext& ctx)
        : m_block(block)
        , m_layoutContext(ctx)
    {
        m_block.m_lineBoxes.clear();
        // m_block.m_lineBoxes.shrink_to_fit();
        m_block.m_lineBoxes.push_back(LineBox(&m_block));
        m_currentLine = 0;
        m_currentLineWidth = 0;
        m_layoutContext.setLastLineBox(&m_block.m_lineBoxes.back());
    }


    void breakLine()
    {
        m_block.m_lineBoxes.push_back(LineBox(&m_block));
        m_currentLine++;
        m_currentLineWidth = 0;
        m_layoutContext.setLastLineBox(&m_block.m_lineBoxes.back());
    }

    float m_currentLineWidth;
    size_t m_currentLine;
    FrameBlockBox& m_block;
    LayoutContext& m_layoutContext;
};


float FrameBlockBox::layoutInline(LayoutContext& ctx)
{
    float inlineContentWidth = contentWidth();
    LineFormattingContext lineFormattingContext(*this, ctx);

    // we dont need gc_allocator here. because frame-tree-item is referenced by its parent
    std::vector<Frame*> result;
    // NOTE this function collect only normal-flow child!
    collectInlineBox(result, this, this);

    for (unsigned i = 0; i < result.size(); i ++) {
        Frame* f = result[i];
        if (f->isFrameText()) {
            String* txt = f->asFrameText()->text();
            //fast path
            if (f == lastChild()) {
                if (txt->containsOnlyWhitespace()) {
                    continue;
                }
            }

            textDividerForLayout(txt, [&](size_t offset, size_t nextOffset, bool isWhiteSpace) {
                textAppendRetry:
                if(lineFormattingContext.m_currentLineWidth == 0 && isWhiteSpace) {
                    return;
                }

                float textWidth;
                String* resultString;
                if(isWhiteSpace) {
                    resultString = String::spaceString;
                    textWidth = f->style()->font()->measureText(String::spaceString);
                }
                else {
                    String* ss = txt->substring(offset,nextOffset - offset);
                    resultString = ss;
                    textWidth = f->style()->font()->measureText(ss);
                }

                if(textWidth <= inlineContentWidth - lineFormattingContext.m_currentLineWidth || lineFormattingContext.m_currentLineWidth == 0) {
                    lineFormattingContext.m_currentLineWidth += textWidth;
                } else {
                    if (isWhiteSpace)
                        return;
                    // try this at nextline
                    m_lineBoxes.push_back(LineBox(this));
                    lineFormattingContext.breakLine();
                    goto textAppendRetry;
                }

                m_lineBoxes[lineFormattingContext.m_currentLine].m_boxes.push_back(new InlineTextBox(f->node(), f->style(), &m_lineBoxes[lineFormattingContext.m_currentLine], resultString));
                m_lineBoxes[lineFormattingContext.m_currentLine].m_boxes.back()->setWidth(textWidth);
                m_lineBoxes[lineFormattingContext.m_currentLine].m_boxes.back()->setHeight(f->style()->font()->metrics().m_fontHeight);
            });

        } else if (f->isFrameReplaced()) {
            FrameReplaced* r = f->asFrameReplaced();
            r->layout(ctx);

            insertReplacedBox:
            if (r->width() < (inlineContentWidth - lineFormattingContext.m_currentLineWidth) || lineFormattingContext.m_currentLineWidth == 0) {
                m_lineBoxes[lineFormattingContext.m_currentLine].m_boxes.push_back(new InlineReplacedBox(f->node(), f->style(), &m_lineBoxes[lineFormattingContext.m_currentLine], r));
                m_lineBoxes[lineFormattingContext.m_currentLine].m_boxes.back()->setWidth(r->width());
                m_lineBoxes[lineFormattingContext.m_currentLine].m_boxes.back()->setHeight(r->height());
                lineFormattingContext.m_currentLineWidth += r->width();
            } else {
                lineFormattingContext.breakLine();
                goto insertReplacedBox;
            }
        } else if (f->isFrameBlockBox()){
            FrameBlockBox* r = f->asFrameBlockBox();
            f->layout(ctx);
            float ascender = 0;
            if (ctx.lastLineBox() && r->isAncestorOf(ctx.lastLineBox())) {
                // TODO consider margin, border, padding
                float topToLineBox = ctx.lastLineBox()->absolutePoint(r).y();
                ascender = topToLineBox + ctx.lastLineBox()->m_ascender;
            }

            insertBlockBox:
            if (r->width() < (inlineContentWidth - lineFormattingContext.m_currentLineWidth) || lineFormattingContext.m_currentLineWidth == 0) {
                m_lineBoxes[lineFormattingContext.m_currentLine].m_boxes.push_back(new InlineBlockBox(f->node(), f->style(), &m_lineBoxes[lineFormattingContext.m_currentLine], r, ascender));
                m_lineBoxes[lineFormattingContext.m_currentLine].m_boxes.back()->setWidth(r->width());
                m_lineBoxes[lineFormattingContext.m_currentLine].m_boxes.back()->setHeight(r->height());
                lineFormattingContext.m_currentLineWidth += r->width();
            } else {
                lineFormattingContext.breakLine();
                goto insertBlockBox;
            }
        } else if (f->isFrameLineBreak()) {
            lineFormattingContext.breakLine();
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
            float diff = (inlineContentWidth - x) / 2;
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
            } else if (ib->isInlineBlockBox()) {
                float dec = -(b.m_boxes[j]->height() - b.m_boxes[j]->asInlineBlockBox()->m_ascender);
                maxAscender = std::max(b.m_boxes[j]->asInlineBlockBox()->m_ascender, maxAscender);
                maxDecender = std::min(dec, dec);
            } else {
                STARFISH_RELEASE_ASSERT_NOT_REACHED();
            }
        }

        b.m_ascender = maxAscender;
        b.m_decender = maxDecender;

        float height = maxAscender - maxDecender;
        for (size_t j = 0; j < b.m_boxes.size(); j ++) {
            InlineBox* ib = b.m_boxes[j];
            if (ib->isInlineTextBox()) {
                ib->setY(height + maxDecender - ib->height() - ib->asInlineTextBox()->style()->font()->metrics().m_descender);
            } else if (ib->isInlineReplacedBox()) {
                ib->setY(height + maxDecender - ib->height());
            } else if (ib->isInlineBlockBox()) {
                float dec = -(b.m_boxes[j]->height() - b.m_boxes[j]->asInlineBlockBox()->m_ascender);
                ib->setY(height + maxDecender - ib->height() - dec);
            }
        }

        b.m_frameRect.setWidth(inlineContentWidth);
        b.m_frameRect.setHeight(height);
        contentHeight += b.m_frameRect.height();
    }

    return contentHeight;
}

void FrameBlockBox::computePreferredWidth(ComputePreferredWidthContext& ctx)
{
    if (hasBlockFlow()) {
        // TODO add margin - border - padding
        if (style()->width().isFixed()) {
            ctx.setResult(style()->width().fixed());
        } else {
            Frame* child = firstChild();
            while (child) {
                child->computePreferredWidth(ctx);
                child = child->next();
            }
        }

    } else {
        // we dont need gc_allocator here. because frame-tree-item is referenced by its parent
        std::vector<Frame*> result;
        // NOTE this function collect only normal-flow child!
        collectInlineBox(result, this, this);

        float currentLineWidth = 0;
        for (unsigned i = 0; i < result.size(); i ++) {
            Frame* f = result[i];
            if (f->isFrameText()) {
                String* s = f->asFrameText()->text();
                textDividerForLayout(s, [&](size_t offset, size_t nextOffset, bool isWhiteSpace){
                    if (currentLineWidth == 0 && isWhiteSpace) {
                        return;
                    }

                    float w = 0;
                    if (isWhiteSpace) {
                        w = f->style()->font()->measureText(String::spaceString);
                    } else {
                        w = f->style()->font()->measureText(s->substring(offset, nextOffset - offset));
                    }

                    if (currentLineWidth + w < ctx.lastKnownWidth()) {
                        currentLineWidth += w;
                    } else {
                        // CHECK THIS
                        // ctx.setResult(currentLineWidth);
                        ctx.setResult(ctx.lastKnownWidth());
                        currentLineWidth = 0;
                    }

                    if (w > ctx.lastKnownWidth()) {
                        ctx.setResult(currentLineWidth);
                        currentLineWidth = 0;
                    }
                });

            } else if (f->isFrameBlockBox()) {
                f->computePreferredWidth(ctx);
                if (f->style()->width().isFixed()) {
                    ctx.setResult(f->style()->width().fixed());
                }
            } else if (f->isFrameLineBreak()) {
                // linebreaks
                ctx.setResult(currentLineWidth);
                currentLineWidth = 0;
            } else {
                STARFISH_ASSERT(f->isFrameReplaced());
                float w = f->asFrameReplaced()->intrinsicSize().width();

                if (f->style()->width().isFixed()) {
                    w = f->style()->width().fixed();
                }

                ctx.setResult(w);

                if (currentLineWidth + w < ctx.lastKnownWidth()) {
                    currentLineWidth += w;
                } else {
                    ctx.setResult(currentLineWidth);
                    currentLineWidth = w;
                }

                if (currentLineWidth > ctx.lastKnownWidth()) {
                    // linebreaks
                    ctx.setResult(currentLineWidth);
                    currentLineWidth = 0;
                }
            }
        }
        ctx.setResult(currentLineWidth);
    }
}

void FrameBlockBox::paintChildrenWith(FrameBlockBox* block, Canvas* canvas, PaintingStage stage)
{
    if (block->hasBlockFlow()) {
        Frame* child = block->firstChild();
        while (child) {
            canvas->save();
            canvas->translate(child->asFrameBox()->x(), child->asFrameBox()->y());
            child->paint(canvas, stage);
            canvas->restore();
            child = child->next();
        }
    } else {
        for (size_t i = 0; i < block->m_lineBoxes.size(); i ++) {
            canvas->save();
            LineBox& b = block->m_lineBoxes[i];
            canvas->translate(b.frameRect().x(), b.frameRect().y());
            for (size_t j = 0; j < b.m_boxes.size(); j ++) {
                canvas->save();
                canvas->translate(b.m_boxes[j]->x(), b.m_boxes[j]->y());
                b.m_boxes[j]->paint(canvas, stage);
                canvas->restore();
            }
            canvas->restore();
        }
    }
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

        // the child stacking contexts with stack level 0 and the positioned descendants with stack level 0.
        child = firstChild();
        while (child) {
            canvas->save();
            canvas->translate(child->asFrameBox()->x(), child->asFrameBox()->y());
            child->paint(canvas, PaintingPositionedElements);
            canvas->restore();
            child = child->next();
        }

        // TODO the child stacking contexts with positive stack levels (least positive first).
    } else if(isPositionedElement()) {
        if (stage == PaintingPositionedElements) {
            paintBackgroundAndBorders(canvas);
            PaintingStage s = PaintingStage::PaintingStackingContext;
            while (s != PaintingStageEnd) {
                paintChildrenWith(this, canvas, s);
                s = (PaintingStage)(s + 1);
            }
        }
    } else {
        if (stage == PaintingNormalFlowBlock) {
            paintBackgroundAndBorders(canvas);
            paintChildrenWith(this, canvas, stage);
        } else {
            paintChildrenWith(this, canvas, stage);
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

        // the child stacking contexts with stack level 0 and the positioned descendants with stack level 0.
        child = lastChild();
        while (child) {
            float cx = x - child->asFrameBox()->x();
            float cy = y - child->asFrameBox()->y();
            result = child->hitTest(cx ,cy , HitTestPositionedElements);
            if (result)
                return result;
            child = child->previous();
        }

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
    } else if(isPositionedElement() && stage == HitTestPositionedElements) {
        Frame* result = nullptr;
        HitTestStage s = HitTestStage::HitTestPositionedElements;
        while (s != HitTestStageEnd) {
            if (hasBlockFlow()) {
                Frame* child = lastChild();
                while (child) {
                    float cx = x - child->asFrameBox()->x();
                    float cy = y - child->asFrameBox()->y();
                    result = child->hitTest(cx ,cy , s);
                    if (result)
                        return result;
                    child = child->previous();
                }
            } else {
                for (size_t i = 0; i < m_lineBoxes.size(); i ++) {
                    LineBox& b = m_lineBoxes[i];
                    float cx = x - b.m_frameRect.x();
                    float cy = y - b.m_frameRect.y();

                    for (size_t j = 0; j < b.m_boxes.size(); j ++) {
                        float cx2 = cx - b.m_boxes[j]->x();
                        float cy2 = cy - b.m_boxes[j]->y();
                        result = b.m_boxes[j]->hitTest(cx2, cy2, s);
                        if (result)
                            return result;
                    }
                }
            }
            s = (HitTestStage)(s + 1);
        }

        return result;
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
        } else {
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

void InlineBlockBox::paint(Canvas* canvas, PaintingStage stage)
{
    if (stage == PaintingNormalFlowInline) {
        STARFISH_ASSERT(!m_frameBlockBox->isEstablishesStackingContext());
        PaintingStage s = PaintingStage::PaintingNormalFlowBlock;
        while (s != PaintingStageEnd) {
            m_frameBlockBox->paint(canvas, s);
            s = (PaintingStage)(s + 1);
        }
    }
}

Frame* InlineBlockBox::hitTest(float x, float y, HitTestStage stage)
{
    if (stage == HitTestStage::HitTestNormalFlowInline) {
        Frame* result = nullptr;
        HitTestStage s = HitTestStage::HitTestPositionedElements;
        while (s != HitTestStageEnd) {
            result = m_frameBlockBox->hitTest(x, y, s);
            if (result)
                return result;
            s = (HitTestStage)(s + 1);
        }
    }
    return nullptr;
}

void FrameBlockBox::dump(int depth)
{
    FrameBox::dump(depth);
    if (!hasBlockFlow()) {
        if (m_lineBoxes.size() && m_lineBoxes[0].m_boxes.size()) {
            for (size_t i = 0; i < m_lineBoxes.size(); i ++) {
                puts("");
                for(int k = 0; k < depth + 1; k ++)
                    printf("  ");
                printf("LineBox(%g,%g,%g,%g)\n", m_lineBoxes[i].m_frameRect.x(), m_lineBoxes[i].m_frameRect.y()
                    , m_lineBoxes[i].m_frameRect.width(), m_lineBoxes[i].m_frameRect.height());

                const LineBox& lb = m_lineBoxes[i];
                for (size_t j = 0; j < lb.m_boxes.size(); j ++) {
                    for(int k = 0; k < depth + 2; k ++)
                        printf("  ");
                    printf("%s", lb.m_boxes[j]->name());
                    lb.m_boxes[j]->dump(depth + 3);
                    if (j != lb.m_boxes.size() - 1)
                        puts("");
                }
            }
        }
    }
}


}
