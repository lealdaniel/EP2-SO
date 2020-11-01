#!/bin/bash


for number in {8..15}
do
	CWD="\time -o time/time9_$number ./ep2 25000 1000 0 output/output9_$number"
	echo $CWD
	$CWD
done
