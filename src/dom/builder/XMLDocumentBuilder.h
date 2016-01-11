#ifndef __StarFishXMLDocumentBuilderElement__
#define __StarFishXMLDocumentBuilderElement__

#include "dom/builder/DocumentBuilder.h"

namespace StarFish {

class Window;
class XMLDocumentBuilder : public DocumentBuilder {
public:
    XMLDocumentBuilder()
    {
    }

    virtual void build(Document* element, String* filePath);

protected:
};


}

#endif
