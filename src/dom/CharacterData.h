#ifndef __StarFishCharacterData__
#define __StarFishCharacterData__

#include "dom/Node.h"

namespace StarFish {

class Text;

class CharacterData : public Node {
public:
    CharacterData(Document* document, String* data)
        : Node(document)
    {
        initScriptWrappable(this);
        m_data = data;
    }

    virtual NodeType nodeType()
    {
        return TEXT_NODE;
    }

    virtual Element* parentElement()
    {
        return nullptr;
    }

    virtual void paint(Canvas* canvas)
    {
    }

    virtual Node* hitTest(float x, float y)
    {
        return NULL;
    }

    String* data()
    {
        return m_data;
    }

    void setData(String* data)
    {
        STARFISH_ASSERT(data);
        m_data = data;
        setNeedsStyleRecalc();
    }

    size_t length()
    {
        return m_data->length();
    }

    virtual bool isCharacterData()
    {
        return true;
    }

    virtual bool isText()
    {
        return false;
    }

    Text* asText()
    {
        STARFISH_ASSERT(isText());
        return (Text*)this;
    }

    static std::string replaceAll(const std::string &str, const std::string &pattern, const std::string &replace)
    {
        std::string result = str;
        std::string::size_type pos = 0;
        std::string::size_type offset = 0;

        while((pos = result.find(pattern, offset)) != std::string::npos)
        {
            result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
            offset = pos + replace.size();
        }

        return result;
    }

    virtual void dump()
    {
        Node::dump();

        std::string str = m_data->utf8Data();
        str = replaceAll(str, "\n", "\\n");
        printf("data:%s ", str.data());
    }
protected:
    String* m_data;
};


}

#endif
