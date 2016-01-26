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

    /* 4.4 Interface Node */

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("ELEMENT_NODE"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::ELEMENT_NODE);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("ATTRIBUTE_NODE"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::ATTRIBUTE_NODE);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("TEXT_NODE"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::TEXT_NODE);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("CDATA_SECTION_NODE"),
        [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::CDATA_SECTION_NODE);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("ENTITY_REFERENCE_NODE"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::ENTITY_REFERENCE_NODE);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("ENTITY_NODE"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::ENTITY_NODE);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("PROCESSING_INSTRUCTION_NODE"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::PROCESSING_INSTRUCTION_NODE);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("COMMENT_NODE"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::COMMENT_NODE);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("DOCUMENT_NODE"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::DOCUMENT_NODE);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("DOCUMENT_TYPE_NODE"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::DOCUMENT_TYPE_NODE);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("DOCUMENT_FRAGMENT_NODE"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::DOCUMENT_FRAGMENT_NODE);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("NOTATION_NODE"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::NOTATION_NODE);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("nodeType"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        unsigned short nodeType = ((Node *)originalObj)->nodeType();
        return escargot::ESValue(nodeType);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("nodeName"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        String* nodeName = ((Node *)originalObj)->nodeName();
        return toJSString(nodeName);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("baseURI"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        String* uri = ((Node *)originalObj)->baseURI();
        return toJSString(uri);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("ownerDocument"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Document* doc = ((Node *)originalObj)->ownerDocument();
        if(doc == nullptr) {
            return escargot::ESValue(escargot::ESValue::ESNull);
        }
        return escargot::ESValue((escargot::ESObject*)doc);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("parentNode"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* p = ((Node *)originalObj)->parentNode();
        if(p == nullptr) {
            return escargot::ESValue(escargot::ESValue::ESNull);
        }
        return escargot::ESValue((escargot::ESObject*)p);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("parentElement"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Element* p = ((Node *)originalObj)->parentElement();
        if(p == nullptr) {
            return escargot::ESValue(escargot::ESValue::ESNull);
        }
        return escargot::ESValue((escargot::ESObject*)p);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("childNodes"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        NodeList* list = ((Node *)originalObj)->childNodes();
        STARFISH_ASSERT(list);
        return escargot::ESValue((escargot::ESObject*)list);
    }, NULL, false, false, false);

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

    auto nodeValueGetter = [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        String* s = ((Node *)originalObj)->nodeValue();
        if (s == nullptr)
            return escargot::ESValue(escargot::ESValue::ESNull);
        return toJSString(s);
    };
    auto nodeValueSetter = [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name, const escargot::ESValue& v) {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        ((Node*)originalObj)->setNodeValue(toBrowserString(v));
    };
    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("nodeValue"), nodeValueGetter, nodeValueSetter, false, false, false);

    auto textContentGetter = [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        String* s = ((Node *)originalObj)->textContent();
        if (s == nullptr)
            return escargot::ESValue(escargot::ESValue::ESNull);
        return toJSString(s);
    };
    auto textContentSetter = [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name, const escargot::ESValue& v) {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        ((Node *)originalObj)->setTextContent(toBrowserString(v));
    };
    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("textContent"), textContentGetter, textContentSetter, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("cloneNode"), false, false, false,
        escargot::ESFunctionObject::create(nullptr, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
            escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
            CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
            Node* obj = (Node*)thisValue.asESPointer()->asESObject();
            escargot::ESValue arg = instance->currentExecutionContext()->readArgument(0);
            bool deepClone = false;
            if(arg.isBoolean()) {
                deepClone = arg.asBoolean();
            }
            Node* node = obj->cloneNode(deepClone);
            return escargot::ESValue((escargot::ESObject*)node);
        }, escargot::ESString::create("cloneNode"), 1, false)
    );

    NodeFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("isEqualNode"), false, false, false,
        escargot::ESFunctionObject::create(nullptr, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
            escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
            CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
            CHECK_TYPEOF(instance->currentExecutionContext()->readArgument(0), ScriptWrappable::Type::NodeObject);
            Node* obj = (Node*)thisValue.asESPointer()->asESObject();
            Node* node = (Node*)instance->currentExecutionContext()->readArgument(0).asESPointer()->asESObject();
            bool found = obj->isEqualNode(node);
            return escargot::ESValue(found);
        }, escargot::ESString::create("isEqualNode"), 1, false)
    );

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("DOCUMENT_POSITION_DISCONNECTED"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::DOCUMENT_POSITION_DISCONNECTED);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("DOCUMENT_POSITION_PRECEDING"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::DOCUMENT_POSITION_PRECEDING);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("DOCUMENT_POSITION_FOLLOWING"),
        [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::DOCUMENT_POSITION_FOLLOWING);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("DOCUMENT_POSITION_CONTAINS"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::DOCUMENT_POSITION_CONTAINS);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("DOCUMENT_POSITION_CONTAINED_BY"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::DOCUMENT_POSITION_CONTAINED_BY);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESValue(Node::DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC);
    }, NULL, false, false, false);

    NodeFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("compareDocumentPosition"), false, false, false,
        escargot::ESFunctionObject::create(nullptr, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
            escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
            CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
            CHECK_TYPEOF(instance->currentExecutionContext()->readArgument(0), ScriptWrappable::Type::NodeObject);
            Node* obj = (Node*)thisValue.asESPointer()->asESObject();
            Node* nodeRef = (Node*)instance->currentExecutionContext()->readArgument(0).asESPointer()->asESObject();
            unsigned short pos = obj->compareDocumentPosition(nodeRef);
            return escargot::ESValue(pos);
        }, escargot::ESString::create("compareDocumentPosition"), 1, false)
    );

    NodeFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("contains"), false, false, false,
        escargot::ESFunctionObject::create(nullptr, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
            escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
            CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
            CHECK_TYPEOF(instance->currentExecutionContext()->readArgument(0), ScriptWrappable::Type::NodeObject);
            Node* obj = (Node*)thisValue.asESPointer()->asESObject();
            Node* nodeRef = (Node*)instance->currentExecutionContext()->readArgument(0).asESPointer()->asESObject();
            bool found = obj->contains(nodeRef);
            return escargot::ESValue(found);
        }, escargot::ESString::create("contains"), 1, false)
    );

    NodeFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("lookupPrefix"), false, false, false,
        escargot::ESFunctionObject::create(nullptr, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
            escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
            CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
            CHECK_TYPEOF(instance->currentExecutionContext()->readArgument(0), ScriptWrappable::Type::NodeObject);
            Node* obj = (Node*)thisValue.asESPointer()->asESObject();
            String* namespaceUri = (String*)instance->currentExecutionContext()->readArgument(0).asESPointer()->asESObject();
            String* ns = obj->lookupPrefix(namespaceUri);
            if (ns == nullptr) {
                return escargot::ESValue(escargot::ESValue::ESNull);
            }
            return toJSString(ns);
        }, escargot::ESString::create("lookupPrefix"), 1, false)
    );

    NodeFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("lookupNamespaceURI"), false, false, false,
        escargot::ESFunctionObject::create(nullptr, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
            escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
            CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
            CHECK_TYPEOF(instance->currentExecutionContext()->readArgument(0), ScriptWrappable::Type::NodeObject);
            Node* obj = (Node*)thisValue.asESPointer()->asESObject();
            String* prefix = (String*)instance->currentExecutionContext()->readArgument(0).asESPointer()->asESObject();
            String* ns = obj->lookupNamespaceURI(prefix);
            if (ns == nullptr) {
                return escargot::ESValue(escargot::ESValue::ESNull);
            }
            return toJSString(ns);
        }, escargot::ESString::create("lookupNamespaceURI"), 1, false)
    );

    NodeFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("isDefaultNamespace"), false, false, false,
        escargot::ESFunctionObject::create(nullptr, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
            escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
            CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
            CHECK_TYPEOF(instance->currentExecutionContext()->readArgument(0), ScriptWrappable::Type::NodeObject);
            Node* obj = (Node*)thisValue.asESPointer()->asESObject();
            String* namespaceUri = (String*)instance->currentExecutionContext()->readArgument(0).asESPointer()->asESObject();
            bool ns = obj->isDefaultNamespace(namespaceUri);
            return escargot::ESValue(ns);
        }, escargot::ESString::create("isDefaultNamespace"), 1, false)
    );

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

    NodeFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("removeChild"), false, false, false,
        escargot::ESFunctionObject::create(nullptr, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
            escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
            CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
            CHECK_TYPEOF(instance->currentExecutionContext()->readArgument(0), ScriptWrappable::Type::NodeObject);
            Node* obj = (Node*)thisValue.asESPointer()->asESObject();
            Node* child = (Node*)instance->currentExecutionContext()->readArgument(0).asESPointer()->asESObject();
            Node* n = obj->removeChild(child);
            return escargot::ESValue((escargot::ESObject *)n);
        }, escargot::ESString::create("removeChild"), 1, false)
    );

    NodeFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("replaceChild"), false, false, false,
        escargot::ESFunctionObject::create(nullptr, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
            escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
            CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
            CHECK_TYPEOF(instance->currentExecutionContext()->readArgument(0), ScriptWrappable::Type::NodeObject);
            Node* obj = (Node*)thisValue.asESPointer()->asESObject();
            Node* node = (Node*)instance->currentExecutionContext()->readArgument(0).asESPointer()->asESObject();
            Node* child = (Node*)instance->currentExecutionContext()->readArgument(1).asESPointer()->asESObject();
            Node* n = obj->replaceChild(node, child);
            return escargot::ESValue((escargot::ESObject *)n);
        }, escargot::ESString::create("replaceChild"), 1, false)
    );

    NodeFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("insertBefore"), false, false, false,
        escargot::ESFunctionObject::create(nullptr, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
            escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
            CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
            CHECK_TYPEOF(instance->currentExecutionContext()->readArgument(0), ScriptWrappable::Type::NodeObject);
            Node* obj = (Node*)thisValue.asESPointer()->asESObject();
            Node* node = (Node*)instance->currentExecutionContext()->readArgument(0).asESPointer()->asESObject();
            Node* child = (Node*)instance->currentExecutionContext()->readArgument(1).asESPointer()->asESObject();
            Node* n = obj->insertBefore(node, child);
            return escargot::ESValue((escargot::ESObject *)n);
        }, escargot::ESString::create("insertBefore"), 1, false)
    );

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

    auto nextElementChildGetter = [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj)->nextElementSibling();
        if (nd == nullptr)
            return escargot::ESValue(escargot::ESValue::ESNull);
        return escargot::ESValue((escargot::ESObject *)nd);
    };

    auto previousElementChildGetter = [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj)->previousElementSibling();
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
        lastElementChildGetter, NULL, false, false, false);
    ElementFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("nextElementSibling"),
        nextElementChildGetter, NULL, false, false, false);
    ElementFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("previousElementSibling"),
        previousElementChildGetter, NULL, false, false, false);
    ElementFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("childElementCount"),
        childElementCountGetter, NULL, false, false, false);

    ElementFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("localName"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj);
        if (nd->isElement()) {
            return escargot::ESString::create(nd->asElement()->getAttribute(String::fromUTF8("localName"))->utf8Data());
        } else {
            THROW_ILLEGAL_INVOCATION();
        }
    }, NULL, true, true, true);

    ElementFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("tagName"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        Node* nd = ((Node *)originalObj);
        if (nd->isElement()) {
            // FIXME(JMP): We have to fix this to follow DOM spec after implementing Namespace
            return escargot::ESString::create(nd->asElement()->getAttribute(String::fromUTF8("localName"))->toUpper()->utf8Data());
        } else {
            THROW_ILLEGAL_INVOCATION();
        }
    }, NULL, true, true, true);

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

    ElementFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("className"),
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

    auto childrenGetter = [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        HTMLCollection* nd = ((Node *)originalObj)->children();
        if (nd == nullptr)
            return escargot::ESValue(escargot::ESValue::ESUndefined);
        return escargot::ESValue((escargot::ESObject *)nd);
    };
    ElementFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("children"), childrenGetter, NULL, false, false, false);

    auto classListGetter = [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        DOMTokenList* nd = ((Node *)originalObj)->classList();
        if (nd == nullptr)
            return escargot::ESValue(escargot::ESValue::ESUndefined);
        return escargot::ESValue((escargot::ESObject *)nd);
    };
    ElementFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("classList"), classListGetter, NULL, false, false, false);

    auto attributesGetter = [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        NamedNodeMap* nd = ((Node *)originalObj)->attributes();
        if (nd == nullptr)
            return escargot::ESValue(escargot::ESValue::ESUndefined);
        return escargot::ESValue((escargot::ESObject *)nd);
    };
    ElementFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("attributes"), attributesGetter, NULL, false, false, false);

    escargot::ESFunctionObject* removeFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
        Node* obj = (Node*)thisValue.asESPointer()->asESObject();
        obj->remove();
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("remove"), 0, false);
    ElementFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("remove"), false, false, false, removeFunction);

    auto styleGetter = [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        if(!((Node*)originalObj)->isElement()) {
            return escargot::ESValue(escargot::ESValue::ESNull);
        }
        CSSStyleDeclaration* s = ((Element *)originalObj)->inlineStyle();
        if(s == nullptr) {
            return escargot::ESValue(escargot::ESValue::ESNull);
        } else {
            return escargot::ESValue((escargot::ESObject *)s);
        }
    };
    ElementFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("style"), styleGetter, NULL, false, false, false);

    DEFINE_FUNCTION(DocumentType, NodeFunction->protoType());
    fetchData(this)->m_documentType = DocumentTypeFunction;

    DocumentTypeFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("remove"), false, false, false, removeFunction);


    /* 4.7 Interface DocumentType */

    DocumentTypeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("name"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        String* s = ((DocumentType *)originalObj)->nodeName();
        return toJSString(s);
    }, NULL, false, false, false);

    DocumentTypeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("publicId"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        String* s = ((DocumentType *)originalObj)->publicId();
        return toJSString(s);
    }, NULL, false, false, false);

    DocumentTypeFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("systemId"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        String* s = ((DocumentType *)originalObj)->systemId();
        return toJSString(s);
    }, NULL, false, false, false);

    DEFINE_FUNCTION(Document, NodeFunction->protoType());
    fetchData(this)->m_document = DocumentFunction;

    DocumentFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("body"),
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

    DocumentFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("URL"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESString::create("NOT supported yet!");
    }, NULL, false, false, false);

    DocumentFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("documentURI"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESString::create("NOT supported yet!");
    }, NULL, false, false, false);

    DocumentFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("origin"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESString::create("NOT supported yet!");
    }, NULL, false, false, false);

    DocumentFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("compatMode"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESString::create("NOT supported yet!");
    }, NULL, false, false, false);

    DocumentFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("characterSet"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESString::create("NOT supported yet!");
    }, NULL, false, false, false);

    DocumentFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("charset"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESString::create("NOT supported yet!");
    }, NULL, false, false, false);

    DocumentFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("inputEncoding"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESString::create("NOT supported yet!");
    }, NULL, false, false, false);

    DocumentFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("contentType"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        return escargot::ESString::create("NOT supported yet!");
    }, NULL, false, false, false);

    DocumentFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("firstElementChild"),
        firstElementChildGetter, NULL, false, false, false);
    DocumentFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("lastElementChild"),
        lastElementChildGetter, NULL, false, false, false);
    DocumentFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("childElementCount"),
        firstElementChildGetter, NULL, false, false, false);

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

    escargot::ESFunctionObject* getDoctypeFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
        Node* obj = (Node*)thisValue.asESPointer()->asESObject();

        if (obj->isDocument()) {
            Node* docTypeNode = obj->firstChild();
            if(docTypeNode->isDocumentType()){
                return escargot::ESValue((escargot::ESObject *)docTypeNode);
            }
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("doctype"), 1, false);
    DocumentFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("doctype"), false, false, false, getDoctypeFunction);

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

    escargot::ESFunctionObject* createCommentNodeFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
        Node* obj = (Node*)thisValue.asESPointer()->asESObject();

        if (obj->isDocument()) {
            Document* doc = obj->asDocument();
            escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
            if (argValue.isESString()) {
                escargot::ESString* argStr = argValue.asESString();
                Comment* elem = doc->createComment(String::fromUTF8(argStr->utf8Data()));
                if (elem != nullptr)
                    return escargot::ESValue((escargot::ESObject *)elem);
            }
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("createComment"), 1, false);
    DocumentFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("createComment"), false, false, false, createCommentNodeFunction);

    escargot::ESFunctionObject* getElementsByTagNameFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
        Node* obj = (Node*)thisValue.asESPointer()->asESObject();

        if (obj->isDocument()) {
            Document* doc = obj->asDocument();
            escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
            if (argValue.isESString()) {
                escargot::ESString* argStr = argValue.asESString();
                HTMLCollection* result = doc->getElementsByTagName(String::fromUTF8(argStr->utf8Data()));
                if (result != nullptr)
                    return escargot::ESValue((escargot::ESObject *)result);
            }
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("getElementsByTagName"), 1, false);
    DocumentFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("getElementsByTagName"), false, false, false, getElementsByTagNameFunction);

    escargot::ESFunctionObject*  getElementsByClassNameFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
        Node* obj = (Node*)thisValue.asESPointer()->asESObject();

        if (obj->isDocument()) {
            Document* doc = obj->asDocument();
            escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
            if (argValue.isESString()) {
                escargot::ESString* argStr = argValue.asESString();
                HTMLCollection* result = doc->getElementsByClassName(String::fromUTF8(argStr->utf8Data()));
                if (result != nullptr)
                    return escargot::ESValue((escargot::ESObject *)result);
            }
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("getElementsByClassName"), 1, false);
    DocumentFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("getElementsByClassName"), false, false, false, getElementsByClassNameFunction);

    escargot::ESFunctionObject*   createAttributeFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NodeObject);
        Node* obj = (Node*)thisValue.asESPointer()->asESObject();

        if (obj->isDocument()) {
            Document* doc = obj->asDocument();
            escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
            if (argValue.isESString()) {
                escargot::ESString* argStr = argValue.asESString();
                Attr* result = doc->createAttribute(String::fromUTF8(argStr->utf8Data()));
                if (result != nullptr)
                    return escargot::ESValue((escargot::ESObject *)result);
            }
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("createAttribute"), 1, false);
    DocumentFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("createAttribute"), false, false, false, createAttributeFunction);


    DocumentFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("children"), childrenGetter, NULL, false, false, false);

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

    CharacterDataFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("nextElementSibling"),
        nextElementChildGetter, NULL, false, false, false);
    CharacterDataFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("previousElementSibling"),
        previousElementChildGetter, NULL, false, false, false);

    CharacterDataFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("remove"), false, false, false, removeFunction);

    DEFINE_FUNCTION(Text, CharacterDataFunction->protoType());
    fetchData(this)->m_text = TextFunction;

    /* 4.10 Interface Text */

    TextFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("wholeText"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NodeObject);
        String* text = ((Text *)originalObj)->wholeText();
        return toJSString(text);
    }, NULL, false, false, false);

    DEFINE_FUNCTION(Comment, CharacterDataFunction->protoType());
    fetchData(this)->m_comment = CommentFunction;

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

    DEFINE_FUNCTION(HTMLBRElement, HTMLElementFunction->protoType());
    fetchData(this)->m_htmlBrElement = HTMLBRElementFunction;

    DEFINE_FUNCTION(HTMLCollection, fetchData(this)->m_instance->globalObject()->objectPrototype());
    fetchData(this)->m_htmlCollection = HTMLCollectionFunction;

    HTMLCollectionFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("length"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::HTMLCollectionObject);

        uint32_t len = ((HTMLCollection *)originalObj)->length();
        return escargot::ESValue(len);
    }, NULL, false, false, false);

    escargot::ESFunctionObject* itemFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::HTMLCollectionObject);

        escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
        if (argValue.isUInt32()) {
            Element* elem = ((HTMLCollection*) thisValue.asESPointer()->asESObject())->item(argValue.asUInt32());
            if (elem != nullptr)
                return escargot::ESValue((escargot::ESObject *)elem);
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("item"), 1, false);
    HTMLCollectionFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("item"), false, false, false, itemFunction);

    escargot::ESFunctionObject* namedItemFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::HTMLCollectionObject);

        escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
        if (argValue.isESString()) {
            Element* elem = ((HTMLCollection*) thisValue.asESPointer()->asESObject())->namedItem(String::fromUTF8(argValue.asESString()->utf8Data()));
            if (elem != nullptr)
                return escargot::ESValue((escargot::ESObject *)elem);
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("namedItem"), 1, false);
    HTMLCollectionFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("namedItem"), false, false, false, namedItemFunction);

    DEFINE_FUNCTION(NodeList, fetchData(this)->m_instance->globalObject()->objectPrototype());
    fetchData(this)->m_nodeList = NodeListFunction;

    DEFINE_FUNCTION(DOMTokenList, fetchData(this)->m_instance->globalObject()->objectPrototype());
    fetchData(this)->m_domTokenList = DOMTokenListFunction;

    DOMTokenListFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("length"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::DOMTokenListObject);

        uint32_t len = ((DOMTokenList *)originalObj)->length();
        return escargot::ESValue(len);
    }, NULL, false, false, false);

    escargot::ESFunctionObject* domTokenListItemFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::DOMTokenListObject);

        escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
        if (argValue.isUInt32()) {
            String* elem = ((DOMTokenList*) thisValue.asESPointer()->asESObject())->item(argValue.asUInt32());
            if (elem != nullptr)
                return escargot::ESString::create(elem->utf8Data());
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("item"), 1, false);
    DOMTokenListFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("item"), false, false, false, domTokenListItemFunction);

    escargot::ESFunctionObject* domTokenListContainsFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::DOMTokenListObject);

        escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
        if (argValue.isESString()) {
            bool res = ((DOMTokenList*) thisValue.asESPointer()->asESObject())->contains(String::fromUTF8(argValue.asESString()->utf8Data()));
            return escargot::ESValue(res);
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
    }, escargot::ESString::create("contains"), 1, false);
    DOMTokenListFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("contains"), false, false, false, domTokenListContainsFunction);

    escargot::ESFunctionObject* domTokenListAddFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::DOMTokenListObject);

        std::vector<String*, gc_allocator<String*>> tokens;
        int argCount = instance->currentExecutionContext()->argumentCount();
        for (int i = 0; i < argCount; i++) {
            escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(i);
            if (argValue.isESString()) {
                String* aa = String::fromUTF8(argValue.asESString()->utf8Data());
                tokens.push_back(aa);
            } else {
                THROW_ILLEGAL_INVOCATION()
            }
        }
        if (argCount > 0)
            ((DOMTokenList*) thisValue.asESPointer()->asESObject())->add(&tokens);
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("add"), 1, false);
    DOMTokenListFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("add"), false, false, false, domTokenListAddFunction);

    escargot::ESFunctionObject* domTokenListRemoveFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::DOMTokenListObject);

        std::vector<String*, gc_allocator<String*>> tokens;
        int argCount = instance->currentExecutionContext()->argumentCount();
        for (int i = 0; i < argCount; i++) {
            escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(i);
            if (argValue.isESString()) {
                String* aa = String::fromUTF8(argValue.asESString()->utf8Data());
                tokens.push_back(aa);
            } else {
                THROW_ILLEGAL_INVOCATION()
            }
        }
        if (argCount > 0)
            ((DOMTokenList*) thisValue.asESPointer()->asESObject())->remove(&tokens);
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("remove"), 1, false);
    DOMTokenListFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("remove"), false, false, false, domTokenListRemoveFunction);

    escargot::ESFunctionObject* domTokenListToggleFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::DOMTokenListObject);

        int argCount = instance->currentExecutionContext()->argumentCount();
        escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
        escargot::ESValue forceValue;
        if (argCount >= 2)
            forceValue = instance->currentExecutionContext()->readArgument(1);
        if (argCount > 0 && argValue.isESString()) {
            bool didAdd;
            if (argCount == 1) {
                didAdd = ((DOMTokenList*) thisValue.asESPointer()->asESObject())->toggle(String::fromUTF8(argValue.asESString()->utf8Data()), false, false);
            } else {
                ASSERT(forceValue.isBoolean());
                didAdd = ((DOMTokenList*) thisValue.asESPointer()->asESObject())->toggle(String::fromUTF8(argValue.asESString()->utf8Data()), true, forceValue.asBoolean());
              }
            return escargot::ESValue(didAdd);
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("toggle"), 1, false);
    DOMTokenListFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("toggle"), false, false, false, domTokenListToggleFunction);

    DEFINE_FUNCTION(DOMSettableTokenList, fetchData(this)->m_instance->globalObject()->objectPrototype());
    fetchData(this)->m_domSettableTokenList = DOMSettableTokenListFunction;

    DOMSettableTokenListFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("length"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::DOMSettableTokenListObject);

        uint32_t len = ((DOMTokenList *)originalObj)->length();
        return escargot::ESValue(len);
    }, NULL, false, false, false);

    escargot::ESFunctionObject* domSettableTokenListItemFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::DOMSettableTokenListObject);

        escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
        if (argValue.isUInt32()) {
            String* elem = ((DOMTokenList*) thisValue.asESPointer()->asESObject())->item(argValue.asUInt32());
            if (elem != nullptr)
                return escargot::ESString::create(elem->utf8Data());
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("item"), 1, false);
    DOMSettableTokenListFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("item"), false, false, false, domSettableTokenListItemFunction);

    escargot::ESFunctionObject* domSettableTokenListContainsFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::DOMSettableTokenListObject);

        escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
        if (argValue.isESString()) {
            bool res = ((DOMTokenList*) thisValue.asESPointer()->asESObject())->contains(String::fromUTF8(argValue.asESString()->utf8Data()));
            return escargot::ESValue(res);
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
    }, escargot::ESString::create("contains"), 1, false);
    DOMSettableTokenListFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("contains"), false, false, false, domSettableTokenListContainsFunction);

    escargot::ESFunctionObject* domSettableTokenListAddFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::DOMSettableTokenListObject);

        std::vector<String*, gc_allocator<String*>> tokens;
        int argCount = instance->currentExecutionContext()->argumentCount();
        for (int i = 0; i < argCount; i++) {
            escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(i);
            if (argValue.isESString()) {
                String* aa = String::fromUTF8(argValue.asESString()->utf8Data());
                tokens.push_back(aa);
            } else {
                THROW_ILLEGAL_INVOCATION()
            }
        }
        if (argCount > 0)
            ((DOMTokenList*) thisValue.asESPointer()->asESObject())->add(&tokens);
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("add"), 1, false);
    DOMSettableTokenListFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("add"), false, false, false, domSettableTokenListAddFunction);

    escargot::ESFunctionObject* domSettableTokenListRemoveFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::DOMSettableTokenListObject);

        std::vector<String*, gc_allocator<String*>> tokens;
        int argCount = instance->currentExecutionContext()->argumentCount();
        for (int i = 0; i < argCount; i++) {
            escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(i);
            if (argValue.isESString()) {
                String* aa = String::fromUTF8(argValue.asESString()->utf8Data());
                tokens.push_back(aa);
            } else {
                THROW_ILLEGAL_INVOCATION()
            }
        }
        if (argCount > 0)
            ((DOMTokenList*) thisValue.asESPointer()->asESObject())->remove(&tokens);
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("remove"), 1, false);
    DOMSettableTokenListFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("remove"), false, false, false, domSettableTokenListRemoveFunction);

    escargot::ESFunctionObject* domSettableTokenListToggleFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
        escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
        CHECK_TYPEOF(thisValue, ScriptWrappable::Type::DOMSettableTokenListObject);

        int argCount = instance->currentExecutionContext()->argumentCount();
        escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
        escargot::ESValue forceValue;
        if (argCount >= 2)
            forceValue = instance->currentExecutionContext()->readArgument(1);
        if (argCount > 0 && argValue.isESString()) {
            bool didAdd;
            if (argCount == 1) {
                didAdd = ((DOMTokenList*) thisValue.asESPointer()->asESObject())->toggle(String::fromUTF8(argValue.asESString()->utf8Data()), false, false);
            } else {
                ASSERT(forceValue.isBoolean());
                didAdd = ((DOMTokenList*) thisValue.asESPointer()->asESObject())->toggle(String::fromUTF8(argValue.asESString()->utf8Data()), true, forceValue.asBoolean());
              }
            return escargot::ESValue(didAdd);
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("toggle"), 1, false);
    DOMSettableTokenListFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("toggle"), false, false, false, domSettableTokenListToggleFunction);

    auto DOMSettableTokenListValueGetter = [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::DOMSettableTokenListObject);

        String* value = ((DOMSettableTokenList*) originalObj)->value();
        if (value != nullptr)
            return escargot::ESString::create(value->utf8Data());
        return escargot::ESValue(escargot::ESValue::ESNull);
    };

    auto DOMSettableTokenListValueSetter = [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name, const escargot::ESValue& v) {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::DOMSettableTokenListObject);

        ((DOMSettableTokenList*) originalObj)->setValue(toBrowserString(v));
    };

    DOMSettableTokenListFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("value"), DOMSettableTokenListValueGetter, DOMSettableTokenListValueSetter, false, false, false);



    DEFINE_FUNCTION(NamedNodeMap, fetchData(this)->m_instance->globalObject()->objectPrototype());
    fetchData(this)->m_namedNodeMap = NamedNodeMapFunction;

    NamedNodeMapFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("length"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::NamedNodeMapObject);

        uint32_t len = ((NamedNodeMap *)originalObj)->length();
        return escargot::ESValue(len);
    }, NULL, false, false, false);

    escargot::ESFunctionObject* NamedNodeMapItemFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
       escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
       CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NamedNodeMapObject);

       escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
       if (argValue.isUInt32()) {
           Attr* elem = ((NamedNodeMap*) thisValue.asESPointer()->asESObject())->item(argValue.asUInt32());
           if (elem != nullptr)
               return escargot::ESValue((escargot::ESObject *)elem);
       } else {
           THROW_ILLEGAL_INVOCATION()
       }
       return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("item"), 1, false);
    NamedNodeMapFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("item"), false, false, false, NamedNodeMapItemFunction);

    escargot::ESFunctionObject* NamedNodeMapGetNamedItemFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
       escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
       CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NamedNodeMapObject);

       escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
       if (argValue.isESString()) {
           Attr* elem = ((NamedNodeMap*) thisValue.asESPointer()->asESObject())->getNamedItem(String::fromUTF8(argValue.asESString()->utf8Data()));
           if (elem != nullptr)
               return escargot::ESValue((escargot::ESObject *)elem);
       } else {
           THROW_ILLEGAL_INVOCATION()
       }
       return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("getNamedItem"), 1, false);
    NamedNodeMapFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("getNamedItem"), false, false, false, NamedNodeMapGetNamedItemFunction);

    escargot::ESFunctionObject* NamedNodeMapSetNamedItemFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
       escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
       CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NamedNodeMapObject);

       escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
       CHECK_TYPEOF(argValue, ScriptWrappable::Type::AttrObject);

       Attr* old = ((NamedNodeMap*) thisValue.asESPointer()->asESObject())->getNamedItem(((Attr*) argValue.asESPointer()->asESObject())->name());
       Attr* toReturn = new Attr(((NamedNodeMap*) thisValue.asESPointer()->asESObject())->striptBindingInstance(), old->name(), old->value());
       ((NamedNodeMap*) thisValue.asESPointer()->asESObject())->setNamedItem((Attr*) argValue.asESPointer()->asESObject());
       if (toReturn != nullptr)
           return escargot::ESValue((escargot::ESObject *)toReturn);
       return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("setNamedItem"), 1, false);
    NamedNodeMapFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("setNamedItem"), false, false, false, NamedNodeMapSetNamedItemFunction);

    escargot::ESFunctionObject* NamedNodeMapRemoveNamedItemFunction = escargot::ESFunctionObject::create(NULL, [](escargot::ESVMInstance* instance) -> escargot::ESValue {
       escargot::ESValue thisValue = instance->currentExecutionContext()->resolveThisBinding();
       CHECK_TYPEOF(thisValue, ScriptWrappable::Type::NamedNodeMapObject);

       escargot::ESValue argValue = instance->currentExecutionContext()->readArgument(0);
       if (argValue.isESString()) {
           String* name = String::fromUTF8(argValue.asESString()->utf8Data());
           Attr* old = ((NamedNodeMap*) thisValue.asESPointer()->asESObject())->getNamedItem(name);
           Attr* toReturn = new Attr(((NamedNodeMap*) thisValue.asESPointer()->asESObject())->striptBindingInstance(), name, old->value());
           ((NamedNodeMap*) thisValue.asESPointer()->asESObject())->removeNamedItem(name);
           if (toReturn != nullptr)
               return escargot::ESValue((escargot::ESObject *)toReturn);
       } else
           THROW_ILLEGAL_INVOCATION()
       return escargot::ESValue(escargot::ESValue::ESNull);
    }, escargot::ESString::create("removeNamedItem"), 1, false);
    NamedNodeMapFunction->protoType().asESPointer()->asESObject()->defineDataProperty(escargot::ESString::create("removeNamedItem"), false, false, false, NamedNodeMapRemoveNamedItemFunction);

    DEFINE_FUNCTION(Attr, fetchData(this)->m_instance->globalObject()->objectPrototype());
    fetchData(this)->m_attr = AttrFunction;

    AttrFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("name"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::AttrObject);

        String* n = ((Attr*) originalObj)->name();
        if (n != nullptr)
            return escargot::ESString::create(n->utf8Data());
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, NULL, false, false, false);

    auto valueGetter = [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::AttrObject);

        String* value = ((Attr*) originalObj)->value();
        if (value != nullptr)
            return escargot::ESString::create(value->utf8Data());
        return escargot::ESValue(escargot::ESValue::ESNull);
    };

    auto valueSetter = [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name, const escargot::ESValue& v) {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::AttrObject);

        ((Attr*) originalObj)->setValue(toBrowserString(v));
        // FIXME(JMP): Actually this function have to return old Attr's value but we have to modify 'typedef void (*ESNativeSetter)(...)' in escargot/src/runtime/ESValue.h
        // Because this need to many changes, we do the modification latter
    };

    AttrFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("value"), valueGetter, valueSetter, false, false, false);
    AttrFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("nodeValue"), valueGetter, valueSetter, false, false, false);
    AttrFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("textContent"), valueGetter, valueSetter, false, false, false);

    AttrFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("ownerElement"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::AttrObject);

        Element* elem = ((Attr*) originalObj)->ownerElement();
        if (elem != nullptr)
            return escargot::ESValue((escargot::ESObject *)elem);
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, NULL, false, false, false);

    AttrFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("specified"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::AttrObject);
        return escargot::ESValue(true);
    }, NULL, false, false, false);

    DEFINE_FUNCTION(HTMLUnknownElement, HTMLElementFunction->protoType());
    fetchData(this)->m_htmlUnknownElement = HTMLUnknownElementFunction;

    DEFINE_FUNCTION(Event, fetchData(this)->m_instance->globalObject()->objectPrototype());
    fetchData(this)->m_event = EventFunction;

    DEFINE_FUNCTION(UIEvent, EventFunction->protoType());
    fetchData(this)->m_uiEvent = UIEventFunction;

    DEFINE_FUNCTION(MouseEvent, UIEventFunction->protoType());
    fetchData(this)->m_mouseEvent = MouseEventFunction;

    /* style-related getter/setter start here */

    DEFINE_FUNCTION(CSSStyleDeclaration, CSSStyleDeclarationFunction->protoType());
    fetchData(this)->m_cssStyleDeclaration = CSSStyleDeclarationFunction;

    CSSStyleDeclarationFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("color"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::CSSStyleDeclarationObject);

        String* c = ((CSSStyleDeclaration*) originalObj)->color();
        if (c != nullptr)
            return escargot::ESString::create(c->utf8Data());
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name, const escargot::ESValue& v) {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::CSSStyleDeclarationObject);
        if (v.isESString()) {
            ((CSSStyleDeclaration*) originalObj)->setColor(String::fromUTF8(v.asESString()->utf8Data()));
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
    }, false, false, false);

    CSSStyleDeclarationFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("direction"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::CSSStyleDeclarationObject);
        String* d = ((CSSStyleDeclaration*)originalObj)->direction();
        STARFISH_ASSERT(d);
        return toJSString(d);
    }, [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name, const escargot::ESValue& v) {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::CSSStyleDeclarationObject);
        if (v.isESString()) {
            ((CSSStyleDeclaration*)originalObj)->addValuePair(CSSStyleValuePair::fromString("direction", v.asESString()->utf8Data()));
        }
    }, false, false, false);
    CSSStyleDeclarationFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("marginTop"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::CSSStyleDeclarationObject);

        String* c = ((CSSStyleDeclaration*) originalObj)->marginTop();
        if (c != nullptr)
            return escargot::ESString::create(c->utf8Data());
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name, const escargot::ESValue& v) {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::CSSStyleDeclarationObject);
        if (v.isESString()) {
            ((CSSStyleDeclaration*) originalObj)->setMarginTop(v.asESString()->utf8Data());
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
    }, false, false, false);

    CSSStyleDeclarationFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("marginBottom"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::CSSStyleDeclarationObject);

        String* c = ((CSSStyleDeclaration*) originalObj)->marginBottom();
        if (c != nullptr)
            return escargot::ESString::create(c->utf8Data());
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name, const escargot::ESValue& v) {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::CSSStyleDeclarationObject);
        if (v.isESString()) {
            ((CSSStyleDeclaration*) originalObj)->setMarginBottom(v.asESString()->utf8Data());
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
    }, false, false, false);

    CSSStyleDeclarationFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("marginRight"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::CSSStyleDeclarationObject);

        String* c = ((CSSStyleDeclaration*) originalObj)->marginRight();
        if (c != nullptr)
            return escargot::ESString::create(c->utf8Data());
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name, const escargot::ESValue& v) {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::CSSStyleDeclarationObject);
        if (v.isESString()) {
            ((CSSStyleDeclaration*) originalObj)->setMarginRight(v.asESString()->utf8Data());
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
    }, false, false, false);

    CSSStyleDeclarationFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("marginLeft"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::CSSStyleDeclarationObject);

        String* c = ((CSSStyleDeclaration*) originalObj)->marginLeft();
        if (c != nullptr)
            return escargot::ESString::create(c->utf8Data());
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name, const escargot::ESValue& v) {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::CSSStyleDeclarationObject);
        if (v.isESString()) {
            ((CSSStyleDeclaration*) originalObj)->setMarginLeft(v.asESString()->utf8Data());
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
    }, false, false, false);

    CSSStyleDeclarationFunction->protoType().asESPointer()->asESObject()->defineAccessorProperty(escargot::ESString::create("margin"),
            [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name) -> escargot::ESValue {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::CSSStyleDeclarationObject);

        String* c = ((CSSStyleDeclaration*) originalObj)->margin();
        if (c != nullptr)
            return escargot::ESString::create(c->utf8Data());
        return escargot::ESValue(escargot::ESValue::ESNull);
    }, [](::escargot::ESObject* obj, ::escargot::ESObject* originalObj, escargot::ESString* name, const escargot::ESValue& v) {
        CHECK_TYPEOF(originalObj, ScriptWrappable::Type::CSSStyleDeclarationObject);
        if (v.isESString()) {
            ((CSSStyleDeclaration*) originalObj)->setMargin(v.asESString()->utf8Data());
        } else {
            THROW_ILLEGAL_INVOCATION()
        }
    }, false, false, false);

    DEFINE_FUNCTION(CSSStyleRule, CSSStyleRuleFunction->protoType());
    fetchData(this)->m_cssStyleRule = CSSStyleRuleFunction;
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


