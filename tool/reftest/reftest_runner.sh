#!/bin/bash

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BOLD='\033[1m'
RESET='\033[0m'

DEVICE=$1
VERSION=$2
TC=$3
TIMEOUT=10
SERIAL=`sdb devices | tail --lines=+2 | cut -d' ' -f1 | sort`
export WEB_WIDGET_TEST_HOME=/opt/usr/starfish

if [ $VERSION = "3.0" ]; then
    VERSION="3"
fi

function main {
    prerequisite

    # Check the connected devices
    echo -e "\n${BOLD}2. Checking the connected device... ${RESET}"
    checkDevice
    echo -e "${BOLD}\tdone.${RESET}"

    # Build the binary
    echo -e "\n${BOLD}3. Building...${RESET}"
    buildBinary
    echo -e "${BOLD}\tdone.${RESET}" 

    # Push the reftest to the devices
    echo -e "\n${BOLD}4. Pushing the testsuites to the device... ${RESET}"
    pushTestSuite
    echo -e "${BOLD}\tdone.${RESET}" 

    # Running the tests
    echo -e "\n${BOLD}5. Running the tests...${RESET}\n"
    runTest

    # Send the result
    sendResult "$@"

    # Remove unnecessary files
    cleanup
}

function prerequisite {
    echo -e "${BOLD}\n1. Please check the following list.${RESET}"
    echo -e "\ta. Platform binary or emulator image"
    echo -e "\tb. Network status"
    echo -e "\tc. Verified boot status (target 2.3 only)"
    echo -e "\t   $ verityctl disable; sync; reboot"
    echo -e "\t   $ change-booting-mode.sh --update"
    echo -e "\td. Commit number of starfish and escargot"
    echo -e "\te. \`./build_third_party.sh\`"
    echo -ne "${BOLD}\tAre you ready? (y/n): "
    read -t $TIMEOUT READ
    if [[ $READ == "n"* ]]; then
        echo -e "\n${BOLD}\tPlease check it again.${RESET}\n"
        exit
    fi
    echo -e "${BOLD}\tLet's start!${RESET}"
}

function checkDevice {
    for i in ${SERIAL[@]}; do
        SDB="sdb -s $i"
        SDB_SHELL="$SDB shell su - -c"
        $SDB root on &> /dev/null 2>&1
        $SDB_SHELL "cat /etc/info.ini" &> version
        if [[ `grep -Eo "Tizen-${VERSION}" version` != "Tizen-"${VERSION} &&
              `grep -Eo "Tizen${VERSION}" version` != "Tizen"${VERSION} ]]; then
            echo -e "${BOLD}\tCheck your device [$i]:${RESET}"
            echo -ne "\t"
            perl -i -pe "s/\\n/\\n\\t/g" version
            cat version
            echo -ne "${BOLD}Keep going? (y/n) ${RESET}"
            read -t $TIMEOUT READ
            if [[ $READ == "n"* ]]; then
                echo -e "${BOLD}\tPlease check it again.${RESET}\n"
                exit
            fi
            rm version
        fi
    done
}

function checkBinary {
    if [ $DEVICE = "emulator" ]; then
        if [[ `file StarFish` != *"ELF 32-bit"* || `file StarFish` = *"ARM"* ]]; then
            echo -e "${RED}Build Failed${RESET}"
            exit
        fi
    else
        if [[ `file StarFish` != *"ARM"* ]]; then
            echo -e "${RED}Build Failed${RESET}"
            exit
        fi
    fi
}

function buildBinary {
    # ./build_third_party.sh
    rm StarFish &> /dev/null 2>&1
    make clean &> /dev/null 2>&1
    if [ $DEVICE = "emulator" ]; then
        if [ $VERSION = "2.3" ]; then
            make tizen_wearable_emulator.exe.debug -j &> /dev/null 2>&1
            checkBinary 
            cp StarFish test/regression/bin/tizen-wearable-2.3-emulator
        else
            cp -rf deps/tizen/lib/tizen-wearable-2.3.1-i386 deps/tizen/lib/tizen-wearable-3.0-i386
            make tizen3_wearable_emulator.exe.debug -j &> /dev/null 2&>1
            checkBinary
            cp StarFish test/regression/bin/tizen-wearable-3.0-emulator
        fi
    else
        if [ $VERSION = "2.3" ]; then
            make tizen_wearable_arm.exe.debug -j &> /dev/null 2&>1
            checkBinary
            cp StarFish test/regression/bin/tizen-wearable-2.3-target-arm
        else
            make tizen3_wearable_arm.exe.debug -j &> /dev/null 2&>1
            checkBinary
            cp StarFish test/regression/bin/tizen-wearable-3.0-target-arm
        fi
    fi
}

function pushTestSuite {
    cd test
    echo -e "${BOLD}\tCompressing files...${RESET}"
    cp -rf regression test
    tar zcf test.tgz test &> /dev/null 2>&1
    for i in ${SERIAL[@]}; do
        SDB="sdb -s $i"
        SDB_SHELL="$SDB shell su - -c"
        $SDB_SHELL "mkdir -p $WEB_WIDGET_TEST_HOME" &> /dev/null 2>&1
        echo -e "${BOLD}\tPushing files... [$i]${RESET}"
        $SDB push test.tgz $WEB_WIDGET_TEST_HOME &> /dev/null 2>&1
        echo -e "${BOLD}\tExtracting files...[$i]${RESET}"
        $SDB_SHELL "tar zxf $WEB_WIDGET_TEST_HOME/test.tgz -C $WEB_WIDGET_TEST_HOME" &> /dev/null 2>&1
    done
    cd - &> /dev/null 2>&1
}

