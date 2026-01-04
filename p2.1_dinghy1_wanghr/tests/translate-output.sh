#!/bin/bash

# translate alu_xxx_ref.out and alu_xxx_student.out to readable forms to ./compare_output/

folder=compare_output
rm -rf ${folder}
mkdir ${folder}

names=(add mult div-rem slt-sub mulh comprehensive comprehensive-2 sir cnto)
for name in ${names[@]}
do
	python binary_to_hex_alu.py reference_output/alu-${name}-ref.out > ${folder}/alu-${name}-ref.out
	python binary_to_hex_alu.py student_output/alu-${name}-student.out > ${folder}/alu-${name}-student.out
	echo comparing alu-${name}-ref.out and alu-${name}-student.out
	diff ${folder}/alu-${name}-ref.out ${folder}/alu-${name}-student.out
done