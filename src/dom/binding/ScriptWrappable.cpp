#include "StarFishConfig.h"
#include "ScriptWrappable.h"

#include "platform/window/Window.h"
#include "dom/DOM.h"

#include "Escargot.h"
#include "vm/ESVMInstance.h"

#include "dom/binding/ScriptBindingInstance.h"
#include "dom/binding/escargot/ScriptBindingInstanceDataEscargot.h"


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
}
}
