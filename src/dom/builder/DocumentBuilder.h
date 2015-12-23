#ifndef __StarFishDocumentBuilderElement__
#define __StarFishDocumentBuilderElement__

#include "dom/DocumentElement.h"

namespace StarFish {

class DocumentBuilder : public gc {
public:
    DocumentBuilder()
    {
    }

    virtual ~DocumentBuilder()
    {

    }

    virtual void build(DocumentElement* element, String* filePath) = 0;
};


}

#endif
