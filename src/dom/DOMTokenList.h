#ifndef __StarFishDOMTokenList__
#define __StarFishDOMTokenList__

#include "dom/binding/ScriptWrappable.h"

namespace StarFish {

class Node;
class Element;
class DOMTokenList : public ScriptWrappable {
public:
    DOMTokenList(ScriptBindingInstance* instance, Element* element, String* localName)
    : m_element(element), m_localName(localName)
    {
        initScriptWrappable(this, instance);
    }
    static void tokenize(std::vector<String*, gc_allocator<String*>>* tokens, String* src);
    unsigned long length();
    String* item(unsigned long index);
    bool contains(String* token);
    String* addSingleToken(String* src, std::vector<String*, gc_allocator<String*>>* tokens, String* token);
    void add(std::vector<String*, gc_allocator<String*>>* tokens);
    int checkMatchedTokens(bool* flags, std::vector<String*, gc_allocator<String*>>* tokens, String* token);
    void remove(String* token);
    void remove(std::vector<String*, gc_allocator<String*>>* tokens);
    bool toggle(String* token, bool isForced, bool forceValue);
private:
    Element* m_element;
    String* m_localName;
};

}

#endif
