#/bin/bash

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BOLD='\033[1m'
RESET='\033[0m'

# Variables
TIMEOUT=3
TMPFILE="tmp"
PASSFILE="test/regression/reftest/web-platform-tests/dom_regression.res"
PASSFILENEW="dom_regression.res"
PASSTC=0
FAILTC=0
TCFILE=0

if [ "$1" = "" ]; then
    tc=$(find test/reftest/web-platform-tests -name "*htm*")
else
    tc=$1
fi

if [[ "$tc" == *"/" ]]; then
    tc=$(find $tc -name "*.htm*" | sort)
elif [[ "$tc" == *".res" ]]; then
    tc=$(cat $tc)
fi

if [ "$2" = true ]; then
    REGRESSION=true
fi

echo -e "${BOLD}###### Web Platform Tests ######${RESET}\n"

for i in $tc ; do
    # Running the tests
    ./run.sh $i > $TMPFILE &
    sleep $TIMEOUT
    killall StarFish

    # Collect the result
    replace1='s/\\n"//g'
    replace2='s/"//g'
    replace3='s/\n//g'
    replace4='s/spawnSTDOUT:\s//g'
    perl -i -pe $replace1 $TMPFILE
    perl -i -pe $replace2 $TMPFILE
    perl -i -pe $replace3 $TMPFILE
    perl -i -pe $replace4 $TMPFILE

    PASS=`grep -o Pass $TMPFILE | wc -l`
    FAIL=`grep -o Fail $TMPFILE | wc -l`
    SUM=`expr $PASS + $FAIL`
    PASSTC=`expr $PASSTC + $PASS`
    FAILTC=`expr $FAILTC + $FAIL`
    TCFILE=`expr $TCFILE + 1`

    if [ $SUM -eq 0 ]; then
        echo -e "${YELLOW}[CHECK]${RESET}" $i "(${BOLD}No results${RESET})"
    elif [ $FAIL -eq 0 ]; then
        echo -e "${GREEN}[PASS]${RESET}" $i "(${GREEN}PASS:" $PASS"${RESET})"
        if [ "$REGRESSION" = true ]; then
            echo -e $i >> $PASSFILENEW
        fi
    elif [ $PASS -eq 0 ]; then
        echo -e "${RED}[FAIL]${RESET}" $i "(${RED}FAIL:" $FAIL"${RESET})"
    else
        echo -e "${RED}[FAIL]${RESET}" $i "(${GREEN}PASS:" $PASS"${RESET}," "${RED}FAIL:" $FAIL"${RESET})"
    fi
done

# Print the summary
echo -e "\n${BOLD}###### Summary ######${RESET}\n"
echo -e "${YELLOW}Run" `expr $PASSTC + $FAILTC` "test cases ("$TCFILE" files):" $PASSTC "passed," $FAILTC "failed.${RESET}\n"

# Remove temporary file
rm $TMPFILE

# Regression test
if [ "$REGRESSION" = true ]; then
    echo -e "${BOLD}###### Regression Test ######${RESET}\n"

    diff=`diff -u $PASSFILE $PASSFILENEW`
    if [ "$diff" = "" ] ; then
        echo -e "${GREEN}[PASSED] no change${RESET}\n"
    else
        echo -e "${RED}[CHECKED] some changes${RESET}\n"
        diff -u $PASSFILE $PASSFILENEW
        echo

        # Update the regression lists
        cp $PASSFILENEW test/regression/reftest/web-platform-tests

        # Copy the converted files
        file=$(cat $PASSFILENEW)
        for i in $file ; do
            dest=test/regression/${i#*/}
            destdir=${dest%/*}
            mkdir -p $destdir
            cp $i $dest
        done

        # Copy the original files
        replace='s/dom_converted/dom/g'
        perl -i -pe $replace $PASSFILENEW
        file=$(cat $PASSFILENEW)
        for i in $file ; do
            dest=test/regression/${i#*/}
            destdir=${dest%/*}
            mkdir -p $destdir
            cp $i $dest
        done

        # Copy the resource files
        cp -rf test/reftest/web-platform-tests/resources test/regression/reftest/web-platform-tests/
    fi

    # Remove temporary file
    rm $PASSFILENEW
fi

