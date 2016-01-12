#include "StarFishConfig.h"
#include "XMLDocumentBuilder.h"
#include "tinyxml2.h"

#include "dom/DOM.h"
#include "dom/binding/ScriptBindingInstance.h"

namespace StarFish {


void XMLDocumentBuilder::build(Document* document, String* filePath)
{
    document->setFirstChild(nullptr);

    tinyxml2::XMLDocument doc;
    doc.LoadFile(filePath->utf8Data());

    if(doc.Error()) {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }


    doc.FirstChild();
    std::function<void (Node* parentNode, tinyxml2::XMLElement* xmlElement)> fn = [&fn, &document](Node* parentNode, tinyxml2::XMLElement* xmlElement)
    {
        Node* newNode = nullptr;
        int type = xmlElement->IntAttribute("nodeType");
        if (type == 9) {
            newNode = document;
        } else if (type == 10) {
            newNode = new DocumentType(document);
        } else if (type == 3) {
            newNode = new Text(document, String::fromUTF8(xmlElement->FirstChildElement()->FirstChild()->Value()));
            parentNode->appendChild(newNode);
            return;
        } else if (type == 1) {
            const char* name = xmlElement->Attribute("name");
            if (strcmp(name, "html") == 0) {
                newNode = new HTMLHtmlElement(document);
            } else if (strcmp(name, "head") == 0) {
                newNode = new HTMLHeadElement(document);
            } else if(strcmp(name, "script") == 0) {
                newNode = new HTMLScriptElement(document);
                if (parentNode) {
                    parentNode->appendChild(newNode);
                }

                tinyxml2::XMLElement* child = xmlElement->FirstChildElement();
                while(child) {
                    fn(newNode, child);
                    child = child->NextSiblingElement();
                }

                String* script = newNode->asElement()->firstChild()->asCharacterData()->asText()->data();
                document->window()->starFish()->evaluate(script);
                return;
            } else if (strcmp(name, "body") == 0) {
                newNode = new HTMLBodyElement(document);
            } else {
                STARFISH_RELEASE_ASSERT_NOT_REACHED();
            }
        }

        STARFISH_ASSERT(newNode);

        if (parentNode) {
            parentNode->appendChild(newNode);
        }

        tinyxml2::XMLElement* child = xmlElement->FirstChildElement();
        while(child) {
            fn(newNode, child);
            child = child->NextSiblingElement();
        }

    };

    fn(nullptr, doc.FirstChildElement()->FirstChildElement());
}

}

