#include "StarFishConfig.h"
#include "Drawable.h"

namespace StarFish {

Drawable Drawable::fromString(StarFish* sf, String* str)
{
    const char* s = str->utf8Data();
    Drawable d;
    if (strstr(s, "rgba") == s) {
        d.m_type = SolidColor;
        d.m_color = Color::fromString(str);
    } else if (strstr(s, "url") == s) {
        d.m_type = Image;

        // FIXME prevent overflow!!!
        char buf[256];
        sscanf(s, "url(%s", buf);
        unsigned len = strlen(buf);
        if (buf[len - 1] == ')') {
            buf[len - 1] = 0;
        }
        d.m_image = ImageData::create(sf->makeResourcePath(String::createASCIIString(buf)));
        d.m_imageSrc = str;
    } else {
        RELEASE_ASSERT_NOT_REACHED();
    }
    return d;
}

}
