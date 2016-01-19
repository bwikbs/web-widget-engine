#ifndef __StarFishLength__
#define __StarFishLength__

namespace StarFish {

class Length {
public:
    enum Type {
        Auto,
        Percent,
        Fixed,
    };

    Length(Type type = Auto, float data = 0.f)
        : m_type(type)
        , m_data(data)
    {

    }

    bool isSpecified() const { return isFixed() || isPercent(); }
    bool isAuto() const { return m_type == Auto; }
    bool isFixed() const { return m_type == Fixed; }
    bool isPercent() const { return m_type == Percent; }
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

protected:
    Type m_type;
    float m_data;
};

}

#endif
