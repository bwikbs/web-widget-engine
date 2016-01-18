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

    virtual bool isText()
    {
        return true;
    }

    virtual String* nodeName()
    {
        return String::createASCIIString("#text");
    }

    virtual String* localName();
protected:
};


}

#endif
