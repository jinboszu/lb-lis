#!/bin/bash

mkdir outputs

size=$1

LIMITS=10
LIMITT=7

ALLINPUTS=()
ALLOUTPUTS=()

for ((s=6; s<=LIMITS; s++))
do
	for ((t=3; t<=LIMITT; t++))
	do
		INPUT="../inputs/S=$s,T=$t,$size.txt"

		OUTPUT="outputs/S=$s,T=$t,$size.txt"

		ALLINPUTS+=($INPUT)

		ALLOUTPUTS+=($OUTPUT)
	done
done


for ((IND=0; IND<25; IND++))
do
	./solve < ${ALLINPUTS[$IND]} > ${ALLOUTPUTS[$IND]}
done
