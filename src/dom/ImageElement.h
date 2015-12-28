#ifndef __StarFishImageElement__
#define __StarFishImageElement__

#include "dom/Element.h"

namespace StarFish {

class ImageElement : public Element {
public:
    ImageElement(DocumentElement* documentElement)
        : Element(documentElement)
    {
        initScriptWrappable(this);
        m_src = nullptr;
        m_imageData = nullptr;
    }

    void setSrc(String* src)
    {
        m_src = src;
        m_imageData = ImageData::create(m_src);
        setNeedsRendering();
    }

    String* src()
    {
        return m_src;
    }

    virtual void paint(Canvas* canvas)
    {
        Element::paint(canvas);
        canvas->save();
        if (m_imageData != nullptr)
            canvas->drawImage(m_imageData, Rect(0,0,m_computedRect.width(), m_computedRect.height()));
        canvas->restore();
    }

protected:
    String* m_src;
    ImageData* m_imageData;
};


}

#endif
