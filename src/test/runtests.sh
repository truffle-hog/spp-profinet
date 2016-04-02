#!/bin/bash
echo "Running unit tests:";

VALGRIND="valgrind --log-file=valgrind"

ARRAY=();

COUNTER=0;

ARRAY_SUCCESS=()
SUCCESS_COUNTER=0

for i in $(find . -name '*_test')
do
    if test -f $i
    then
        filename="${i##*/}"
        if $VALGRIND"-"$filename".log" ./$i 2>> tests.log
        then
            ARRAY_SUCCESS[$SUCCESS_COUNTER]=$i
            SUCCESS_COUNTER=$((SUCCESS_COUNTER+1))
            echo $i PASS
        else
            ARRAY[$COUNTER]=$i
            COUNTER=$((COUNTER+1))
            echo "ERROR in test $i: here's tests.log"
            echo "------"
            tail tests.log
        fi
    fi
done

echo ""
echo $SUCCESS_COUNTER "testsuites successfully passed:"

for ss in "${ARRAY_SUCCESS[@]}"
do
    echo "$ss"
done
echo ""

echo $COUNTER "testsuites failed:"

for s in "${ARRAY[@]}"
do
    echo "$s"
done
