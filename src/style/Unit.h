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

    bool isEmpty() const { return m_width <= 0 || m_height <= 0; }

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

inline Size operator-(const Location& a, const Location& b)
{
    return Size(a.x() - b.x(), a.y() - b.y());
}

class Rect {
public:
    Rect(float x, float y, float w, float h)
        : m_location(x, y)
        , m_size(w, h)
    {
    }

    float x() const { return m_location.x(); }
    float y() const { return m_location.y(); }
    float maxX() const { return x() + width(); }
    float maxY() const { return y() + height(); }
    float width() const { return m_size.width(); }
    float height() const { return m_size.height(); }

    void setX(float x) { m_location.setX(x); }
    void setY(float y) { m_location.setY(y); }
    void setWidth(float width) { m_size.setWidth(width); }
    void setHeight(float height) { m_size.setHeight(height); }

    bool isEmpty() const { return m_size.isEmpty(); }

    bool contains(float px, float py) const
    {
        return px >= x() && px < (x() + width()) && py >= y() && py < (y() + height());
    }

    void unite(const Rect& other)
    {
        if (other.isEmpty())
            return;
        if (isEmpty()) {
            *this = other;
            return;
        }

        Location newLocation(std::min(x(), other.x()), std::min(y(), other.y()));
        Location newMaxPoint(std::max(maxX(), other.maxX()), std::max(maxY(), other.maxY()));

        m_location = newLocation;
        m_size = newMaxPoint - newLocation;
    }

private:
    Location m_location;
    Size m_size;
};

class BoxSurroundData {
public:
    BoxSurroundData(float top = 0, float right = 0, float bottom = 0, float left = 0)
    {
        m_top = top;
        m_right = right;
        m_bottom = bottom;
        m_left = left;
    }

    float top()
    {
        return m_top;
    }

    float right()
    {
        return m_right;
    }

    float bottom()
    {
        return m_bottom;
    }

    float left()
    {
        return m_left;
    }

    void setTop(float v)
    {
        m_top = v;
    }

    void setRight(float v)
    {
        m_right = v;
    }

    void setBottom(float v)
    {
        m_bottom = v;
    }

    void setLeft(float v)
    {
        m_left = v;
    }

protected:
    float m_top;
    float m_right;
    float m_bottom;
    float m_left;
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
    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
        : m_r(r)
        , m_g(g)
        , m_b(b)
        , m_a(a)
    {
    }

    bool operator==(const Color& c) const
    {
        return m_r == c.m_r && m_g == c.m_g && m_b == c.m_b && m_a == c.m_a;
    }

    bool operator!=(const Color& c) const
    {
        return !this->operator==(c);
    }

    bool isTransparent()
    {
        return m_a == 0;
    }

    String* toString() const
    {
        char* buf;
        if (m_a == 255) {
            asprintf(&buf, "rgb(%d, %d, %d)", m_r, m_g, m_b);
        } else {
            float a = (float)m_a / 255;
            if (a > 0.05)
                asprintf(&buf, "rgba(%d, %d, %d, %.1f)", m_r, m_g, m_b, a);
            else
                asprintf(&buf, "rgba(%d, %d, %d, 0)", m_r, m_g, m_b);
        }

        String* toStr = String::createASCIIString(buf);
        free(buf);
        return toStr;
    }

    unsigned char r() const { return m_r; }
    unsigned char g() const { return m_g; }
    unsigned char b() const { return m_b; }
    unsigned char a() const { return m_a; }

    unsigned char m_r, m_g, m_b, m_a;
};
}

#endif
