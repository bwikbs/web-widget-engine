#ifndef __FileIO__
#define __FileIO__

#include "StarFishConfig.h"

namespace StarFish {

class Window;
class FileIO : public gc_cleanup {
protected:
    FileIO() { }
public:
    static FileIO* create();
    virtual ~FileIO()
    {

    }

    virtual bool open(const char* fileName) = 0;
    virtual long int length() = 0;
    virtual size_t read(void* buf, size_t size, size_t count) = 0;
    virtual int close() = 0;
    virtual const char* matchLocation(const char* fileName) = 0;
};

}

#endif
