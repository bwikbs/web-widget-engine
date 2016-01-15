#include "StarFishConfig.h"
#include "FrameBlockBox.h"
#include "FrameText.h"

namespace StarFish {

void FrameBlockBox::layoutBlock(LayoutContext& ctx)
{
    // TODO Determine horizontal margins of this object.
    float parentContentWidth = m_parent->asFrameBlockBox()->contentWidth();

    // https://www.w3.org/TR/CSS2/visudet.html#the-width-property
    // TODO consider negative width
    if (m_style->width().isAuto()) {
        setWidth(parentContentWidth - marginWidth());
    } else if (m_style->width().isFixed()) {
        setWidth(m_style->width().fixed() + paddingWidth() + borderWidth());
    } else if (m_style->width().isPercent()) {
        setWidth(parentContentWidth * m_style->width().percent() + paddingWidth() + borderWidth());
    }

    float normalFlowHeight = 0;

    Frame* child = firstChild();
    while (child) {
        // TODO Place the child.
        child->asFrameBlockBox()->setX(0);
        child->asFrameBlockBox()->setY(normalFlowHeight);

        // Lay out the child
        child->layout(ctx);

        normalFlowHeight += child->asFrameBlockBox()->height();

        child = child->next();
    }

    // Now the intrinsic height of the object is known because the children are placed
    // TODO Determine the final height
    setHeight(normalFlowHeight + paddingHeight() + borderHeight());
}

void FrameBlockBox::layoutInline(LayoutContext& ctx)
{
    m_lineBoxes.clear();
    // m_lineBoxes.shrink_to_fit();

    float parentContentWidth = m_parent->asFrameBlockBox()->contentWidth();
    setWidth(parentContentWidth);

    m_lineBoxes.push_back(LineBox());
    Frame* f = firstChild();
    float nowLineWidth = 0;
    size_t nowLine = 0;
    while (f) {
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
                    textWidth = f->style()->font()->measureText(String::spaceString).width();
                }
                else {
                    String* ss = txt->substring(offset,nextOffset-offset);
                    resultString = ss;
                    textWidth = f->style()->font()->measureText(ss).width();
                }

                if(textWidth <= parentContentWidth - nowLineWidth) {
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
                m_lineBoxes[nowLine].m_boxes.back()->setHeight(f->style()->font()->fontHeight());
                offset = nextOffset;
            }
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
        f = f->next();
    }

    // position each line
    float contentHeight = 0;
    for (size_t i = 0; i < m_lineBoxes.size(); i ++) {
        LineBox& b = m_lineBoxes[i];

        b.m_frameRect.setX(0);
        b.m_frameRect.setY(contentHeight);

        // align boxes
        float x = 0;
        float maxH = 0;
        for (size_t j = 0; j < b.m_boxes.size(); j ++) {
            b.m_boxes[j]->setX(x);
            b.m_boxes[j]->setY(0);
            x += b.m_boxes[j]->width();
            maxH = std::max(maxH, b.m_boxes[j]->height());
        }

        b.m_frameRect.setWidth(x);
        b.m_frameRect.setHeight(maxH);
        contentHeight += maxH;
    }


    // compute object height
    setHeight(contentHeight);
}

void FrameBlockBox::paint(Canvas* canvas)
{
    // TODO draw background color
    // TODO draw background image
    // TODO draw border

    if (hasBlockFlow()) {
        Frame* child = firstChild();
        while (child) {
            canvas->save();
            canvas->translate(child->asFrameBox()->x(), child->asFrameBox()->y());
            child->paint(canvas);
            canvas->restore();
            child = child->next();
        }
    } else {
        for (size_t i = 0; i < m_lineBoxes.size(); i ++) {
            canvas->save();
            LineBox& b = m_lineBoxes[i];
            canvas->translate(b.m_frameRect.x(), b.m_frameRect.y());
            for (size_t j = 0; j < b.m_boxes.size(); j ++) {
                canvas->save();
                canvas->translate(b.m_boxes[j]->x(), b.m_boxes[j]->y());
                b.m_boxes[j]->paint(canvas);
                canvas->restore();
            }
            canvas->restore();
        }
    }
}

}
