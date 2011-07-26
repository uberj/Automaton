#!/bin/bash
# run <file> <radius> <latice_size> <steps> <lower_rule> <upper_rule>

if [ $# -ne 6 ];then
    echo "run <file> <radius> <latice_size> <steps> <lower_rule> <upper_rule>"
    exit
fi

file=$1
radius=$2
latice_size=$3
steps=$4
lower_rule=$5
upper_rule=$6

c=$lower_rule
while [ $c -lt $upper_rule ]
do
    ./run $file $radius $latice_size $steps $lower_rule $upper_rule
    c=$((c+1))
done

c=$lower_rule
while [ $c -lt $upper_rule ]
do
    echo "=========================="
    echo 
    echo
    python gen_pic.py $file$c
    c=$((c+1))
done
