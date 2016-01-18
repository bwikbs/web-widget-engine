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

    std::function<void (Node* parentNode, tinyxml2::XMLElement* xmlElement)> fn = [&fn, &document](Node* parentNode, tinyxml2::XMLElement* xmlElement)
    {
        Node* newNode = nullptr;
        int type = xmlElement->IntAttribute("nodeType");
        if (type == 0) {
            // invaild node.
            return ;
        }
        if (type == 9) {
            newNode = document;
        } else if (type == 10) {
            newNode = new DocumentType(document);
        } else if (type == 3) {
            newNode = new Text(document, String::fromUTF8(xmlElement->FirstChildElement()->FirstChild()->Value()));
            parentNode->appendChild(newNode);
            return;
        } else if (type == 1) {
            const char* name = xmlElement->Attribute("localName");
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
            } else if (strcmp(name, "style") == 0) {
                newNode = new HTMLStyleElement(document);

                //resolve styles.

                tinyxml2::XMLElement* e = xmlElement->FirstChildElement();
                CSSStyleSheet* sheet = new CSSStyleSheet;
                while (e) {
                    if (strcmp(e->Value(),"style") == 0) {
                        const char* selectorText = e->Attribute("selectorText");
                        CSSStyleRule::Kind kind;
                        String* st;
                        if (selectorText[0] == '.') {
                            kind = CSSStyleRule::Kind::ClassSelector;
                            st = String::fromUTF8(&selectorText[1]);
                        } else if (selectorText[0] == '#') {
                            kind = CSSStyleRule::Kind::IdSelector;
                            st = String::fromUTF8(&selectorText[1]);
                        } else if (selectorText[0] == '*') {
                            kind = CSSStyleRule::Kind::UniversalSelector;
                        } else {
                            kind = CSSStyleRule::Kind::TypeSelector;
                            if (selectorText[0] == '*') {
                                st = String::emptyString;
                            } else {
                                st = String::fromUTF8(selectorText);
                            }
                        }
                        CSSStyleRule rule(kind, st);
                        const tinyxml2::XMLAttribute* attr = e->FirstAttribute();
                        while (attr) {
                            if (strcmp(attr->Name(), "selectorText") != 0) {
                                const char* n = attr->Name();
                                const char* v = attr->Value();
                                rule.styleDeclaration()->addValuePair(CSSStyleValuePair::fromString(n, v));
                            }
                            attr = attr->Next();
                        }
                        sheet->addRule(rule);
                    }
                    e = e->NextSiblingElement();
                }

                document->window()->styleResolver()->addSheet(sheet);

                // TODO add child
                return ;
            } else if (strcmp(name, "body") == 0) {
                newNode = new HTMLBodyElement(document);
            } else if (strcmp(name, "div") == 0) {
                newNode = new HTMLDivElement(document);
            } else if (strcmp(name, "img") == 0) {
                newNode = new HTMLImageElement(document);
            } else {
                puts(name);
                STARFISH_RELEASE_ASSERT_NOT_REACHED();
            }
        } else {
            printf("invalid node type %d\n", type);
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }

        STARFISH_ASSERT(newNode);

        if (newNode->isElement()) {
            const tinyxml2::XMLAttribute* attr = xmlElement->FirstAttribute();
            while(attr) {
                if (strcmp(attr->Name(), "nodeType") == 0 || strcmp(attr->Name(), "localName") == 0) {
                    attr = attr->Next();
                    continue;
                }

                newNode->asElement()->setAttribute(String::fromUTF8(attr->Name()), String::fromUTF8(attr->Value()));
                attr = attr->Next();
            }
        }

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

