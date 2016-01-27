#ifndef __StarBorderImageLength__
#define __StarBorderImageLength__

#include "style/Length.h"

namespace StarFish {
    class BorderImageLength {
    public:
        enum Type {
            LengthType,
            NumberType
        };

        BorderImageLength()
            : m_type(NumberType)
            , m_number(1.0)
        {
        }

        BorderImageLength(Length length)
            : m_type(LengthType)
            , m_length(length)
        {
        }

        BorderImageLength(double number)
            : m_type(NumberType)
            , m_number(number)
        {
        }

        void setValue(Length length)
        {
            m_type = LengthType;
            m_length = length;
        }

        void setValue(double number)
        {
            m_type = NumberType;
            m_number = number;
        }

        bool isLength() {
            return m_type == LengthType;
        }

        bool isNumber() {
            return m_type == NumberType;
        }

        Length& length() {
            STARFISH_ASSERT(m_type==LengthType);
            return m_length;
        }

        double number() {
            STARFISH_ASSERT(m_type==NumberType);
            return m_number;
        }

        String* dumpString()
        {
            if (m_type == LengthType)
                return m_length.dumpString();
            else if (m_type == NumberType)
            {
                char temp[100];
                sprintf(temp, "%f", m_number);
                return String::fromUTF8(temp);
            }
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }

        Type m_type;
        Length m_length;
        double m_number;
    };

    class BorderImageLengthBox {
    public:
        BorderImageLengthBox()
        {
        }

        BorderImageLengthBox(Length l)
            : m_left(l)
            , m_right(l)
            , m_top(l)
            , m_bottom(l)
        {
        }

        BorderImageLengthBox(double num)
            : m_left(num)
            , m_right(num)
            , m_top(num)
            , m_bottom(num)
        {
        }

        BorderImageLengthBox(BorderImageLength& l, BorderImageLength& r, BorderImageLength& t, BorderImageLength& b)
            : m_left(l)
            , m_right(r)
            , m_top(t)
            , m_bottom(b)
        {
        }

        BorderImageLength& left() { return m_left; }
        BorderImageLength& right() { return m_right; }
        BorderImageLength& top() { return m_top; }
        BorderImageLength& bottom() { return m_bottom; }

private:
        BorderImageLength m_left;
        BorderImageLength m_right;
        BorderImageLength m_top;
        BorderImageLength m_bottom;
    };
}

#endif