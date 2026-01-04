.data

# Constant integer specifying the lines of RVC codes

# DO NOT MODIFY THIS VARIABLE
.globl lines_of_rvc_codes
lines_of_rvc_codes:
    .word 21


# RVC codes, 16-bits instructions mixed with 32-bits instructions
# A 16/32-bits binary number represents one line of rvc code.
# You can suppose all of the input codes are valid. 

# DO NOT MODIFY THIS VARIABLE
.globl rvc_codes
rvc_codes:
	.half 0b1000001000000101
	.half 0b1000001000001001
	.half 0b1000001000010001
	.half 0b1000001000100001
	.half 0b1000001001000001
	.half 0b1000011000000101
	.half 0b1000011000001001
	.half 0b1000011000010001
	.half 0b1000011000100001
	.half 0b1000011001000001
	.half 0b1001101000000001
	.half 0b1001101001000001
	.half 0b1001101001100001
	.half 0b1001101001110001
	.half 0b1001101001111001
	.half 0b1001101001111101
	.half 0b1000101000000101
	.half 0b1000101000001001
	.half 0b1000101000010001
	.half 0b1000101000100001
	.half 0b1000101001000001

