#include "StarFishConfig.h"
#include "StarFish.h"

#include "dom/TextElement.h"
#include "dom/ImageElement.h"
#include "dom/DocumentElement.h"

using namespace StarFish;

int main(int argc, char *argv[])
{
    StarFish::StarFish* sf = new StarFish::StarFish();
    if (argc == 1) {
        puts("please specify xml path");
        return -1;
    }
    sf->window()->loadXMLDocument(String::createASCIIString(argv[1]));
    sf->run();
    return 0;
}

/*
#include <Elementary.h>

int main()
{
    elm_init(0,0);
    elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
    Evas_Object* wnd = elm_win_add(NULL, "", ELM_WIN_BASIC);

    elm_win_autodel_set(wnd, EINA_TRUE);
    Evas_Object* box = elm_box_add(wnd);
    evas_object_size_hint_weight_set (box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(wnd, box);
    evas_object_show(box);

    evas_object_show(wnd);
    elm_run();
    return 0;
}
*/
