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

#ifndef __StarFishStyleBackgroundData__
#define __StarFishStyleBackgroundData__

#include "style/Style.h"
#include "loader/ImageResource.h"

namespace StarFish {

class ImageResource;

class StyleBackgroundData : public gc {
public:
    StyleBackgroundData()
        : m_image(String::emptyString)
        , m_imageResource(NULL)
        , m_sizeType(BackgroundSizeType::SizeValue)
        , m_sizeValue(new LengthSize())
        , m_repeatX(BackgroundRepeatValue::RepeatRepeatValue)
        , m_repeatY(BackgroundRepeatValue::RepeatRepeatValue)
    {
    }

    ~StyleBackgroundData()
    {
    }

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

    void setBgImageResource(ImageResource* data)
    {
        m_imageResource = data;
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
        if (m_imageResource)
            return m_imageResource->imageData();
        return nullptr;
    }

    ImageResource* bgImageResource()
    {
        return m_imageResource;
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
        if (m_sizeValue)
            m_sizeValue->checkComputed(fontSize, font);
    }

private:
    friend inline bool operator==(const StyleBackgroundData& a, const StyleBackgroundData& b);
    friend inline bool operator!=(const StyleBackgroundData& a, const StyleBackgroundData& b);

    Color m_color;

    String* m_image;
    ImageResource* m_imageResource;

    // background-size
    BackgroundSizeType m_sizeType;
    LengthSize* m_sizeValue;

    // background-repeat
    BackgroundRepeatValue m_repeatX;
    BackgroundRepeatValue m_repeatY;
};

bool operator==(const StyleBackgroundData& a, const StyleBackgroundData& b)
{
    if (a.m_color != b.m_color)
        return false;

    if (a.m_sizeType != b.m_sizeType)
        return false;

    if (a.m_sizeType == BackgroundSizeType::SizeValue && *a.m_sizeValue != *b.m_sizeValue) {
        return false;
    }

    if (a.m_repeatX != b.m_repeatX)
        return false;

    if (a.m_repeatY != b.m_repeatY)
        return false;

    return true;
}

bool operator!=(const StyleBackgroundData& a, const StyleBackgroundData& b)
{
    return !operator==(a, b);
}
}

#endif
