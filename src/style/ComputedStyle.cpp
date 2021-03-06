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
#include "ComputedStyle.h"

#include "StarFish.h"
#include "dom/Node.h"
#include "dom/Document.h"
#include "platform/window/Window.h"
#include "layout/Frame.h"
#include "layout/FrameBlockBox.h"

namespace StarFish {

bool ComputedStyle::hasTransforms(Frame* frame)
{
    return transforms(frame) != nullptr;
}

StyleTransformDataGroup* ComputedStyle::transforms(Frame* frame)
{
    // https://www.w3.org/TR/css-transforms-1/#transformable-element
    if (frame->isFrameInline() || (frame->isFrameBox() && frame->asFrameBox()->isInlineBox() && frame->asFrameBox()->asInlineBox()->isInlineNonReplacedBox())) {
        return nullptr;
    }
    return m_transforms;
}

class StupidImageResourceClientBecauseItIsNotConsiderRePaintRegion : public ResourceClient {
public:
    StupidImageResourceClientBecauseItIsNotConsiderRePaintRegion(Resource* res, Document* document)
        : ResourceClient(res)
        , m_document(document)
    { }
    virtual void didLoadFinished()
    {
        m_document->setNeedsPainting();
    }

    Document* m_document;
};

void ComputedStyle::loadResources(Node* consumer, ComputedStyle* prevComputedStyleValueForReferenceLoadedResources)
{
    StarFish* sf = consumer->document()->window()->starFish();
    float fontSize = m_inheritedStyles.m_fontSize.fixed();

    char style = m_inheritedStyles.m_fontStyle;
    char fontWeight = 4;

    switch (m_inheritedStyles.m_fontWeight) {
    case OneHundredFontWeightValue:
        fontWeight = 1;
        break;
    case TwoHundredsFontWeightValue:
        fontWeight = 2;
        break;
    case ThreeHundredsFontWeightValue:
        fontWeight = 3;
        break;
    case NormalFontWeightValue:
        fontWeight = 4;
        break;
    case FiveHundredsFontWeightValue:
        fontWeight = 5;
        break;
    case SixHundredsFontWeightValue:
        fontWeight = 6;
        break;
    case BoldFontWeightValue:
        fontWeight = 7;
        break;
    case EightHundredsFontWeightValue:
        fontWeight = 8;
        break;
    case NineHundredsFontWeightValue:
        fontWeight = 9;
        break;
    default:
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }

#ifdef STARFISH_ENABLE_TEST
    if (g_enablePixelTest)
        m_font = sf->fetchFont(String::fromUTF8("Ahem"), fontSize, style, fontWeight);
    else {
        if (sf->startUpFlag() & StarFishStartUpFlag::enableRegressionTest)
            m_font = sf->fetchFont(String::fromUTF8("SamsungOne"), fontSize, style, fontWeight);
        else
            m_font = sf->fetchFont(String::emptyString, fontSize, style, fontWeight);
    }
#else
    m_font = sf->fetchFont(String::emptyString, fontSize, style, fontWeight);
#endif

    size_t bgIndex = 0;
    while (bgIndex < backgroundLayerSize()) {
        if (!backgroundImage(bgIndex)->equals(String::emptyString)) {
            URL* u = URL::createURL(consumer->document()->documentURI()->baseURI(), backgroundImage(bgIndex));

            if (prevComputedStyleValueForReferenceLoadedResources && prevComputedStyleValueForReferenceLoadedResources->background()
                && prevComputedStyleValueForReferenceLoadedResources->background()->bgImageResource(bgIndex)
                && *(prevComputedStyleValueForReferenceLoadedResources->background()->bgImageResource(bgIndex)->url()) == *u) {
                consumer->document()->resourceLoader()->notifyImageResourceActiveState(prevComputedStyleValueForReferenceLoadedResources->background()->bgImageResource());
                setBackgroundImageResource(prevComputedStyleValueForReferenceLoadedResources->background()->bgImageResource(), bgIndex);
            } else {
                ImageResource* res = consumer->document()->resourceLoader()->fetchImage(u);
                setBackgroundImageResource(res, bgIndex);
                res->markThisResourceIsDoesNotAffectWindowOnLoad();
                res->addResourceClient(new StupidImageResourceClientBecauseItIsNotConsiderRePaintRegion(res, consumer->document()));
#ifdef STARFISH_ENABLE_TEST
                bool enableRegressionTest = sf->startUpFlag() & StarFishStartUpFlag::enableRegressionTest;
                res->request((g_enablePixelTest || enableRegressionTest) ? Resource::ResourceRequestSyncLevel::AlwaysSync : Resource::ResourceRequestSyncLevel::SyncIfAlreadyLoaded);
#else
                res->request(Resource::ResourceRequestSyncLevel::SyncIfAlreadyLoaded);
#endif
            }
        }
        bgIndex++;
    }

    if (!borderImageSource()->equals(String::emptyString)) {
        URL* u = URL::createURL(consumer->document()->documentURI()->baseURI(), borderImageSource());

        if (prevComputedStyleValueForReferenceLoadedResources && prevComputedStyleValueForReferenceLoadedResources->hasBorderImageData()
            && prevComputedStyleValueForReferenceLoadedResources->surround()->border.image().imageResource()
            && *(prevComputedStyleValueForReferenceLoadedResources->surround()->border.image().imageResource()->url()) == *u) {
            consumer->document()->resourceLoader()->notifyImageResourceActiveState(prevComputedStyleValueForReferenceLoadedResources->surround()->border.image().imageResource());
            ImageResource* res = prevComputedStyleValueForReferenceLoadedResources->surround()->border.image().imageResource();
            setBorderImageResource(res);
        } else {
            ImageResource* res = consumer->document()->resourceLoader()->fetchImage(u);
            res->markThisResourceIsDoesNotAffectWindowOnLoad();
            res->addResourceClient(new StupidImageResourceClientBecauseItIsNotConsiderRePaintRegion(res, consumer->document()));
#ifdef STARFISH_ENABLE_TEST
            bool enableRegressionTest = sf->startUpFlag() & StarFishStartUpFlag::enableRegressionTest;
            res->request((g_enablePixelTest || enableRegressionTest) ? Resource::ResourceRequestSyncLevel::AlwaysSync : Resource::ResourceRequestSyncLevel::SyncIfAlreadyLoaded);
#else
            res->request(Resource::ResourceRequestSyncLevel::SyncIfAlreadyLoaded);
#endif
            setBorderImageResource(res);
        }

        setBorderLeftStyle(BorderStyleValue::SolidBorderStyleValue);
        setBorderTopStyle(BorderStyleValue::SolidBorderStyleValue);
        setBorderRightStyle(BorderStyleValue::SolidBorderStyleValue);
        setBorderBottomStyle(BorderStyleValue::SolidBorderStyleValue);
    }
}

void ComputedStyle::arrangeStyleValues(ComputedStyle* parentStyle, Node* current)
{
    if (current != nullptr && current->isElement() && current->asElement() && current->asElement()->isHTMLElement() && current->asElement()->asHTMLElement()->isHTMLHtmlElement()) {
        if (m_display == DisplayValue::InlineDisplayValue || m_display == DisplayValue::InlineBlockDisplayValue) {
            m_display = DisplayValue::BlockDisplayValue;
        }
    }
    // 9.7 Relationships between 'display', 'position', and 'float'
    m_originalDisplay = m_display;
    if (m_originalDisplay != DisplayValue::NoneDisplayValue && position() == AbsolutePositionValue) {
        m_display = DisplayValue::BlockDisplayValue;
    }

    if (lineHeight().isPercent()) {
        if (lineHeight().percent() == -100) {
        } else {
            // The computed value of the property is this percentage multiplied by the element's computed font size. Negative values are illegal.
            setLineHeight(Length(Length::Fixed, lineHeight().percent() * fontSize().fixed()));
        }
    }

    // Convert all non-computed Lengths to computed Length
    STARFISH_ASSERT(m_inheritedStyles.m_fontSize.isFixed());
    Length baseFontSize = fontSize();
    m_inheritedStyles.m_letterSpacing.changeToFixedIfNeeded(baseFontSize, font());
    m_inheritedStyles.m_lineHeight.changeToFixedIfNeeded(baseFontSize, font());
    m_width.changeToFixedIfNeeded(baseFontSize, font());
    m_height.changeToFixedIfNeeded(baseFontSize, font());
    m_verticalAlignLength.changeToFixedIfNeeded(baseFontSize, font());
    if (m_transforms) {
        size_t sz = m_transforms->size();
        for (size_t i = 0; i < sz; i++) {
            StyleTransformData std = m_transforms->at(i);
            if (std.type() != StyleTransformData::OperationType::Translate)
                continue;
            std.changeToFixedIfNeeded(baseFontSize, font());
        }
    }

    if (m_surround) {
        m_surround->margin.checkComputed(baseFontSize, font());

        m_surround->padding.checkComputed(baseFontSize, font());

        m_surround->offset.checkComputed(baseFontSize, font());

        m_surround->border.checkComputed(baseFontSize, font());

        if (hasBorderStyle() && !hasBorderColor()) {
            // If an element's border color is not specified with a border property,
            // user agents must use the value of the element's 'color' property as the computed value for the border color.
            setBorderTopColor(m_inheritedStyles.m_color);
            setBorderRightColor(m_inheritedStyles.m_color);
            setBorderBottomColor(m_inheritedStyles.m_color);
            setBorderLeftColor(m_inheritedStyles.m_color);
        }
    }

    if (m_background)
        m_background->checkComputed(baseFontSize, font(), color());
}

ComputedStyleDamage compareStyle(ComputedStyle* oldStyle, ComputedStyle* newStyle)
{
    ComputedStyleDamage damage = ComputedStyleDamage::ComputedStyleDamageNone;
    if (memcmp(&oldStyle->m_inheritedStyles, &newStyle->m_inheritedStyles, sizeof(ComputedStyle::InheritedStyles)) != 0) {
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageInherited | damage);
    }

