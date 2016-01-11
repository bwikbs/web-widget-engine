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

// TODO
// every function have to check typeof this

void ScriptBindingInstance::initBinding(StarFish* sf)
{
    // EventTarget
    escargot::ESFunctionObject* eventTargetFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("EventTarget"), 0, false);
    eventTargetFunction->protoType().asESPointer()->asESObject()->forceNonVectorHiddenClass(false);
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("EventTarget"), false, false, false, eventTargetFunction);

    escargot::ESFunctionObject* windowFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("Window"), 0, false);
    windowFunction->protoType().asESPointer()->asESObject()->forceNonVectorHiddenClass(false);
    windowFunction->protoType().asESPointer()->asESObject()->set__proto__(eventTargetFunction->protoType());
    // Window
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("window"), false, false, false, fetchData(this)->m_instance->globalObject());
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("Window"), false, false, false, windowFunction);

    fetchData(this)->m_instance->globalObject()->set__proto__(windowFunction->protoType());
    fetchData(this)->m_window = windowFunction;

    windowFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("document"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj) -> escargot::ESValue {
        ASSERT(escargot::ESValue((escargot::ESObject *)((Window *)originalObj)->document()).isObject());
        return (escargot::ESObject *)((Window *)ScriptWrappableGlobalObject::fetch())->document();
    }, NULL, false, false, false);

    fetchData(this)->m_instance->globalObject()->defineAccessorProperty(escargot::ESString::create("document"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj) -> escargot::ESValue {
        return (escargot::ESObject *)((Window *)ScriptWrappableGlobalObject::fetch())->document();
    }, NULL, false, false, false);

    // Node
    escargot::ESFunctionObject* nodeFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("Node"), 0, false);
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("Node"), false, false, false, nodeFunction);
    fetchData(this)->m_node = nodeFunction;

    nodeFunction->protoType().asESPointer()->asESObject()->set__proto__(eventTargetFunction->protoType());

    escargot::ESFunctionObject* elementFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("Element"), 0, false);
    elementFunction->protoType().asESPointer()->asESObject()->set__proto__(nodeFunction->protoType());
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("Element"), false, false, false, elementFunction);
    fetchData(this)->m_element = elementFunction;

    escargot::ESFunctionObject* documentTypeFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("DocumentType"), 0, false);
    documentTypeFunction->protoType().asESPointer()->asESObject()->set__proto__(nodeFunction->protoType());
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("DocumentType"), false, false, false, documentTypeFunction);
    fetchData(this)->m_documentType = documentTypeFunction;

    escargot::ESFunctionObject* documentFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("Document"), 0, false);
    documentFunction->protoType().asESPointer()->asESObject()->set__proto__(nodeFunction->protoType());
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("Document"), false, false, false, documentFunction);
    fetchData(this)->m_document = documentFunction;

    escargot::ESFunctionObject* htmlDocumentFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("HTMLDocument"), 0, false);
    htmlDocumentFunction->protoType().asESPointer()->asESObject()->set__proto__(documentFunction->protoType());
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("HTMLDocument"), false, false, false, htmlDocumentFunction);
    fetchData(this)->m_htmlDocument = htmlDocumentFunction;

    escargot::ESFunctionObject* characterDataFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("CharacterData"), 0, false);
    characterDataFunction->protoType().asESPointer()->asESObject()->set__proto__(nodeFunction->protoType());
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("CharacterData"), false, false, false, characterDataFunction);
    fetchData(this)->m_characterData = characterDataFunction;

    escargot::ESFunctionObject* textFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("Text"), 0, false);
    textFunction->protoType().asESPointer()->asESObject()->set__proto__(characterDataFunction->protoType());
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("Text"), false, false, false, textFunction);
    fetchData(this)->m_text = textFunction;

    escargot::ESFunctionObject* htmlElementFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("HTMLElement"), 0, false);
    htmlElementFunction->protoType().asESPointer()->asESObject()->set__proto__(elementFunction->protoType());
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("HTMLElement"), false, false, false, htmlElementFunction);
    fetchData(this)->m_htmlElement = htmlElementFunction;

    escargot::ESFunctionObject* htmlHtmlElementFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("HTMLHtmlElement"), 0, false);
    htmlHtmlElementFunction->protoType().asESPointer()->asESObject()->set__proto__(htmlElementFunction->protoType());
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("HTMLHtmlElement"), false, false, false, htmlHtmlElementFunction);
    fetchData(this)->m_htmlHtmlElement = htmlHtmlElementFunction;

    escargot::ESFunctionObject* htmlHeadElementFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("HTMLHeadElement"), 0, false);
    htmlHeadElementFunction->protoType().asESPointer()->asESObject()->set__proto__(htmlElementFunction->protoType());
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("HTMLHeadElement"), false, false, false, htmlHeadElementFunction);
    fetchData(this)->m_htmlHeadElement = htmlHeadElementFunction;

    escargot::ESFunctionObject* htmlBodyElementFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("HTMLBodyElement"), 0, false);
    htmlBodyElementFunction->protoType().asESPointer()->asESObject()->set__proto__(htmlElementFunction->protoType());
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("HTMLBodyElement"), false, false, false, htmlBodyElementFunction);
    fetchData(this)->m_htmlBodyElement = htmlBodyElementFunction;
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


/*
    escargot::ESFunctionObject* getElementByIdFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue v = instance->currentExecutionContext()->resolveThisBinding();
        if (v.isObject()) {
            if (v.asESPointer()->asESObject()->extraData() == ScriptWrappable::NodeObject) {
                Node* nd = (Node*)v.asESPointer()->asESObject();
                if (nd->isElement()) {
                    Element* ret = nd->asElement()->getElementById(String::createASCIIString(instance->currentExecutionContext()->readArgument(0).toString()->utf8Data()));
                    if (ret) {
                        return (escargot::ESObject*)ret;
                    }
                }
            }
        }
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("getElementById"), 1, false);
    elementFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("getElementById"), false, false, false, getElementByIdFunction);
*/
