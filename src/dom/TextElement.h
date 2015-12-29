#ifndef __StarFishTextElement__
#define __StarFishTextElement__

#include "dom/Element.h"
#include "platform/canvas/font/Font.h"
namespace StarFish {

class Font;

class TextElement : public Element {
public:
    enum TextAlignKind {
        TextAlignLeft,
        TextAlignCenter,
        TextAlignRight
    };

    TextElement(DocumentElement* documentElement)
        : Element(documentElement)
    {
        initScriptWrappable(this);
        m_text = nullptr;
        m_font = FontSelector::loadFont(String::createASCIIString(""), 10);
        m_textColor = Color(255, 255, 255, 255);
        m_textAlign = TextAlignLeft;
    }

    void setText(String* text)
    {
        m_text = text;
        setNeedsRendering();
    }

    String* text()
    {
        return m_text;
    }

    void setTextSize(float siz)
    {
        m_font = FontSelector::loadFont(String::createASCIIString(""), siz);
        setNeedsRendering();
    }

    float textSize()
    {
        return m_font->size();
    }

    void setTextColor(Color color)
    {
        m_textColor = color;
        setNeedsRendering();
    }

    Color textColor()
    {
        return m_textColor;
    }

    void setTextAlign(TextAlignKind a)
    {
        m_textAlign = a;
        setNeedsRendering();
    }

    void setTextAlign(String* str)
    {
        if (str->equals("left")) {
            setTextAlign(TextAlignLeft);
        } else if (str->equals("right")) {
            setTextAlign(TextAlignRight);
        } else if (str->equals("center")) {
            setTextAlign(TextAlignCenter);
        }
    }

    TextAlignKind textAlign()
    {
        return m_textAlign;
    }

    virtual void paint(Canvas* canvas)
    {
        Element::paint(canvas);
        canvas->save();
        canvas->setColor(m_textColor);
        canvas->setFont(m_font);

        Size siz = m_font->measureText(m_text);
        float v = (m_computedRect.height() - siz.height()) / 2;
        float h;

        if (m_textAlign == TextAlignLeft) {
            h = 0;
        } else if (m_textAlign == TextAlignRight) {
            h = m_computedRect.width() - siz.width();
        } else if (m_textAlign == TextAlignCenter) {
            h = (m_computedRect.width() - siz.width()) / 2;
        }

        canvas->drawText(h, v, m_text);
        canvas->restore();
    }

protected:
    String* m_text;
    Color m_textColor;
    Font* m_font;
    TextAlignKind m_textAlign;
};


}

#endif
