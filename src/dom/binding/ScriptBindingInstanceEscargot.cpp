#include "StarFishConfig.h"
#include "ScriptBindingInstance.h"

#include "ScriptWrappable.h"

#include "platform/window/Window.h"
#include "dom/DOM.h"

#include "Escargot.h"
#include "vm/ESVMInstance.h"

#include "dom/binding/escargot/ScriptBindingInstanceDataEscargot.h"


namespace StarFish {

static ScriptBindingInstanceDataEscargot* fetchData(ScriptBindingInstance* instance)
{
    return (ScriptBindingInstanceDataEscargot *)instance->data();
}

ScriptBindingInstance::ScriptBindingInstance()
{
    m_data = new ScriptBindingInstanceDataEscargot();
    fetchData(this)->m_instance = new escargot::ESVMInstance();
    fetchData(this)->m_instance->enter();
}

#define DEFINE_FUNCTION(functionName, parentName) \
    escargot::ESString* functionName##String = escargot::ESString::create(#functionName); \
    escargot::ESFunctionObject* functionName##Function = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue \
        { \
            return escargot::ESValue(); \
        }, functionName##String, 0, true, false); \
        functionName##Function->protoType().asESPointer()->asESObject()->forceNonVectorHiddenClass(false); \
        fetchData(this)->m_instance->globalObject()->defineDataProperty(functionName##String, false, false, false, functionName##Function); \
        functionName##Function->protoType().asESPointer()->asESObject()->set__proto__(parentName);

// TypeError: Illegal invocation
#define THROW_ILLEGAL_INVOCATION() \
    escargot::ESVMInstance::currentInstance()->throwError(escargot::ESValue(escargot::TypeError::create(escargot::ESString::create("Illegal invocation")))); \
    STARFISH_RELEASE_ASSERT_NOT_REACHED();

#define CHECK_TYPEOF(thisValue, type) \
    {\
        escargot::ESValue v = thisValue;\
        if (!(v.isObject() && v.asESPointer()->asESObject()->extraData() == type)) { \
            THROW_ILLEGAL_INVOCATION()\
        }\
    }\

String* toBrowserString(const escargot::ESValue& v)
{
    escargot::NullableUTF8String s = v.toString()->toNullableUTF8String();
    return String::fromUTF8(s.m_buffer, s.m_bufferSize);
}

escargot::ESValue toJSString(String* v)
{
    return escargot::ESString::create(v->utf8Data());
}

void ScriptBindingInstance::initBinding(StarFish* sf)
{
    escargot::ESValue v;

    DEFINE_FUNCTION(EventTarget, fetchData(this)->m_instance->globalObject()->objectPrototype());
    DEFINE_FUNCTION(Window, EventTargetFunction->protoType());
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("window"), false, false, false, fetchData(this)->m_instance->globalObject());
    fetchData(this)->m_instance->globalObject()->set__proto__(WindowFunction->protoType());
    fetchData(this)->m_window = WindowFunction;

    fetchData(this)->m_instance->globalObject()->defineAccessorProperty(escargot::ESString::create("document"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        return (escargot::ESObject *)((Window *)ScriptWrappableGlobalObject::fetch())->document();
    }, NULL, false, false, false);

    DEFINE_FUNCTION(Node, EventTargetFunction->protoType());
    fetchData(this)->m_node = NodeFunction;

    escargot::ESString* nextSiblingString = escargot::ESString::create("nextSibling");
    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(nextSiblingString,
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj)->nextSibling();
        if (nd == nullptr)
            return escargot::ESValue(escargot::ESValue::ESNull);
        return escargot::ESValue((escargot::ESObject *)nd);
    }, NULL, false, false, false);

    escargot::ESString* prevSiblingString = escargot::ESString::create("previousSibling");
    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(prevSiblingString,
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj)->previousSibling();
        if (nd == nullptr)
            return escargot::ESValue(escargot::ESValue::ESNull);
        return escargot::ESValue((escargot::ESObject *)nd);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("firstChild"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj)->firstChild();
        if (nd == nullptr)
            return escargot::ESValue(escargot::ESValue::ESNull);
        return escargot::ESValue((escargot::ESObject *)nd);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("lastChild"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj)->lastChild();
        if (nd == nullptr)
            return escargot::ESValue(escargot::ESValue::ESNull);
        return escargot::ESValue((escargot::ESObject *)nd);
    }, NULL, false, false, false);

    escargot::ESFunctionObject* appendChildFunction = escargot::ESFunctionObject::create(nullptr, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
        CHECK_TYPEOF(instance->currentExecutionContext()->readArgument(0), ScriptWrappable::Type::NodeObject);
        Node* obj = (Node*)thisValue.asESPointer()->asESObject();
        Node* child = (Node*)instance->currentExecutionContext()->readArgument(0).asESPointer()->asESObject();
        obj->appendChild(child);
        return escargot::ESValue((escargot::ESObject *)child);
    }, escargot::ESString::create("appendChild"), 1, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("appendChild"), false, false, false, appendChildFunction);

    DEFINE_FUNCTION(Element, NodeFunction->protoType());
    fetchData(this)->m_element = ElementFunction;

    auto firstElementChildGetter = [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj)->firstElementChild();
        if (nd == nullptr)
            return escargot::ESValue(escargot::ESValue::ESNull);
        return escargot::ESValue((escargot::ESObject *)nd);
    };

    auto lastElementChildGetter = [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj)->lastElementChild();
        if (nd == nullptr)
            return escargot::ESValue(escargot::ESValue::ESNull);
        return escargot::ESValue((escargot::ESObject *)nd);
    };

    auto childElementCountGetter = [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(((Node *)originalObj)->childElementCount());
    };

    ElementFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("firstElementChild"),
        firstElementChildGetter, NULL, false, false, false);
    ElementFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("lastElementChild"),
        firstElementChildGetter, NULL, false, false, false);
    ElementFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("childElementCount"),
        firstElementChildGetter, NULL, false, false, false);

    ElementFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("id"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj);
        if (nd->isElement()) {
            return escargot::ESString::create(nd->asElement()->getAttribute(nd->document()->window()->starFish()->staticStrings()->m_id)->utf8Data());
        } else {
            THROW_ILLEGAL_INVOCATION();
        }
    }, [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name, const escargot::ESValue& v) {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj);
        if (nd->isElement()) {
            nd->asElement()->setAttribute(nd->document()->window()->starFish()->staticStrings()->m_id, toBrowserString(v));
        } else {
            THROW_ILLEGAL_INVOCATION();
        }
    }, true, true, true);

    ElementFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("class"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj);
        if (nd->isElement()) {
            return escargot::ESString::create(nd->asElement()->getAttribute(nd->document()->window()->starFish()->staticStrings()->m_class)->utf8Data());
        } else {
            THROW_ILLEGAL_INVOCATION();
        }
    }, [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name, const escargot::ESValue& v) {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj);
        if (nd->isElement()) {
            nd->asElement()->setAttribute(nd->document()->window()->starFish()->staticStrings()->m_class, toBrowserString(v));
        } else {
            THROW_ILLEGAL_INVOCATION();
        }
    }, true, true, true);


    DEFINE_FUNCTION(DocumentType, NodeFunction->protoType());
    fetchData(this)->m_documentType = DocumentTypeFunction;

    DEFINE_FUNCTION(Document, NodeFunction->protoType());
    fetchData(this)->m_document = DocumentFunction;

    DocumentFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("firstElementChild"),
        firstElementChildGetter, NULL, false, false, false);
    DocumentFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("lastElementChild"),
        firstElementChildGetter, NULL, false, false, false);
    DocumentFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("childElementCount"),
        firstElementChildGetter, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("body"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj);
        if (nd->isDocument()) {
            Document* document = nd->asDocument();
            Node* body = document->childMatchedBy(document, [](Node* nd) -> bool {
                if (nd->isElement() && nd->asElement()->isHTMLElement() && nd->asElement()->asHTMLElement()->isHTMLBodyElement()) {
                    return true;
                }
                return false;
            });
            if (body) {
                // NOTE. this casting is not necessary. only needed for check its type for debug.
                HTMLBodyElement* e = body->asElement()->asHTMLElement()->asHTMLBodyElement();
                return escargot::ESValue((escargot::ESObject *)e);
            }
        } else {
            THROW_ILLEGAL_INVOCATION();
        }
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, NULL, false, false, false);

    escargot::ESFunctionObject* getElementByIdFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
        Node* obj = (Node*)thisValue.asESPointer()->asESObject();

        if (obj->isDocument()) {
            Document* doc = obj->asDocument();
            escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
            if (argValue.isESString()) {
                escargot::ESString* argStr = argValue.asESString();
                Element* elem = doc->getElementById(String::fromUTF8(argStr->utf8Data()));
                if (elem != nullptr)
                    return escargot::ESValue((escargot::ESObject *)elem);
            }
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("getElementById"), 1, false);
    DocumentFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("getElementById"), false, false, false, getElementByIdFunction);

    escargot::ESFunctionObject* createElementFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
        Node* obj = (Node*)thisValue.asESPointer()->asESObject();

        if (obj->isDocument()) {
            Document* doc = obj->asDocument();
            escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
            if (argValue.isESString()) {
                escargot::ESString* argStr = argValue.asESString();
                Element* elem = doc->createElement(String::fromUTF8(argStr->utf8Data()));
                if (elem != nullptr)
                    return escargot::ESValue((escargot::ESObject *)elem);
            }
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("createElement"), 1, false);
    DocumentFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("createElement"), false, false, false, createElementFunction);

    escargot::ESFunctionObject* createTextNodeFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
        Node* obj = (Node*)thisValue.asESPointer()->asESObject();

        if (obj->isDocument()) {
            Document* doc = obj->asDocument();
            escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
            if (argValue.isESString()) {
                escargot::ESString* argStr = argValue.asESString();
                Text* elem = doc->createTextNode(String::fromUTF8(argStr->utf8Data()));
                if (elem != nullptr)
                    return escargot::ESValue((escargot::ESObject *)elem);
            }
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("createTextNode"), 1, false);
    DocumentFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("createTextNode"), false, false, false, createTextNodeFunction);

    DEFINE_FUNCTION(HTMLDocument, DocumentFunction->protoType());
    fetchData(this)->m_htmlDocument = HTMLDocumentFunction;

    DEFINE_FUNCTION(CharacterData, NodeFunction->protoType());
    fetchData(this)->m_characterData = CharacterDataFunction;

    CharacterDataFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("data"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj);
        if (nd->isCharacterData()) {
            return toJSString(nd->asCharacterData()->data());
        }
        THROW_ILLEGAL_INVOCATION();
        RELEASE_ASSERT_NOT_REACHED();
    }, [] (::escargot::ESObject* obj, ::escargot::ESObject* originalObj, ::escargot::ESString* propertyName, const ::escargot::ESValue& value) {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj);
        if (nd->isCharacterData()) {
            nd->asCharacterData()->setData(toBrowserString(value));
            return;
        }
        THROW_ILLEGAL_INVOCATION();
    } , true, true, false);

    DEFINE_FUNCTION(Text, CharacterDataFunction->protoType());
    fetchData(this)->m_text = TextFunction;

    DEFINE_FUNCTION(HTMLElement, ElementFunction->protoType());
    fetchData(this)->m_htmlElement = HTMLElementFunction;

    DEFINE_FUNCTION(HTMLHtmlElement, HTMLElementFunction->protoType());
    fetchData(this)->m_htmlHtmlElement = HTMLHtmlElementFunction;

    DEFINE_FUNCTION(HTMLHeadElement, HTMLElementFunction->protoType());
    fetchData(this)->m_htmlHeadElement = HTMLHeadElementFunction;

    DEFINE_FUNCTION(HTMLScriptElement, HTMLElementFunction->protoType());
    fetchData(this)->m_htmlScriptElement = HTMLScriptElementFunction;

    DEFINE_FUNCTION(HTMLStyleElement, HTMLElementFunction->protoType());
    fetchData(this)->m_htmlStyleElement = HTMLStyleElementFunction;

    DEFINE_FUNCTION(HTMLBodyElement, HTMLElementFunction->protoType());
    fetchData(this)->m_htmlBodyElement = HTMLBodyElementFunction;

    DEFINE_FUNCTION(HTMLDivElement, HTMLElementFunction->protoType());
    fetchData(this)->m_htmlDivElement = HTMLDivElementFunction;

    DEFINE_FUNCTION(HTMLImageElement, HTMLElementFunction->protoType());
    fetchData(this)->m_htmlImageElement = HTMLImageElementFunction;

    escargot::ESString* srcString = escargot::ESString::create("src");
    HTMLImageElementFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(srcString,
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj);
        if (nd->isElement()) {
            if (nd->asElement()->isHTMLElement()) {
                if (nd->asElement()->asHTMLElement()->isHTMLImageElement()) {
                    return toJSString(nd->asElement()->asHTMLElement()->asHTMLImageElement()->src());
                }
            }
        }
        THROW_ILLEGAL_INVOCATION();
        RELEASE_ASSERT_NOT_REACHED();
    }, [] (::escargot::ESObject* obj, ::escargot::ESObject* originalObj, ::escargot::ESString* propertyName, const ::escargot::ESValue& value) {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj);
        if (nd->isElement()) {
            if (nd->asElement()->isHTMLElement()) {
                if (nd->asElement()->asHTMLElement()->isHTMLImageElement()) {
                    nd->asElement()->asHTMLElement()->asHTMLImageElement()->setSrc(toBrowserString(value));
                    return;
                }
            }
        }
        THROW_ILLEGAL_INVOCATION();
    } , true, true, false);

    DEFINE_FUNCTION(Event, fetchData(this)->m_instance->globalObject()->objectPrototype());
    fetchData(this)->m_event = EventFunction;

    DEFINE_FUNCTION(UIEvent, EventFunction->protoType());
    fetchData(this)->m_uiEvent = UIEventFunction;

    DEFINE_FUNCTION(MouseEvent, UIEventFunction->protoType());
    fetchData(this)->m_mouseEvent = MouseEventFunction;
}

void ScriptBindingInstance::evaluate(String* str)
{
    std::jmp_buf tryPosition;
    if (setjmp(fetchData(this)->m_instance->registerTryPos(&tryPosition)) == 0) {
        escargot::ESValue ret = fetchData(this)->m_instance->evaluate(escargot::ESString::create(str->utf8Data()));
        fetchData(this)->m_instance->printValue(ret);
        fetchData(this)->m_instance->unregisterTryPos(&tryPosition);
    } else {
        escargot::ESValue err = fetchData(this)->m_instance->getCatchedError();
        printf("Uncaught %s\n", err.toString()->utf8Data());
    }
}

}


