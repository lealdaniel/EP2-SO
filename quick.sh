#!/bin/bash


for number in {1..15}
do
	CWD="./ep2 250 10 0 output/output1_$number"
	echo $CWD
	$CWD
done
