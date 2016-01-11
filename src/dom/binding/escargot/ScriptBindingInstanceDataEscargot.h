#ifndef __StarFishScriptBindingInstanceDataEscargot__
#define __StarFishScriptBindingInstanceDataEscargot__


namespace StarFish {

class ScriptBindingInstanceDataEscargot : public gc {
public:
    escargot::ESVMInstance* m_instance;
    escargot::ESFunctionObject* m_window;
    escargot::ESFunctionObject* m_node;
    escargot::ESFunctionObject* m_element;
    escargot::ESFunctionObject* m_documentElement;
    escargot::ESFunctionObject* m_htmlDocumentElement;
};

}

#endif
