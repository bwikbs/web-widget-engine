#ifndef __StarFishDrawable__
#define __StarFishDrawable__

#include "util/String.h"
#include "style/Unit.h"
#include "platform/canvas/image/ImageData.h"

namespace StarFish {

class Drawable {
public:
    enum DrawableType {
        None,
        SolidColor,
        Image
    };

    Drawable()
    {
        m_type = None;
        m_image = nullptr;
        m_imageSrc = nullptr;
    }

    static Drawable fromString(StarFish* sf, String* str);

    String* toString() const
    {
        if (m_type == None) {
            return String::createASCIIString("");
        } else if (m_type == SolidColor) {
            return m_color.toString();
        } else {
            STARFISH_ASSERT(m_type == Image);
            return m_imageSrc;
        }
    }

    DrawableType type()
    {
        return m_type;
    }

    Color color()
    {
        STARFISH_ASSERT(m_type == SolidColor);
        return m_color;
    }

    ImageData* image()
    {
        STARFISH_ASSERT(m_type == Image);
        return m_image;
    }

protected:
    DrawableType m_type;
    Color m_color;
    ImageData* m_image;
    String* m_imageSrc;
};

}

#endif
