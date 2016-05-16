#!/bin/bash
echo "Running unit tests:";

VALGRIND="valgrind --log-file=valgrind"

ARRAY=();

COUNTER=0;

ARRAY_SUCCESS=()
ARRAY_TESTCOUNT=()
SUCCESS_COUNTER=0

for i in $(find . -name '*_test')
do
    if test -f $i
    then
        filename="${i##*/}"
        #ARRAY_TESTCOUNT[$SUCCESS_COUNTER]=$VALGRIND"-"$filename".log" ./$i 2>> tests.log

        eval $VALGRIND"-"$filename".log" ./$i 2>> tests.log
        ARRAY_TESTCOUNT[$SUCCESS_COUNTER]=$?
        if [ "${ARRAY_TESTCOUNT[$SUCCESS_COUNTER]}" != "0" ]
        #if ! $VALGRIND"-"$filename".log" ./$i 2>> tests.log
        #echo "uuu"
        #echo ${ARRAY_TESTCOUNT[$SUCCESS_COUNTER]}
        #if ${ARRAY_TESTCOUNT[$SUCCESS_COUNTER]}

        #echo "${testss=$($VALGRIND"-"$filename".log" ./$i 2>> tests.log)}"

        #if ! "${foo=$($VALGRIND"-"$filename".log" ./$i 2>> tests.log)}"
        #echo "$testss"
        #if true
        then
            ARRAY_SUCCESS[$SUCCESS_COUNTER]=$i
            SUCCESS_COUNTER=$((SUCCESS_COUNTER+1))
            echo $i PASS
            tail tests.log
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
echo $SUCCESS_COUNTER "testsuite(s) passed:"

for i in "${!ARRAY_SUCCESS[@]}"; do
  printf "%s test run(s) in:\t%s\n" "${ARRAY_TESTCOUNT[$i]}" "${ARRAY_SUCCESS[$i]}"
done
echo ""
echo $COUNTER "testsuite(s) failed:"

for i in "${!ARRAY[@]}"; do
  printf "%s test run(s) in:\t%s\n" "${ARRAY_TESTCOUNT[$i]}" "${ARRAY[$i]}"
done
