#!/bin/bash

## Generating DOM coverage
mkdir -p out

list="
dom.wpt.raw
dom.dct.raw
dom.blink.dom.raw
dom.blink.fast.dom.raw
dom.blink.fast.html.raw
dom.webkit.dom.raw
dom.webkit.fast.dom.raw
dom.webkit.fast.html.raw
dom.gecko.dom.raw
dom.bidi.raw
dom.css1.raw
dom.css21.raw
dom.css.backgrounds.3.raw
dom.css.color.3.raw
dom.css.transforms.1.raw
"

list2=(
"Web Platform Test"
"DCT"
"Vender Blink"
"Vender Blink Fast DOM"
"Vender Blink Fast HTML"
"Vender Webkit"
"Vender Webkit Fast DOM"
"Vender Webkit Fast HTML"
"Vender Gecko"
"W3C International Text"
"W3C CSS 1"
"W3C CSS 2.1"
"W3C CSS Backgrounds 3"
"W3C CSS Color 3"
"W3C CSS Transforms 1"
);

i=0;
for f in $list; do
    node genTable.js "${list2[i]}" in/specDOM.txt in/$f > out/$f.csv
    i=$((i+1));
done

rm -f out/dom.raw
for f in $list; do
    cat in/$f >> out/dom.raw
done

rm -f out/dom.csv.tmp
for f in $list; do
    cat out/$f.csv | tail -n +3 >> out/dom.csv.tmp
done

head -n 2 out/dom.dct.raw.csv > out/head.txt
node genTable.js "" in/specDOM.txt out/dom.raw | tail -1 > t.txt

cat out/head.txt out/dom.csv.tmp t.txt > dom.csv
rm out/dom.csv.tmp t.txt

# Demo
node genTable.js "Demo" in/specDOM.txt in/dom.demo.raw > dom.demo.csv


## Generating CSS coverage Data

csslist="
css.wpt.raw
css.dct.raw
css.blink.dom.raw
css.blink.fast.dom.raw
css.blink.fast.html.raw
css.webkit.dom.raw
css.webkit.fast.dom.raw
css.webkit.fast.html.raw
css.gecko.dom.raw
css.bidi.raw
css.css1.raw
css.css21.raw
css.css.backgrounds.3.raw
css.css.color.3.raw
css.css.transforms.1.raw
"

i=0;
for f in $csslist; do
    node genTable.js "${list2[i]}" in/specCSS.txt in/$f > out/$f.csv
    i=$((i+1));
done

rm -f out/css.raw
for f in $csslist; do
    cat in/$f >> out/css.raw
done

rm -f out/css.csv.tmp
for f in $csslist; do
    cat out/$f.csv | tail -n +3 >> out/css.csv.tmp
done

head -n 2 out/css.dct.raw.csv > out/head.txt
node genTable.js "" in/specCSS.txt out/css.raw | tail -1 > t.txt

cat out/head.txt out/css.csv.tmp t.txt > css.csv
rm out/css.csv.tmp t.txt

# Demo
node genTable.js "Demo" in/specCSS.txt in/css.demo.raw > css.demo.csv


## Generating HTML coverage

htmllist="
html.wpt.raw
html.dct.raw
html.blink.dom.raw
html.blink.fast.dom.raw
html.blink.fast.html.raw
html.webkit.dom.raw
html.webkit.fast.dom.raw
html.webkit.fast.html.raw
html.gecko.dom.raw
html.bidi.raw
html.css1.raw
html.css21.raw
html.css.backgrounds.3.raw
html.css.color.3.raw
html.css.transforms.1.raw
"

i=0;
for f in $htmllist; do
    node genTable.js "${list2[i]}" in/specHTML.txt in/$f > out/$f.csv
    i=$((i+1));
done

rm -f out/html.raw
for f in $htmllist; do
    cat in/$f >> out/html.raw
done

rm -f out/html.csv.tmp
for f in $htmllist; do
    cat out/$f.csv | tail -n +3 >> out/html.csv.tmp
done

head -n 2 out/html.dct.raw.csv > out/head.txt
node genTable.js "" in/specHTML.txt out/html.raw | tail -1 > t.txt

cat out/head.txt out/html.csv.tmp t.txt > html.csv
rm out/html.csv.tmp t.txt

rm -fr out

# Demo
node genTable.js "Demo" in/specHTML.txt in/html.demo.raw > html.demo.csv

