#include "StarFishConfig.h"
#include "ScriptWrappable.h"

#include "platform/window/Window.h"
#include "dom/DOM.h"

#include "vm/ESVMInstance.h"

#include "dom/binding/ScriptBindingInstance.h"
#include "dom/binding/escargot/ScriptBindingInstanceDataEscargot.h"


namespace StarFish {

static ScriptBindingInstanceDataEscargot* fetchData(ScriptBindingInstance* instance)
{
    return (ScriptBindingInstanceDataEscargot *)instance->data();
}

ScriptWrappable::ScriptWrappable()
    : escargot::ESObject(escargot::ESPointer::Type::ESObject, escargot::ESValue())
{

}

ScriptWrappableGlobalObject::ScriptWrappableGlobalObject()
{
}

ScriptWrappableGlobalObject* ScriptWrappableGlobalObject::fetch()
{
    auto a = escargot::ESVMInstance::currentInstance()->globalObject()->get(escargot::ESString::create("__windowData"));
    return (ScriptWrappableGlobalObject*)a.asESPointer();
}



void ScriptWrappableGlobalObject::initScriptWrappableWindow(Window* window)
{
    m_object = escargot::ESVMInstance::currentInstance()->globalObject();
    escargot::ESVMInstance::currentInstance()->globalObject()->defineDataProperty(escargot::ESString::create("__windowData"), false, false, false, escargot::ESValue((escargot::ESPointer*)this));
    auto data = fetchData(window->starFish()->scriptBindingInstance());
    ((escargot::ESObject *)this->m_object)->set__proto__(data->m_window->protoType());
    ((escargot::ESObject *)this->m_object)->setExtraData(ScriptWrappable::WindowObject);

    escargot::ESFunctionObject* setTimeoutFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue v = instance->currentExecutionContext()->resolveThisBinding();
        if (v.isObject()) {
            if (v.asESPointer()->asESObject()->extraData() == ScriptWrappable::WindowObject) {
                if (instance->currentExecutionContext()->readArgument(0).isESPointer() &&
                        instance->currentExecutionContext()->readArgument(0).asESPointer() &&
                        instance->currentExecutionContext()->readArgument(0).asESPointer()->isESFunctionObject()) {
                    if (instance->currentExecutionContext()->readArgument(1).isNumber()) {
                        Window* wnd = (Window*)ScriptWrappableGlobalObject::fetch();
                        wnd->setTimeout([](Window* wnd, void* data) {
                            escargot::ESFunctionObject* fn = (escargot::ESFunctionObject*)data;
                            escargot::ESFunctionObject::call(escargot::ESVMInstance::currentInstance(),
                                    fn, escargot::ESValue(), NULL, 0, false);
                        }, instance->currentExecutionContext()->readArgument(1).toUint32(),
                        instance->currentExecutionContext()->readArgument(0).asESPointer());
                    }
                }
            }
        }
        return escargot::ESValue();
    }, escargot::ESString::create("setTimeout"), 2, false);
    ((escargot::ESObject *)this->m_object)->defineDataProperty(escargot::ESString::create("setTimeout"), false, false, false, setTimeoutFunction);

}

void ScriptWrappable::initScriptWrappable(Node* ptr)
{
    Node* node = (Node*)this;
    initScriptWrappable(ptr, node->document()->scriptBindingInstance());
}

void ScriptWrappable::initScriptWrappable(Node* ptr, ScriptBindingInstance* instance)
{
    auto data = fetchData(instance);
    ((escargot::ESObject *)this)->set__proto__(data->m_node->protoType());
    ((escargot::ESObject *)this)->setExtraData(NodeObject);
}

void ScriptWrappable::initScriptWrappable(Element* element)
{
    Node* node = (Node*)this;
    initScriptWrappable(element, node->document()->scriptBindingInstance());
}

void ScriptWrappable::initScriptWrappable(DocumentType* element)
{
    auto data = fetchData(element->document()->scriptBindingInstance());
    ((escargot::ESObject *)this)->set__proto__(data->m_documentType->protoType());
}

void ScriptWrappable::initScriptWrappable(Element* element, ScriptBindingInstance* instance)
{
    auto data = fetchData(instance);
    ((escargot::ESObject *)this)->set__proto__(data->m_element->protoType());

}

void ScriptWrappable::initScriptWrappable(Document*)
{
    Node* node = (Node*)this;
    auto data = fetchData(node->document()->scriptBindingInstance());
    ((escargot::ESObject *)this)->set__proto__(data->m_document->protoType());
}

void ScriptWrappable::initScriptWrappable(HTMLDocument*)
{
    Node* node = (Node*)this;
    auto data = fetchData(node->document()->scriptBindingInstance());
    ((escargot::ESObject *)this)->set__proto__(data->m_htmlDocument->protoType());
}

