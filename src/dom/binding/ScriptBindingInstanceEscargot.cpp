#include "StarFishConfig.h"
#include "ScriptBindingInstance.h"

#include "ScriptWrappable.h"

#include "platform/window/Window.h"
#include "dom/DOM.h"

#include "Escargot.h"
#include "vm/ESVMInstance.h"

#include "dom/binding/escargot/ScriptBindingInstanceDataEscargot.h"


namespace StarFish {

COMPILE_ASSERT(sizeof(ScriptWrappable) == sizeof(escargot::ESObject), "");

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

void ScriptBindingInstance::initBinding(StarFish* sf)
{
    escargot::ESFunctionObject* windowFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("Window"), 0, false);
    windowFunction->protoType().asESPointer()->asESObject()->forceNonVectorHiddenClass(false);
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("Window"), false, false, false, windowFunction);
    fetchData(this)->m_window = windowFunction;

    windowFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("document"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj) -> escargot::ESValue {
        ASSERT(escargot::ESValue((escargot::ESObject *)((Window *)originalObj)->document()).isObject());
        return (escargot::ESObject *)((Window *)originalObj)->document();
    }, NULL, false, false, false);

    fetchData(this)->m_instance->globalObject()->defineAccessorProperty(escargot::ESString::create("document"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj) -> escargot::ESValue {
        return escargot::ESVMInstance::currentInstance()->globalObject()->get(escargot::ESString::create("window")).asESPointer()->asESObject()->get(escargot::ESString::create("document"));
    }, NULL, false, false, false);

    escargot::ESFunctionObject* nodeFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("Node"), 0, false);
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("Node"), false, false, false, nodeFunction);
    fetchData(this)->m_node = nodeFunction;

    escargot::ESFunctionObject* elementFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("Element"), 0, false);
    elementFunction->protoType().asESPointer()->asESObject()->set__proto__(nodeFunction->protoType());
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("Element"), false, false, false, elementFunction);
    fetchData(this)->m_element = elementFunction;

    escargot::ESFunctionObject* documentElementFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("DocumentElement"), 0, false);
    documentElementFunction->protoType().asESPointer()->asESObject()->set__proto__(elementFunction->protoType());
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("DocumentElement"), false, false, false, documentElementFunction);
    fetchData(this)->m_documentElement = documentElementFunction;

    escargot::ESFunctionObject* imageElementFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("ImageElement"), 0, false);
    imageElementFunction->protoType().asESPointer()->asESObject()->set__proto__(elementFunction->protoType());
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("ImageElement"), false, false, false, imageElementFunction);
    fetchData(this)->m_imageElement = imageElementFunction;

    escargot::ESFunctionObject* textElementFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance*) -> escargot::ESValue
        {
            return escargot::ESValue();
        }, escargot::ESString::create("TextElement"), 0, false);
    textElementFunction->protoType().asESPointer()->asESObject()->forceNonVectorHiddenClass(false);
    textElementFunction->protoType().asESPointer()->asESObject()->set__proto__(elementFunction->protoType());
    fetchData(this)->m_instance->globalObject()->defineDataProperty(escargot::ESString::create("TextElement"), false, false, false, textElementFunction);
    fetchData(this)->m_textElement = textElementFunction;
}

void ScriptBindingInstance::evaluate(String* str)
{
    fetchData(this)->m_instance->evaluate(escargot::ESString::create(str->utf8Data()));
}

}
