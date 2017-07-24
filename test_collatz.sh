if [[ $# -eq 1 ]]; then
    prog=$1
else
    prog=collatz
fi

echo "Testing" $prog"..."

let test=0

# ----------------------------------------------------------------------------
# test argument validation
# test for core dump
# ----------------------------------------------------------------------------
# set -A param "" "0" "-1" "1 1 1" "9999999999999999999"
# let params=${#param[*]}-1
# for i in {0..${params}}; do 
#     test_param=${param[${i}]}
#     `$prog ${test_param} 2>/dev/null`
#     if [[ $? -gt 1 ]]; then
#         echo "Test failed for param: \"${test_param}\""
#         return 1;
#     fi
# done

# ----------------------------------------------------------------------------
# Test edge cases 1 2 3 explicitly
# ----------------------------------------------------------------------------

if [[ `$prog 1` -ne 0 ]]; then echo "$prog 1 failed"; fi
if [[ `$prog 2` -ne 1 ]]; then echo "$prog 2 failed"; fi
if [[ `$prog 3` -ne 2 ]]; then echo "$prog 3 failed"; fi

# ----------------------------------------------------------------------------
# Test all values
#   * Based on numbers with longest sequences: https://oeis.org/A006877/list
# ----------------------------------------------------------------------------

set -A records 1 2 3 6 7 9 18 25 27 54 73 97 129 171 231 313 327 649 703 871 1161 2223 2463 2919 3711 6171 10971 13255 17647
let size=${#records[*]}-2

for i in {0..${size}}; do 
    let expect=${records[${i}]}
    let start=${expect}+1
    let end=${records[${i}+1]}

    printf "\n  checking range %5d - %5d (expect %5d)" ${start} ${end} ${expect}
    for n in {${start}..${end}}; do
        answer=`$prog $n`
        # echo "$prog $n=$answer"
        if [[ $answer -ne $expect ]]; then printf "\n\tFAIL: %s %d" $prog $n; fi
        let test+=1
    done
done

printf "\n\nTesting completed %d tests\n\n" ${test}
exit 0