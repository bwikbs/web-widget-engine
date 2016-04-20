#!/bin/bash

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BOLD='\033[1m'
RESET='\033[0m'

# Variables
WPTTEST=1
RESULTFILE="result"
PASSTC=0
SKIPTC=0
FAILTC=0
TCFILE=0
PASSTCFILE=0

tc=$1
if [[ "$tc" == *"test_dom" ]]; then
    tc=$(cat $tc)
    WPTTEST=0
elif [[ "$tc" == *"test_wpt_dom" ]]; then
    tc=$(cat $tc)
elif [[ "$tc" == *"test_wpt_html" ]]; then
    tc=$(cat $tc)
elif [[ "$tc" == *"test_wpt_xhr" ]]; then
    tc=$(cat $tc)
else
    echo "Usage: ./runner.js [input]"
    exit
fi

rm $RESULTFILE > /dev/null 2>&1

echo -e "${BOLD}###### Tests ######${RESET}\n"

cnt=-1
ROTATE="$(nproc)"
filenames=()
array=( $tc )
pos=$(( ${#array[*]} - 1 ))
last=${array[$pos]}

for i in $tc ; do
    cnt=$((cnt+1))
    if [[ $cnt == $ROTATE ]]; then
        cnt=0
    fi
    RESFILE="tmp"$cnt

    # Convert the html file
    XML=${i%.*}".xml"
    ./preprocessor/convert.sh $i
    cp preporcessor/out/result.xml $XML 

    # Running the tests
    filenames[$cnt]=$i
    # FIXME: run.sh -> StarFish
    #./run.sh $i --result-folder="out/$i" --hide-window > $RESFILE &
    ./StarFish $XML --hide-window > $RESFILE &
    if [[ $cnt != $((ROTATE-1)) ]] && [[ $i != $last ]]; then
        continue;
    fi
    wait;

    for c in $(seq 0 $cnt); do
        TMPFILE="tmp"$c

        # Collect the result
        replace1='s/\\n"//g'
        replace2='s/"//g'
        replace3='s/\n//g'
        replace4='s/spawnSTDOUT:\s//g'
        perl -i -pe $replace1 $TMPFILE
        perl -i -pe $replace2 $TMPFILE
        perl -i -pe $replace3 $TMPFILE
        perl -i -pe $replace4 $TMPFILE

        if [ $WPTTEST -eq 1 ]; then
            PASS=`grep -o Pass $TMPFILE | wc -l`
            FAIL=`grep -o Fail $TMPFILE | wc -l`
            SUM=`expr $PASS + $FAIL`
            PASSTC=`expr $PASSTC + $PASS`
            FAILTC=`expr $FAILTC + $FAIL`
            TCFILE=`expr $TCFILE + 1`

            if [ $SUM -eq 0 ]; then
                echo -e "${YELLOW}[CHECK]${RESET}" ${filenames[$c]} "(${BOLD}No results${RESET})"
                echo -e "[CHECK]" ${filenames[$c]} "(No results)" >> $RESULTFILE
            elif [ $FAIL -eq 0 ]; then
                PASSTCFILE=`expr $PASSTCFILE + 1`
                echo -e "${GREEN}[PASS]${RESET}" ${filenames[$c]} "(${GREEN}PASS:" $PASS"${RESET})"
                echo -e "[PASS]" ${filenames[$c]} "(PASS:" $PASS")" >> $RESULTFILE
            elif [ $PASS -eq 0 ]; then
                echo -e "${RED}[FAIL]${RESET}" ${filenames[$c]} "(${RED}FAIL:" $FAIL"${RESET})"
                echo -e "[FAIL]" ${filenames[$c]} "(FAIL:" $FAIL")" >> $RESULTFILE
            else
                echo -e "${RED}[FAIL]${RESET}" ${filenames[$c]} "(${GREEN}PASS:" $PASS"${RESET}," "${RED}FAIL:" $FAIL"${RESET})"
                echo -e "[FAIL]" ${filenames[$c]} "(PASS:" $PASS"," "FAIL:" $FAIL")" >> $RESULTFILE
            fi
        elif [ $WPTTEST -eq 0 ]; then
            # Expected result
            EXPECTED_FILE=${filenames[$c]%.*}"-expected.txt"
            EXPECTED=$(cat $EXPECTED_FILE)
            EXPECTED=${EXPECTED##*Status:}
            EXPECTED=${EXPECTED%Detail*}

            SKIP=`grep -o Skipped $TMPFILE | wc -l`
            RESULT=`grep -o $EXPECTED $TMPFILE | wc -l`
            TCFILE=`expr $TCFILE + 1`

            if [ $SKIP -eq 1 ]; then
                SKIPTC=`expr $SKIPTC + 1`
                echo -e "${YELLOW}[SKIP]${RESET}" ${filenames[$c]}
                echo -e "[SKIP]" ${filenames[$c]} >> $RESULTFILE
            elif [ $RESULT -eq 1 ]; then
                PASSTC=`expr $PASSTC + 1`
                echo -e "${GREEN}[PASS]${RESET}" ${filenames[$c]}
                echo -e "[PASS]" ${filenames[$c]} >> $RESULTFILE
            else
                FAILTC=`expr $FAILTC + 1`
                echo -e "${RED}[FAIL]${RESET}" ${filenames[$c]}
                echo -e "[FAIL]" ${filenames[$c]} >> $RESULTFILE
            fi
        fi

        rm $TMPFILE
    done
done

wait;

# Print the summary
if [ $WPTTEST -eq 1 ]; then
    echo -e "\n${BOLD}###### Summary ######${RESET}\n"
    echo -e "${YELLOW}Run" `expr $PASSTC + $FAILTC` "test cases ("$TCFILE" files):" $PASSTC "passed ("$PASSTCFILE "files)," $FAILTC "failed.${RESET}\n"
    echo -e "\n###### Summary ######\n" >> $RESULTFILE
    echo -e "Run" `expr $PASSTC + $FAILTC` "test cases ("$TCFILE" files):" $PASSTC "passed ("$PASSTCFILE "files)," $FAILTC "failed.\n" >> $RESULTFILE
elif [ $WPTTEST -eq 0 ]; then
    echo -e "\n${BOLD}###### Summary ######${RESET}\n"
    echo -e "${YELLOW}Run" `expr $TCFILE` "test cases:" $PASSTC "passed," $FAILTC "failed," $SKIPTC "skipped.${RESET}\n"
    echo -e "\n###### Summary ######\n" >> $RESULTFILE
    echo -e "Run" `expr $TCFILE` "test cases:" $PASSTC "passed," $FAILTC "failed," $SKIPTC "skipped.\n" >> $RESULTFILE
fi

