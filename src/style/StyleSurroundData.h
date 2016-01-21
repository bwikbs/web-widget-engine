#ifndef __StarFishStyleSurroundData__
#define __StarFishStyleSurroundData__

#include "style/BorderData.h"

namespace StarFish {

class StyleSurroundData : public gc {
public:
    StyleSurroundData()
    {
        border.setTop(Length(Length::Fixed, 0));
        border.setRight(Length(Length::Fixed, 0));
        border.setBottom(Length(Length::Fixed, 0));
        border.setLeft(Length(Length::Fixed, 0));
    }

    virtual ~StyleSurroundData()
    {

    }

    BorderData border;

    // TODO: Add surroundable data such as padding, margin.
};

} /* namespace StarFish */

#endif /* __StarFishStyleSurroundData__ */
