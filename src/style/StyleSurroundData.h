#ifndef __StarFishStyleSurroundData__
#define __StarFishStyleSurroundData__

#include "style/BorderData.h"
#include "style/LengthData.h"

namespace StarFish {

class StyleSurroundData : public gc {
public:
    StyleSurroundData()
        : margin(Length(Length::Fixed, 0))
        , padding(Length(Length::Fixed, 0))
        , offset(Length())
    {
    }

    virtual ~StyleSurroundData()
    {
    }

    BorderData border;
    LengthData margin;
    LengthData padding;
    LengthData offset;
};

} /* namespace StarFish */

#endif /* __StarFishStyleSurroundData__ */
