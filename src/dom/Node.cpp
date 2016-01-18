#include "StarFishConfig.h"
#include "Node.h"

#include "Document.h"
#include "Traverse.h"

namespace StarFish {

Element* Node::firstElementChild()
{
    return (Element*) Traverse::firstChild(this, [](Node* child) {
        if (child->isElement())
            return true;
        else
            return false;
    });
}

Element* Node::lastElementChild()
{
    return nullptr;
}

unsigned long Node::childElementCount()
{
    return 0;
}

}
