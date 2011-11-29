#!/bin/bash

rm -f 1-10-2.txt
rm -f 1-15-3.txt
rm -f 1-20-4.txt

rm -f 2-10-2.txt
rm -f 2-15-3.txt
rm -f 2-20-4.txt

for i in `seq 1 1000` ; do
    echo $i
    for SOLUTION in 1 2 ; do
        ./sol$SOLUTION -n 10 -k 2 >> $SOLUTION-10-2.txt
        ./sol$SOLUTION -n 15 -k 3 >> $SOLUTION-15-3.txt
        ./sol$SOLUTION -n 20 -k 4 >> $SOLUTION-20-4.txt
    done
done

