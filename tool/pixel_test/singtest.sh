#!/bin/sh
cd /home/june0cho/webTF/starfish/
./StarFish /home/june0cho/webTF/starfish/test/demo/clock/clock.xml &
sleep 1s; import -window StarFish tool/pixel_test/result.png
cd -
./bin/image_diff --diff result.png orig.png diff.png
killall StarFish
