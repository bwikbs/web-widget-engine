#include "StarFishConfig.h"
#include "String.h"

namespace StarFish {

String* String::emptyString = String::createASCIIString("");
String* String::spaceString = String::createASCIIString(" ");

}
