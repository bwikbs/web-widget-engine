#include "StarFishConfig.h"
#include "ScriptWrappable.h"

#include "platform/window/Window.h"
#include "dom/DOM.h"

#include "Escargot.h"
#include "vm/ESVMInstance.h"

#include "dom/binding/ScriptBindingInstance.h"
#include "dom/binding/escargot/ScriptBindingInstanceDataEscargot.h"

#include <Elementary.h>
#include <Ecore_X.h>
#include <Ecore_Input.h>
#include <Ecore_Input_Evas.h>


namespace StarFish {

static ScriptBindingInstanceDataEscargot* fetchData(ScriptBindingInstance* instance)
{
    return (ScriptBindingInstanceDataEscargot *)instance->data();
}


void ScriptWrappable::initScriptWrappableWindow(Window* window)
{
    auto data = fetchData(window->starFish()->scriptBindingInstance());
    data->m_instance->globalObject()->set(escargot::ESString::create("window"), window);
    ((escargot::ESObject *)this)->set__proto__(data->m_window->protoType());
    ((escargot::ESObject *)this)->setExtraData(WindowObject);

    escargot::ESFunctionObject* setTimeoutFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue v = instance->currentExecutionContext()->resolveThisBinding();
        if (v.isObject()) {
            if (v.asESPointer()->asESObject()->extraData() == ScriptWrappable::WindowObject) {
                if (instance->currentExecutionContext()->readArgument(0).isESPointer() &&
                        instance->currentExecutionContext()->readArgument(0).asESPointer() &&
                        instance->currentExecutionContext()->readArgument(0).asESPointer()->isESFunctionObject()) {
                    if (instance->currentExecutionContext()->readArgument(1).isNumber()) {
                        Window* wnd = (Window*)v.asESPointer()->asESObject();
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
    ((escargot::ESObject *)this)->defineDataProperty(escargot::ESString::create("setTimeout"), false, false, false, setTimeoutFunction);

}

void ScriptWrappable::initScriptWrappable(Node* ptr)
{
    Node* node = (Node*)this;
    initScriptWrappable(ptr, node->documentElement()->scriptBindingInstance());
}

void ScriptWrappable::initScriptWrappable(Node* ptr, ScriptBindingInstance* instance)
{
    auto data = fetchData(instance);
    ((escargot::ESObject *)this)->set__proto__(data->m_node->protoType());

    ((escargot::ESObject *)this)->defineAccessorProperty(escargot::ESString::create("nextSibling"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj) -> escargot::ESValue {
        Node* nd = ((Node *)originalObj)->nextSibling();
        if (nd == nullptr)
            return escargot::ESValue(escargot::ESValue::ESNull);
        return escargot::ESValue((escargot::ESObject *)nd);
    }, NULL, false, false, false);


    ((escargot::ESObject *)this)->defineAccessorProperty(escargot::ESString::create("angle"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj) -> escargot::ESValue {
        return escargot::ESValue(((Node *)originalObj)->angle());
    }, [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, const escargot::ESValue& value) {
        ((Node *)originalObj)->setAngle(value.toNumber());
        ((Node *)originalObj)->setNeedsRendering();
    }, true, false, false);

    ((escargot::ESObject *)this)->defineAccessorProperty(escargot::ESString::create("scale"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj) -> escargot::ESValue {
        return escargot::ESValue(((Node *)originalObj)->scale());
    }, [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, const escargot::ESValue& value) {
        ((Node *)originalObj)->setScale(value.toNumber());
        ((Node *)originalObj)->setNeedsRendering();
    }, true, false, false);

    ((escargot::ESObject *)this)->setExtraData(NodeObject);
}

void ScriptWrappable::initScriptWrappable(Element* element)
{
    Node* node = (Node*)this;
    initScriptWrappable(element, node->documentElement()->scriptBindingInstance());
}

void ScriptWrappable::initScriptWrappable(Element* element, ScriptBindingInstance* instance)
{
    auto data = fetchData(instance);
    ((escargot::ESObject *)this)->set__proto__(data->m_element->protoType());

    ((escargot::ESObject *)this)->defineAccessorProperty(escargot::ESString::create("firstChild"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj) -> escargot::ESValue {
        Node* nd = ((Element *)originalObj)->firstChild();
        if (nd == nullptr)
            return escargot::ESValue(escargot::ESValue::ESNull);
        return escargot::ESValue((escargot::ESObject *)nd);
    }, NULL, false, false, false);

}

void ScriptWrappable::initScriptWrappable(DocumentElement*)
{
    Node* node = (Node*)this;
    auto data = fetchData(node->documentElement()->scriptBindingInstance());
    ((escargot::ESObject *)this)->set__proto__(data->m_documentElement->protoType());
}

void ScriptWrappable::initScriptWrappable(ImageElement*)
{
    Node* node = (Node*)this;
    auto data = fetchData(node->documentElement()->scriptBindingInstance());
    ((escargot::ESObject *)this)->set__proto__(data->m_imageElement->protoType());

    ((escargot::ESObject *)this)->defineAccessorProperty(escargot::ESString::create("src"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj) -> escargot::ESValue {
        ImageElement* nd = ((ImageElement *)originalObj);
        if (nd->src() == nullptr)
            return escargot::ESVMInstance::currentInstance()->strings().emptyString.string();
        return escargot::ESString::create(nd->src()->utf8Data());
    }, [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, const escargot::ESValue& value) {
        ImageElement* nd = ((ImageElement *)originalObj);
        nd->setSrc(String::createASCIIString(value.toString()->utf8Data()));
    }, true, false, false);
}

void ScriptWrappable::initScriptWrappable(TextElement*)
{
    Node* node = (Node*)this;
    auto data = fetchData(node->documentElement()->scriptBindingInstance());
    ((escargot::ESObject *)this)->set__proto__(data->m_textElement->protoType());

    ((escargot::ESObject *)this)->defineAccessorProperty(escargot::ESString::create("text"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj) -> escargot::ESValue {
        TextElement* nd = ((TextElement *)originalObj);
        if (nd->text() == nullptr)
            return escargot::ESVMInstance::currentInstance()->strings().emptyString.string();
        return escargot::ESString::create(nd->text()->utf8Data());
    }, [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, const escargot::ESValue& value) {
        TextElement* nd = ((TextElement *)originalObj);
        nd->setText(String::createASCIIString(value.toString()->utf8Data()));
    }, true, false, false);

    ((escargot::ESObject *)this)->defineAccessorProperty(escargot::ESString::create("textColor"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj) -> escargot::ESValue {
        TextElement* nd = ((TextElement *)originalObj);
        return escargot::ESString::create(nd->textColor().toString()->utf8Data());
    }, [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, const escargot::ESValue& value) {
        TextElement* nd = ((TextElement *)originalObj);
        nd->setTextColor(Color::fromString(String::createASCIIString(value.toString()->utf8Data())));
    }, true, false, false);
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

}