function runTest {
    setEnv

    # Running the tests
    cd test
    ./test/tool/runner.sh $DEVICE $TC $VERSION

    # Print the result
    echo -e "${BOLD}You can find the result in test/result_* file.${RESET}\n"
    cd - &> /dev/null 2>&1

    restoreEnv
}


function setEnv {
    rm test/result &> /dev/null 2>&1
    for i in ${SERIAL[@]}; do
        SDB="sdb -s $i"
        SDB_SHELL="$SDB shell su - -c"
        if [ $VERSION = "3" ]; then
            if [ $DEVICE = "emulator" ]; then
                $SDB push deps/tizen/lib/tizen-wearable-3.0-i386 $WEB_WIDGET_TEST_HOME/lib &> /dev/null 2>&1
            else
                $SDB push deps/tizen/lib/tizen-wearable-3.0-arm $WEB_WIDGET_TEST_HOME/lib &> /dev/null 2>&1
            fi
        else
            if [ $DEVICE = "emulator" ]; then
                $SDB push deps/tizen/lib/tizen-wearable-2.3.1-i386 $WEB_WIDGET_TEST_HOME/lib &> /dev/null 2>&1
            else
                $SDB push deps/tizen/lib/tizen-wearable-2.3.1-arm $WEB_WIDGET_TEST_HOME/lib &> /dev/null 2>&1
            fi
        fi
        $SDB_SHELL "ln -s $WEB_WIDGET_TEST_HOME/lib/libicui18n.so /usr/lib/libicui18n.so.51" &> /dev/null 2>&1
        $SDB_SHELL "ln -s $WEB_WIDGET_TEST_HOME/lib/libicudata.so /usr/lib/libicudata.so.51" &> /dev/null 2>&1
        $SDB_SHELL "ln -s $WEB_WIDGET_TEST_HOME/lib/libicuio.so /usr/lib/libicuio.so.51" &> /dev/null 2>&1
        $SDB_SHELL "ln -s $WEB_WIDGET_TEST_HOME/lib/libicuuc.so /usr/lib/libicuuc.so.51" &> /dev/null 2>&1

        # imgdiff
        if [ $DEVICE = "target" ]; then
            $SDB_SHELL "mkdir -p $WEB_WIDGET_TEST_HOME/tool/imgdiff" &> /dev/null 2>&1
            $SDB push test/regression/tool/imgdiffEvas $WEB_WIDGET_TEST_HOME/tool/imgdiff/imgdiff &> /dev/null 2>&1
        else
            $SDB_SHELL "mkdir -p $WEB_WIDGET_TEST_HOME/tool/imgdiff" &> /dev/null 2>&1
            $SDB push test/regression/tool/imgdiffEvasEmul $WEB_WIDGET_TEST_HOME/tool/imgdiff/imgdiff &> /dev/null 2>&1
        fi
    done
}

function restoreEnv {
    for i in ${SERIAL[@]}; do
        SDB="sdb -s $i"
        SDB_SHELL="$SDB shell su - -c"
        if [[ $VERSION = "3" && $DEVICE = "emulator" ]]; then
            rm -rf deps/tizen/lib/tizen-wearable-3.0-i386 &> /dev/null 2>&1
        fi
        $SDB_SHELL "rm /usr/lib/libicui18n.so.51"
        $SDB_SHELL "rm /usr/lib/libicudata.so.51"
        $SDB_SHELL "rm /usr/lib/libicuio.so.51"
        $SDB_SHELL "rm /usr/lib/libicuuc.so.51"
    done
}

function sendResult {
    cd test

    DATE=`date +"%y.%m.%d"`
    PASS=`cat result_* | grep PASS | wc -l`
    FAIL=`cat result_* | grep FAIL | wc -l`
    CHECK=`cat result_* | grep CHECK | wc -l`
    RESULT="summary.csv"
    echo -e "DEVICE\t"$DEVICE >> $RESULT
    echo -e "VERSION\t"$VERSION >> $RESULT
    echo -e "DATE\t"$DATE >> $RESULT
    echo -e "PASS\t"$PASS >> $RESULT
    echo -e "FAIL\t"$FAIL >> $RESULT
    echo -e "CEHCK\t"$CHECK >> $RESULT

    ssh webtf@10.113.64.195 "mkdir -p share/Test_Result/$DATE/$2"_"$DEVICE" &> /dev/null 2>&1
    scp $RESULT result_* webtf@10.113.64.195:share/Test_Result/$DATE/$2"_"$DEVICE &> /dev/null 2>&1

    cd - &> /dev/null 2>&1
}

function cleanup {
    rm version
    rm -rf test/test test/test.tgz
    for i in ${SERIAL[@]}; do
        SDB="sdb -s $i"
        SDB_SHELL="$SDB shell su - -c"
        # $SDB_SHELL "rm -rf $WEB_WIDGET_TEST_HOME"
    done
}

main "$@"

