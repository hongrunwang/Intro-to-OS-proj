#!/bin/bash

# translate alu_xxx_ref.out and alu_xxx_student.out to readable forms to ./compare_output/

folder=compare_output
rm -rf ${folder}
mkdir ${folder}

names=(add_lui_sll addi br_jalr branch jump mem)
for name in ${names[@]}
do
	filename=CPU-${name}.out
	refname=CPU-${name}-ref.out
	python binary_to_hex.py reference_output/${filename} > ${folder}/${refname}
	python binary_to_hex.py output/${filename} > ${folder}/${filename}
	# echo checking ${filename}
	# diff ${folder}/${filename} ${folder}/${refname}
done