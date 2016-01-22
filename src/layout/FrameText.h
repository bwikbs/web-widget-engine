#ifndef __StarFishFrameText__
#define __StarFishFrameText__

#include "layout/Frame.h"

namespace StarFish {

class FrameText : public Frame {
public:
    FrameText (Node* node, ComputedStyle* style)
        : Frame(node, style)
    {
        m_text = node->asCharacterData()->data();
    }

    virtual bool isFrameText()
    {
        return true;
    }

    virtual const char* name()
    {
        return "FrameText";
    }

    String* text()
    {
        return m_text;
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

    virtual void dump(int depth)
    {
        std::string str = m_node->asCharacterData()->asText()->data()->utf8Data();
        str = replaceAll(str, "\n", "\\n");
        printf("text-> %s", str.data());
    }

    virtual void layout(LayoutContext& ctx)
    {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }

protected:
    String* m_text;
};

}

#endif
