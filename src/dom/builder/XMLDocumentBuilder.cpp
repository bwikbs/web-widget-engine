#include "StarFishConfig.h"
#include "XMLDocumentBuilder.h"
#include "tinyxml2.h"

#include "dom/DOM.h"
#include "dom/binding/ScriptBindingInstance.h"

namespace StarFish {


void XMLDocumentBuilder::build(DocumentElement* documentElement, String* filePath)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filePath->utf8Data());

    if(doc.Error()) {
        RELEASE_ASSERT_NOT_REACHED();
    }

    tinyxml2::XMLElement* document = doc.FirstChildElement("Document");

    std::function<void (Element* element, tinyxml2::XMLElement* xmlElement)> parseElementAttribute = [](Element* element, tinyxml2::XMLElement* xmlElement)
    {
        element->setX(Length(Length::Fixed, xmlElement->IntAttribute("x")));
        element->setY(Length(Length::Fixed, xmlElement->IntAttribute("y")));
        element->setWidth(Length(Length::Fixed, xmlElement->IntAttribute("width")));
        element->setHeight(Length(Length::Fixed, xmlElement->IntAttribute("height")));

        if (xmlElement->Attribute("id")) {
            element->setId(String::createASCIIString(xmlElement->Attribute("id")));
        }

        if (xmlElement->Attribute("background")) {
            element->setBackground(Drawable::fromString(String::createASCIIString(xmlElement->Attribute("background"))));
        }

        if (xmlElement->Attribute("backgroundWhenActive")) {
            element->setBackgroundWhenActive(Drawable::fromString(String::createASCIIString(xmlElement->Attribute("backgroundWhenActive"))));
        }
    };
    std::function<void (Element* parentElement, tinyxml2::XMLElement* xmlElement)> fn = [&fn, &parseElementAttribute, &documentElement](Element* parentElement, tinyxml2::XMLElement* xmlElement)
    {
        Element* currentElement;
        if (strcmp(xmlElement->Name(), "Element") == 0) {
            Element* element = new Element(documentElement);
            parseElementAttribute(element, xmlElement);
            currentElement = element;
        } else if (strcmp(xmlElement->Name(), "Image") == 0) {
            ImageElement* imageElement = new ImageElement(documentElement);
            parseElementAttribute(imageElement, xmlElement);
            imageElement->setSrc(String::createASCIIString(xmlElement->Attribute("src")));
            currentElement = imageElement;
        } else if (strcmp(xmlElement->Name(), "Text") == 0) {
            TextElement* textElement = new TextElement(documentElement);
            parseElementAttribute(textElement, xmlElement);
            textElement->setText(String::createASCIIString(xmlElement->Attribute("text")));
            if (xmlElement->Attribute("textSize")) {
                textElement->setTextSize(xmlElement->FloatAttribute("textSize"));
            }
            if (xmlElement->Attribute("textAlign")) {
                textElement->setTextAlign(String::createASCIIString(xmlElement->Attribute("textAlign")));
            }
            if (xmlElement->Attribute("textColor")) {
                textElement->setTextColor(Color::fromString(String::createASCIIString(xmlElement->Attribute("textColor"))));
            }
            currentElement = textElement;
        } else if (strcmp(xmlElement->Name(), "Script") == 0) {
            escargot::ESVMInstance* instance = escargot::ESVMInstance::currentInstance();
            std::jmp_buf tryPosition;
            if (setjmp(instance->registerTryPos(&tryPosition)) == 0) {
                documentElement->window()->starFish()->scriptBindingInstance()->evaluate(String::createASCIIString(xmlElement->GetText()));
                instance->unregisterTryPos(&tryPosition);
            } else {
                escargot::ESValue err = instance->getCatchedError();
                printf("Uncaught %s\n", err.toString()->utf8Data());
            }
            return ;
        }

        parentElement->appendChild(currentElement);

        tinyxml2::XMLElement* child = xmlElement->FirstChildElement();
        while(child) {
            fn(currentElement, child);
            child = child->NextSiblingElement();
        }
    };

    tinyxml2::XMLElement* child = document->FirstChildElement();
    while(child) {
        fn(documentElement, child);
        child = child->NextSiblingElement();
    }

}

}

