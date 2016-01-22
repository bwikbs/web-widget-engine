#ifndef __StarFishLength__
#define __StarFishLength__

namespace StarFish {

class Length {
public:
    enum Type {
        Auto,
        Percent,
        Fixed,

        //After finishing resolveStyle, ex/em values should be changed to Fixed
        ExToBeFixed,
        EmToBeFixed
    };

    Length(Type type = Auto, float data = 0.f)
        : m_type(type)
        , m_data(data)
    {

    }

    void changeToFixedIfNeeded(float fontSize, Font* font) {
        if(!isComputed()) {
            if (m_type == EmToBeFixed) {
                m_data = fontSize * m_data;
                m_type = Fixed;
            } else if (m_type == ExToBeFixed) {
                // TODO: calculate ex (x-height of font)
            }
        }
    }

    bool isSpecified() const { return isFixed() || isPercent(); }
    bool isAuto() const { return m_type == Auto; }
    bool isFixed() const { return m_type == Fixed; }
    bool isPercent() const { return m_type == Percent; }
    bool isComputed() const { return isFixed() || isPercent() || isAuto(); }
    Type type() const { return m_type; }

    float percent() const
    {
        STARFISH_ASSERT(m_type == Percent);
        //0~1
        return m_data;
    }

    float fixed() const
    {
        STARFISH_ASSERT(m_type == Fixed);
        return m_data;
    }

    float specifiedValue(float parentLength) const
    {
        STARFISH_ASSERT(isSpecified());
        if (isFixed())
            return fixed();
        else
            return percent() * parentLength;
    }

    bool operator == (const Length& src)
    {
        return this->m_type == src.m_type && this->m_data == src.m_data;
    }

    bool operator != (const Length& src)
    {
        return !operator ==(src);
    }

    String* dumpString()
    {
        char temp[100];
        if (isFixed())
            sprintf(temp, "%f", fixed());
        else if(isPercent())
            sprintf(temp, "%f", percent());
        else if(isAuto())
            sprintf(temp, "auto");
        return String::fromUTF8(temp);
    }

protected:
    Type m_type;
    float m_data;
};

class LengthSize : public gc {
public:
    LengthSize()
    {
    }

    LengthSize(Length width)
        : m_width(width) {
    }

    LengthSize(Length width, Length height)
        : m_width(width),
          m_height(height) {
    }

    Length width() {
        return m_width;
    }

    Length height() {
        return m_height;
    }

    void checkComputed(float fontSize, Font* font)
    {
        m_width.changeToFixedIfNeeded(fontSize, font);
        m_height.changeToFixedIfNeeded(fontSize, font);
    }

    Length m_width;
    Length m_height;
};

}

#endif
