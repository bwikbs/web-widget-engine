#include "StarFishConfig.h"
#include "Font.h"

#include <Elementary.h>

namespace StarFish {

Evas* internalCanvas();

class FontImplEFL : public Font {
public:
    FontImplEFL(String* familyName,float size)
    {
        m_size = size;
        /*
        if(textStyle & EireneTextStyleBold) {
            WTF::CString cs = m_fontFamily.utf8();
            familyName.append(":style=Bold");
        }*/
        m_fontFamily = familyName;

    }
    ~FontImplEFL()
    {
    }

    void loadFont()
    {
        m_text = evas_object_text_add(internalCanvas());
        evas_object_text_font_set(m_text,m_fontFamily->utf8Data(), m_size);
    }

    void unloadFont()
    {
        evas_object_del(m_text);
    }

    virtual Size measureText(String* str)
    {
        loadFont();
        evas_object_text_text_set(m_text,str->utf8Data());
        Evas_Coord minw, minh;
        evas_object_geometry_get(m_text,0,0,&minw,&minh);
        unloadFont();
        return Size(minw, minh);
    }
    virtual void* unwrap()
    {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
        return m_text;
    }

    virtual float size()
    {
        return m_size;
    }

    virtual String* familyName()
    {
        return m_fontFamily;
    }

    // TODO implement font metrics
    virtual float fontHeight()
    {
        return m_size * 1.4f;
    }
protected:
    float m_size;
    String* m_fontFamily;
    Evas_Object* m_text;
};

Font* FontSelector::loadFont(String* familyName,float size)
{
    return new FontImplEFL(familyName,size);
}

void FontSelector::clearCache()
{
}

}
