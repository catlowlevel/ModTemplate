#!/bin/bash
# echo com.hitapps.figgerits

out=com.hitapps.figgerits

#check if last_out.txt exist and read last_out.txt
lastout=`cat last_out.txt`

#if lastout is not empty, set it to out
if [ -n "$lastout" ]; then
    out=$lastout
fi

if [ -n "$1" ]; then
    out=$1
fi

echo $out
echo $out > last_out.txt
