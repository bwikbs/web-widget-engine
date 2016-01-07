#ifndef __ImageData__
#define __ImageData__

namespace StarFish {

class ImageData : public gc {
protected:
    ImageData()
    {

    }
public:
    static ImageData* create(String* imageSrc);
    virtual void* unwrap() = 0;
    virtual int width() = 0;
    virtual int height() = 0;
    virtual ~ImageData()
    {

    }
};

}

#endif
