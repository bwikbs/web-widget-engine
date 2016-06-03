#include "StarFishConfig.h"
#include "dom/Document.h"
#include "HTMLStyleElement.h"
#include "dom/Text.h"
#include "dom/Traverse.h"
#include "platform/message_loop/MessageLoop.h"

#include "style/CSSParser.h"

namespace StarFish {

bool isCSSType(const char* type)
{
    if (strcmp("", type) == 0) {
        return true;
    } else if (strcmp("text/css", type) == 0) {
        return true;
    }
    return false;
}

void HTMLStyleElement::didCharacterDataModified(String* before, String* after)
{
    HTMLElement::didCharacterDataModified(before, after);
    if (isInDocumentScope()) {
        removeStyleSheet();
        generateStyleSheet();
    }
}

void HTMLStyleElement::didNodeInserted(Node* parent, Node* newChild)
{
    HTMLElement::didNodeInserted(parent, newChild);
    if (isInDocumentScope()) {
        removeStyleSheet();
        generateStyleSheet();
    }
}

void HTMLStyleElement::didNodeRemoved(Node* parent, Node* oldChild)
{
    HTMLElement::didNodeInserted(parent, oldChild);
    if (isInDocumentScope()) {
        removeStyleSheet();
        generateStyleSheet();
    }
}

void HTMLStyleElement::didNodeInsertedToDocumenTree()
{
    HTMLElement::didNodeInsertedToDocumenTree();
    generateStyleSheet();
    dispatchLoadEvent();
}

void HTMLStyleElement::didNodeRemovedFromDocumenTree()
{
    HTMLElement::didNodeRemovedFromDocumenTree();
    removeStyleSheet();
}

void HTMLStyleElement::generateStyleSheet()
{
    STARFISH_ASSERT(isInDocumentScope());

    if (m_inParsing)
        return;

    if (m_generatedSheet)
        removeStyleSheet();

    CSSParser parser(document());

    String* str = String::emptyString;

    Node* child = firstChild();
    while (child) {
        if (child->isCharacterData() && child->asCharacterData()->isText()) {
            str = str->concat(child->asCharacterData()->data());
        }
        child = child->nextSibling();
    }

    CSSStyleSheet* sheet = new CSSStyleSheet(this, str);
    m_generatedSheet = sheet;
    document()->styleResolver()->addSheet(sheet);
    document()->window()->setWholeDocumentNeedsStyleRecalc();
}

void HTMLStyleElement::removeStyleSheet()
{
    if (m_generatedSheet) {
        document()->styleResolver()->removeSheet(m_generatedSheet);
        document()->window()->setWholeDocumentNeedsStyleRecalc();
        m_generatedSheet = nullptr;
    }
}

void HTMLStyleElement::dispatchLoadEvent()
{
    document()->window()->starFish()->messageLoop()->addIdler([](size_t handle, void* data) {
        HTMLStyleElement* element = (HTMLStyleElement*)data;
        if (!element->hasLoaded()) {
            String* eventType = element->document()->window()->starFish()->staticStrings()->m_load.localName();
            Event* e = new Event(eventType, EventInit(false, false));
            element->dispatchEvent(e);
            element->setLoaded();
        }
    }, this);
}

}
