#include "StarFishConfig.h"
#include "Style.h"
#include "ComputedStyle.h"
#include "layout/Frame.h"
#include "dom/Element.h"
#include "dom/Document.h"

namespace StarFish {

bool startsWith(const char* base, const char* str)
{
    return (strstr(base, str) - base) == 0;
}

bool endsWith(const char* base, const char* str)
{
    int blen = strlen(base);
    int slen = strlen(str);
    return (blen >= slen) && (0 == strcmp(base + blen - slen, str));
}

#define VALUE_IS_STRING(str) \
    (strcmp(value,str)) == 0

#define VALUE_IS_INHERIT() \
    VALUE_IS_STRING("inherit")

void parsePercentageOrLength(CSSStyleValuePair& ret, const char* value)
{
    if (endsWith(value, "%")) {
        float f;
        sscanf(value, "%f%%", &f);
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Percentage;
        f = f / 100.f;
        ret.m_value.m_floatValue = f;
    } else if (endsWith(value, "px")) {
        float f;
        sscanf(value, "%fpx", &f);
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Length;
        ret.m_value.m_length = CSSLength(f);
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }
}

CSSStyleValuePair CSSStyleValuePair::fromString(const char* key, const char* value)
{
    CSSStyleValuePair ret;
    if (strcmp(key, "display") == 0) {
        // <inline> | block | list-item | inline-block | table | inline-table | table-row-group | table-header-group | table-footer-group | table-row | table-column-group | table-column | table-cell | table-caption | none | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Display;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::DisplayValueKind;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if (VALUE_IS_STRING("block")) {
            ret.m_value.m_display = DisplayValue::BlockDisplayValue;
        } else if (VALUE_IS_STRING("inline")) {
            ret.m_value.m_display = DisplayValue::InlineDisplayValue;
        } else if (VALUE_IS_STRING("none")) {
            ret.m_value.m_display = DisplayValue::NoneDisplayValue;
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    } else if (strcmp(key, "width") == 0) {
        // length | percentage | <auto> | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Width;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Auto;

        if (VALUE_IS_STRING("auto")) {
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "height") == 0) {
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Height;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Auto;

        if (VALUE_IS_STRING("auto")) {
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "font-size") == 0) {
        // absolute-size | relative-size | length | percentage | inherit // initial value -> medium
        //        X      |       X       |   O    |    O       |    O
        ret.m_keyKind = CSSStyleValuePair::KeyKind::FontSize;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "color") == 0) {
        // color | inherit // initial value -> depends on user agent
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Color;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            // TODO check string has right color string
            ret.m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
            ret.m_value.m_stringValue = String::fromUTF8(value);
        }
    } else if (strcmp(key, "background-color") == 0) {
        // color | <transparent> | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BackgroundColor;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Transparent;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            // TODO: Consider CSS Colors (Hexadecimal/etc. colors)
            //       Check the value has right color strings
            ret.m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
            ret.m_value.m_stringValue = String::fromUTF8(value);
        }
    } else if (strcmp(key, "text-align") == 0) {
        // left | right | center | justify | <inherit>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::TextAlign;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        }  else if (VALUE_IS_STRING("left")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::TextAlignValueKind;
            ret.m_value.m_textAlign = TextAlignValue::LeftTextAlignValue;
        } else if (VALUE_IS_STRING("center")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::TextAlignValueKind;
            ret.m_value.m_textAlign = TextAlignValue::CenterTextAlignValue;
        } else if (VALUE_IS_STRING("right")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::TextAlignValueKind;
            ret.m_value.m_textAlign = TextAlignValue::RightTextAlignValue;
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    } else if (strcmp(key, "background-size") == 0) {
        // [length | percentage | auto]{1,2} | cover | contain // initial value -> auto
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BackgroundSize;
        if (VALUE_IS_STRING("contain")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Contain;
        } else if (VALUE_IS_STRING("cover")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Cover;
        } else {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::SizeValueKind;
            ret.m_value.m_sizeValue = SizeValue::fromString(value);
        }
    } else if (strcmp(key, "background-repeat-x") == 0) {
    	// repeat | no-repeat | initial | inherit // initial value -> repeat
    	ret.m_keyKind = CSSStyleValuePair::KeyKind::BackgroundRepeatX;
    	ret.m_valueKind = CSSStyleValuePair::ValueKind::BackgroundRepeatValueKind;

    	if (VALUE_IS_INHERIT()) {
    		ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    	} else if (VALUE_IS_STRING("no-repeat")) {
    		ret.m_value.m_backgroundRepeatX = BackgroundRepeatValue::NoRepeatRepeatValue;
    	} else if (VALUE_IS_STRING("repeat") || VALUE_IS_STRING("initial")) {
    		ret.m_value.m_backgroundRepeatX = BackgroundRepeatValue::RepeatRepeatValue;
    	} else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    } else if (strcmp(key, "background-repeat-y") == 0) {
    	// repeat | no-repeat | initial | inherit // initial value -> repeat
    	ret.m_keyKind = CSSStyleValuePair::KeyKind::BackgroundRepeatY;
    	ret.m_valueKind = CSSStyleValuePair::ValueKind::BackgroundRepeatValueKind;

    	if (VALUE_IS_INHERIT()) {
    		ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
    	} else if (VALUE_IS_STRING("no-repeat")) {
    		ret.m_value.m_backgroundRepeatY = BackgroundRepeatValue::NoRepeatRepeatValue;
    	} else if (VALUE_IS_STRING("repeat") || VALUE_IS_STRING("initial")) {
    		ret.m_value.m_backgroundRepeatY = BackgroundRepeatValue::RepeatRepeatValue;
    	} else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    } else if (strcmp(key, "bottom") == 0) {
        // length | percentage | <auto> | inherit
        ret.m_keyKind = CSSStyleValuePair::KeyKind::Bottom;

        if (VALUE_IS_STRING("auto")) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Auto;
        } else if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else {
            parsePercentageOrLength(ret, value);
        }
    } else if (strcmp(key, "border-image-repeat") == 0) {
        // <stretch> | repeat | round | space
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderImageRepeat;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::BorderImageRepeatValueKind;
        ret.m_value.m_borderImageRepeat = BorderImageRepeatValue::StretchValue;

        if (VALUE_IS_STRING("repeat")) {
            ret.m_value.m_borderImageRepeat = BorderImageRepeatValue::RepeatValue;
        } else if (VALUE_IS_STRING("round")) {
            ret.m_value.m_borderImageRepeat = BorderImageRepeatValue::RoundValue;
        } else if (VALUE_IS_STRING("space")) {
            ret.m_value.m_borderImageRepeat = BorderImageRepeatValue::SpaceValue;
        }
    } else if (strcmp(key, "border-image-source") == 0) {
        // none | <image>
        ret.m_keyKind = CSSStyleValuePair::KeyKind::BorderImageSource;
        ret.m_valueKind = CSSStyleValuePair::ValueKind::None;

        if (VALUE_IS_INHERIT()) {
            ret.m_valueKind = CSSStyleValuePair::ValueKind::Inherit;
        } else if(VALUE_IS_STRING("none") || VALUE_IS_STRING("initial")) {
            // Do nothing!!
        } else {
            // TODO: parse "url()"
            ret.m_valueKind = CSSStyleValuePair::ValueKind::StringValueKind;
            ret.m_value.m_stringValue = String::fromUTF8(value);
        }
    } else {
        STARFISH_LOG_ERROR("CSSStyleValuePair::fromString -> unsupport key = %s\n", key);
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }

    return ret;
}
SizeValue* SizeValue::fromString(const char* value) {
    unsigned len = strlen(value);
    char copyvalue[len];
    char* current = copyvalue;
    strcpy(copyvalue, value);
    char* firstVal = CSSPropertyParser::getNextSingleValue(current);

    SizeValueComponent width;
    if (endsWith(firstVal, "%")) {
        float f;
        sscanf(firstVal, "%f%%", &f);
        f = f / 100.f;
        width.m_valueKind = SizeValueComponent::ValueKind::Percentage;
        width.m_value.m_floatValue = f;
    } else if (endsWith(firstVal, "px")) {
        float f;
        sscanf(firstVal, "%fpx", &f);
        width.m_valueKind = SizeValueComponent::ValueKind::Length;
        width.m_value.m_length = CSSLength(f);
    } else {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }

    if (strlen(firstVal) >= len)
        return new SizeValue(width);
    current = firstVal + strlen(firstVal) + 1;

    char* secondVal = CSSPropertyParser::getNextSingleValue(current);
    SizeValueComponent height;
    if (strlen(secondVal) > 0) {
        if (endsWith(secondVal, "%")) {
            float f;
            sscanf(secondVal, "%f%%", &f);
            f = f / 100.f;
            height.m_valueKind = SizeValueComponent::ValueKind::Percentage;
            height.m_value.m_floatValue = f;
        } else if (endsWith(secondVal, "px")) {
            float f;
            sscanf(secondVal, "%fpx", &f);
            height.m_valueKind = SizeValueComponent::ValueKind::Length;
            height.m_value.m_length = CSSLength(f);
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    }
    return new SizeValue(width, height);
}

ComputedStyle* StyleResolver::resolveDocumentStyle()
{
    ComputedStyle* ret = new ComputedStyle();
    ret->m_display = DisplayValue::BlockDisplayValue;
    ret->m_inheritedStyles.m_color = Color(0,0,0,255);
    // TODO implement ltr, rtl
    ret->m_inheritedStyles.m_textAlign = TextAlignValue::LeftTextAlignValue;
    return ret;
}

Color parseColor(String* str)
{
    const char* s = str->utf8Data();
    if (startsWith(s, "rgba")) {
        float r, g, b, a;
        sscanf(s, "rgba(%f,%f,%f,%f)", &r, &g, &b, &a);
        return Color(r, g, b, a*255);
    } else if (startsWith(s, "rgb")) {
        float r, g, b;
        sscanf(s, "rgb(%f,%f,%f)", &r, &g, &b);
        return Color(r, g, b, 255);
    } else {
        if (strcmp("black", s) == 0) {
            return Color(0, 0, 0, 255);
        } else if (strcmp("red", s) == 0) {
            return Color(255, 0, 0, 255);
        } else if (strcmp("green", s) == 0) {
            return Color(0, 255, 0, 255);
        } else if (strcmp("blue", s) == 0) {
            return Color(0, 0, 255, 255);
        } else if (strcmp("blue", s) == 0) {
            return Color(0, 0, 255, 255);
        } else {
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
    }
}

ComputedStyle* StyleResolver::resolveStyle(Element* element, ComputedStyle* parent)
{
    ComputedStyle* ret = new ComputedStyle(parent);

    auto apply = [](std::vector<CSSStyleValuePair, gc_allocator<CSSStyleValuePair>>& cssValues, ComputedStyle* style, ComputedStyle* parentStyle) {
        for (unsigned k = 0; k < cssValues.size(); k ++) {
            switch (cssValues[k].keyKind()) {
            case CSSStyleValuePair::KeyKind::Display:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_display = parentStyle->m_display;
                } else {
                    STARFISH_ASSERT(CSSStyleValuePair::ValueKind::DisplayValueKind == cssValues[k].valueKind());
                    style->m_display = cssValues[k].displayValue();
                }
                break;
            case CSSStyleValuePair::KeyKind::Width:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_width = parentStyle->m_width;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Auto) {
                    style->m_width = Length();
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length) {
                    style->m_width = cssValues[k].lengthValue().toLength();
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage) {
                    style->m_width = Length(Length::Percent, cssValues[k].percentageValue());
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::Height:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_height = parentStyle->m_height;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Auto) {
                    style->m_height = Length();
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length) {
                    style->m_height = cssValues[k].lengthValue().toLength();
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage) {
                    style->m_height = Length(Length::Percent, cssValues[k].percentageValue());
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::Color:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_inheritedStyles.m_color = parentStyle->m_inheritedStyles.m_color;
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::StringValueKind);
                    style->m_inheritedStyles.m_color = parseColor(cssValues[k].stringValue());
                }
                break;
            case CSSStyleValuePair::KeyKind::FontSize:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_inheritedStyles.m_fontSize = parentStyle->m_inheritedStyles.m_fontSize;
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length);
                    style->m_inheritedStyles.m_fontSize = cssValues[k].lengthValue().toLength().fixed();
                }
                break;
            case CSSStyleValuePair::KeyKind::TextAlign:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_inheritedStyles.m_textAlign = parentStyle->m_inheritedStyles.m_textAlign;
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::TextAlignValueKind);
                    style->m_inheritedStyles.m_textAlign = cssValues[k].textAlignValue();
                }
                break;
            case CSSStyleValuePair::KeyKind::BackgroundColor:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_bgColor = parentStyle->m_bgColor;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Transparent) {
                    style->m_bgColor = Color(0, 0, 0, 0);
                } else {
                    STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::StringValueKind);
                    style->m_bgColor = parseColor(cssValues[k].stringValue());
                }
                break;
            // no inherited
            case CSSStyleValuePair::KeyKind::BackgroundSize:
                break;
            case CSSStyleValuePair::KeyKind::BackgroundRepeatX:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_backgroundRepeatX = parentStyle->m_backgroundRepeatX;
                } else {
                    STARFISH_ASSERT(CSSStyleValuePair::ValueKind::BackgroundRepeatValueKind == cssValues[k].valueKind());
                    style->m_backgroundRepeatX = cssValues[k].backgroundRepeatXValue();
                }
                break;
            case CSSStyleValuePair::KeyKind::BackgroundRepeatY:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_backgroundRepeatY = parentStyle->m_backgroundRepeatY;
                } else {
                    STARFISH_ASSERT(CSSStyleValuePair::ValueKind::BackgroundRepeatValueKind == cssValues[k].valueKind());
                    style->m_backgroundRepeatY = cssValues[k].backgroundRepeatYValue();
                }
                break;
            case CSSStyleValuePair::KeyKind::Bottom:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_bottom = parentStyle->m_bottom;
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Auto) {
                    style->m_bottom = Length();
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Length) {
                    style->m_bottom = cssValues[k].lengthValue().toLength();
                } else if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Percentage) {
                    style->m_bottom = Length(Length::Percent, cssValues[k].percentageValue());
                } else {
                    STARFISH_RELEASE_ASSERT_NOT_REACHED();
                }
                break;
            case CSSStyleValuePair::KeyKind::BorderImageRepeat:
                //STARFISH_ASSERT(cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::BorderImageRepeatValueKind);
                //remove assert. duplicate in borderImageRepeatValue()
                style->m_borderImageRepeat = cssValues[k].borderImageRepeatValue();
                break;
            case CSSStyleValuePair::KeyKind::BorderImageSource:
                if (cssValues[k].valueKind() == CSSStyleValuePair::ValueKind::Inherit) {
                    style->m_borderImageSource = parentStyle->m_borderImageSource;
                } else {
                    STARFISH_ASSERT(CSSStyleValuePair::ValueKind::StringValueKind == cssValues[k].valueKind());
                    style->m_borderImageSource = cssValues[k].stringValue();
                }
                break;
            }
        }
    };

    // * selector
    for (unsigned i = 0; i < m_sheets.size(); i ++) {
        CSSStyleSheet* sheet = m_sheets[i];
        for (unsigned j = 0; j < sheet->m_rules.size(); j ++) {
            if (sheet->m_rules[j].m_kind == CSSStyleRule::UniversalSelector) {
                auto cssValues = sheet->m_rules[j].styleDeclaration()->m_cssValues;
                apply(cssValues, ret, parent);
            }
        }
    }

    // type selector
    for (unsigned i = 0; i < m_sheets.size(); i ++) {
        CSSStyleSheet* sheet = m_sheets[i];
        for (unsigned j = 0; j < sheet->m_rules.size(); j ++) {
            if (sheet->m_rules[j].m_kind == CSSStyleRule::TypeSelector) {
                if (sheet->m_rules[j].m_ruleText->equals(element->localName())) {
                    auto cssValues = sheet->m_rules[j].styleDeclaration()->m_cssValues;
                    apply(cssValues, ret, parent);
                }
            }
        }
    }

    // class selector
    for (unsigned i = 0; i < m_sheets.size(); i ++) {
        CSSStyleSheet* sheet = m_sheets[i];
        for (unsigned j = 0; j < sheet->m_rules.size(); j ++) {
            if (sheet->m_rules[j].m_kind == CSSStyleRule::ClassSelector) {
                auto className = element->classNames();

                for (unsigned f = 0; f < className.size(); f ++) {
                    if (className[f]->equals(sheet->m_rules[j].m_ruleText)) {
                        auto cssValues = sheet->m_rules[j].styleDeclaration()->m_cssValues;
                        apply(cssValues, ret, parent);
                    }
                }
            }
        }
    }

    // id selector
    for (unsigned i = 0; i < m_sheets.size(); i ++) {
        CSSStyleSheet* sheet = m_sheets[i];
        for (unsigned j = 0; j < sheet->m_rules.size(); j ++) {
            if (sheet->m_rules[j].m_kind == CSSStyleRule::IdSelector) {
                if (element->id()->equals(sheet->m_rules[j].m_ruleText)) {
                    auto cssValues = sheet->m_rules[j].styleDeclaration()->m_cssValues;
                    apply(cssValues, ret, parent);
                }
            }
        }
    }

    return ret;
}

