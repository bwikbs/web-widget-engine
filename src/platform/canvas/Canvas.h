/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef __Canvas__
#define __Canvas__

namespace StarFish {

class ImageData;
class Font;
class Window;
class CanvasSurface : public gc {
protected:
    CanvasSurface()
    {

    }
public:
    static CanvasSurface* create(Window* window, size_t w, size_t h);
    virtual void* unwrap() = 0;
    virtual size_t width() = 0;
    virtual size_t height() = 0;
    virtual void clear() = 0;
    virtual void detachNativeBuffer() = 0;
    virtual ~CanvasSurface()
    {

    }
};

class Canvas : public gc {
protected:
    Canvas()
    {

    }
public:
    static Canvas* createDirect(void* data);
    static Canvas* create(CanvasSurface* data);

    virtual ~Canvas()
    {

    }

    virtual void clearColor(const Color& clr) = 0;

    // state
    virtual void save() = 0; // push state on state stack
    virtual void restore() = 0; // pop state stack and restore state
    // transformations (default transform is the identity matrix)
    virtual void scale(double x, double y) = 0;
    virtual void scale(double x, double y, double ox, double oy) = 0;
    virtual void rotate(double angle) = 0;
    virtual void rotate(double angle, double ox, double oy) = 0;
    virtual void translate(double x, double y) = 0;
    virtual void translate(LayoutUnit x, LayoutUnit y) = 0;
    virtual void postMatrix(const SkMatrix& matrix) = 0;

    virtual void clip(const Rect& rt) = 0;

    virtual void setColor(const Color& clr) = 0;
    virtual Color color() = 0;
    virtual void beginOpacityLayer(float c) = 0;
    virtual void endOpacityLayer() = 0;
    virtual void setFont(Font* font) = 0;
    virtual void setNeedsUnderline(bool b) = 0;
    virtual void setNeedsLineThrough(bool b) = 0;
    virtual void setUnderlineColor(Color clr) = 0;
    virtual void setLineThroughColor(Color clr) = 0;
    virtual void drawRect(const Rect& rt) = 0;
    virtual void drawRect(const LayoutRect& rt) = 0;
    virtual void drawRect(LayoutLocation p1, LayoutLocation p2, LayoutLocation p3, LayoutLocation p4) = 0; // left, top, right, bottom
    virtual void drawText(LayoutUnit x, LayoutUnit y, LayoutUnit stringWidth, const StringView& text) = 0;

    virtual void drawImage(ImageData* data, const Rect& dst) = 0;
    virtual void drawImage(CanvasSurface* data, const Rect& dst) = 0;
    virtual void drawBorderImage(ImageData* data, const Rect& dst, size_t l, size_t t, size_t r, size_t b, double scale, bool fill) = 0;
    virtual void drawRepeatImage(ImageData* data, const Rect& dst, float imageWidth, float imageHeight, bool xRepeat, bool yRepeat, bool isRootElement) = 0;


    virtual void applyMatrixTo(LayoutLocation lp) = 0;
    virtual void applyMatrixTo(LayoutRect lp) = 0;

    virtual void setVisible(bool visible) = 0;

    virtual void* unwrap() = 0;
};

}

#endif
