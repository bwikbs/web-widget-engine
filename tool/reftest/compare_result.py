#!/usr/bin/python

import sys
import re

DEBUG = False

Search_PATTERN='Success|failure|error'

def compare(expected, result):

    extract_expected =[]

    if DEBUG:
        print 'expeted:',expected
        print 'result:,',result

    extract_expected=re.findall(Search_PATTERN,expected)
    extract_result = re.findall(Search_PATTERN, result)

    for pattern in extract_expected:
        for pattern2 in extract_result:
            if pattern2==pattern:
                return 'Pass'

    if DEBUG:
        print 'expeted:', expected
        print 'result:,', result
        print 'DEBUG', str(extract_expected), str(extract_result)
    return 'Fail'


def main():
    Compare_RESULT = 'Fail'
    if DEBUG:
        print 'argument number:', len(sys.argv)
        print 'argument list', str(sys.argv)

    if len(sys.argv) != 3:
        print 'Usage compare_result.py Expected_file_path result_file_path'

    fp = open(sys.argv[1], 'r')
    EXPECTED_Str = fp.read();
    fp.close()
    fp = open(sys.argv[2], 'r')
    RESULT_Str = fp.read();
    fp.close()

    if DEBUG:
        print '1st file:', EXPECTED_Str
        print '2nd file:', RESULT_Str

    Compare_RESULT = compare(EXPECTED_Str, RESULT_Str)
    exit(Compare_RESULT)

main()