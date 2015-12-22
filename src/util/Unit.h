#ifndef __StarFishUnit__
#define __StarFishUnit__

#include "StarFish.h"

namespace StarFish {

class Size {
public:
    Size(float w, float h)
    {
        m_width = w;
        m_height = h;
    }

    void setWidth(float w)
    {
        m_width = w;
    }

    void setHeight(float h)
    {
        m_height = h;
    }

    float width() const
    {
        return m_width;
    }

    float height() const
    {
        return m_height;
    }
protected:
    float m_width, m_height;
};

class Location {
public:
    Location(float x, float y)
    {
        m_x = x;
        m_y = y;
    }

    void setX(float x)
    {
        m_x = x;
    }

    void setY(float y)
    {
        m_y = y;
    }

    float x() const
    {
        return m_x;
    }

    float y() const
    {
        return m_y;
    }
protected:
    float m_x, m_y;
};

class Rect {
public:
    Rect(float x, float y, float w, float h)
        : m_location(x, y)
        , m_size(w, h)
    {
    }

    float x() const { return m_location.x(); }
    float y() const { return m_location.y(); }
    float width() const { return m_size.width(); }
    float height() const { return m_size.height(); }

    void setX(float x) { m_location.setX(x); }
    void setY(float y) { m_location.setY(y); }
    void setWidth(float width) { m_size.setWidth(width); }
    void setHeight(float height) { m_size.setHeight(height); }
private:
    Location m_location;
    Size m_size;
};

class Color {
public:
    Color()
        : m_r(0)
        , m_g(0)
        , m_b(0)
        , m_a(0)
    {

    }
    Color(unsigned char r,unsigned char g,unsigned char b,unsigned char a)
        : m_r(r)
        , m_g(g)
        , m_b(b)
        , m_a(a)
    {

    }

    bool operator==(const Color& c) {
        return m_r == c.m_r && m_g == c.m_g && m_b == c.m_b && m_a == c.m_a;
    }

    bool operator!=(const Color& c) {
        return !this->operator ==(c);
    }

    unsigned char r() const { return m_r; }
    unsigned char g() const { return m_g; }
    unsigned char b() const { return m_b; }
    unsigned char a() const { return m_a; }

    unsigned char m_r,m_g,m_b,m_a;
};


}

#endif
