#ifndef __StarFishStyleSurroundData__
#define __StarFishStyleSurroundData__

#include "style/BorderData.h"

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
};

} /* namespace StarFish */

#endif /* __StarFishStyleSurroundData__ */
