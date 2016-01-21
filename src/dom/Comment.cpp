#include "StarFishConfig.h"
#include "Comment.h"

#include "Document.h"

namespace StarFish {

String* Comment::nodeName()
{
    return document()->window()->starFish()->staticStrings()->m_commentLocalName;
}

String* Comment::localName()
{
    return document()->window()->starFish()->staticStrings()->m_commentLocalName;
}

}
