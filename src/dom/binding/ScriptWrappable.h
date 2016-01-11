#ifndef __StarFishScriptWrappable__
#define __StarFishScriptWrappable__

namespace StarFish {

class EventTarget;
class Window;
class Node;
class Element;
class Document;
class DocumentType;
class HTMLDocument;
class CharacterData;
class Text;
class HTMLElement;
class HTMLHtmlElement;
class HTMLHeadElement;
class HTMLBodyElement;

class ScriptBindingInstance;

class ScriptWrappable {
public:
    enum Type {
        None,
        WindowObject,
        NodeObject
    };

    ScriptWrappable();
    ScriptWrappable(void* object);

    void initScriptWrappableWindow(Window* window);

    void initScriptWrappable(EventTarget* ptr);
    void initScriptWrappable(Node* ptr);
    void initScriptWrappable(Node* ptr, ScriptBindingInstance*);
    void initScriptWrappable(DocumentType* ptr);
    void initScriptWrappable(Element* ptr);
    void initScriptWrappable(Element* ptr, ScriptBindingInstance*);
    void initScriptWrappable(Document* ptr);
    void initScriptWrappable(HTMLDocument* ptr);
    void initScriptWrappable(CharacterData* ptr);
    void initScriptWrappable(Text* ptr);
    void initScriptWrappable(HTMLElement* ptr);
    void initScriptWrappable(HTMLHtmlElement* ptr);
    void initScriptWrappable(HTMLHeadElement* ptr);
    void initScriptWrappable(HTMLBodyElement* ptr);

    void callFunction(String* name);

protected:
    void* m_object;
};


}

#endif
