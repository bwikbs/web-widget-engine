#include "StarFishConfig.h"
#include "Font.h"

#include <Elementary.h>

namespace StarFish {

Evas* internalCanvas();

class FontImplEFL : public Font {
    friend class EireneFontSelectorSkia;
public:
    FontImplEFL(String* familyName,float size)
    {
        m_size = size;
        m_text = evas_object_text_add(internalCanvas());
        /*
        if(textStyle & EireneTextStyleBold) {
            WTF::CString cs = m_fontFamily.utf8();
            familyName.append(":style=Bold");
        }*/
        m_fontFamily = familyName;
        evas_object_text_font_set(m_text,m_fontFamily->utf8Data(),size);
    }
    ~FontImplEFL()
    {
        evas_object_unref(m_text);
    }

    virtual Size measureText(String* str)
    {
        evas_object_text_text_set(m_text,str->utf8Data());
        Evas_Coord minw, minh;
        evas_object_geometry_get(m_text,0,0,&minw,&minh);
        return Size(minw, minh);
    }
    virtual void* unwrap()
    {
        RELEASE_ASSERT_NOT_REACHED();
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
