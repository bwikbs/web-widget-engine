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
    }

    void setSrc(String* src)
    {
        m_src = src;
        setNeedsRendering();
    }

    String* src()
    {
        return m_src;
    }

    virtual void paint(Canvas* canvas)
    {
        canvas->save();
        ImageData* data = ImageData::create(m_src);
        canvas->drawImage(data, Rect(0,0,m_computedRect.width(), m_computedRect.height()));
        canvas->restore();
    }

protected:
    String* m_src;
};


}

#endif
