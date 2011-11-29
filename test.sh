#!/bin/bash

NUM=255
COUNT=0
GOODCOUNT=0

finish()
{
    echo $NUM is found min
    echo $GOODCOUNT -- $COUNT
    exit 0
}

trap finish SIGINT SIGTERM

while `true` ; do
    TEMP=`./sol1 -n 20 -k 4`
    echo "        $TEMP"
    COUNT=$((COUNT+1))
    if [ $TEMP -lt $NUM ] ; then
        NUM=$TEMP
        echo $NUM "--" $COUNT
        GOODCOUNT=1
    elif [ $TEMP -eq $NUM ] ; then
        GOODCOUNT=$((GOODCOUNT+1))
        echo $GOODCOUNT "--" $COUNT
    fi
done
