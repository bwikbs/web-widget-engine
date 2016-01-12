#ifndef __StarFishText__
#define __StarFishText__

#include "dom/CharacterData.h"

namespace StarFish {

class Text : public CharacterData {
public:
    Text(Document* document, String* data)
        : CharacterData(document, data)
    {
        initScriptWrappable(this);
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

    virtual bool isText()
    {
        return true;
    }

protected:
};


}

#endif
