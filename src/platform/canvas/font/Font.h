#ifndef __Font__
#define __Font__

namespace StarFish {

class Font : public gc {
    friend class FontSelector;
protected:
    Font()
    {

    }
public:
    virtual ~Font() {

    }

    virtual Size measureText(String*) = 0;
    virtual void* unwrap() = 0;
    virtual float size() = 0;
    virtual String* familyName() = 0;
protected:
};

class FontSelector {
protected:
    FontSelector()
    {

    }
    ~FontSelector()
    {

    }
public:
    static Font* loadFont(String* familyName,float size);
    static void clearCache();
};

};
#endif
