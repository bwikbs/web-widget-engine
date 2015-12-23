#ifndef __StarFishScriptWrappable__
#define __StarFishScriptWrappable__

#include "Escargot.h"

namespace StarFish {

class Window;
class Node;
class Element;
class DocumentElement;
class ImageElement;
class TextElement;

class ScriptBindingInstance;

class ScriptWrappable : public escargot::ESObject {
public:
    ScriptWrappable()
        : escargot::ESObject(escargot::ESPointer::Type::ESObject, escargot::ESValue())
    {
    }

    void initScriptWrappableWindow(Window* window);

    void initScriptWrappable(Node* ptr);
    void initScriptWrappable(Node* ptr, ScriptBindingInstance*);
    void initScriptWrappable(Element* ptr);
    void initScriptWrappable(Element* ptr, ScriptBindingInstance*);
    void initScriptWrappable(DocumentElement* ptr);
    void initScriptWrappable(ImageElement* ptr);
    void initScriptWrappable(TextElement* ptr);

protected:
};


}

#endif
