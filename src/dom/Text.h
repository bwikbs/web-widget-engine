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

    /* 4.4 Interface Node */

    virtual NodeType nodeType()
    {
        return Node::TEXT_NODE;
    }

    virtual bool isText() const
    {
        return true;
    }

    virtual String* nodeName();

    virtual String* localName();

    virtual String* nodeValue()
    {
        return data();
    }

    virtual String* textContent()
    {
        return data();
    }

    virtual Node* clone()
    {
        return new Text(document(), data());
    }

    /* 4.10 Interface Text */

    Text* splitText(unsigned long offset);

    String* wholeText();

protected:
};


}

#endif
