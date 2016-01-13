#ifndef __StarFishAttribute__
#define __StarFishAttribute__

#include "util/String.h"

namespace StarFish {

class Attribute {
public:
    Attribute()
    {
        m_value = m_name = String::emptyString;
    }

    Attribute(String* name, String* value)
    {
        m_name = name;
        m_value = value;
    }

    String* name()
    {
        STARFISH_ASSERT(m_name->length());
        return m_name;
    }

    String* value()
    {
        STARFISH_ASSERT(m_name->length());
        return m_value;
    }

    void setValue(String* v)
    {
        m_value = v;
    }
protected:
    String* m_name;
    String* m_value;
};


}

#endif
