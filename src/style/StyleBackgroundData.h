#ifndef __StarFishStyleBackgroundData__
#define __StarFishStyleBackgroundData__

#include "style/Style.h"

namespace StarFish {

class StyleBackgroundData : public gc {
public:
    StyleBackgroundData()
        : m_sizeType(BackgroundSizeType::SizeValue),
          m_sizeValue(NULL)
    {}

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

    void setbgColor(Color color)
    {
        m_color = color;
    }

    Color bgColor()
    {
        return m_color;
    }

    BackgroundSizeType sizeType()
    {
        return m_sizeType;
    }

    LengthSize* sizeValue()
    {
        STARFISH_ASSERT(m_sizeType == BackgroundSizeType::SizeValue);
        if (m_sizeValue == NULL)
            m_sizeValue = new LengthSize();
        return m_sizeValue;
    }

    Color m_color;

    //background-size
    BackgroundSizeType m_sizeType;
    LengthSize* m_sizeValue;
};

}

#endif