void resolveDOMStyleInner(StyleResolver* resolver, Element* element, ComputedStyle* parentStyle)
{
    ComputedStyle* style = resolver->resolveStyle(element, parentStyle);

    element->setStyle(style);

    ComputedStyle* childStyle = nullptr;
    Node* child = element->firstChild();
    while (child) {
        if (child->isElement()) {
            resolveDOMStyleInner(resolver, child->asElement(), style);
        } else {
            if (childStyle == nullptr) {
                childStyle = new ComputedStyle(style);
            }
            child->setStyle(childStyle);
        }

        child = child->nextSibling();
    }
}

void StyleResolver::resolveDOMStyle(Document* document)
{
    Node* child = document->firstChild();
    while (child) {
        if (child->isElement()) {
            resolveDOMStyleInner(this, child->asElement(), document->style());
        }
        child = child->nextSibling();
    }
}

void dump(Node* node, unsigned depth)
{
    for (unsigned i = 0; i < depth; i ++) {
        printf("  ");
    }

    node->dumpStyle();
    printf("\n");

    Node* child = node->firstChild();
    while (child) {
        if (child->isElement()) {
            dump(child, depth + 1);
        }
        child = child->nextSibling();
    }
}

void StyleResolver::dumpDOMStyle(Document* document)
{
    dump(document->asNode(), 0);
    printf("\n");
}

}
