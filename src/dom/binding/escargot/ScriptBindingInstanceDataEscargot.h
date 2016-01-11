#ifndef __StarFishScriptBindingInstanceDataEscargot__
#define __StarFishScriptBindingInstanceDataEscargot__


namespace StarFish {

class ScriptBindingInstanceDataEscargot : public gc {
public:
    escargot::ESVMInstance* m_instance;
    escargot::ESFunctionObject* m_window;
    escargot::ESFunctionObject* m_node;
    escargot::ESFunctionObject* m_element;
    escargot::ESFunctionObject* m_document;
    escargot::ESFunctionObject* m_documentType;
    escargot::ESFunctionObject* m_htmlDocument;
    escargot::ESFunctionObject* m_characterData;
    escargot::ESFunctionObject* m_text;
    escargot::ESFunctionObject* m_htmlElement;
    escargot::ESFunctionObject* m_htmlHtmlElement;
    escargot::ESFunctionObject* m_htmlHeadElement;
    escargot::ESFunctionObject* m_htmlBodyElement;
};

}

#endif
