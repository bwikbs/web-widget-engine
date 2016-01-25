#ifndef __StarFishStyleSurroundData__
#define __StarFishStyleSurroundData__

#include "style/BorderData.h"
#include "style/LengthData.h"

namespace StarFish {

class StyleSurroundData : public gc {
public:
    StyleSurroundData()
    {
    }

    virtual ~StyleSurroundData()
    {
    }

    BorderData border;
    LengthData margin;
    LengthData padding;
};

} /* namespace StarFish */

#endif /* __StarFishStyleSurroundData__ */
