#ifndef __StarFishCharacterData__
#define __StarFishCharacterData__

#include "dom/Node.h"

namespace StarFish {

class CharacterData : public Node {
public:
    CharacterData(Document* document, String* data)
        : Node(document)
    {
        initScriptWrappable(this);
        m_data = data;
    }

    virtual void computeStyle()
    {
    }

    virtual void computeLayout()
    {
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

protected:
    String* m_data;
};


}

#endif
