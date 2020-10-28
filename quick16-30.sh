#!/bin/bash


for number in {16..30}
do
	CWD="./ep2 250 10 0 output/output1_$number"
	echo $CWD
	$CWD
done
