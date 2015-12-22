#include "StarFishConfig.h"
#include "StarFish.h"

#include "dom/TextElement.h"
#include "dom/ImageElement.h"
#include "dom/DocumentElement.h"

using namespace StarFish;

int main()
{
    StarFish::StarFish* sf = new StarFish::StarFish();
    TextElement* txt = new TextElement(sf->window()->document());
    txt->setX(Length(Length::Fixed, 50));
    txt->setY(Length(Length::Fixed, 50));
    txt->setWidth(Length(Length::Fixed, 100));
    txt->setHeight(Length(Length::Fixed, 100));
    txt->setText(String::createASCIIString("test string"));
    ImageElement* img = new ImageElement(sf->window()->document());
    img->setX(Length(Length::Fixed, 150));
    img->setY(Length(Length::Fixed, 150));
    img->setWidth(Length(Length::Fixed, 100));
    img->setHeight(Length(Length::Fixed, 100));
    img->setSrc(String::createASCIIString("test.jpg"));
    sf->window()->document()->appendChild(txt);
    sf->window()->document()->appendChild(img);
    sf->run();
    return 0;
}
