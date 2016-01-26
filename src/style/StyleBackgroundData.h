#ifndef __StarFishStyleBackgroundData__
#define __StarFishStyleBackgroundData__

#include "style/Style.h"

namespace StarFish {

class ImageData;

class StyleBackgroundData : public gc {
public:
    StyleBackgroundData()
        : m_image(String::emptyString)
        , m_imageData(NULL)
        , m_sizeType(BackgroundSizeType::SizeValue)
        , m_sizeValue(NULL)
    { }

    ~StyleBackgroundData() { }

    void setSizeType(BackgroundSizeType type)
    {
        m_sizeType = type;
    }

    void setSizeValue(LengthSize* size)
    {
        m_sizeType = BackgroundSizeType::SizeValue;
        m_sizeValue = size;
    }

    void setBgColor(Color color)
    {
        m_color = color;
    }

    void setBgImage(String* img)
    {
        m_image = img;
    }

    void setBgImageData(ImageData* data)
    {
        m_imageData = data;
    }

    void setRepeatX(BackgroundRepeatValue repeat)
    {
        m_repeatX = repeat;
    }

    void setRepeatY(BackgroundRepeatValue repeat)
    {
        m_repeatY = repeat;
    }

    Color bgColor()
    {
        return m_color;
    }

    String* bgImage()
    {
        return m_image;
    }

    ImageData* bgImageData()
    {
        return m_imageData;
    }

    BackgroundSizeType sizeType()
    {
        return m_sizeType;
    }

    BackgroundRepeatValue repeatX()
    {
        return m_repeatX;
    }

    BackgroundRepeatValue repeatY()
    {
        return m_repeatY;
    }

    LengthSize* sizeValue()
    {
        STARFISH_ASSERT(m_sizeType == BackgroundSizeType::SizeValue);
        if (m_sizeValue == NULL)
            m_sizeValue = new LengthSize();
        return m_sizeValue;
    }

    void checkComputed(Length fontSize, Font* font)
    {
        if(m_sizeValue)
            m_sizeValue->checkComputed(fontSize, font);
    }

private:
    Color m_color;

    String* m_image;
    ImageData* m_imageData;

    //background-size
    BackgroundSizeType m_sizeType;
    LengthSize* m_sizeValue;

    //background-repeat
    BackgroundRepeatValue m_repeatX;
    BackgroundRepeatValue m_repeatY;
};

}

#endif
