#include "StarFishConfig.h"
#include "ImageData.h"

#include <Elementary.h>

namespace StarFish {

Evas* internalCanvas();

class ImageDataEFL : public ImageData {
public:
    ImageDataEFL(String* imageSrc)
    {
        m_image = evas_object_image_add(internalCanvas());
        evas_object_image_file_set(m_image,imageSrc->utf8Data(),NULL);
        RELEASE_ASSERT(evas_object_image_colorspace_get(m_image) == EVAS_COLORSPACE_ARGB8888);

        int w,h;
        evas_object_image_size_get(m_image,&w,&h);
        m_width = w;
        m_height = h;
    }
    virtual ~ImageDataEFL()
    {
        evas_object_unref(m_image);
    }

    virtual void* unwrap()
    {
        return m_image;
    }

    virtual int width()
    {
        return m_width;
    }

    virtual int height()
    {
        return m_height;
    }

protected:
    Evas_Object* m_image;
    int m_width;
    int m_height;
};

ImageData* ImageData::create(String* imageSrc)
{
    return new ImageDataEFL(imageSrc);
}


}
