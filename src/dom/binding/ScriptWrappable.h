#ifndef __StarFishScriptWrappable__
#define __StarFishScriptWrappable__

namespace StarFish {

class EventTarget;
class Window;
class Node;
class Element;
class DocumentElement;
class HTMLDocumentElement;

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
    void initScriptWrappable(Element* ptr);
    void initScriptWrappable(Element* ptr, ScriptBindingInstance*);
    void initScriptWrappable(DocumentElement* ptr);
    void initScriptWrappable(HTMLDocumentElement* ptr);

    void callFunction(String* name);

protected:
    void* m_object;
};


}

#endif