void ScriptWrappable::initScriptWrappable(CharacterData* ptr)
{
    Node* node = (Node*)this;
    auto data = fetchData(node->document()->scriptBindingInstance());
    ((escargot::ESObject *)this)->set__proto__(data->m_characterData->protoType());
}

void ScriptWrappable::initScriptWrappable(Text* ptr)
{
    Node* node = (Node*)this;
    auto data = fetchData(node->document()->scriptBindingInstance());
    ((escargot::ESObject *)this)->set__proto__(data->m_text->protoType());
}

void ScriptWrappable::initScriptWrappable(HTMLElement* ptr)
{
    Node* node = (Node*)this;
    auto data = fetchData(node->document()->scriptBindingInstance());
    ((escargot::ESObject *)this)->set__proto__(data->m_htmlElement->protoType());
}

void ScriptWrappable::initScriptWrappable(HTMLHtmlElement* ptr)
{
    Node* node = (Node*)this;
    auto data = fetchData(node->document()->scriptBindingInstance());
    ((escargot::ESObject *)this)->set__proto__(data->m_htmlHtmlElement->protoType());
}

void ScriptWrappable::initScriptWrappable(HTMLHeadElement* ptr)
{
    Node* node = (Node*)this;
    auto data = fetchData(node->document()->scriptBindingInstance());
    ((escargot::ESObject *)this)->set__proto__(data->m_htmlHeadElement->protoType());
}

void ScriptWrappable::initScriptWrappable(HTMLBodyElement* ptr)
{
    Node* node = (Node*)this;
    auto data = fetchData(node->document()->scriptBindingInstance());
    ((escargot::ESObject *)this)->set__proto__(data->m_htmlBodyElement->protoType());
}

void ScriptWrappable::initScriptWrappable(HTMLScriptElement* ptr)
{
    Node* node = (Node*)this;
    auto data = fetchData(node->document()->scriptBindingInstance());
    ((escargot::ESObject *)this)->set__proto__(data->m_htmlScriptElement->protoType());
}

void ScriptWrappable::initScriptWrappable(HTMLImageElement* ptr)
{
    Node* node = (Node*)this;
    auto data = fetchData(node->document()->scriptBindingInstance());
    ((escargot::ESObject *)this)->set__proto__(data->m_htmlImageElement->protoType());
}

void ScriptWrappable::initScriptWrappable(HTMLDivElement* ptr)
{
    Node* node = (Node*)this;
    auto data = fetchData(node->document()->scriptBindingInstance());
    ((escargot::ESObject *)this)->set__proto__(data->m_htmlDivElement->protoType());
}

bool ScriptWrappable::hasProperty(String* name)
{
    return escargot::ESObject::hasProperty(escargot::ESString::create(name->utf8Data()));
}

void ScriptWrappable::callFunction(String* name)
{
    escargot::ESObject* obj = (escargot::ESObject*)this;
    escargot::ESValue fn = obj->get(escargot::ESString::create(name->utf8Data()));
    escargot::ESVMInstance* instance = escargot::ESVMInstance::currentInstance();

    std::jmp_buf tryPosition;
    if (setjmp(instance->registerTryPos(&tryPosition)) == 0) {
        escargot::ESFunctionObject::call(instance, fn, obj, NULL, 0, false);
        instance->unregisterTryPos(&tryPosition);
    } else {
        escargot::ESValue err = instance->getCatchedError();
        printf("Uncaught %s\n", err.toString()->utf8Data());
    }
}

void ScriptWrappable::initScriptWrappable(Event* ptr, ScriptBindingInstance* instance)
{
    auto data = fetchData(instance);
    ((escargot::ESObject *)this)->set__proto__(data->m_event->protoType());
    ((escargot::ESObject *)this)->setExtraData(EventObject);
}

void ScriptWrappable::initScriptWrappable(UIEvent* ptr, ScriptBindingInstance* instance)
{
    auto data = fetchData(instance);
    ((escargot::ESObject *)this)->set__proto__(data->m_uiEvent->protoType());
}

void ScriptWrappable::initScriptWrappable(MouseEvent* ptr, ScriptBindingInstance* instance)
{
    auto data = fetchData(instance);
    ((escargot::ESObject *)this)->set__proto__(data->m_mouseEvent->protoType());
}

void ScriptWrappable::initScriptWrappable(HTMLCollection* ptr, ScriptBindingInstance* instance)
{
    auto data = fetchData(instance);
    ((escargot::ESObject *)this)->set__proto__(data->m_htmlCollection->protoType());
    ((escargot::ESObject *)this)->setExtraData(HTMLCollectionObject);
}


}
