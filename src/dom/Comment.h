#ifndef __StarFishComment__
#define __StarFishComment__

#include "dom/CharacterData.h"

namespace StarFish {

class Comment : public CharacterData {
public:
    Comment(Document* document, String* data = String::emptyString)
        : CharacterData(document, data)
    {
        initScriptWrappable(this);
    }

    virtual bool isComment()
    {
        return true;
    }

    virtual NodeType nodeType()
    {
        return Node::COMMENT_NODE;
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

protected:
};

}

#endif
