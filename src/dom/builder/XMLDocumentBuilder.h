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

    virtual void build(DocumentElement* element, String* filePath);

protected:
};


}

#endif
