#include "StarFishConfig.h"
#include "Node.h"
#include "DocumentElement.h"

namespace StarFish {

bool Node::onTouchEvent(Window::TouchEventKind kind, float x, float y)
{
    if (kind == Window::TouchEventUp) {
        if (documentElement()->window()->activeNodeWithTouchDown() == this) {
            Node* node = this;
            while (node) {
                node->callFunction(String::createASCIIString("onClick"));
                node = node->parentElement();
            }
        }
    }
    return false;
}

}
