#include "StarFishConfig.h"
#include "Canvas.h"
#include "platform/canvas/font/Font.h"
#include "platform/canvas/image/ImageData.h"

#include <Evas.h>
#include <Evas_Engine_Buffer.h>
#include <Elementary.h>

#include <vector>
#include <SkMatrix.h>


static Evas* g_internalCanvas;


namespace StarFish {

static void initInternalCanvas()
{
    if(!g_internalCanvas) {
        Evas *canvas;
        int width=16;
        int height=16;
        Evas_Engine_Info_Buffer *einfo;
        int method;
        void *pixels;
        method = evas_render_method_lookup("buffer");
        if(method <= 0)
        {
            fputs("ERROR: evas was not compiled with 'buffer' engine!\n", stderr);
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
        canvas = evas_new();
        if(!canvas)
        {
            fputs("ERROR: could not instantiate new evas canvas.\n", stderr);
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
        evas_output_method_set(canvas, method);
        evas_output_size_set(canvas, width, height);
        evas_output_viewport_set(canvas, 0, 0, width, height);
        einfo = (Evas_Engine_Info_Buffer *) evas_engine_info_get(canvas);

        if(!einfo)
        {
            fputs("ERROR: could not get evas engine info!\n", stderr);
            evas_free(canvas);
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }

        // ARGB32 is sizeof(int), that is 4 bytes, per pixel
        pixels = malloc(width * height * sizeof(int));
        if(!pixels)
        {
            fputs("ERROR: could not allocate canvas pixels!\n", stderr);
            evas_free(canvas);
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }

        einfo->info.depth_type = EVAS_ENGINE_BUFFER_DEPTH_ARGB32;
        einfo->info.dest_buffer = pixels;
        einfo->info.dest_buffer_row_bytes = width * sizeof(int);
        einfo->info.use_color_key = 0;
        einfo->info.alpha_threshold = 0;
        einfo->info.func.new_update_region = NULL;
        einfo->info.func.free_update_region = NULL;
        evas_engine_info_set(canvas, (Evas_Engine_Info *) einfo);
        g_internalCanvas = canvas;
    }
}

Evas* internalCanvas()
{
    initInternalCanvas();
    return g_internalCanvas;
}

Evas_Object* createClipper(Evas* canvas, int l, int t, int w, int h)
{
    Evas_Object* eo = evas_object_rectangle_add(canvas);
    evas_object_color_set(eo, 255, 255, 255, 255);
    evas_object_move(eo,l,t);
    evas_object_resize(eo, w,h);
    evas_object_show(eo);

    return eo;
}

class CanvasState {
public:
    SkMatrix m_matrix;
    Color m_color;
    Evas_Object* m_clipper;
    SkRect m_clipRect;
    float m_opacity;

    CanvasState()
    {
        m_clipper = NULL;
        m_opacity = 1;
    }
};




class CanvasEFL : public Canvas {
    void initFromBuffer(void* buffer,int width,int height,int stride) {
        Evas *canvas;
        int method;
        method = evas_render_method_lookup("buffer");
        if(method <= 0)
        {
            fputs("ERROR: evas was not compiled with 'buffer' engine!\n", stderr);
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
        canvas = evas_new();
        if(!canvas)
        {
            fputs("ERROR: could not instantiate new evas canvas.\n", stderr);
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }
        evas_output_method_set(canvas, method);

        if(stride == -1) {
            stride = width*4;
        }

        Evas_Engine_Info_Buffer *einfo;
        void *pixels = buffer;


        evas_output_size_set(canvas, width, height);
        evas_output_viewport_set(canvas, 0, 0, width, height);
        einfo = (Evas_Engine_Info_Buffer *) evas_engine_info_get(canvas);

        if(!einfo)
        {
            fputs("ERROR: could not get evas engine info!\n", stderr);
            evas_free(canvas);
            STARFISH_RELEASE_ASSERT_NOT_REACHED();
        }

        einfo->info.depth_type = EVAS_ENGINE_BUFFER_DEPTH_ARGB32;
        einfo->info.dest_buffer = pixels;
        einfo->info.dest_buffer_row_bytes = stride;
        einfo->info.use_color_key = 0;
        einfo->info.alpha_threshold = 0;
        einfo->info.func.new_update_region = NULL;
        einfo->info.func.free_update_region = NULL;
        evas_engine_info_set(canvas, (Evas_Engine_Info *) einfo);

        m_width = width;
        m_height = height;
        m_canvas = canvas;
        m_buffer = buffer;

        save();
    }
public:
    CanvasEFL(void* data)
    {
        m_image = NULL;
        m_buffer = NULL;
        m_directDraw = true;
        struct dummy {
            void* a;
            void* b;
            int w;
            int h;
            std::vector<Evas_Object*>* objList;
        };
        dummy* d = (dummy*)data;
        m_canvas = (Evas *)d->a;
        m_prevDrawnImageMap = (std::unordered_map<ImageData*, std::vector<std::pair<Evas_Object*, bool>>> *)d->b;

        auto iter = m_prevDrawnImageMap->begin();
        while(iter != m_prevDrawnImageMap->end()) {
            std::vector<std::pair<Evas_Object*, bool>>& vec = iter->second;
            for(unsigned i = 0 ; i < vec.size() ; i ++) {
                vec[i].second = false;
            }
            ++iter;
        }
        m_width = d->w;
        m_height = d->h;
        m_objList = d->objList;
        save();
    }

    ~CanvasEFL()
    {
        restore();
        if(m_directDraw) {
            evas_damage_rectangle_add(m_canvas, 0, 0, m_width, m_height);
        } else {
            evas_render(m_canvas);
            evas_free(m_canvas);
        }

        if(m_image && m_buffer) {
            evas_object_image_data_update_add(m_image, 0, 0, m_width, m_height);
        }

        if(m_prevDrawnImageMap) {
            auto iter = m_prevDrawnImageMap->begin();

            while(iter != m_prevDrawnImageMap->end()) {
                std::vector<std::pair<Evas_Object*, bool>>& vec = iter->second;
                for(unsigned i = 0 ; i < vec.size() ; ) {
                    if(!vec[i].second) {
                        evas_object_hide(vec[i].first);
                    }
                    i++;
                    /*
                    if(!vec[i].second) {
                        evas_object_hide(vec[i].first);
                        evas_object_del(vec[i].first);
                        vec.erase(vec.begin() + i);
                    } else {
                        i++;
                    }*/
                }

                ++iter;
            }
        }
    }

    virtual void clearColor(const Color& clr)
    {
        Evas_Object* eo = evas_object_rectangle_add(m_canvas);
        if(m_objList) m_objList->push_back(eo);
        int r = clr.r(),g = clr.g(),b = clr.b();

        evas_color_argb_premul(clr.a(),&r,&g,&b);
        evas_object_color_set(eo,r,g,b,clr.a());
        evas_object_move(eo,0,0);
        evas_object_resize(eo, m_width, m_height);
        applyClippers(eo);
        evas_object_show(eo);
    }

    // state
    virtual void save()
    {
        CanvasState state;
        if(m_state.size()) {
            state.m_matrix = lastState().m_matrix;
            state.m_clipRect = lastState().m_clipRect;
            state.m_clipper = lastState().m_clipper;
            state.m_color = lastState().m_color;
            state.m_opacity = lastState().m_opacity;
        } else {
            state.m_matrix.reset();
            state.m_clipRect.setLTRB(0,0,SkFloatToScalar((float)m_width),SkFloatToScalar((float)m_height));
            state.m_clipper = NULL;
        }
        m_state.push_back(state);
    }

    // pop state stack and restore state
    virtual void restore()
    {
        m_state.erase(m_state.end()-1);
    }

    // transformations (default transform is the identity matrix)
    virtual void scale(double x, double y)
    {
        lastState().m_matrix.preScale(SkDoubleToScalar(x),SkDoubleToScalar(y));
    }

    virtual void scale(double x, double y,double ox,double oy)
    {
        lastState().m_matrix.preScale(SkDoubleToScalar(x),SkDoubleToScalar(y),SkDoubleToScalar(ox),SkDoubleToScalar(oy));
    }

    virtual void rotate(double angle)
    {
        lastState().m_matrix.preRotate(SkDoubleToScalar(angle));
    }

    virtual void rotate(double angle,double ox,double oy)
    {
        lastState().m_matrix.preRotate(SkDoubleToScalar(angle),SkDoubleToScalar(ox),SkDoubleToScalar(oy));
    }

    virtual void translate(double x, double y)
    {
        lastState().m_matrix.preTranslate(x,y);
    }

    virtual void beginOpacityLayer(float c)
    {
        save();
        lastState().m_opacity = c;
    }

    virtual void endOpacityLayer()
    {
        restore();
    }


    virtual void clip(const Rect& rt)
    {
        /*
        if(hasMatrixAffine()) {
            EireneLog::logError("clip with angle not supported yet");
        }
        */
        SkRect sss = SkRect::MakeXYWH(
                SkFloatToScalar((float)rt.x()),
                SkFloatToScalar((float)rt.y()),
                SkFloatToScalar((float)rt.width()),
                SkFloatToScalar((float)rt.height())
                );
        lastState().m_matrix.mapRect(&sss);

        if(sss.isEmpty() || !lastState().m_clipRect.intersect(sss)) {
            lastState().m_clipRect.setEmpty();
            lastState().m_clipper = NULL;
        } else {
            lastState().m_clipRect.sort();
            sss.sort();

            SkRect tmp;
            tmp.fLeft = std::max(lastState().m_clipRect.fLeft, sss.fLeft);
            tmp.fRight = std::min(lastState().m_clipRect.fRight, sss.fRight);
            tmp.fTop = std::max(lastState().m_clipRect.fTop, sss.fTop);
            tmp.fBottom = std::min(lastState().m_clipRect.fBottom, sss.fBottom);
            lastState().m_clipRect = tmp;
            lastState().m_clipper = NULL;
        }
    }

    void applyClippers(Evas_Object* eo)
    {
        if(!lastState().m_clipper) {
            lastState().m_clipper = createClipper(m_canvas,lastState().m_clipRect.x(),lastState().m_clipRect.y(),
                    lastState().m_clipRect.width(),lastState().m_clipRect.height());
            if(m_objList) m_objList->push_back(lastState().m_clipper);
        }
        evas_object_clip_set(eo,lastState().m_clipper);
    }

    virtual void setColor(const Color& clr_)
    {
        Color clr = clr_;
        int r = clr.r(),g = clr.g(),b = clr.b();
        evas_color_argb_premul(clr.a(),&r,&g,&b);
        clr.m_a = clr_.m_a;
        clr.m_r = r;
        clr.m_g = g;
        clr.m_b = b;
        lastState().m_color = clr;
    }

    virtual Color color()
    {
        int r = lastState().m_color.r(),g = lastState().m_color.g(),b = lastState().m_color.b();
        evas_color_argb_unpremul(lastState().m_color.a(),&r,&g,&b);
        Color clr(r,g,b,lastState().m_color.a());
        return clr;
    }

    virtual void setTextShadowColor(const Color& clr)
    {
        m_textShadowColor = clr;
    }

    virtual void setFont(Font* font)
    {
        m_font = font;
    }

    virtual void drawRect(const Rect& rt)
    {
        SkRect sss = SkRect::MakeXYWH(
                SkFloatToScalar((float)rt.x()),
                SkFloatToScalar((float)rt.y()),
                SkFloatToScalar((float)rt.width()),
                SkFloatToScalar((float)rt.height())
                );
        if(!shouldApplyEvasMap())
            lastState().m_matrix.mapRect(&sss);

        Evas_Object* eo = evas_object_rectangle_add(m_canvas);
        if(m_objList) m_objList->push_back(eo);
        evas_object_color_set(eo,lastState().m_color.r(),lastState().m_color.g(),lastState().m_color.b(),lastState().m_color.a());
        evas_object_move(eo,sss.x(),sss.y());
        evas_object_resize(eo, sss.width(), sss.height());
        applyClippers(eo);

        if(shouldApplyEvasMap()) {
           applyEvasMapIfNeeded(eo, rt);
        }
        evas_object_show(eo);
    }

    virtual void drawText(const float x,const float y,String* text)
    {
        Evas_Object* eo = evas_object_text_add(m_canvas);
        if(m_objList) m_objList->push_back(eo);
        Size sz = m_font->measureText(text);
        Rect rt(x,y,sz.width(),sz.height());
        SkRect sss = SkRect::MakeXYWH(
        SkFloatToScalar((float)rt.x()),
        SkFloatToScalar((float)rt.y()),
        SkFloatToScalar((float)rt.width()),
        SkFloatToScalar((float)rt.height())
        );
        if(!shouldApplyEvasMap())
            lastState().m_matrix.mapRect(&sss);

        evas_object_text_font_set(eo,m_font->familyName()->utf8Data(),m_font->size());
        evas_object_color_set(eo, lastState().m_color.r(),lastState().m_color.g(),lastState().m_color.b(),lastState().m_color.a());
        evas_object_text_style_set(eo,Evas_Text_Style_Type::EVAS_TEXT_STYLE_SHADOW);
        evas_object_text_shadow_color_set(eo,m_textShadowColor.r(),m_textShadowColor.g(),m_textShadowColor.b(),m_textShadowColor.a());
        evas_object_text_text_set(eo, text->utf8Data());

        evas_object_move(eo,sss.x(),sss.y());
        applyClippers(eo);

        if(shouldApplyEvasMap()) {
           applyEvasMapIfNeeded(eo, rt);
        }
        evas_object_show(eo);
    }

    Evas_Object* findPrevDrawnData(ImageData* data)
    {
        if(m_prevDrawnImageMap) {
            Evas_Object* eo = NULL;
            std::vector<std::pair<Evas_Object*, bool>>& vec = (*m_prevDrawnImageMap)[data];
            for(unsigned i = 0 ; i < vec.size() ; i ++) {
                if(!vec[i].second) {
                    eo = vec[i].first;
                    evas_object_raise(eo);
                    evas_object_map_enable_set(eo, EINA_FALSE);
                    vec[i].second = true;
                    break;
                }
            }
            return eo;
        } else {
            return NULL;
        }
    }

    void pushPrevDrawnData(ImageData* data,Evas_Object* eo)
    {
        if(m_prevDrawnImageMap) {
            std::vector<std::pair<Evas_Object*, bool>>& vec = (*m_prevDrawnImageMap)[data];
            vec.push_back(std::pair<Evas_Object*, bool>(eo, true));
        }
    }
    virtual void drawImage(ImageData* data, const Rect& dst)
    {
        SkRect sss = SkRect::MakeXYWH(
                SkFloatToScalar((float)dst.x()),
                SkFloatToScalar((float)dst.y()),
                SkFloatToScalar((float)dst.width()),
                SkFloatToScalar((float)dst.height())
                );
        if(!shouldApplyEvasMap())
            lastState().m_matrix.mapRect(&sss);

        Evas_Object* eo = findPrevDrawnData(data);

        if(!eo) {
            eo = evas_object_image_add(m_canvas);
            if(!m_prevDrawnImageMap) {
                if(m_objList) m_objList->push_back(eo);
            }

            Evas_Object* imgData = (Evas_Object*)data->unwrap();
            void* imgBuf = evas_object_image_data_get(imgData,EINA_FALSE);

            evas_object_image_size_set(eo, data->width(),data->height());
            evas_object_image_colorspace_set(eo,evas_object_image_colorspace_get(imgData));
            evas_object_image_data_set(eo, imgBuf);
            evas_object_image_filled_set(eo, EINA_TRUE);
            evas_object_image_alpha_set(eo,EINA_TRUE);
            //evas_object_anti_alias_set(eo,EINA_TRUE);

            pushPrevDrawnData(data, eo);
        }

        evas_object_move(eo,sss.x(),sss.y());
        evas_object_resize(eo, sss.width(), sss.height());

        applyClippers(eo);
        if(shouldApplyEvasMap()) {
           applyEvasMapIfNeeded(eo, dst, true);
        }
        evas_object_show(eo);
    }

    virtual void setMatrix(const SkMatrix& matrix)
    {
        lastState().m_matrix = matrix;
    }

    virtual SkMatrix matrix()
    {
        return lastState().m_matrix;
    }

    virtual void* unwrap()
    {
        STARFISH_RELEASE_ASSERT_NOT_REACHED();
        return NULL;
    }

    CanvasState& lastState()
    {
        STARFISH_ASSERT(m_state.size());
        return m_state[m_state.size()-1];
    }

    bool hasMatrixAffine()
    {
        return lastState().m_matrix.getType() & SkMatrix::TypeMask::kAffine_Mask;
    }
    bool shouldApplyEvasMap()
    {
        if(lastState().m_opacity != 1) {
            return true;
        }
        return hasMatrixAffine();
    }

    void applyEvasMapIfNeeded(Evas_Object* eo, const Rect& dst,bool isImage = false)
    {
        if(shouldApplyEvasMap()) {
            Evas_Map* map = evas_map_new(4);

            evas_map_util_points_populate_from_object(map, eo);
            if(isImage) {
                int img_w,img_h;
                evas_object_image_size_get(eo, &img_w, &img_h);
                evas_map_point_image_uv_set(map, 0, 0, 0);
                evas_map_point_image_uv_set(map, 1, img_w, 0);
                evas_map_point_image_uv_set(map, 2, img_w, img_h);
                evas_map_point_image_uv_set(map, 3, 0, img_h);
            }

            {
                SkScalar fromX;
                SkScalar fromY;
                SkPoint to;
                fromX = SkFloatToScalar((float)dst.x());
                fromY = SkFloatToScalar((float)dst.y());
                lastState().m_matrix.mapXY(fromX,fromY,&to);
                evas_map_point_coord_set(map,0,SkScalarToFloat(to.x()),SkScalarToFloat(to.y()),0);

                fromX = SkFloatToScalar((float)(dst.x()+dst.width()));
                fromY = SkFloatToScalar((float)dst.y());
                lastState().m_matrix.mapXY(fromX,fromY,&to);
                evas_map_point_coord_set(map,1,SkScalarToFloat(to.x()),SkScalarToFloat(to.y()),0);

                fromX = SkFloatToScalar((float)(dst.x()+dst.width()));
                fromY = SkFloatToScalar((float)(dst.y()+dst.height()));
                lastState().m_matrix.mapXY(fromX,fromY,&to);
                evas_map_point_coord_set(map,2,SkScalarToFloat(to.x()),SkScalarToFloat(to.y()),0);

                fromX = SkFloatToScalar((float)dst.x());
                fromY = SkFloatToScalar((float)(dst.y()+dst.height()));
                lastState().m_matrix.mapXY(fromX,fromY,&to);
                evas_map_point_coord_set(map,3,SkScalarToFloat(to.x()),SkScalarToFloat(to.y()),0);
            }

            evas_object_anti_alias_set(eo,EINA_TRUE);

            evas_map_alpha_set(map,EINA_TRUE);
            if(lastState().m_opacity != 1) {
                int c = lastState().m_opacity*255;
                evas_map_point_color_set(map,0,c,c,c,c);
                evas_map_point_color_set(map,1,c,c,c,c);
                evas_map_point_color_set(map,2,c,c,c,c);
                evas_map_point_color_set(map,3,c,c,c,c);
            }

            evas_object_map_set(eo, map);
            evas_object_map_enable_set(eo, EINA_TRUE);
            evas_map_free(map);
        }
    }


protected:
    std::vector<CanvasState> m_state;
    //FIXME(ksh8281) Move these into state
    Font* m_font;
    Color m_textShadowColor;
    ////////////////////
    Evas* m_canvas;
    bool m_directDraw;
    Evas_Object* m_image;
    void* m_buffer;
    unsigned m_width;
    unsigned m_height;
    std::vector<Evas_Object*>* m_objList;
    std::unordered_map<ImageData*, std::vector<std::pair<Evas_Object*, bool>>>* m_prevDrawnImageMap;
};

Canvas* Canvas::createDirect(void* data)
{
    return new CanvasEFL(data);
}


}
