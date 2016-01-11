#ifndef __StarFishDocumentBuilderElement__
#define __StarFishDocumentBuilderElement__

#include "../Document.h"

namespace StarFish {

class DocumentBuilder : public gc {
public:
    DocumentBuilder()
    {
    }

    virtual ~DocumentBuilder()
    {

    }

    virtual void build(Document* element, String* filePath) = 0;
};


}

#endif
