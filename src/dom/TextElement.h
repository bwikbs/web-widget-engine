#ifndef __StarFishTextElement__
#define __StarFishTextElement__

#include "dom/Element.h"
#include "platform/canvas/font/Font.h"
namespace StarFish {

class Font;

class TextElement : public Element {
public:
    TextElement(DocumentElement* documentElement)
        : Element(documentElement)
    {
        initScriptWrappable(this);
        m_text = nullptr;
        m_font = FontSelector::loadFont(String::createASCIIString(""), 10);
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

    virtual void paint(Canvas* canvas)
    {
        canvas->save();
        canvas->setFont(m_font);
        canvas->drawText(0, 0, m_text);
        canvas->restore();
    }

protected:
    String* m_text;
    Font* m_font;
};


}

#endif
