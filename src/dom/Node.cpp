#include "StarFishConfig.h"
#include "Node.h"
#include "DocumentElement.h"

namespace StarFish {

bool Node::onTouchEvent(Window::TouchEventKind kind, float x, float y)
{
    if (kind == Window::Up) {
        if (documentElement()->window()->activeNodeWithTouchDown() == this) {
            callFunction(String::createASCIIString("onClick"));
        }
    }
    return false;
}

}
