#include "StarFishConfig.h"
#include "Font.h"

#include <Evas.h>
// #include <cairo.h>
// #include <cairo-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <fontconfig/fontconfig.h>

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
        float ptSize = m_size * 12.f / 16.f;
        evas_object_text_font_set(m_text,m_fontFamily->utf8Data(), ptSize);
    }

    void unloadFont()
    {
        evas_object_del(m_text);
    }

    virtual float measureText(String* str)
    {
        loadFont();
        evas_object_text_text_set(m_text,str->utf8Data());
        Evas_Coord minw, minh;
        evas_object_geometry_get(m_text,0,0,&minw,&minh);
        unloadFont();
        return minw;
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

#define DEFAULT_FONT_SIZE 10
/*
cairo_font_extents_t loadDefault()
{
    static cairo_font_extents_t extent;
    static bool isInit = false;
    if (!isInit) {
        cairo_surface_t *surface;
        cairo_t *cr;

        surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 32, 32);
        cr = cairo_create (surface);
        cairo_set_font_size(cr, DEFAULT_FONT_SIZE);
        cairo_font_extents(cr, &extent);

        cairo_destroy(cr);
        cairo_surface_destroy(surface);
    }
    return extent;
}*/

#define CHECK_ERROR if (error) STARFISH_RELEASE_ASSERT_NOT_REACHED();

Font::FontMetrics loadFontMetrics(String* familyName, double size)
{
    FcConfig* config = FcInitLoadConfigAndFonts();

    FcPattern* pattern = FcPatternCreate();
    FcPatternAddString(pattern, FC_FAMILY, (FcChar8*)familyName->utf8Data());

    FcConfigSubstitute(config, pattern, FcMatchPattern);
    FcDefaultSubstitute (pattern);

    FcResult res;
    FcFontSet* set = FcFontSort(config, pattern, FcTrue, NULL, &res);

    if (!set) {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
    }

    std::string fontPath;
    for (int i = 0; i < set->nfont; i++) {
        FcPattern* font = set->fonts[i];
        FcChar8 *file;
        if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch) {
            fontPath = (char*)file;
            break;
        }
    }

    FcFontSetDestroy(set);
    FcPatternDestroy(pattern);
    FcConfigDestroy(config);

    FT_Library library;
    FT_Error error;
    error = FT_Init_FreeType(&library);
    CHECK_ERROR;

    FT_Face face;
    error = FT_New_Face(library, fontPath.data(), 0, &face);
    CHECK_ERROR;

    Font::FontMetrics met;
    met.m_ascender = ((float)face->ascender / (float)face->units_per_EM) * size;
    met.m_descender = ((float)face->descender / (float)face->units_per_EM) * size;
    met.m_fontHeight = ((float)face->height / (float)face->units_per_EM) * size;

    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return met;

}

Font* FontSelector::loadFont(String* familyName, float size, int style)
{
    FontImplEFL* f = nullptr;

    for (unsigned i = 0; i < m_fontCache.size(); i ++) {
        if (std::get<1>(m_fontCache[i])->equals(familyName)) {
            if (std::get<2>(m_fontCache[i]) == size && std::get<3>(m_fontCache[i]) == style) {
                return std::get<0>(m_fontCache[i]);
            }
        }
    }

    f = new FontImplEFL(familyName,size);
    f->m_metrics = loadFontMetrics(familyName, size);

    m_fontCache.push_back(std::make_tuple(f, familyName, size, style));
    return f;
}

}