    if (newStyle->m_inheritedStyles.m_color != oldStyle->m_inheritedStyles.m_color)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamagePainting | damage);

    if (newStyle->m_inheritedStyles.m_direction != oldStyle->m_inheritedStyles.m_direction)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageRebuildFrame | damage);

    if (newStyle->m_inheritedStyles.m_whiteSpace != oldStyle->m_inheritedStyles.m_whiteSpace)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageLayout | damage);

    if (newStyle->m_inheritedStyles.m_fontSize != oldStyle->m_inheritedStyles.m_fontSize)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageLayout | damage);

    if (newStyle->m_inheritedStyles.m_fontStyle != oldStyle->m_inheritedStyles.m_fontStyle)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageLayout | damage);

    if (newStyle->m_inheritedStyles.m_fontWeight != oldStyle->m_inheritedStyles.m_fontWeight)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageLayout | damage);

    if (newStyle->m_inheritedStyles.m_letterSpacing != oldStyle->m_inheritedStyles.m_letterSpacing)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageLayout | damage);

    if (newStyle->m_inheritedStyles.m_lineHeight != oldStyle->m_inheritedStyles.m_lineHeight)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageLayout | damage);

    if (newStyle->m_inheritedStyles.m_textAlign != oldStyle->m_inheritedStyles.m_textAlign)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageLayout | damage);

    if (newStyle->m_inheritedStyles.m_visibility != oldStyle->m_inheritedStyles.m_visibility)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamagePainting | damage);

    if (newStyle->m_display != oldStyle->m_display)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageRebuildFrame | damage);

    if (newStyle->m_originalDisplay != oldStyle->m_originalDisplay)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageRebuildFrame | damage);

    if (newStyle->m_position != oldStyle->m_position)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageRebuildFrame | damage);

    if (newStyle->m_width != oldStyle->m_width)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageLayout | damage);

    if (newStyle->m_height != oldStyle->m_height)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageLayout | damage);

    if (newStyle->m_unicodeBidi != oldStyle->m_unicodeBidi)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageLayout | damage);

    if (newStyle->m_verticalAlign != oldStyle->m_verticalAlign)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageLayout | damage);

    if (newStyle->m_verticalAlignLength != oldStyle->m_verticalAlignLength)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageLayout | damage);

    if (newStyle->m_overflow != oldStyle->m_overflow)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageRebuildFrame | damage);

    // NOTE.
    // text-decoration is not inherited.
    // but it influence its child boxes, within it's inline formatting context
    // and is further propagated to any in-flow block-level boxes that split the inline (see section 9.2.1.1).
    // we store text-decoration values in FrameBlockBox
    // so we need to rebuild frame from this point
    // https://www.w3.org/TR/CSS2/text.html#propdef-text-decoration
    if (newStyle->m_textDecoration != oldStyle->m_textDecoration)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageRebuildFrame | damage);

    if (newStyle->m_opacity != oldStyle->m_opacity) {
        if ((newStyle->m_opacity) < 1 && (oldStyle->m_opacity < 1)) {
            damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageComposite | damage);
        } else {
            damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageRebuildFrame | damage);
        }
    }

    // FIXME changing z-index not always cause tree-rebuild
    if (newStyle->m_zIndex != oldStyle->m_zIndex)
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageRebuildFrame | damage);

    if (newStyle->m_background == NULL && oldStyle->m_background == NULL) {

    } else if (newStyle->m_background == NULL || oldStyle->m_background == NULL) {
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamagePainting | damage);
    } else if (*newStyle->m_background != *oldStyle->m_background) {
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamagePainting | damage);
    }

    if (newStyle->m_surround == NULL && oldStyle->m_surround == NULL) {

    } else if (newStyle->m_surround == NULL || oldStyle->m_surround == NULL) {
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageLayout | damage);
    } else if (*newStyle->m_surround != *oldStyle->m_surround) {
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageLayout | damage);
    }

    if (newStyle->m_transforms == NULL && oldStyle->m_transforms == NULL) {

    } else if (newStyle->m_transforms == NULL || oldStyle->m_transforms == NULL) {
        // if element has transform, we should re-layout for building stacking-context
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageLayout | damage);
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamagePainting | damage);
    } else {
        if (*newStyle->m_transforms != *oldStyle->m_transforms) {
            damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageComposite | damage);
        }
    }

    if (newStyle->m_transformOrigin == NULL && oldStyle->m_transformOrigin == NULL) {

    } else if (newStyle->m_transformOrigin == NULL || oldStyle->m_transformOrigin == NULL) {
        damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageComposite | damage);
    } else {
        if (!(*newStyle->m_transformOrigin == *oldStyle->m_transformOrigin)) {
            damage = (ComputedStyleDamage)(ComputedStyleDamage::ComputedStyleDamageComposite | damage);
        }
    }

    return damage;
}

