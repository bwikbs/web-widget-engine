/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "StarFishConfig.h"
#include "FrameBlockBox.h"
#include "FrameText.h"
#include "FrameInline.h"

namespace StarFish {

static LayoutUnit computeLineHeight(ComputedStyle* style)
{
    LayoutUnit fontSize = style->font()->metrics().m_ascender - style->font()->metrics().m_descender;

    if (!style->hasNormalLineHeight()) {
        if (style->lineHeight().isFixed()) {
            return style->lineHeight().fixed();
        } else if (style->lineHeight().isInheritableNumber()) {
            return fontSize * style->lineHeight().number();
        } else {
            // Only Fixed | InheritableNumer possible here
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    }
    return fontSize;
}

static LayoutUnit computeVerticalProperties(FrameBox* parentBox, ComputedStyle* parentStyle, LayoutUnit& ascenderInOut, LayoutUnit& descenderInOut, LineFormattingContext& ctx, bool dueToBr, bool isLineBox)
{
    LayoutUnit maxAscender = ascenderInOut;
    LayoutUnit maxDescender = descenderInOut;
    bool hasBoxOtherThanText = false;
    bool hasNormalFlowChild = false;

    std::vector<FrameBox*, gc_allocator<FrameBox*> >* boxes;
    if (parentBox->isLineBox()) {
        boxes = &parentBox->asLineBox()->boxes();
        if (!boxes->size()) {
            maxAscender = parentStyle->font()->metrics().m_ascender;
            maxDescender = parentStyle->font()->metrics().m_descender;
        }
    } else {
        boxes = &parentBox->asInlineBox()->asInlineNonReplacedBox()->boxes();
    }

    if (isLineBox) {
        if (!boxes->size()) {
            if (dueToBr) {
                ascenderInOut = maxAscender;
                descenderInOut = maxDescender;
                return ascenderInOut - descenderInOut;
            }
            ascenderInOut = 0;
            descenderInOut = 0;
            return 0;
        }

        bool hasOnlyCollapsedInlineNonReplacedBox = true;
        for (size_t i = 0; i < boxes->size(); i ++) {
            if (boxes->at(i)->isInlineBox() && boxes->at(i)->asInlineBox()->isInlineNonReplacedBox() && boxes->at(i)->asInlineBox()->asInlineNonReplacedBox()->isCollapsed()) {

            } else {
                hasOnlyCollapsedInlineNonReplacedBox = false;
                break;
            }
        }

        if (hasOnlyCollapsedInlineNonReplacedBox) {
            ascenderInOut = 0;
            descenderInOut = 0;
            return 0;
        }
    }

    LayoutUnit pascender = parentStyle->font()->metrics().m_ascender;
    LayoutUnit pdescender = parentStyle->font()->metrics().m_descender;

    // 1. set relative y-pos from baseline (only if it needs)
    // 2. find max ascender and descender
    LayoutUnit maxAscenderSoFar = 0;
    LayoutUnit maxDescenderSoFar = intMaxForLayoutUnit;
    for (size_t k = 0; k < boxes->size(); k ++) {
        FrameBox* box = boxes->at(k);
        if (!box->isNormalFlow()) {
            continue;
        } else {
            hasNormalFlowChild = true;
        }
        VerticalAlignValue va = box->style()->verticalAlign();
        if (box->isInlineBox()) {
            if (box->asInlineBox()->isInlineTextBox()) {
            } else {
                hasBoxOtherThanText = true;
                InlineNonReplacedBox* rb = box->asInlineBox()->asInlineNonReplacedBox();
                if (va == VerticalAlignValue::BaselineVAlignValue) {
                    maxAscenderSoFar = std::max(rb->ascender(), maxAscenderSoFar);
                    maxDescenderSoFar = std::min(rb->decender(), maxDescenderSoFar);
                } else if (va == VerticalAlignValue::TopVAlignValue) {
                    // NO WORK TO DO
                } else if (va == VerticalAlignValue::BottomVAlignValue) {
                    // NO WORK TO DO
                } else if (va == VerticalAlignValue::MiddleVAlignValue) {
                    LayoutUnit halfHeight = rb->height() / 2;
                    LayoutUnit halfXHeight = (parentStyle->font()->metrics().m_xheightRate * parentStyle->font()->size()) / 2;
                    rb->setY(halfHeight + halfXHeight);
                    maxAscenderSoFar = std::max(halfHeight + halfXHeight, maxAscenderSoFar);
                    maxDescenderSoFar = std::min(-1 * (halfHeight - halfXHeight), maxDescenderSoFar);
                } else if (va == VerticalAlignValue::SubVAlignValue) {
                    rb->setY(pdescender + rb->ascender());
                    maxAscenderSoFar = std::max(pdescender + rb->ascender(), maxAscenderSoFar);
                    maxDescenderSoFar = std::min(pdescender + rb->decender(), maxDescenderSoFar);
                } else if (va == VerticalAlignValue::SuperVAlignValue) {
                    // Placing a superscript is font and browser dependent.
                    // We place superscript above the baseline by 1/2 of ascender (following blink)
                    // (i.e, the baseline of superscript is aligned with 1/2 of the ascender)
                    rb->setY(pascender / 2 + rb->ascender());
                    maxAscenderSoFar = std::max(pascender / 2 + rb->ascender(), maxAscenderSoFar);
                    maxDescenderSoFar = std::min(pascender / 2 + rb->decender(), maxDescenderSoFar);
                } else if (va == VerticalAlignValue::TextTopVAlignValue) {
                    maxDescenderSoFar = std::min(pascender - rb->height(), maxDescenderSoFar);
                } else if (va == VerticalAlignValue::TextBottomVAlignValue) {
                    maxAscenderSoFar = std::max(pdescender + rb->height(), maxAscenderSoFar);
                } else if (va == VerticalAlignValue::NumericVAlignValue) {
                    Length len = box->style()->verticalAlignLength();
                    LayoutUnit y;
                    if (len.isPercent()) {
                        y = rb->ascender() + computeLineHeight(box->style()) * len.percent();
                    } else if (len.isFixed()) {
                        y = rb->ascender() + LayoutUnit::fromPixel(len.fixed());
                    }
                    maxAscenderSoFar = std::max(y, maxAscenderSoFar);
                    maxDescenderSoFar = std::min(y - rb->height(), maxDescenderSoFar);
                    rb->setY(y);
                }  else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
            }
        } else if (box->isFrameReplaced()) {
            hasBoxOtherThanText = true;
            LayoutUnit boxHeight = box->height() + box->marginHeight();
            if (va == VerticalAlignValue::BaselineVAlignValue) {
                maxAscenderSoFar = std::max(boxHeight, maxAscenderSoFar);
            } else if (va == VerticalAlignValue::TopVAlignValue) {
                // NO WORK TO DO
            } else if (va == VerticalAlignValue::BottomVAlignValue) {
                // NO WORK TO DO
            } else if (va == VerticalAlignValue::MiddleVAlignValue) {
                LayoutUnit halfHeight = boxHeight / 2;
                LayoutUnit halfXHeight = (parentStyle->font()->metrics().m_xheightRate * parentStyle->font()->size()) / 2;
                box->setY(halfHeight + halfXHeight);
                maxAscenderSoFar = std::max(halfHeight + halfXHeight, maxAscenderSoFar);
                maxDescenderSoFar = std::min(-1 * (halfHeight - halfXHeight), maxDescenderSoFar);
            } else if (va == VerticalAlignValue::SubVAlignValue) {
                box->setY(pdescender + boxHeight);
                maxAscenderSoFar = std::max(pdescender + boxHeight, maxAscenderSoFar);
            } else if (va == VerticalAlignValue::SuperVAlignValue) {
                box->setY(pascender / 2 + boxHeight);
                maxAscenderSoFar = std::max(pascender / 2 + boxHeight, maxAscenderSoFar);
            } else if (va == VerticalAlignValue::TextTopVAlignValue) {
                maxDescenderSoFar = std::min(pascender - boxHeight, maxDescenderSoFar);
            } else if (va == VerticalAlignValue::TextBottomVAlignValue) {
                maxAscenderSoFar = std::max(pdescender + boxHeight, maxAscenderSoFar);
            } else if (va == VerticalAlignValue::NumericVAlignValue) {
                Length len = box->style()->verticalAlignLength();
                LayoutUnit amount;
                if (len.isPercent()) {
                    amount = boxHeight + computeLineHeight(box->style()) * len.percent();
                } else if (len.isFixed()) {
                    amount = boxHeight + LayoutUnit::fromPixel(len.fixed());
                }
                maxAscenderSoFar = std::max(amount, maxAscenderSoFar);
                maxDescenderSoFar = std::min(amount - boxHeight, maxDescenderSoFar);
                box->setY(amount);
            } else {
                STARFISH_RELEASE_ASSERT_NOT_REACHED();
            }
        } else if (box->isFrameBlockBox() && box->style()->display() == InlineBlockDisplayValue) {
            hasBoxOtherThanText = true;
            LayoutUnit boxHeight = box->height() + box->marginHeight();
            if (va == VerticalAlignValue::BaselineVAlignValue) {
                LayoutUnit ascender = ctx.inlineBlockAscender(box->asFrameBlockBox());
                if (ascender == box->height()) {
                    maxAscenderSoFar = std::max(ascender + box->marginHeight(), maxAscenderSoFar);
                    maxDescenderSoFar = std::min(LayoutUnit(0), maxDescenderSoFar);
                } else {
                    LayoutUnit descender = -(box->height() - ascender) - box->marginBottom();
                    maxAscenderSoFar = std::max(ascender + box->marginTop(), maxAscenderSoFar);
                    maxDescenderSoFar = std::min(descender, maxDescenderSoFar);
                }
            } else if (va == VerticalAlignValue::TopVAlignValue) {
                // NO WORK TO DO
            } else if (va == VerticalAlignValue::BottomVAlignValue) {
                // NO WORK TO DO
            } else if (va == VerticalAlignValue::MiddleVAlignValue) {
                LayoutUnit halfHeight = boxHeight / 2;
                LayoutUnit halfXHeight = (parentStyle->font()->metrics().m_xheightRate * parentStyle->font()->size()) / 2;
                box->setY(halfHeight + halfXHeight);
                maxAscenderSoFar = std::max(halfHeight + halfXHeight, maxAscenderSoFar);
                maxDescenderSoFar = std::min(-1 * (halfHeight - halfXHeight), maxDescenderSoFar);
            } else if (va == VerticalAlignValue::SubVAlignValue) {
                // TODO : Need Implement Here
                STARFISH_RELEASE_ASSERT_NOT_REACHED();
            } else if (va == VerticalAlignValue::SuperVAlignValue) {
                // TODO : Need Implement Here
                STARFISH_RELEASE_ASSERT_NOT_REACHED();
            } else if (va == VerticalAlignValue::TextTopVAlignValue) {
                maxDescenderSoFar = std::min(pascender - boxHeight, maxDescenderSoFar);
            } else if (va == VerticalAlignValue::TextBottomVAlignValue) {
                maxAscenderSoFar = std::max(pdescender + boxHeight, maxAscenderSoFar);
            } else if (va == VerticalAlignValue::NumericVAlignValue) {
                LayoutUnit ascender = ctx.inlineBlockAscender(box->asFrameBlockBox());
                Length len = box->style()->verticalAlignLength();
                LayoutUnit amount;
                if (len.isPercent()) {
                    amount = ascender + computeLineHeight(box->style()) * len.percent();
                } else if (len.isFixed()) {
                    amount = ascender + LayoutUnit::fromPixel(len.fixed());
                }
                maxAscenderSoFar = std::max(amount, maxAscenderSoFar);
                maxDescenderSoFar = std::min(amount - ascender, maxDescenderSoFar);
                box->setY(amount);
            } else {
                STARFISH_RELEASE_ASSERT_NOT_REACHED();
            }
        }
    }

    if (!isLineBox && !hasNormalFlowChild && parentBox->width() == 0 && parentBox->marginLeft() == 0 && parentBox->marginRight() == 0 && parentBox->style()->hasNormalLineHeight()) {
        parentBox->asInlineBox()->asInlineNonReplacedBox()->markCollapsed();
        ascenderInOut = pascender;
        descenderInOut = pdescender;
        return pascender - pdescender;
    }

    if (!hasNormalFlowChild && boxes->size() != 0) {
        ascenderInOut = descenderInOut = 0;
        return 0;
    }

    // Consider parent's font ascender/descender
    maxAscender = std::max(maxAscenderSoFar, pascender);
    maxDescender = std::min(maxDescenderSoFar, pdescender);

    // If maxDescenderSoFar is initial value(=intMaxForLayoutUnit), set it to 0.
    maxDescenderSoFar = maxDescenderSoFar.toInt() == intMaxForLayoutUnit ? LayoutUnit(0) : maxDescenderSoFar;

    // 3. adjusting the line height
    if (!parentStyle->hasNormalLineHeight()) {
        LayoutUnit lineHeight = computeLineHeight(parentStyle);
        LayoutUnit diff = (lineHeight - parentStyle->font()->metrics().m_fontHeight) / 2;
        LayoutUnit ascenderShouldBe = parentStyle->font()->metrics().m_ascender + diff;
        LayoutUnit descenderShouldBe = parentStyle->font()->metrics().m_descender - diff;

        if (hasBoxOtherThanText) {
            maxAscender = std::max(maxAscenderSoFar, ascenderShouldBe);
            maxDescender = std::min(maxDescenderSoFar, descenderShouldBe);
        } else {
            // Only Text Children
            maxAscender = ascenderShouldBe;
            maxDescender = descenderShouldBe;
        }
    }

    // VA: top/bottom
    for (size_t k = 0; k < boxes->size(); k ++) {
        FrameBox* box = boxes->at(k);
        VerticalAlignValue va = box->style()->verticalAlign();

        // Ignore non normalFlow
        if (!box->isNormalFlow())
            continue;

        // Ignore inlineBox's marginHeight
        LayoutUnit marginHeight;
        if (!box->isInlineBox())
            marginHeight = box->marginHeight();

        // Update maxAsc & maxDes
        if (!(box->isInlineBox() && box->asInlineBox()->isInlineTextBox())) {
            if (va == VerticalAlignValue::TopVAlignValue) {
                maxDescender = std::min(maxAscender - (box->height() + marginHeight), maxDescender);
            } else if (va == VerticalAlignValue::BottomVAlignValue) {
                maxAscender = std::max(maxDescender + (box->height() + marginHeight), maxAscender);
            }
        }
    }

    ascenderInOut = maxAscender;
    descenderInOut = maxDescender;
    LayoutUnit height = maxAscender - maxDescender;
    for (size_t k = 0; k < boxes->size(); k ++) {
        FrameBox* f = boxes->at(k);

        if (f->isNormalFlow()) {
            if (f->isInlineBox() && f->asInlineBox()->isInlineTextBox()) {
                // InlineBox* ib = f->asFrameBox()->asInlineBox();
                // if (UNLIKELY(descenderInOut == 0)) {
                //     if (height < ib->asInlineTextBox()->style()->font()->metrics().m_fontHeight)
                //         ib->setY((height - ib->asInlineTextBox()->style()->font()->metrics().m_fontHeight + ib->asInlineTextBox()->style()->font()->metrics().m_descender) / 2);
                //     else
                //         ib->setY(height - ib->asInlineTextBox()->style()->font()->metrics().m_fontHeight - ib->asInlineTextBox()->style()->font()->metrics().m_descender);
                // } else {
                //     ib->setY(height + maxDescender - ib->height() - ib->asInlineTextBox()->style()->font()->metrics().m_descender);
                // }
                InlineTextBox* ib = f->asFrameBox()->asInlineBox()->asInlineTextBox();
                ib->setY(maxAscender - ib->style()->font()->metrics().m_ascender);
            } else {
                VerticalAlignValue va = f->style()->verticalAlign();
                LayoutUnit marginTop, marginRight, marginBottom, marginLeft;
                if (!f->isInlineBox()) {
                    marginTop = f->marginTop();
                    marginRight = f->marginRight();
                    marginBottom = f->marginBottom();
                    marginLeft = f->marginLeft();
                }
                if (va == VerticalAlignValue::BaselineVAlignValue) {
                    if (f->isInlineBox() && f->asInlineBox()->isInlineNonReplacedBox()) {
                        f->setY(height + maxDescender - f->height() - f->asInlineBox()->asInlineNonReplacedBox()->decender());
                    } else if (f->isFrameReplaced()) {
                        // TODO use this code for when replaced content does not have content
                        /*
                           LayoutUnit asc = ib->marginTop() + ib->asInlineReplacedBox()->replacedBox()->borderTop() + ib->asInlineReplacedBox()->replacedBox()->paddingTop()
                           + ib->asInlineReplacedBox()->replacedBox()->contentHeight();
                           ib->setY(height + maxDescender - asc + ib->marginTop());
                           */
                        f->setY(maxAscender - f->height() - marginBottom);
                    } else {
                        STARFISH_ASSERT(f->isFrameBlockBox() && f->style()->display() == InlineBlockDisplayValue);
                        LayoutUnit ascender = ctx.inlineBlockAscender(f->asFrameBlockBox());
                        if (ascender == f->height()) {
                            f->setY(maxAscender - ascender - marginBottom);
                        } else {
                            f->setY(maxAscender - ascender);
                        }
                    }
                // 4. convert a y pos relative to the baseline to a y pos relative to the top-left corner of the box
                } else if (va == VerticalAlignValue::TopVAlignValue) {
                    f->setY(marginTop);
                } else if (va == VerticalAlignValue::BottomVAlignValue) {
                    f->setY(height - f->height() - marginBottom);
                } else if (va == VerticalAlignValue::MiddleVAlignValue) {
                    f->setY(maxAscender - f->y() + marginTop);
                } else if (va == VerticalAlignValue::SubVAlignValue) {
                    f->setY(maxAscender - f->y() + marginTop);
                } else if (va == VerticalAlignValue::SuperVAlignValue) {
                    // pascender / 2 + ib->asInlineNonReplacedBox()->ascender()
                    f->setY(maxAscender - f->y() + marginTop);
                } else if (va == VerticalAlignValue::TextTopVAlignValue) {
                    f->setY(maxAscender - pascender + marginTop);
                } else if (va == VerticalAlignValue::TextBottomVAlignValue) {
                    f->setY(maxAscender - pdescender - f->height() - marginBottom);
                } else if (va == VerticalAlignValue::NumericVAlignValue) {
                    f->setY(maxAscender - f->y() + marginTop);
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
            }
        } else {
            // out of flow boxes
            STARFISH_ASSERT(!f->isNormalFlow());
        }
    }

    return ascenderInOut - descenderInOut;
}

static FrameBox* findLastInlineBoxNonReplacedBoxCase(InlineNonReplacedBox* b)
{
    FrameBox* result = nullptr;

    auto iter = b->boxes().begin();
    while (iter != b->boxes().end()) {
        FrameBox* f = *iter;
        if (f->asFrameBox()->isInlineBox()) {
            InlineBox* ib = f->asFrameBox()->asInlineBox();
            if (ib->isInlineNonReplacedBox()) {
                auto r = findLastInlineBoxNonReplacedBoxCase(ib->asInlineNonReplacedBox());
                if (r) {
                    result = r;
                }
            } else {
                result = ib;
            }
        } else if (f->isNormalFlow()) {
            result = *iter;
        }
        iter++;
    }

    return result;
}

static FrameBox* findLastInlineBox(LineBox* lb)
{
    FrameBox* result = nullptr;
    for (size_t i = 0; i < lb->boxes().size(); i ++) {
        if (lb->boxes()[i]->isInlineBox()) {
            InlineBox* b = lb->boxes()[i]->asInlineBox();
            if (b->isInlineNonReplacedBox()) {
                auto r = findLastInlineBoxNonReplacedBoxCase(b->asInlineNonReplacedBox());
                if (r) {
                    result = r;
                }
            } else {
                result = b;
            }
        } else if (lb->boxes()[i]->isNormalFlow()) {
            result = lb->boxes()[i];
        }
    }

    return result;
}

static char charDirection(char32_t c)
{
    auto property = u_getIntPropertyValue(c, UCHAR_BIDI_CLASS);
    if ((property == U_RIGHT_TO_LEFT) || (property == U_RIGHT_TO_LEFT_ARABIC) || (property == U_RIGHT_TO_LEFT_EMBEDDING) || (property == U_RIGHT_TO_LEFT_OVERRIDE)) {
        return 0;
    } else if ((property == U_LEFT_TO_RIGHT) || (property == U_LEFT_TO_RIGHT_EMBEDDING) || (property == U_LEFT_TO_RIGHT_OVERRIDE)) {
        return 1;
    } else {
        return 2;
    }
}

static InlineTextBox::CharDirection charDirFromICUDir(UBiDiDirection dir)
{
    if (dir == UBIDI_LTR) {
        return InlineTextBox::CharDirection::Ltr;
    } else if (dir == UBIDI_RTL) {
        return InlineTextBox::CharDirection::Rtl;
    } else if (dir == UBIDI_NEUTRAL) {
        return InlineTextBox::CharDirection::Netural;
    } else {
        STARFISH_ASSERT(dir == UBIDI_MIXED);
        return InlineTextBox::CharDirection::Mixed;
    }
}

static UBiDiDirection getTextDir(String* text)
{
    UTF16String str = text->toUTF16String();
    UBiDiDirection dir = ubidi_getBaseDirection((const UChar*)str.data(), str.length());
    return dir;
}

static bool isNumberChar(char32_t d)
{
    if (('0' <= d && d <= '9')
        || (0x0660 <= d && d <= 0x0669) // 0 to 9 in Arabic
        || (0x06F0 <= d && d <= 0x06F9)) { // 0 to 9 in Extended Arabic
        return true;
    } else {
        return false;
    }
}

static bool isNumber(String* text)
{
    for (unsigned i = 0; i < text->length(); i++) {
        if (isNumberChar(text->charAt(i))) {
        } else {
            return false;
        }
    }
    return true;
}

static bool hasIsolateBidiContent(InlineNonReplacedBox* box)
{
    // TODO add isolate
    if (box->style()->unicodeBidi() == UnicodeBidiValue::EmbedUnicodeBidiValue) {
        return true;
    }
    return false;
}

static FrameBox* fetchContentForResolveBidi(FrameBox* box)
{
    if (box->isInlineBox()) {
        if (box->asInlineBox()->isInlineNonReplacedBox()) {
            InlineNonReplacedBox* b = box->asInlineBox()->asInlineNonReplacedBox();
            if (hasIsolateBidiContent(b)) {
                return b;
            }
            if (b->boxes().size()) {
                STARFISH_ASSERT(b->boxes().size() == 1);
                return fetchContentForResolveBidi(b->boxes().at(0));
            }
            return b;
        } else {
            return box;
        }
    } else {
        return box;
    }
}

struct TextRun {
    String* m_text;
    InlineTextBox::CharDirection m_direction;

    TextRun(String* txt, InlineTextBox::CharDirection dir)
    {
        m_text = txt;
        m_direction = dir;
    }
};

static void breakTextRun(std::vector<TextRun, gc_allocator<TextRun> >& runs, size_t breakRunIndex, size_t breakTextIndex)
{
    TextRun target = runs[breakRunIndex];
    runs.erase(runs.begin() + breakRunIndex);

    String* newText = target.m_text->substring(0, breakTextIndex + 1);
    runs.insert(runs.begin() + breakRunIndex++, TextRun(newText, charDirFromICUDir(getTextDir(newText))));

    newText = target.m_text->substring(breakTextIndex + 1, target.m_text->length() - breakTextIndex - 1);
    runs.insert(runs.begin() + breakRunIndex, TextRun(newText, charDirFromICUDir(getTextDir(newText))));
}

static std::vector<TextRun, gc_allocator<TextRun> > textBidiResolver(String* txt, DirectionValue parentDir)
{
    std::vector<TextRun, gc_allocator<TextRun> > result;
    UBiDi* bidi = ubidi_open();
    UTF16String str = txt->toUTF16String();
    UErrorCode err = (UErrorCode)0;
    ubidi_setPara(bidi, (const UChar*)str.data(), str.length(), parentDir == DirectionValue::LtrDirectionValue ? UBIDI_DEFAULT_LTR : UBIDI_DEFAULT_RTL, NULL, &err);
    STARFISH_ASSERT(U_SUCCESS(err));
    size_t total = ubidi_countRuns(bidi, &err);
    STARFISH_ASSERT(U_SUCCESS(err));
    if (total == 1) {
        UBiDiDirection dir = getTextDir(txt);
        result.push_back(TextRun(txt, charDirFromICUDir(dir)));
    } else {
        int32_t start = 0;
        int32_t end;
        for (size_t i = 0; i < total; i ++) {
            ubidi_getLogicalRun(bidi, start, &end, NULL);
            String* t = txt->substring(start, end-start);
            start = end;
            UBiDiDirection dir = getTextDir(t);
            result.push_back(TextRun(t, charDirFromICUDir(dir)));
        }
    }
    ubidi_close(bidi);

    for (size_t i = 0; i < result.size(); i ++) {
        if (isNumber(result[i].m_text)) {
            result[i].m_direction = InlineTextBox::CharDirection::Netural;
            continue;
        }

        // check it has leading netural chars
        char32_t first = result[i].m_text->charAt(0);
        if (result[i].m_text->length() > 1 && charDirection(first) == 2 && !isNumberChar(first)) {
            breakTextRun(result, i, 0);
            i--;
            continue;
        }

        // check it has trailing neutral chars
        if (result[i].m_text->length() > 1) {
            size_t lastPos = result[i].m_text->length() - 1;
            char32_t last = result[i].m_text->charAt(lastPos);
            if (charDirection(last) == 2 && !isNumberChar(last)) {
                breakTextRun(result, i, lastPos - 1);
                i--;
                continue;
            }
        }
    }

    return result;
}

void splitInlineBoxesAndMarkDirectionForResolveBidi(LineFormattingContext& ctx, DirectionValue parentDir, std::vector<FrameBox*, gc_allocator<FrameBox*>>& boxes)
{
    for (size_t i = 0; i < boxes.size(); i ++) {
        FrameBox* box = boxes[i];
        if (box->isInlineBox()) {
            InlineBox* ib = box->asInlineBox();
            if (ib->isInlineTextBox()) {
                InlineTextBox* tb = ib->asInlineTextBox();
                std::vector<TextRun, gc_allocator<TextRun> > runs = textBidiResolver(tb->text(), parentDir);
                if (runs.size() == 1) {
                    tb->setCharDirection(runs[0].m_direction);
                } else {
                    LayoutUnit orgHeight = tb->height();
                    size_t insertPos = i;
                    boxes.erase(boxes.begin() + i);
                    LayoutUnit x = tb->x();
                    LayoutUnit y = tb->y();
                    for (size_t j = 0; j < runs.size(); j++) {
                        InlineTextBox* box = new InlineTextBox(tb->node(), tb->style(), nullptr, runs[j].m_text, tb->origin(), runs[j].m_direction);
                        box->setLayoutParent(tb->layoutParent());
                        box->setX(x);
                        box->setY(y);
                        LayoutUnit width = tb->style()->font()->measureText(runs[j].m_text);
                        x += width;
                        box->setWidth(width);
                        box->setHeight(tb->height());
                        boxes.insert(boxes.begin() + insertPos++, box);
                    }
                    i += (runs.size() - 1);
                }
            } else if (ib->isInlineNonReplacedBox()) {
                if (!hasIsolateBidiContent(ib->asInlineNonReplacedBox())) {
                    splitInlineBoxesAndMarkDirectionForResolveBidi(ctx, parentDir, ib->asInlineNonReplacedBox()->boxes());
                    // split every content for resolve bidi after
                    InlineNonReplacedBox* inrb = ib->asInlineNonReplacedBox();
                    STARFISH_ASSERT(ctx.m_dataForRestoreLeftRightOfMBPAfterResolveBidiLinePerLine.find(inrb->origin()) == ctx.m_dataForRestoreLeftRightOfMBPAfterResolveBidiLinePerLine.end());

                    DataForRestoreLeftRightOfMBPAfterResolveBidiLinePerLine mbpData;

                    mbpData.m_margin = inrb->m_margin;
                    mbpData.m_border = inrb->m_border;
                    mbpData.m_padding = inrb->m_padding;
                    mbpData.m_orgMargin = inrb->m_orgMargin;
                    mbpData.m_orgBorder = inrb->m_orgBorder;
                    mbpData.m_orgPadding = inrb->m_orgPadding;

                    ctx.m_dataForRestoreLeftRightOfMBPAfterResolveBidiLinePerLine[inrb->origin()] = mbpData;

                    std::vector<FrameBox*, gc_allocator<FrameBox*>>& boxesNeedsCopy = inrb->boxes();
                    if (boxesNeedsCopy.size()) {
                        FrameBox* parent = ib->layoutParent()->asFrameBox();
                        LayoutUnit x = box->x();

                        STARFISH_ASSERT(boxesNeedsCopy.size());
                        boxes.erase(boxes.begin() + i);

                        size_t insertPos = i;

                        for (size_t j = 0; j < boxesNeedsCopy.size(); j++) {
                            InlineNonReplacedBox* newBox = new InlineNonReplacedBox(ib->node(), ib->style(), nullptr, inrb->origin());
                            newBox->setX(boxesNeedsCopy[j]->x() + ib->x());
                            newBox->setY(ib->y());
                            boxesNeedsCopy[j]->setX(0);
                            boxesNeedsCopy[j]->setLayoutParent(newBox);
                            newBox->boxes().push_back(boxesNeedsCopy[j]);
                            newBox->setLayoutParent(parent);
                            if (boxesNeedsCopy[j]->isNormalFlow()) {
                                newBox->setWidth(boxesNeedsCopy[j]->width());
                            }
                            newBox->setHeight(ib->height());
                            newBox->setAscender(inrb->ascender());
                            newBox->setDecender(inrb->decender());
                            newBox->m_orgMargin = inrb->m_orgMargin;
                            newBox->m_orgMargin.setLeft(0);
                            newBox->m_orgMargin.setRight(0);
                            newBox->m_orgBorder = inrb->m_orgBorder;
                            newBox->m_orgBorder.setLeft(0);
                            newBox->m_orgBorder.setRight(0);
                            newBox->m_orgPadding = inrb->m_orgPadding;
                            newBox->m_orgPadding.setLeft(0);
                            newBox->m_orgPadding.setRight(0);
                            boxes.insert(boxes.begin() + insertPos++, newBox);
                        }

                        i += (boxesNeedsCopy.size() - 1);
                    } else {
                        inrb->setWidth(0);
                        inrb->setMarginLeft(0);
                        inrb->setBorderLeft(0);
                        inrb->setPaddingLeft(0);
                        inrb->setMarginRight(0);
                        inrb->setBorderRight(0);
                        inrb->setPaddingRight(0);
                        inrb->m_margin.setLeft(0);
                        inrb->m_border.setLeft(0);
                        inrb->m_padding.setLeft(0);
                        inrb->m_margin.setRight(0);
                        inrb->m_border.setRight(0);
                        inrb->m_padding.setRight(0);
                    }
                }
            }
        }
    }
}

static InlineTextBox::CharDirection contentDir(FrameBox* box)
{
    if (box->isInlineBox()) {
        InlineBox* ib = box->asInlineBox();
        if (ib->isInlineTextBox()) {
            return ib->asInlineTextBox()->charDirection();
        } else if (ib->isInlineNonReplacedBox()) {
            InlineNonReplacedBox* b = ib->asInlineNonReplacedBox();
            if (hasIsolateBidiContent(b)) {
                // TODO
                // when unicode-bidi property is isolate, we should return netural direction
                return b->style()->direction() == DirectionValue::LtrDirectionValue ? InlineTextBox::CharDirection::Ltr : InlineTextBox::CharDirection::Rtl;
            } else {
                const std::vector<FrameBox*, gc_allocator<FrameBox*>>& boxes = b->boxes();
                for (size_t i = 0; i < boxes.size(); i ++) {
                    InlineTextBox::CharDirection dir = contentDir(boxes[i]);
                    if (dir == InlineTextBox::CharDirection::Ltr) {
                        return InlineTextBox::CharDirection::Ltr;
                    } else if (dir == InlineTextBox::CharDirection::Rtl) {
                        return InlineTextBox::CharDirection::Rtl;
                    } else { // netural
                    }
                }
                return InlineTextBox::CharDirection::Netural;
            }
        } else {
            return InlineTextBox::CharDirection::Netural;
        }
    } else {
        return InlineTextBox::CharDirection::Netural;
    }
}

void reassignLeftRightMBPOfInlineNonReplacedBoxPreProcess(LineFormattingContext& ctx, std::vector<FrameBox*, gc_allocator<FrameBox*>>& boxes)
{
    for (size_t i = 0; i < boxes.size(); i ++) {
        FrameBox* box = boxes[i];
        if (box->isInlineBox()) {
            InlineBox* ib = box->asInlineBox();
            if (ib->isInlineTextBox()) {
            } else if (ib->isInlineNonReplacedBox()) {
                if (!hasIsolateBidiContent(ib->asInlineNonReplacedBox())) {
                    reassignLeftRightMBPOfInlineNonReplacedBoxPreProcess(ctx, ib->asInlineNonReplacedBox()->boxes());

                    // find last box
                    FrameInline* origin = ib->asInlineNonReplacedBox()->origin();
                    ctx.m_checkLastInlineNonReplacedPerLine[origin] = ib->asInlineNonReplacedBox();
                }
            }
        }
    }
}

void reassignLeftRightMBPOfInlineNonReplacedBox(LineFormattingContext& ctx, std::vector<FrameBox*, gc_allocator<FrameBox*>>& boxes)
{
    for (size_t i = 0; i < boxes.size(); i ++) {
        FrameBox* box = boxes[i];
        if (box->isInlineBox()) {
            InlineBox* ib = box->asInlineBox();
            if (ib->isInlineTextBox()) {
            } else if (ib->isInlineNonReplacedBox()) {
                if (!hasIsolateBidiContent(ib->asInlineNonReplacedBox())) {
                    reassignLeftRightMBPOfInlineNonReplacedBox(ctx, ib->asInlineNonReplacedBox()->boxes());

                    // restore left
                    FrameInline* origin = ib->asInlineNonReplacedBox()->origin();
                    auto iter = ctx.m_dataForRestoreLeftRightOfMBPAfterResolveBidiLinePerLine.find(origin);

                    if (!iter->second.m_isFirstEdgeProcessed) {
                        ib->asInlineNonReplacedBox()->m_orgPadding.setLeft(iter->second.m_orgPadding.left());
                        ib->asInlineNonReplacedBox()->m_orgBorder.setLeft(iter->second.m_orgBorder.left());
                        ib->asInlineNonReplacedBox()->m_orgMargin.setLeft(iter->second.m_orgMargin.left());

                        ib->setMarginLeft(iter->second.m_margin.left());
                        ib->setBorderLeft(iter->second.m_border.left());
                        ib->setPaddingLeft(iter->second.m_padding.left());

                        LayoutUnit widthNeedToPropagateToParent;
                        widthNeedToPropagateToParent = iter->second.m_margin.left();
                        ib->moveX(widthNeedToPropagateToParent);
                        LayoutUnit bp = iter->second.m_border.left() + iter->second.m_padding.left();
                        widthNeedToPropagateToParent += bp;
                        if (ib->asInlineNonReplacedBox()->boxes().size()) {
                            STARFISH_ASSERT(ib->asInlineNonReplacedBox()->boxes().size() == 1);
                            ib->asInlineNonReplacedBox()->boxes()[0]->moveX(bp);
                        }
                        ib->setWidth(ib->width() + bp);

                        if (widthNeedToPropagateToParent) {
                            FrameBox* parent = ib->layoutParent()->asFrameBox();
                            while (parent->isInlineBox() && parent->asInlineBox()->isInlineNonReplacedBox()) {
                                parent->setWidth(parent->width() + widthNeedToPropagateToParent);
                                parent = parent->layoutParent()->asFrameBox();
                            }
                        }
                        iter->second.m_isFirstEdgeProcessed = true;
                    }

                    // restore right
                    if (ctx.m_checkLastInlineNonReplacedPerLine[origin] == ib) {
                        ib->asInlineNonReplacedBox()->m_orgPadding.setRight(iter->second.m_orgPadding.right());
                        ib->asInlineNonReplacedBox()->m_orgBorder.setRight(iter->second.m_orgBorder.right());
                        ib->asInlineNonReplacedBox()->m_orgMargin.setRight(iter->second.m_orgMargin.right());

                        ib->setMarginRight(iter->second.m_margin.right());
                        ib->setBorderRight(iter->second.m_border.right());
                        ib->setPaddingRight(iter->second.m_padding.right());

                        LayoutUnit widthNeedToPropagateToParent;
                        widthNeedToPropagateToParent = iter->second.m_margin.right();
                        LayoutUnit bp = iter->second.m_border.right() + iter->second.m_padding.right();
                        widthNeedToPropagateToParent += bp;
                        ib->setWidth(ib->width() + bp);

                        if (widthNeedToPropagateToParent) {
                            FrameBox* parent = ib->layoutParent()->asFrameBox();
                            while (parent->isInlineBox() && parent->asInlineBox()->isInlineNonReplacedBox()) {
                                parent->setWidth(parent->width() + widthNeedToPropagateToParent);
                                parent = parent->layoutParent()->asFrameBox();
                            }
                        }
                    }
                }
            }
        }
    }
}

static void resolveBidi(LineFormattingContext& ctx, DirectionValue parentDir, std::vector<FrameBox*, gc_allocator<FrameBox*>>& boxes)
{
    splitInlineBoxesAndMarkDirectionForResolveBidi(ctx, parentDir, boxes);

    if (parentDir == DirectionValue::RtlDirectionValue) {
        // find inline Text Boxes has only number for
        // <LTR> <Number> <RTL> case
        // Number following LTR Text should be LTR
        // <LTR> <Number> <RTL> -> <LTR> <Number-LTR not netural> <RTL>
        for (size_t i = 0; i < boxes.size(); i ++) {
            FrameBox* box = fetchContentForResolveBidi(boxes[i]);
            if (box->isInlineBox()) {
                InlineBox* ib = box->asInlineBox();
                if (ib->isInlineTextBox()) {
                    if (isNumber(ib->asInlineTextBox()->text())) {
                        for (size_t j = i - 1; j != SIZE_MAX; j--) {
                            FrameBox* box2 = fetchContentForResolveBidi(boxes[j]);
                            if (box2->isInlineBox()) {
                                InlineBox* ib2 = box2->asInlineBox();
                                if (ib2->isInlineTextBox()) {
                                    if (ib2->asInlineTextBox()->charDirection() == InlineTextBox::CharDirection::Ltr) {
                                        ib->asInlineTextBox()->setCharDirection(InlineTextBox::CharDirection::Ltr);
                                        break;
                                    } else if (ib2->asInlineTextBox()->charDirection() == InlineTextBox::CharDirection::Rtl) {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        // find inline Text Boxes has only number for
        // <RTL> <Number> <LTR> case
        // Number following RTL Text should be RTL
        // <RTL> <Number> <LTR> -> <RTL> <Number-RTL not netural> <LTR>
        for (size_t i = 0; i < boxes.size(); i ++) {
            FrameBox* box = fetchContentForResolveBidi(boxes[i]);
            if (box->isInlineBox()) {
                InlineBox* ib = box->asInlineBox();
                if (ib->isInlineTextBox()) {
                    if (isNumber(ib->asInlineTextBox()->text())) {
                        for (size_t j = i - 1; j != SIZE_MAX; j--) {
                            FrameBox* box2 = fetchContentForResolveBidi(boxes[j]);
                            if (box2->isInlineBox()) {
                                InlineBox* ib2 = box2->asInlineBox();
                                if (ib2->isInlineTextBox()) {
                                    if (ib2->asInlineTextBox()->charDirection() == InlineTextBox::CharDirection::Rtl) {
                                        ib->asInlineTextBox()->setCharDirection(InlineTextBox::CharDirection::Rtl);
                                        break;
                                    } else if (ib2->asInlineTextBox()->charDirection() == InlineTextBox::CharDirection::Ltr) {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

#ifndef NDEBUG
    for (size_t i = 0; i < boxes.size(); i ++) {
        FrameBox* box = fetchContentForResolveBidi(boxes[i]);
        if (box->isInlineBox()) {
            InlineBox* ib = box->asInlineBox();
            if (ib->isInlineTextBox()) {
                InlineTextBox* tb = ib->asInlineTextBox();
                STARFISH_ASSERT(tb->charDirection() != InlineTextBox::CharDirection::Mixed);
            }
        }
    }
#endif

    if (parentDir == DirectionValue::LtrDirectionValue) {
        std::vector<FrameBox*, gc_allocator<FrameBox*>> oldBoxes = std::move(boxes);
        std::vector<FrameBox*> rtlStorage; // use normal allocator because oldBoxes has strong reference

        InlineTextBox::CharDirection currentDirection = InlineTextBox::CharDirection::Ltr;
        for (size_t i = 0; i < oldBoxes.size(); i ++) {
            InlineTextBox::CharDirection currentDirectionBefore = currentDirection;
            FrameBox* box = fetchContentForResolveBidi(oldBoxes[i]);
            InlineTextBox::CharDirection dir = contentDir(box);
            if (dir == InlineTextBox::CharDirection::Netural) {
                InlineTextBox::CharDirection nextDir = InlineTextBox::CharDirection::Ltr;
                for (size_t j = i + 1; j < oldBoxes.size(); j ++) {
                    nextDir = contentDir(fetchContentForResolveBidi(oldBoxes[j]));
                    if (nextDir != InlineTextBox::CharDirection::Netural) {
                        break;
                    }
                }

                if (currentDirection == InlineTextBox::CharDirection::Rtl && nextDir == InlineTextBox::CharDirection::Netural) {
                    currentDirection = InlineTextBox::CharDirection::Ltr;
                } else if (currentDirection == InlineTextBox::CharDirection::Rtl && nextDir == InlineTextBox::CharDirection::Ltr) {
                    currentDirection = InlineTextBox::CharDirection::Ltr;
                }

                if (box->isInlineBox()) {
                    InlineBox* ib = box->asInlineBox();
                    if (ib->isInlineTextBox()) {
                        String* text = ib->asInlineTextBox()->text();
                        if (text->containsOnlyWhitespace()) {
                        } else {
                            if (isNumber(text)) {
                                currentDirection = currentDirectionBefore;
                            }
                            ib->asInlineTextBox()->setCharDirection(currentDirection);
                        }
                    }
                }
            } else {
                currentDirection = dir;
            }

            if (currentDirection == InlineTextBox::CharDirection::Ltr) {
                if (rtlStorage.size()) {
                    boxes.insert(boxes.end(), rtlStorage.begin(), rtlStorage.end());
                    rtlStorage.clear();
                }
                boxes.push_back(oldBoxes[i]);
            } else {
                rtlStorage.insert(rtlStorage.begin(), oldBoxes[i]);
            }
        }

        if (rtlStorage.size()) {
            boxes.insert(boxes.end(), rtlStorage.begin(), rtlStorage.end());
            rtlStorage.clear();
        }
    } else {
        std::vector<FrameBox*, gc_allocator<FrameBox*>> oldBoxes = std::move(boxes);
        std::vector<FrameBox*> ltrStorage; // use normal allocator because oldBoxes has strong reference

        InlineTextBox::CharDirection currentDirection = InlineTextBox::CharDirection::Rtl;
        for (size_t i = 0; i < oldBoxes.size(); i ++) {
            InlineTextBox::CharDirection currentDirectionBefore = currentDirection;
            FrameBox* box = fetchContentForResolveBidi(oldBoxes[i]);
            InlineTextBox::CharDirection dir = contentDir(box);
            if (dir == InlineTextBox::CharDirection::Netural) {
                InlineTextBox::CharDirection nextDir = InlineTextBox::CharDirection::Rtl;
                for (size_t j = i + 1; j < oldBoxes.size(); j ++) {
                    nextDir = contentDir(fetchContentForResolveBidi(oldBoxes[j]));
                    if (nextDir != InlineTextBox::CharDirection::Netural) {
                        break;
                    }
                }

                if (currentDirection == InlineTextBox::CharDirection::Ltr && nextDir == InlineTextBox::CharDirection::Netural) {
                    currentDirection = InlineTextBox::CharDirection::Rtl;
                } else if (currentDirection == InlineTextBox::CharDirection::Ltr && nextDir == InlineTextBox::CharDirection::Rtl) {
                    currentDirection = InlineTextBox::CharDirection::Rtl;
                }

                if (box->isInlineBox()) {
                    InlineBox* ib = box->asInlineBox();
                    if (ib->isInlineTextBox()) {
                        String* text = ib->asInlineTextBox()->text();
                        if (text->containsOnlyWhitespace()) {
                        } else {
                            if (isNumber(text)) {
                                currentDirection = currentDirectionBefore;
                            }
                            ib->asInlineTextBox()->setCharDirection(currentDirection);
                        }
                    }
                }
            } else {
                currentDirection = dir;
            }

            if (currentDirection == InlineTextBox::CharDirection::Rtl) {
                if (ltrStorage.size()) {
                    boxes.insert(boxes.begin(), ltrStorage.begin(), ltrStorage.end());
                    ltrStorage.clear();
                }
                boxes.insert(boxes.begin(), oldBoxes[i]);
            } else {
                ltrStorage.push_back(oldBoxes[i]);
            }
        }

        if (ltrStorage.size()) {
            boxes.insert(boxes.begin(), ltrStorage.begin(), ltrStorage.end());
            ltrStorage.clear();
        }
    }
    // reverse parenthesis chars
    static const char parenthesisMap[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        41, 40, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        62, 0, 60, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 93, 0, 91, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 123, 0, 125, 0, 0
    };

    for (size_t i = 0; i < boxes.size(); i ++) {
        FrameBox* box = fetchContentForResolveBidi(boxes[i]);
        if (box->isInlineBox()) {
            if (box->asInlineBox()->isInlineTextBox() && box->asInlineBox()->asInlineTextBox()->charDirection() == InlineTextBox::CharDirection::Rtl) {
                InlineTextBox* b = box->asInlineBox()->asInlineTextBox();
                String* t = b->text();
                bool shouldReplaceString = false;
                for (size_t j = 0; j < t->length(); j++) {
                    char32_t ch = t->charAt(j);
                    if (ch < 128) {
                        if (parenthesisMap[ch]) {
                            shouldReplaceString = true;
                            break;
                        }
                    }
                }

                if (shouldReplaceString) {
                    if (t->isASCIIString()) {
                        ASCIIString str;
                        for (size_t j = 0; j < t->length(); j++) {
                            char32_t ch = t->charAt(j);
                            if (ch < 128) {
                                if (parenthesisMap[ch]) {
                                    ch = (char32_t) parenthesisMap[ch];
                                }
                            }
                            str += (char)ch;
                        }
                        t = new StringDataASCII(std::move(str));
                    } else {
                        UTF32String str;
                        for (size_t j = 0; j < t->length(); j++) {
                            char32_t ch = t->charAt(j);
                            if (ch < 128) {
                                if (parenthesisMap[ch]) {
                                    ch = (char32_t) parenthesisMap[ch];
                                }
                            }
                            str += ch;
                        }
                        t = new StringDataUTF32(std::move(str));
                    }
                    b->setText(t);
                }
            }
        }
    }


    // reassign left, right mbp for inlineNonReplacedBox
    reassignLeftRightMBPOfInlineNonReplacedBoxPreProcess(ctx, boxes);
    reassignLeftRightMBPOfInlineNonReplacedBox(ctx, boxes);

    ctx.m_dataForRestoreLeftRightOfMBPAfterResolveBidiLinePerLine.clear();
    ctx.m_checkLastInlineNonReplacedPerLine.clear();
}

static void removeBoxFromLine(FrameBox* box)
{
    if (box->layoutParent()->asFrameBox()->isLineBox()) {
        std::vector<FrameBox*, gc_allocator<FrameBox*> >& boxes = box->layoutParent()->asFrameBox()->asLineBox()->boxes();
        boxes.erase(std::find(boxes.begin(), boxes.end(), box));
    } else {
        std::vector<FrameBox*, gc_allocator<FrameBox*> >& boxes = box->layoutParent()->asFrameBox()->asInlineBox()->asInlineNonReplacedBox()->boxes();
        auto self = box->layoutParent()->asFrameBox()->asInlineBox()->asInlineNonReplacedBox();
        LayoutUnit w = box->asInlineBox()->asInlineTextBox()->width();
        while (true) {
            self->setWidth(self->width() - w);
            if (self->layoutParent()->asFrameBox()->isLineBox())
                break;
            self = self->layoutParent()->asFrameBox()->asInlineBox()->asInlineNonReplacedBox();
        }
        boxes.erase(std::find(boxes.begin(), boxes.end(), box));
    }
}

void LineFormattingContext::registerInlineContent()
{
    if (m_block.m_lineBoxes.size()) {
        LineBox* lb = m_block.m_lineBoxes.back();
        if (lb->boxes().size())
            m_layoutContext.registerYPositionForVerticalAlignInlineBlock(lb);
    }
}

void LineFormattingContext::completeLastLine()
{
    LineBox* back = m_block.m_lineBoxes.back();
    FrameBox* last = nullptr;
    while ((last = findLastInlineBox(back), last) && last->isInlineBox() && last->asInlineBox()->isInlineTextBox()) {
        if (last->asInlineBox()->asInlineTextBox()->text()->equals(String::spaceString)) {
            removeBoxFromLine(last);
        } else {
            break;
        }
    }

    resolveBidi(*this, m_block.style()->direction(), back->boxes());

    LayoutUnit x;
    for (size_t k = 0; k < back->m_boxes.size(); k++) {
        FrameBox* childBox = back->m_boxes[k];
        if (childBox->isNormalFlow()) {
            childBox->setX(x + childBox->marginLeft());
            x += childBox->width() + childBox->marginWidth();
        } else {
            childBox->setX(x);
        }
    }

    // text align
    if (m_block.style()->textAlign() == SideValue::LeftSideValue) {
    } else if (m_block.style()->textAlign() == SideValue::RightSideValue) {
        LayoutUnit diff = (m_lineBoxWidth - x);
        for (size_t k = 0; k < back->m_boxes.size(); k++) {
            FrameBox* childBox = back->m_boxes[k];
            childBox->moveX(diff);
        }
    /*
     * justify: No supported value
    } else if (m_block.style()->textAlign() == SideValue::JustifySideValue) {
        // issue #145
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
        if (lineFormattingContext.isBreakedLineWithoutBR(i)) {
            LayoutUnit remainSpace = (inlineContentWidth - x);
            if (remainSpace > 0) {
                size_t spaceBoxCnt = 0;
                for (size_t j = 0; j < back->m_boxes.size(); j++) {
                    FrameBox* box = back->m_boxes[j];
                    if (box->isInlineBox()) {
                        if (box->asInlineBox()->isInlineTextBox()) {
                            String* str = box->asInlineBox()->asInlineTextBox()->text();
                            if (str->equals(str)) {
                                spaceBoxCnt++;
                            }
                        }
                    }
                }

                if (spaceBoxCnt) {
                    LayoutUnit moreWidthForSpace = remainSpace / spaceBoxCnt;
                    LayoutUnit diff = 0;
                    for (size_t j = 0; j < back->m_boxes.size(); j ++) {
                        FrameBox* box = back->m_boxes[j];
                        box->moveX(diff);
                        if (box->isInlineBox()) {
                            if (box->asInlineBox()->isInlineTextBox()) {
                                String* str = box->asInlineBox()->asInlineTextBox()->text();
                                if (str->equals(str)) {
                                    diff += moreWidthForSpace;
                                }
                            }
                        }
                    }
                }
            }
        }
        */
    } else {
        STARFISH_ASSERT(m_block.style()->textAlign() == SideValue::CenterSideValue);
        LayoutUnit diff = (m_lineBoxWidth - x) / 2;
        if (diff > 0) {
            for (size_t k = 0; k < back->m_boxes.size(); k++) {
                FrameBox* childBox = back->m_boxes[k];
                childBox->moveX(diff);
            }
        }
    }

    DirectionValue dir = m_block.style()->direction();
    for (size_t i = 0; i < m_absolutePositionedBoxes.size(); i ++) {
        FrameBox* box = m_absolutePositionedBoxes[i].first;
        if (box->style()->originalDisplay() == BlockDisplayValue) {
            back->boxes().push_back(box);
            box->setLayoutParent(back);
            if (dir == DirectionValue::LtrDirectionValue)
                box->setX(0);
            else
                box->setX(back->width());
            if (m_absolutePositionedBoxes[i].second)
                box->setY(back->height());
            else
                box->setY(0);
        } else {
            box->setY(0);
            FrameBox* parent = box->layoutParent()->asFrameBox();
            if (parent->isLineBox()) {
                STARFISH_ASSERT(parent == back);
                std::vector<FrameBox*, gc_allocator<FrameBox*> >& boxes = parent->asLineBox()->boxes();
                boxes.erase(std::find(boxes.begin(), boxes.end(), box));
            } else {
                std::vector<FrameBox*, gc_allocator<FrameBox*> >& boxes = parent->asInlineBox()->asInlineNonReplacedBox()->boxes();
                auto pos = box->absolutePoint(back);
                boxes.erase(std::find(boxes.begin(), boxes.end(), box));
                box->setX(pos.x());
            }
            back->boxes().push_back(box);
            box->setLayoutParent(back);
        }
    }

    m_absolutePositionedBoxes.clear();

    registerInlineContent();
}

void LineFormattingContext::breakLine(bool dueToBr, bool isInLineBox)
{
    if (dueToBr == false)
        m_breakedLinesSet.insert(m_block.m_lineBoxes.size() - 1);

    LayoutUnit ascender;
    LayoutUnit descender;
    LineBox* back = m_block.m_lineBoxes.back();
    computeVerticalProperties(back, m_block.style(), ascender, descender, *this, dueToBr, isInLineBox);
    back->m_ascender = ascender;
    back->m_descender = descender;
    back->setHeight(back->ascender() - back->decender());
    completeLastLine();
    m_lineBoxY += back->height();

    m_block.m_lineBoxes.push_back(new LineBox(&m_block));
    m_block.m_lineBoxes.back()->setX(m_lineBoxX);
    m_block.m_lineBoxes.back()->setY(m_lineBoxY);
    m_block.m_lineBoxes.back()->setWidth(m_lineBoxWidth);
    m_currentLine++;
    m_currentLineWidth = 0;
    m_shouldLineBreakForabsolutePositionedBlock = false;
}

template <typename fn>
void textDividerForLayout(StarFish* sf, String* txt, fn f)
{
    // TODO consider white-space
    unsigned offset = 0;
    while (true) {
        if (offset >= txt->length())
            break;
        bool isWhiteSpace = false;
        if (String::isSpaceOrNewline(txt->charAt(offset))) {
            isWhiteSpace = true;
        }

        // find next space
        unsigned nextOffset = offset + 1;
        if (isWhiteSpace) {
            while (nextOffset < txt->length() && String::isSpaceOrNewline((*txt)[nextOffset])) {
                nextOffset++;
            }
            f(txt, offset, nextOffset, isWhiteSpace, true);
        } else {
            size_t start = offset;
            while (nextOffset < txt->length() && !String::isSpaceOrNewline((*txt)[nextOffset])) {
                nextOffset++;
            }

            auto breaker = sf->lineBreaker();
            breaker->setText(txt->toUnicodeString(start, nextOffset));
            int32_t c, prev = 0;
            while ((c = breaker->next()) != icu::BreakIterator::DONE) {
                f(txt, prev + start, c + start, isWhiteSpace, (prev + start != start));
                prev = c;
            }
        }
        offset = nextOffset;
    }
}

void inlineBoxGenerator(Frame* origin, LayoutContext& ctx, LineFormattingContext& lineFormattingContext, LayoutUnit inlineContentWidth, LayoutUnit& extraWidthDueToFrameInlineFirstChild,
    std::function<void(FrameBox*)> gotInlineBoxCallback, std::function<void(bool)> lineBreakCallback, std::function<void(FrameInline*)> frameInlineCallback, std::function<void(FrameBox*)> absBoxCallback)
{
    Frame* f = origin->firstChild();
    while (f) {
        if (!f->isNormalFlow()) {
            ctx.registerAbsolutePositionedFrames(f->asFrameBox());

            lineFormattingContext.m_absolutePositionedBoxes.push_back(std::make_pair(f->asFrameBox(), lineFormattingContext.m_shouldLineBreakForabsolutePositionedBlock));
            absBoxCallback(f->asFrameBox());
            f = f->next();
            continue;
        }

        if (f->isFrameText()) {
            String* txt = f->asFrameText()->text();
            textDividerForLayout(ctx.starFish(), txt, [&](String* srcTxt, size_t offset, size_t nextOffset, bool isWhiteSpace, bool canBreak) {
                textAppendRetry:
                if (isWhiteSpace) {
                    if (offset == 0) {
                        FrameBox* last = findLastInlineBox(lineFormattingContext.currentLine());
                        if (!last)
                            return;
                        if (last && last->isInlineBox() && last->asInlineBox()->isInlineTextBox()) {
                            String* str = last->asInlineBox()->asInlineTextBox()->text();
                            if (str->containsOnlyWhitespace()) {
                                return;
                            }
                        }
                    } else if (nextOffset == srcTxt->length() && f == origin->lastChild()) {
                        if (origin->isFrameInline()) {

                        } else {
                            return;
                        }
                    } else if (lineFormattingContext.m_currentLineWidth == 0) {
                        return;
                    }
                }

                LayoutUnit textWidth;
                String* resultString;
                if (isWhiteSpace) {
                    resultString = String::spaceString;
                    textWidth = f->style()->font()->spaceWidth();
                } else {
                    String* ss = srcTxt->substring(offset, nextOffset - offset);
                    resultString = ss;
                    textWidth = f->style()->font()->measureText(ss);
                }

                /*
                bool couldNotBreak = false;
                if (!isWhiteSpace && !canBreak) {
                    FrameBox* last = findLastInlineBox(lineFormattingContext.currentLine());
                    if (last && last->isInlineBox() && last->asInlineBox()->isInlineTextBox()) {
                        String* str = last->asInlineBox()->asInlineTextBox()->text();
                        if (!str->containsOnlyWhitespace()) {
                            couldNotBreak = true;
                        }
                    }
                }
                */

                if (/*couldNotBreak || */(lineFormattingContext.m_currentLineWidth == 0) || textWidth <= (inlineContentWidth - lineFormattingContext.m_currentLineWidth - extraWidthDueToFrameInlineFirstChild)) {

                } else {
                    // try this at nextline
                    lineBreakCallback(false);
                    goto textAppendRetry;
                }
                lineFormattingContext.m_shouldLineBreakForabsolutePositionedBlock |= !isWhiteSpace;
                InlineBox* ib = new InlineTextBox(f->node(), f->style(), nullptr, resultString, f->asFrameText(), InlineTextBox::CharDirection::Mixed);
                ib->setWidth(textWidth);
                ib->setHeight(f->style()->font()->metrics().m_fontHeight);
                lineFormattingContext.m_currentLineWidth += textWidth;
                gotInlineBoxCallback(ib);
            });

        } else if (f->isFrameReplaced()) {
            FrameReplaced* r = f->asFrameReplaced();
            r->layout(ctx, Frame::LayoutWantToResolve::ResolveAll);
        insertReplacedBox:
            if ((r->width() + r->marginWidth()) <= (inlineContentWidth - lineFormattingContext.m_currentLineWidth - extraWidthDueToFrameInlineFirstChild) || lineFormattingContext.m_currentLineWidth == 0) {
                lineFormattingContext.m_currentLineWidth += (r->width() + r->marginWidth());
                gotInlineBoxCallback(r);
            } else {
                lineBreakCallback(false);
                goto insertReplacedBox;
            }
        } else if (f->isFrameBlockBox()) {
            // inline-block
            FrameBlockBox* r = f->asFrameBlockBox();
            ctx.pushInlineBlockBox(r);
            STARFISH_ASSERT(f->style()->display() == DisplayValue::InlineBlockDisplayValue);
            f->layout(ctx, Frame::LayoutWantToResolve::ResolveAll);

            LayoutUnit ascender;

            std::pair<bool, LayoutUnit> p = ctx.readRegisteredLastLineBoxYPos(r);
            if (p.first && r->style()->overflow() == OverflowValue::VisibleOverflow) {
                ascender = p.second;
            } else {
                ascender = f->asFrameBox()->height();
            }
            ctx.popInlineBlockBox();
            lineFormattingContext.registerInlineBlockAscender(ascender, r);

        insertBlockBox:
            if ((r->width() + r->marginWidth()) <= (inlineContentWidth - lineFormattingContext.m_currentLineWidth - extraWidthDueToFrameInlineFirstChild) || lineFormattingContext.m_currentLineWidth == 0) {
                lineFormattingContext.m_currentLineWidth += (r->width() + r->marginWidth());
                gotInlineBoxCallback(r);
            } else {
                lineBreakCallback(false);
                goto insertBlockBox;
            }
        } else if (f->isFrameLineBreak()) {
            lineBreakCallback(true);
        } else if (f->isFrameInline()) {
            frameInlineCallback(f->asFrameInline());
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
        f = f->next();
    }
}

static void registerRelativePositionInlineBoxes(LayoutContext& ctx, std::vector<FrameBox*, gc_allocator<FrameBox*>>& boxes)
{
    for (size_t k = 0; k < boxes.size(); k++) {
        FrameBox* childBox = boxes[k];
        if (!childBox->isFrameBlockBox()) {
            if (childBox->style()->position() == PositionValue::RelativePositionValue) {
                ctx.registerRelativePositionedFrames(childBox, true);
            }
            if (childBox->isInlineBox() && childBox->asInlineBox()->isInlineNonReplacedBox()) {
                registerRelativePositionInlineBoxes(ctx, childBox->asInlineBox()->asInlineNonReplacedBox()->boxes());
            }
        }
    }
}

LayoutUnit FrameBlockBox::layoutInline(LayoutContext& ctx)
{
    if (!isNecessaryBlockBox()) {
        return 0;
    }

    LayoutRect visibleRect(0, 0, 0, 0);
    LayoutUnit inlineContentWidth = contentWidth();
    LineFormattingContext lineFormattingContext(*this, ctx, paddingLeft() + borderLeft(), paddingTop() + borderTop(), inlineContentWidth);
    LayoutUnit unused;

    inlineBoxGenerator(this, ctx, lineFormattingContext, inlineContentWidth, unused, [&](FrameBox* ib) {
        lineFormattingContext.currentLine()->boxes().push_back(ib);
        ib->setLayoutParent(lineFormattingContext.currentLine());
    }, [&](bool dueToBr)
    {
        lineFormattingContext.breakLine(dueToBr, true);
    }, [&](FrameInline* f)
    {
        InlineNonReplacedBox* inlineBox = new InlineNonReplacedBox(f->node(), f->node()->style(), nullptr, f->asFrameInline());
        InlineNonReplacedBox::layoutInline(inlineBox, ctx, this, &lineFormattingContext, nullptr, true);

        if (inlineBox->m_boxes.size() == 0) {
            if (inlineBox->style()->direction() == LtrDirectionValue) {
                inlineBox->setWidth(inlineBox->width() + inlineBox->borderLeft() + inlineBox->paddingLeft());
            } else {
                inlineBox->setWidth(inlineBox->width() + inlineBox->borderRight() + inlineBox->paddingRight());
            }
        }
    }, [&](FrameBox* box)
    {
        if (box->style()->originalDisplay() != BlockDisplayValue) {
            box->setLayoutParent(lineFormattingContext.currentLine());
            lineFormattingContext.currentLine()->boxes().push_back(box);
        }
    });

    LineBox* back = m_lineBoxes.back();
    LayoutUnit ascender;
    LayoutUnit descender;
    computeVerticalProperties(back, style(), ascender, descender, lineFormattingContext, false, true);
    back->m_ascender = ascender;
    back->m_descender = descender;
    back->m_frameRect.setHeight(ascender - descender);

    lineFormattingContext.completeLastLine();

    if (m_lineBoxes.size() && m_lineBoxes.back()->boxes().size() == 0) {
        m_lineBoxes.erase(m_lineBoxes.end() - 1);
    }

    size_t p = m_lineBoxes.size();
    while (p--) {
        LineBox* lineBox = m_lineBoxes[p];
        if (lineBox->boxes().size() == 1 && lineBox->boxes().at(0)->isInlineBox() && lineBox->boxes().at(0)->asInlineBox()->isInlineNonReplacedBox() && lineBox->boxes().at(0)->width() == 0 && lineBox->boxes().at(0)->marginLeft() == 0 && lineBox->boxes().at(0)->marginRight() == 0) {
            m_lineBoxes.erase(m_lineBoxes.begin() + p);
        }
    }

    lineFormattingContext.registerInlineContent();

    LayoutUnit contentHeight = 0;
    for (size_t i = 0; i < m_lineBoxes.size(); i++) {
        LineBox& b = *m_lineBoxes[i];
        STARFISH_ASSERT(b.isLineBox());

        // register position: relative boxes
        for (size_t k = 0; k < b.m_boxes.size(); k++) {
            FrameBox* childBox = b.m_boxes[k];
            if (!childBox->isFrameBlockBox()) {
                if (childBox->style()->position() == PositionValue::RelativePositionValue) {
                    ctx.registerRelativePositionedFrames(childBox, true);
                }

                if (childBox->isInlineBox() && childBox->asInlineBox()->isInlineNonReplacedBox()) {
                    registerRelativePositionInlineBoxes(ctx, childBox->asInlineBox()->asInlineNonReplacedBox()->boxes());
                }
            }
        }

        for (size_t k = 0; k < b.m_boxes.size(); k++) {
            FrameBox* childBox = b.m_boxes[k];
            LayoutRect vr = childBox->absoluteRect(this);
            visibleRect.unite(vr);
        }
        contentHeight += b.m_frameRect.height();
    }

    return contentHeight;
}

struct InlineNonReplacedBoxMBPStore {
    LayoutBoxSurroundData m_orgPadding, m_orgBorder, m_orgMargin;
};

InlineNonReplacedBox* InlineNonReplacedBox::layoutInline(InlineNonReplacedBox* self, LayoutContext& ctx,
    FrameBlockBox* blockBox, LineFormattingContext* lfc, InlineNonReplacedBox* layoutParentBox, bool freshStart)
{
    LineFormattingContext& lineFormattingContext = *lfc;
    // just lay out children are horizontally.
    LayoutUnit inlineContentWidth = blockBox->contentWidth();

    if (freshStart)
        self->computeBorderMarginPadding(inlineContentWidth);
    if (freshStart) {
        self->m_orgBorder = self->m_border;
        self->m_orgPadding = self->m_padding;
        self->m_orgMargin = self->m_margin;

        self->setMarginTop(0);
        self->setMarginBottom(0);
        self->setBorderTop(0);
        self->setBorderBottom(0);
        self->setPaddingTop(0);
        self->setPaddingBottom(0);

        if (!layoutParentBox) {
            lineFormattingContext.currentLine()->boxes().push_back(self);
            self->setLayoutParent(lineFormattingContext.currentLine());
        } else {
            layoutParentBox->boxes().push_back(self);
            self->setLayoutParent(layoutParentBox);
        }
    }

    if (layoutParentBox) {
        LayoutUnit w = self->marginLeft() + self->borderLeft() + self->paddingLeft();
        lineFormattingContext.m_currentLineWidth += w;
        self->setWidth(self->paddingLeft() + self->borderLeft());
    }

    auto breakLine = [&]()
    {
        std::vector<std::pair<InlineNonReplacedBox*, InlineNonReplacedBoxMBPStore> > stack;

        FrameBox* addingUpWidth = nullptr;
        Frame* currentSelf = self;
        while (!currentSelf->asFrameBox()->isLineBox()) {
            InlineNonReplacedBoxMBPStore store;
            store.m_orgMargin = currentSelf->asFrameBox()->asInlineBox()->asInlineNonReplacedBox()->m_orgMargin;
            store.m_orgBorder = currentSelf->asFrameBox()->asInlineBox()->asInlineNonReplacedBox()->m_orgBorder;
            store.m_orgPadding = currentSelf->asFrameBox()->asInlineBox()->asInlineNonReplacedBox()->m_orgPadding;

            stack.push_back(std::make_pair(currentSelf->asFrameBox()->asInlineBox()->asInlineNonReplacedBox(), store));

            if (addingUpWidth) {
                currentSelf->asFrameBox()->setWidth(currentSelf->asFrameBox()->width() + addingUpWidth->width() + addingUpWidth->marginWidth());
            }

            addingUpWidth = currentSelf->asFrameBox();
            currentSelf = currentSelf->layoutParent();
        }

        auto iter = stack.rbegin();
        while (stack.rend() != iter) {
            InlineNonReplacedBox* origin = iter->first;

            if (origin->style()->direction() == DirectionValue::LtrDirectionValue) {
                origin->m_margin.setRight(0);
                origin->m_border.setRight(0);
                origin->m_padding.setRight(0);

                origin->m_orgMargin.setRight(0);
                origin->m_orgBorder.setRight(0);
                origin->m_orgPadding.setRight(0);
            } else {
                origin->m_margin.setLeft(0);
                origin->m_border.setLeft(0);
                origin->m_padding.setLeft(0);

                origin->m_orgMargin.setLeft(0);
                origin->m_orgBorder.setLeft(0);
                origin->m_orgPadding.setLeft(0);
            }

            iter++;
        }

        lineFormattingContext.breakLine(false, false);

        bool first = true;
        iter = stack.rbegin();
        InlineNonReplacedBox* last = nullptr;
        while (stack.rend() != iter) {
            InlineNonReplacedBox* origin = iter->first;
            InlineNonReplacedBox* newBox = new InlineNonReplacedBox(origin->node(), origin->node()->style(), nullptr, origin->m_origin);

            if (first) {
                lineFormattingContext.currentLine()->boxes().push_back(newBox);
                newBox->setLayoutParent(lineFormattingContext.currentLine());
                first = false;
            } else {
                last->boxes().push_back(newBox);
                newBox->setLayoutParent(last);
            }
            last = newBox;

            if (origin->style()->direction() == DirectionValue::LtrDirectionValue) {
                newBox->m_margin.setRight(iter->second.m_orgMargin.right());
                newBox->m_border.setRight(iter->second.m_orgBorder.right());
                newBox->m_padding.setRight(iter->second.m_orgPadding.right());

                newBox->m_orgMargin.setRight(iter->second.m_orgMargin.right());
                newBox->m_orgBorder.setRight(iter->second.m_orgBorder.right());
                newBox->m_orgPadding.setRight(iter->second.m_orgPadding.right());
            } else {
                newBox->m_margin.setLeft(iter->second.m_orgMargin.left());
                newBox->m_border.setLeft(iter->second.m_orgBorder.left());
                newBox->m_padding.setLeft(iter->second.m_orgPadding.left());

                newBox->m_orgMargin.setLeft(iter->second.m_orgMargin.left());
                newBox->m_orgBorder.setLeft(iter->second.m_orgBorder.left());
                newBox->m_orgPadding.setLeft(iter->second.m_orgPadding.left());
            }

            newBox->m_orgMargin.setTop(origin->m_orgMargin.top());
            newBox->m_orgMargin.setBottom(origin->m_orgMargin.bottom());

            newBox->m_orgBorder.setTop(origin->m_orgBorder.top());
            newBox->m_orgBorder.setBottom(origin->m_orgBorder.bottom());

            newBox->m_orgPadding.setTop(origin->m_orgPadding.top());
            newBox->m_orgPadding.setBottom(origin->m_orgPadding.bottom());
            self = newBox;
            iter++;
        }
    };

    auto finishLayout = [&](bool lastNode = false, bool dueToBr = false)
    {
        if (hasIsolateBidiContent(self)) {
            resolveBidi(lineFormattingContext, self->style()->direction(), self->boxes());
        }
        LayoutUnit x = self->paddingLeft() + self->borderLeft();
        for (size_t k = 0; k < self->m_boxes.size(); k++) {
            FrameBox* childBox = self->m_boxes[k];
            childBox->setX(x + childBox->marginLeft());
            if (childBox->isNormalFlow())
                x += childBox->width() + childBox->marginWidth();
        }
        InlineNonReplacedBox* selfForFinishLayout = self;
        while (selfForFinishLayout) {
            LayoutUnit end = lineFormattingContext.m_currentLineWidth;

            if (selfForFinishLayout->style()->direction() == DirectionValue::LtrDirectionValue) {
                if (end + selfForFinishLayout->paddingRight() + selfForFinishLayout->borderRight() + selfForFinishLayout->marginRight() > inlineContentWidth) {
                    selfForFinishLayout->m_margin.setRight(0);
                    selfForFinishLayout->m_border.setRight(0);
                    selfForFinishLayout->m_padding.setRight(0);
                    if (lastNode) {
                        selfForFinishLayout->m_orgMargin.setRight(0);
                        selfForFinishLayout->m_orgBorder.setRight(0);
                        selfForFinishLayout->m_orgPadding.setRight(0);
                    }
                }
                lineFormattingContext.m_currentLineWidth += selfForFinishLayout->paddingRight() + selfForFinishLayout->borderRight() + selfForFinishLayout->marginRight();
            } else {
                if (end + selfForFinishLayout->paddingLeft() + selfForFinishLayout->borderLeft() + selfForFinishLayout->marginLeft() > inlineContentWidth) {
                    selfForFinishLayout->m_margin.setLeft(0);
                    selfForFinishLayout->m_border.setLeft(0);
                    selfForFinishLayout->m_padding.setLeft(0);
                    if (lastNode) {
                        selfForFinishLayout->m_orgMargin.setLeft(0);
                        selfForFinishLayout->m_orgBorder.setLeft(0);
                        selfForFinishLayout->m_orgPadding.setLeft(0);
                    }
                }
                lineFormattingContext.m_currentLineWidth += selfForFinishLayout->paddingLeft() + selfForFinishLayout->borderLeft() + selfForFinishLayout->marginLeft();
            }

            selfForFinishLayout->setWidth(selfForFinishLayout->width() + selfForFinishLayout->paddingWidth() + selfForFinishLayout->borderWidth());
            if (selfForFinishLayout->width() < 0) {
                selfForFinishLayout->setWidth(0);
            }

            LayoutUnit ascender = std::max(selfForFinishLayout->style()->font()->metrics().m_ascender, blockBox->style()->font()->metrics().m_ascender);
            LayoutUnit descender = std::min(selfForFinishLayout->style()->font()->metrics().m_descender, blockBox->style()->font()->metrics().m_descender);

            computeVerticalProperties(selfForFinishLayout, selfForFinishLayout->style(), ascender, descender, lineFormattingContext, dueToBr, false);
            selfForFinishLayout->m_ascender = ascender;
            selfForFinishLayout->m_descender = descender;

            selfForFinishLayout->setContentHeight(ascender - descender);
            if (selfForFinishLayout->layoutParent()->asFrameBox()->isLineBox()) {
                break;
            }
            if (lastNode) {
                break;
            }
            selfForFinishLayout = selfForFinishLayout->layoutParent()->asFrameBox()->asInlineBox()->asInlineNonReplacedBox();
        }
    };

    LayoutUnit extraWidthDueToFrameInlineFirstChild;
    LayoutUnit unprocessedLeftWidths;
    LayoutUnit unprocessedRightWidths;
    if (!layoutParentBox) {
        unprocessedLeftWidths = self->marginLeft() + self->borderLeft() + self->paddingLeft();
        unprocessedRightWidths = self->marginRight() + self->borderRight() + self->paddingRight();
        extraWidthDueToFrameInlineFirstChild = unprocessedLeftWidths + unprocessedRightWidths;
    }
    inlineBoxGenerator(self->m_origin, ctx, lineFormattingContext, inlineContentWidth, extraWidthDueToFrameInlineFirstChild, [&](FrameBox* ib)
    {
        self->boxes().push_back(ib);
        if (self->style()->direction() == LtrDirectionValue) {
            if (unprocessedLeftWidths) {
                self->setWidth(self->width() + self->borderLeft() + self->paddingLeft());
                lineFormattingContext.m_currentLineWidth += unprocessedLeftWidths;
                unprocessedLeftWidths = 0;
                extraWidthDueToFrameInlineFirstChild = unprocessedRightWidths;
            }
        } else {
            if (unprocessedRightWidths) {
                self->setWidth(self->width() + self->borderRight() + self->paddingRight());
                lineFormattingContext.m_currentLineWidth += unprocessedRightWidths;
                unprocessedRightWidths = 0;
                extraWidthDueToFrameInlineFirstChild = unprocessedLeftWidths;
            }
        }


        ib->setLayoutParent(self);
        if (self->style()->direction() == LtrDirectionValue) {
            ib->setX(self->width() + ib->marginLeft());
        } else {
            ib->setX(self->width() + ib->marginRight());
        }
        self->setWidth(self->width() + ib->width() + ib->marginWidth());
    }, [&](bool dueToBr)
    {
        if (!self->boxes().size() && !layoutParentBox) {
            lineFormattingContext.currentLine()->boxes().erase(std::find(lineFormattingContext.currentLine()->boxes().begin(), lineFormattingContext.currentLine()->boxes().end(), self));
            lineFormattingContext.breakLine(dueToBr, false);
            lineFormattingContext.currentLine()->boxes().push_back(self);
            self->setLayoutParent(lineFormattingContext.currentLine());
        } else {
            finishLayout(dueToBr);
            breakLine();
        }
    }, [&](FrameInline* f)
    {
        LayoutUnit extra;
        if (self->style()->direction() == LtrDirectionValue) {
            if (unprocessedLeftWidths) {
                lineFormattingContext.m_currentLineWidth += unprocessedLeftWidths;
                extra = unprocessedLeftWidths;
                unprocessedLeftWidths = 0;
                extraWidthDueToFrameInlineFirstChild = unprocessedRightWidths;
            }
        } else {
            if (unprocessedRightWidths) {
                lineFormattingContext.m_currentLineWidth += unprocessedRightWidths;
                extra = unprocessedRightWidths;
                unprocessedRightWidths = 0;
                extraWidthDueToFrameInlineFirstChild = unprocessedLeftWidths;
            }
        }
        InlineNonReplacedBox* inlineBox = new InlineNonReplacedBox(f->node(), f->node()->style(), nullptr, f->asFrameInline());
        self->setWidth(self->width() + extra);
        inlineBox->setX(self->width());
        InlineNonReplacedBox* result = InlineNonReplacedBox::layoutInline(inlineBox, ctx, blockBox, lfc, self, true);
        Frame* newSelfCandi = result->layoutParent();
        if (newSelfCandi->asFrameBox()->isInlineBox() && newSelfCandi->asFrameBox()->asInlineBox()->isInlineNonReplacedBox()) {
            InlineNonReplacedBox* newSelf = newSelfCandi->asFrameBox()->asInlineBox()->asInlineNonReplacedBox();
            if (self != newSelf) {
                self = newSelf;
            }
        }
        self->setWidth(self->width() + result->width() + result->marginWidth());
    }, [&](FrameBox* box)
    {
        if (box->style()->originalDisplay() != BlockDisplayValue) {
            box->setLayoutParent(self);
            self->boxes().push_back(box);
        }
    });

    finishLayout(true, false);

    return self;
}

void FrameBlockBox::computePreferredWidth(ComputePreferredWidthContext& ctx)
{
    LayoutUnit remainWidth = ctx.lastKnownWidth();
    LayoutUnit minWidth;

    if (!isNecessaryBlockBox()) {
        return;
    }

    if (hasBlockFlow()) {
        if (style()->width().isFixed()) {
            minWidth += style()->width().fixed();
            ctx.setMinimumWidth(minWidth);
            ctx.setResult(minWidth);
        } else {
            Frame* child = firstChild();
            while (child) {
                if (child->isNormalFlow()) {
                    LayoutUnit mbp = ComputePreferredWidthContext::computeMinimumWidthDueToMBP(child->style());
                    ComputePreferredWidthContext newCtx(ctx.layoutContext(), ctx.lastKnownWidth() - mbp, 0);
                    child->computePreferredWidth(newCtx);
                    ctx.setResult(newCtx.result() + mbp);
                }
                child = child->next();
            }
        }

    } else {
        LayoutUnit currentLineWidth = 0;
        std::function<void(Frame*)> computeInlineLayout = [&](Frame* f)
        {
            // current
            if (!f->isNormalFlow()) {
                return;
            }

            if (f->isFrameText()) {
                String* s = f->asFrameText()->text();
                textDividerForLayout(ctx.layoutContext().starFish(), s, [&](String* srcTxt, size_t offset, size_t nextOffset, bool isWhiteSpace, bool canBreak) {
                    if (isWhiteSpace) {
                        if (offset == 0 && f == f->parent()->firstChild()) {
                            return;
                        } else if (nextOffset == srcTxt->length() && f == f->parent()->lastChild()) {
                            return;
                        }
                    }

                    LayoutUnit w = 0;
                    if (isWhiteSpace) {
                        w = f->style()->font()->spaceWidth();
                    } else {
                        w = f->style()->font()->measureText(srcTxt->substring(offset, nextOffset - offset));
                    }

                    ctx.setMinimumWidth(w);

                    if (currentLineWidth + w < remainWidth) {
                        currentLineWidth += w;
                    } else {
                        // CHECK THIS
                        // ctx.setResult(currentLineWidth);
                        ctx.setResult(remainWidth);
                        currentLineWidth = 0;
                    }

                    if (w > remainWidth) {
                        ctx.setResult(remainWidth);
                        currentLineWidth = 0;
                    }
                });
            } else if (f->isFrameBlockBox()) {
                LayoutUnit mbp = ComputePreferredWidthContext::computeMinimumWidthDueToMBP(f->style());
                ComputePreferredWidthContext newCtx(ctx.layoutContext(), remainWidth - mbp, 0);
                f->computePreferredWidth(newCtx);
                LayoutUnit w = newCtx.result() + mbp;
                ctx.setResult(w);

                if (currentLineWidth + w < remainWidth) {
                    currentLineWidth += w;
                } else {
                    ctx.setResult(currentLineWidth);
                    currentLineWidth = w;
                }

                if (currentLineWidth > remainWidth) {
                    // linebreaks
                    ctx.setResult(remainWidth);
                    currentLineWidth = 0;
                }
            } else if (f->isFrameLineBreak()) {
                // linebreaks
                ctx.setResult(currentLineWidth);
                currentLineWidth = 0;
            } else if (f->isFrameInline()) {
                auto checkMBP = [&](Length l)
                {
                    if (l.isFixed()) {
                        if (currentLineWidth + l.fixed() < remainWidth) {
                            currentLineWidth += l.fixed();
                        } else {
                            // CHECK THIS
                            // ctx.setResult(currentLineWidth);
                            ctx.setResult(remainWidth);
                            currentLineWidth = l.fixed();
                        }
                    }
                };

                checkMBP(f->style()->marginLeft());
                checkMBP(f->style()->borderLeftWidth());
                checkMBP(f->style()->paddingLeft());
                checkMBP(f->style()->paddingRight());
                checkMBP(f->style()->borderRightWidth());
                checkMBP(f->style()->marginRight());

            } else {
                STARFISH_ASSERT(f->isFrameReplaced());

                LayoutUnit mbp = ComputePreferredWidthContext::computeMinimumWidthDueToMBP(f->style());
                ComputePreferredWidthContext newCtx(ctx.layoutContext(), remainWidth - mbp, 0);
                f->computePreferredWidth(newCtx);
                LayoutUnit w = newCtx.result() + mbp;
                ctx.setResult(w);

                if (currentLineWidth + w < remainWidth) {
                    currentLineWidth += w;
                } else {
                    ctx.setResult(currentLineWidth);
                    currentLineWidth = w;
                }

                if (currentLineWidth > remainWidth) {
                    // linebreaks
                    ctx.setResult(remainWidth);
                    currentLineWidth = 0;
                }
            }

            Frame* c = f->firstChild();
            while (c) {
                computeInlineLayout(c);
                c = c->next();
            }

        };

        Frame* c = this->firstChild();
        while (c) {
            computeInlineLayout(c);
            c = c->next();
        }
        ctx.setResult(currentLineWidth);
    }
}

void FrameBlockBox::paintChildrenWith(Canvas* canvas, PaintingStage stage)
{
    if (hasBlockFlow()) {
        FrameBox::paintChildrenWith(canvas, stage);
    } else {
        for (size_t i = 0; i < m_lineBoxes.size(); i++) {
            canvas->save();
            LineBox& b = *m_lineBoxes[i];
            canvas->translate(b.frameRect().x(), b.frameRect().y());
            for (size_t k = 0; k < b.m_boxes.size(); k++) {
                FrameBox* childBox = b.m_boxes[k];
                canvas->save();
                canvas->translate(childBox->x(), childBox->y());
                childBox->paint(canvas, stage);
                canvas->restore();
            }
            canvas->restore();
        }
    }
}

void InlineTextBox::paint(Canvas* canvas, PaintingStage stage)
{
    if (stage == PaintingNormalFlowInline) {
        if (style()->visibility() == VisibilityValue::HiddenVisibilityValue) {
            canvas->setVisible(false);
        } else {
            canvas->setVisible(true);
        }

        if (m_origin->textDecorationData()) {
            auto data = m_origin->textDecorationData();
            canvas->setNeedsLineThrough(data->m_hasLineThrough);
            canvas->setNeedsUnderline(data->m_hasUnderLine);
            canvas->setLineThroughColor(data->m_lineThroughColor);
            canvas->setUnderlineColor(data->m_underLineColor);
        }

        canvas->setFont(style()->font());
        canvas->setColor(style()->color());
        canvas->drawText(0, 0, m_text);
    }
}

void InlineNonReplacedBox::paintBackgroundAndBorders(Canvas* canvas)
{
    if (!isCollapsed()) {
        LayoutRect frameRectBack = m_frameRect;
        LayoutBoxSurroundData paddingBack = m_padding, borderBack = m_border, marginBack = m_margin;

        m_padding = m_orgPadding;
        m_margin = m_orgMargin;
        m_border = m_orgBorder;

        canvas->save();
        canvas->translate(LayoutUnit(0), m_ascender  - (style()->font()->metrics().m_ascender) - borderTop() - paddingTop());
        setContentHeight(style()->font()->metrics().m_ascender - style()->font()->metrics().m_descender);
        setHeight(contentHeight() + paddingHeight() + borderHeight());
        FrameBox::paintBackgroundAndBorders(canvas);
        canvas->restore();

        m_frameRect = frameRectBack;
        m_padding = paddingBack;
        m_border = borderBack;
        m_margin = marginBack;
    }
}

void InlineNonReplacedBox::paint(Canvas* canvas, PaintingStage stage)
{
    if (isEstablishesStackingContext()) {
        return;
    }
    // CHECK THIS at https://www.w3.org/TR/CSS2/zindex.html#stacking-defs
    if (isPositionedElement()) {
        if (stage == PaintingPositionedElements) {
            paintBackgroundAndBorders(canvas);
            PaintingStage s = PaintingStage::PaintingNormalFlowBlock;
            while (s != PaintingStageEnd) {
                paintChildrenWith(canvas, s);
                s = (PaintingStage)(s + 1);
            }
        }
    } else if (stage == PaintingNormalFlowInline) {
        paintBackgroundAndBorders(canvas);
        paintChildrenWith(canvas, stage);
    } else {
        paintChildrenWith(canvas, stage);
    }
}

Frame* InlineNonReplacedBox::hitTest(LayoutUnit x, LayoutUnit y, HitTestStage stage)
{
    if (isEstablishesStackingContext()) {
        return nullptr;
    }

    if (stage == HitTestStage::HitTestNormalFlowInline) {
        Frame* result = nullptr;
        HitTestStage s = HitTestStage::HitTestPositionedElements;

        while (s != HitTestStageEnd) {
            auto iter = boxes().rbegin();
            while (iter != boxes().rend()) {
                FrameBox* f = *iter;
                LayoutUnit cx = x - f->asFrameBox()->x();
                LayoutUnit cy = y - f->asFrameBox()->y();
                result = f->hitTest(cx, cy, s);
                if (result)
                    return result;
                iter++;
            }
            s = (HitTestStage)(s + 1);
        }
    }
    return nullptr;
}
#ifdef STARFISH_ENABLE_TEST
void InlineNonReplacedBox::dump(int depth)
{
    InlineBox::dump(depth);

    printf(" origin %p", m_origin);

    puts("");
    auto iter = boxes().begin();
    while (iter != boxes().end()) {
        FrameBox* f = *iter;
        for (int k = 0; k < depth + 1; k++)
            printf("  ");
        printf("%s", f->name());
        f->dump(depth + 2);
        if (iter + 1 != boxes().end())
            puts("");
        iter++;
    }
}
#endif
}
