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

    virtual NodeType nodeType() {
        return TEXT_NODE;
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
protected:
    String* m_data;
};


}

#endif