inline double deg2rad(float degree)
{
    return degree * 3.14159265358979323846 / 180;
}

SkMatrix ComputedStyle::transformsToMatrix(LayoutUnit containerWidth, LayoutUnit containerHeight, bool isTransformable)
{
    SkMatrix matrix;
    matrix.reset();
    if (!m_transforms || !isTransformable) {
        return matrix;
    }
    for (size_t i = 0; i < m_transforms->size(); i ++) {
        StyleTransformData t = m_transforms->at(i);
        if (t.type() == StyleTransformData::Matrix) {
            MatrixTransform* m = t.matrix();
            // [ a c e ]
            // [ b d f ]
            // [ x x x ]
            matrix.set(0, m->a());
            matrix.set(1, m->c());
            matrix.set(2, m->e());
            matrix.set(3, m->b());
            matrix.set(4, m->d());
            matrix.set(5, m->f());
        } else if (t.type() == StyleTransformData::Scale) {
            ScaleTransform* m = t.scale();
            matrix.preScale(m->x(), m->y());
        } else if (t.type() == StyleTransformData::Rotate) {
            RotateTransform* m = t.rotate();
            matrix.preRotate(m->angle());
        } else if (t.type() == StyleTransformData::Skew) {
            SkewTransform* m = t.skew();
            matrix.preSkew(tan(deg2rad(m->angleX())), tan(deg2rad(m->angleY())));
        } else if (t.type() == StyleTransformData::Translate) {
            TranslateTransform* m = t.translate();
            matrix.preTranslate(m->tx().specifiedValue(containerWidth), m->ty().specifiedValue(containerHeight));
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    }
    return matrix;
}

}
