#   Project 1.2 RVC instructions to RISC-V instructions in RISC-V

#   You can suppose you will get the number of machine codes you should translate from 
#   "lines_of_rvc_codes" and get the 16 or 32 bits-machine codes from "rvc_codes". 
#   Both of them are stored in file "input.S"

#   When we test your project, we will change the input files content.

# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.import input.S

# Your code to convert the RVC codes to RVI codes.
main:
	lw s1, lines_of_rvc_codes # int s1=lines_of_rvc_codes
	la s2, rvc_codes # int *s2=&rvc_codes
	addi sp sp -8 # allocate 2 words in memory
	mv s3, sp # s3 points to the address of Caller-save variables
	slli a0, s1, 2
	sub sp sp a0 # allocate lines_of_rvc_codes*4 words in memory

	# read in and extend instructions, store them in sp[0~lines_of_rvc_codes)
	li a0, 0 # a0 is the iteration number
	li a2, 0 # a2 is the address of the next instruction(relative to s2)
main_l1: # main-loop1: for a0 in [0,lines_of_rvc_codes)
	add a1, s2, a2 # a1 is the absolute address of the next instruction
	sw a0, 0(s3)
	sw a2, 4(s3)
	jal read_instr # a1=read_instr(a1) (meanwhile a0 is set to be 2 or 4)
	
	lw a2, 4(s3)
	add a2, a2, a0 # a2+=a0, update the address of next instruction
	sw a2, 4(s3)
	
	jal extend_instr # a1=extend_instr(a1)
	lw a0, 0(s3)
	slli a0, a0, 2
	add a0, a0, sp
	sw a1, 0(a0) # store a1 in sp+a0*4
	
	lw a2, 4(s3)
	lw a0, 0(s3)
	addi a0, a0, 1 # a0+=1
	blt a0, s1, main_l1 # if a0<lines_of_rvc_codes

	# adjust the offset of 32-bit instructions
	mv a6, sp
	add a7 x0 x0
	jal adjust_offset # adjust_offset(a1)

	# print the converted instructions
	li a0, 0 # a0 is the iteration number
main_l2: # main-loop2: for a0 in [0,lines_of_rvc_codes)
	slli a1, a0, 2
	add a1, a1, sp
	lw a1, 0(a1) # int a1=*(sp+a0*4)
	
	sw a0, 0(s3)
	jal print_instr # print_instr(a1)
	lw a0, 0(s3)
	addi a0, a0, 1
	blt a0, s1, main_l2 # if a0<lines_of_rvc_codes

	# free the allocated spaces
	slli a0, s1, 2
	add sp sp a0 # free lines_of_rvc_codes*4 words of memory
	addi sp sp 8 # free 2 words in memory
	# Exit with error code 0
	add	   a1, x0, x0
	addi   a0, x0, 17
	ecall

# function read_instr(int *a1)
read_instr: # read a 16/32-bit instruction from adress a1
	# return value:
	# int a0 is the length of this instruction(in bytes, 2 or 4)
	# int a1 is the instruction(16-bit instructions stored in lower positions)
	lw a1, 0(a1) # int a1=*a1, retrive the instruction
	andi a2, a1, 0b11
	li a0, 0b11
	bne a2, a0, read_16 # if (a1&0x11)!=0x11, it's 16-bit
	li a0, 4 # 32 bits=4 bytes
	jr ra
read_16:
	li a2, 0xffff
	and a1, a1, a2 # a1&=0xffff
	li a0, 2 # 16 bits=2 bytes
	jr ra

# function print_instr(int a1)
print_instr: # print instruction a1(no return value)
	li a0, 11 # a0=11: prints ASCII character in a1
	mv a3, a1 # int a3=a1
	li a2, 0x80000000
print_l1: # print-loop1: for a2=(1<<i), i=31,...,0
	and a1, a3, a2 # a1=a3&a2
	beq a1, x0, print_b1 # if a3&a2==0
	li a1, 49 # else char a1='1'
	j print_c1
print_b1: # print-branch1
	li a1, 48 # char a1='0'
print_c1: # print-continue1
	ecall
	srli a2, a2, 1 # a2>>=1
	bne a2, x0, print_l1 # if a2!=0

	li a1, 10
	ecall # putchar('\n')
	jr ra # return

# function: extend_instr(int a1)
extend_instr: # extend a 16-bit instruction to 32-bit
	# return value: a1 is the extended 32-bit instruction(if it is originally 16-bit, the first bit is marked as 0)
	andi a2, a1, 0b11
	li a0, 0b11
	bne a2, a0, extend_16 # if (a1&0x11)!=0x11, it's 16-bit
	jr ra # for 32-bit instructions, directly return
extend_16: # extend-16bit
	andi a0, a1, 0b11 # get opcode
	li a2, 0b10
	beq a0, a2, extend_b1 # opcode==0b10
	li a2, 0b00
	beq a0, a2, extend_b2 # opcode==0b00
	li a2, 0b01
	beq a0, a2, extend_b3 # opcode==0b01
	jr ra # !error: invalid opcode
extend_b1: # CR Format or slli
	srli a2, a1, 12
	andi a0, a2, 0b1111 # get funct4
	li a2, 0b1001
	beq a0, a2, extend_b1_b1 # funct4==0b1001
	li a2, 0b1000
	beq a0, a2, extend_b1_b2  # funct4==0b1000
	j extend_slli # CI.slli, funct3 should be 0b000
	# might also be invalid, but too lazy to judge
extend_b1_b1: # add or jalr
	srli a2, a1, 2
	andi a0, a2, 0b11111 # get rs2
	beq a0, x0, extend_jalr # rs2==0
	j extend_add # rs2!=0
extend_b1_b2: # mv or jr
	srli a2, a1, 2
	andi a0, a2, 0b11111 # get rs2
	beq a0, x0, extend_jr # rs2==0
	j extend_mv # rs2!=0
extend_b2: # sw or lw
	srli a2, a1, 13
	andi a0, a2, 0b111 # get funct3
	li a2, 0b010
	beq a0, a2, extend_lw # funct3==0b010
	li a2, 0b110
	beq a0, a2, extend_sw # funct3==0b110
	jr ra # !error: invalid funct3
extend_b3: # CI(slli excluded) or CS-type2 or CB or CJ
	srli a2, a1, 13
	andi a0, a2, 0b111 # get funct3
	li a2, 0b010
	beq a0, a2, extend_li # funct3==0b010
	li a2, 0b011
	beq a0, a2, extend_lui # funct3==0b011
	li a2, 0b000
	beq a0, a2, extend_addi # funct3==0b000
	li a2, 0b110
	beq a0, a2, extend_beqz # funct3==0b110
	li a2, 0b111
	beq a0, a2, extend_bnez # funct3==0b111
	li a2, 0b101
	beq a0, a2, extend_j # funct3==0b101
	li a2, 0b001
	beq a0, a2, extend_jal # funct3==0b001
	# funct3==0x100
	srli a2, a1, 10
	andi a0, a2, 0b11 # get funct2 of CB-TYPE2
	li a2, 0b00
	beq a0, a2, extend_srli # funct2==0b00
	li a2, 0b01
	beq a0, a2, extend_srai # funct2==0b01
	li a2, 0b10
	beq a0, a2, extend_andi # funct2==0b10
	# funct2==0x11, CS-TYPE2
	srli a2, a1, 5
	andi a0, a2, 0b11 # get funct2 of CS-TYPE2
	li a2, 0b11
	beq a0, a2, extend_and # funct2==0b11
	li a2, 0b10
	beq a0, a2, extend_or # funct2==0b10
	li a2, 0b01
	beq a0, a2, extend_xor # funct2==0b01
	li a2, 0b00
	beq a0, a2, extend_sub # funct2==0b00
	jr ra # !error: this line is impossible
extend_add: # add rd, rd, rs2
	li a2, 0b0110011 # opcode=0b0110011
	srli a0, a1, 7
	andi a3, a0, 0b11111 # rd=(a1>>7)&0b11111
	li a4, 0x0 # func3=0x0
	mv a5, a3 # rs1=rd
	srli a0, a1, 2
	andi a6, a0, 0b11111 # rs2=(a1>>2)&0b11111
	li a7, 0x00 # func7=0x00
	j extend_c1
extend_mv: # add rd, x0, rs2
	li a2, 0b0110011 # opcode=0b0110011
	srli a0, a1, 7
	andi a3, a0, 0b11111 # rd=(a1>>7)&0b11111
	li a4, 0x0 # func3=0x0
	li a5, 0 # rs1=0
	srli a0, a1, 2
	andi a6, a0, 0b11111 # rs2=(a1>>2)&0b11111
	li a7, 0x00 # func7=0x00
	j extend_c1
extend_jr: # jalr x0, 0(rs1)
	li a2, 0b1100111 # opcode=0b1100111
	li a3, 0 # rd=0
	li a4, 0x0 # func3=0x0
	srli a0, a1, 7
	andi a5, a0, 0b11111 # rs1=(a1>>7)&0b11111
	li a1, 0 # imm=0
	j extend_c1
extend_jalr: # jalr x1, 0(rs1)
	li a2, 0b1100111 # opcode=0b1100111
	li a3, 1 # rd=1
	li a4, 0x0 # func3=0x0
	srli a0, a1, 7
	andi a5, a0, 0b11111 # rs1=(a1>>7)&0b11111
	li a1, 0 # imm=0
	j extend_c1
extend_slli: # slli rd, rd, shamt
	li a2, 0b0010011 # opcode=0b0010011
	srli a0, a1, 7
	andi a3, a0, 0b11111 # rd=(a1>>7)&0b11111
	li a4, 0x1 # func3=0x1
	mv a5, a3 # rs1=rd
	srli a0, a1, 2
	andi a1, a0, 0b11111 # imm=(a1>>2)&0b11111
	j extend_c1
extend_lw: # lw rd', offset(rs1')
	li a2, 0b0000011 # opcode=0b0000011
	srli a0, a1, 2
	andi a3, a0, 0b111
	addi a3, a3, 8 # rd=((a1>>2)&0b111)+8
	li a4, 0x2 # func3=0x2
	srli a0, a1, 7
	andi a5, a0, 0b111
	addi a5, a5, 8 # rs1=((a1>>7)&0b111)+8
	srli a0, a1, 7 # imm[5:3]=a1[12:10]
	andi a7, a0, 0b111000 # imm=(a1>>7)&0b111000
	slli a0, a1, 1 # imm[6]=a1[5]
	andi a0, a0, 0b1000000
	or a7, a7, a0 # imm|=(a1<<1)&0b1000000
	srli a0, a1, 4 # imm[2]=a1[6]
	andi a0, a0, 0b100
	or a7, a7, a0 # imm|=(a1>>4)&0b100
	mv a1, a7
	j extend_c1
extend_sw: # sw rs2, offset(rs1')
	li a2, 0b0100011 # opcode=0b0100011
	li a4, 0x2 # func3=0x2
	srli a0, a1, 7
	andi a5, a0, 0b111
	addi a5, a5, 8 # rs1=((a1>>7)&0b111)+8
	srli a0, a1, 2
	andi a6, a0, 0b111
	addi a6, a6, 8 # rs2=((a1>>2)&0b111)+8
	srli a0, a1, 7 # imm[5:3]=a1[12:10]
	andi a7, a0, 0b111000 # imm=(a1>>7)&0b111000
	slli a0, a1, 1 # imm[6]=a1[5]
	andi a0, a0, 0b1000000
	or a7, a7, a0 # imm|=(a1<<1)&0b1000000
	srli a0, a1, 4 # imm[2]=a1[6]
	andi a0, a0, 0b100
	or a7, a7, a0 # imm|=(a1>>4)&0b100
	mv a1, a7
	j extend_c1
extend_li: # addi rd, x0, imm
	li a2, 0b0010011 # opcode=0b0010011
	srli a0, a1, 7
	andi a3, a0, 0b11111 # rd=(a1>>7)&0b11111
	li a4, 0x0 # func3=0x0
	li a5, 0 # rs1=0
	srli a0, a1, 2 # imm[4:0]=a1[6:2]
	andi a7, a0, 0b11111 # imm=(a1>>2)&0b11111
	srli a0, a1, 7 # imm[5]=a1[12]
	andi a0, a0, 0b100000
	or a7, a7, a0 # imm|=(a1>>7)&0b100000
	slli a7, a7, 26
	srai a7, a7, 26 # msb-extends
	mv a1, a7
	j extend_c1
extend_lui: # lui rd, nzimm
	li a2, 0b0110111 # opcode=0b0110111
	srli a0, a1, 7
	andi a3, a0, 0b11111 # rd=(a1>>7)&0b11111
	srli a0, a1, 2 # imm[4:0]=a1[6:2]
	andi a7, a0, 0b11111 # imm=(a1>>2)&0b11111
	srli a0, a1, 7 # imm[5]=a1[12]
	andi a0, a0, 0b100000
	or a7, a7, a0 # imm|=(a1>>7)&0b100000
	slli a7, a7, 26
	srai a7, a7, 26 # msb-extends
	mv a1, a7
	j extend_c1
extend_addi: # addi rd, rd, nzimm
	li a2, 0b0010011 # opcode=0b0010011
	srli a0, a1, 7
	andi a3, a0, 0b11111 # rd=(a1>>7)&0b11111
	li a4, 0x0 # func3=0x0
	mv a5, a3 # rs1=rd
	srli a0, a1, 2 # imm[4:0]=a1[6:2]
	andi a7, a0, 0b11111 # imm=(a1>>2)&0b11111
	srli a0, a1, 7 # imm[5]=a1[12]
	andi a0, a0, 0b100000
	or a7, a7, a0 # imm|=(a1>>7)&0b100000
	slli a7, a7, 26
	srai a7, a7, 26 # msb-extends
	mv a1, a7
	j extend_c1
extend_beqz: # beq rs1', x0, offset
	li a2, 0b1100011 # opcode=0b1100011
	li a4, 0x0 # func3=0x0
	srli a0, a1, 7
	andi a5, a0, 0b111
	addi a5, a5, 8 # rs1=((a1>>7)&0b111)+8
	li a6, 0 # rs2=0
	slli a0, a1, 3 # imm[5]=a1[2]
	andi a7, a0, 0b100000 # imm=(a1<<3)&0b100000
	srli a0, a1, 2 # imm[2:1]=a1[4:3]
	andi a0, a0, 0b110
	or a7, a7, a0 # imm|=(a1>>2)&0b110
	slli a0, a1, 1 # imm[7:6]=a1[6:5]
	andi a0, a0, 0b11000000
	or a7, a7, a0 # imm|=(a1<<1)&0b11000000
	srli a0, a1, 7 # imm[4:3]=a1[11:10]
	andi a0, a0, 0b11000
	or a7, a7, a0 # imm|=(a1>>7)&0b11000
	srli a0, a1, 4 # imm[8]=a1[12]
	andi a0, a0, 0b100000000
	or a7, a7, a0 # imm|=(a1>>4)&0b100000000
	slli a7, a7, 23
	srai a7, a7, 23 # msb-extends
	mv a1, a7
	j extend_c1
extend_bnez: # bne rs1', x0, offset	
	li a2, 0b1100011 # opcode=0b1100011
	li a4, 0x1 # func3=0x1
	srli a0, a1, 7
	andi a5, a0, 0b111
	addi a5, a5, 8 # rs1=((a1>>7)&0b111)+8
	li a6, 0 # rs2=0
	slli a0, a1, 3 # imm[5]=a1[2]
	andi a7, a0, 0b100000 # imm=(a1<<3)&0b100000
	srli a0, a1, 2 # imm[2:1]=a1[4:3]
	andi a0, a0, 0b110
	or a7, a7, a0 # imm|=(a1>>2)&0b110
	slli a0, a1, 1 # imm[7:6]=a1[6:5]
	andi a0, a0, 0b11000000
	or a7, a7, a0 # imm|=(a1<<1)&0b11000000
	srli a0, a1, 7 # imm[4:3]=a1[11:10]
	andi a0, a0, 0b11000
	or a7, a7, a0 # imm|=(a1>>7)&0b11000
	srli a0, a1, 4 # imm[8]=a1[12]
	andi a0, a0, 0b100000000
	or a7, a7, a0 # imm|=(a1>>4)&0b100000000
	slli a7, a7, 23
	srai a7, a7, 23 # msb-extends
	mv a1, a7
	j extend_c1
extend_j: # jal x0, offset
	li a2, 0b1101111 # opcode=0b1101111
	li a3, 0 # rd=0
	slli a0, a1, 3 # imm[5]=a1[2]
	andi a7, a0, 0b100000 # imm=(a1<<3)&0b100000
	srli a0, a1, 2 # imm[3:1]=a1[5:3]
	andi a0, a0, 0b1110
	or a7, a7, a0 # imm|=(a1>>2)&0b1110
	slli a0, a1, 1 # imm[7]=a1[6]
	andi a0, a0, 0b10000000
	or a7, a7, a0 # imm|=(a1<<1)&0b10000000
	srli a0, a1, 1 # imm[6]=a1[7]
	andi a0, a0, 0b1000000
	or a7, a7, a0 # imm|=(a1>>1)&0b1000000
	slli a0, a1, 2 # imm[10]=a1[8]
	andi a0, a0, 0b10000000000
	or a7, a7, a0 # imm|=(a1<<2)&0b10000000000
	srli a0, a1, 1 # imm[9:8]=a1[10:9]
	andi a0, a0, 0b1100000000
	or a7, a7, a0 # imm|=(a1>>1)&0b1100000000
	srli a0, a1, 7 # imm[4]=a1[11]
	andi a0, a0, 0b10000
	or a7, a7, a0 # imm|=(a1>>7)&0b10000
	srli a0, a1, 1 # imm[11]=a1[12]
	andi a0, a0, -2048 # i.e.0b100000000000
	or a7, a7, a0 # imm|=(a1>>1)&0b100000000000
	slli a7, a7, 23
	srai a7, a7, 23 # msb-extends
	mv a1, a7
	j extend_c1
extend_jal: # jal x1, offset
	li a2, 0b1101111 # opcode=0b1101111
	li a3, 1 # rd=1
	slli a0, a1, 3 # imm[5]=a1[2]
	andi a7, a0, 0b100000 # imm=(a1<<3)&0b100000
	srli a0, a1, 2 # imm[3:1]=a1[5:3]
	andi a0, a0, 0b1110
	or a7, a7, a0 # imm|=(a1>>2)&0b1110
	slli a0, a1, 1 # imm[7]=a1[6]
	andi a0, a0, 0b10000000
	or a7, a7, a0 # imm|=(a1<<1)&0b10000000
	srli a0, a1, 1 # imm[6]=a1[7]
	andi a0, a0, 0b1000000
	or a7, a7, a0 # imm|=(a1>>1)&0b1000000
	slli a0, a1, 2 # imm[10]=a1[8]
	andi a0, a0, 0b10000000000
	or a7, a7, a0 # imm|=(a1<<2)&0b10000000000
	srli a0, a1, 1 # imm[9:8]=a1[10:9]
	andi a0, a0, 0b1100000000
	or a7, a7, a0 # imm|=(a1>>1)&0b1100000000
	srli a0, a1, 7 # imm[4]=a1[11]
	andi a0, a0, 0b10000
	or a7, a7, a0 # imm|=(a1>>7)&0b10000
	srli a0, a1, 1 # imm[11]=a1[12]
	andi a0, a0, -2048 # i.e.0b100000000000
	or a7, a7, a0 # imm|=(a1>>1)&0b100000000000
	slli a7, a7, 20
	srai a7, a7, 20 # msb-extends
	mv a1, a7
	j extend_c1
extend_srli: # srli rd', rd', shamt
	li a2, 0b0010011 # opcode=0b0010011
	srli a0, a1, 7
	andi a3, a0, 0b111
	addi a3, a3, 8 # rd=((a1>>7)&0b111)+8
	li a4, 0x5 # funct3=0x5
	mv a5, a3 # rs1=rd
	li a7, 0x00 #funct7=0x00
	srli a0, a1, 2
	andi a1, a0, 0b11111 # imm=(a1>>2)&0b1111
	j extend_c1
extend_srai: # srai rd', rd', shamt
	li a2, 0b0010011 # opcode=0b0010011
	srli a0, a1, 7
	andi a3, a0, 0b111
	addi a3, a3, 8 # rd=((a1>>7)&0b111)+8
	li a4, 0x5 # funct3=0x5
	mv a5, a3 # rs1=rd
	li a7, 0x20 #funct7=0x20
	srli a0, a1, 2
	andi a1, a0, 0b11111 # imm=(a1>>2)&0b1111
	j extend_c1
extend_andi: # andi rd', rd', imm
	li a2, 0b0010011 # opcode=0b0010011
	srli a0, a1, 7
	andi a3, a0, 0b111
	addi a3, a3, 8 # rd=((a1>>7)&0b111)+8
	li a4, 0x7 # funct3=0x7
	mv a5, a3 # rs1=rd
	srli a0, a1, 2 # imm[4:0]=a1[6:2]
	andi a7, a0, 0b11111 # imm=(a1>>2)&0b11111
	srli a0, a1, 7 # imm[5]=a1[12]
	andi a0, a0, 0b100000
	or a7, a7, a0 # imm|=(a1>>7)&0b100000
	slli a7, a7, 26
	srai a7, a7, 26 # msb-extends
	mv a1, a7
	j extend_c1
extend_and: # and rd', rd', rs2'
	li a2, 0b0110011 # opcode=0b0110011
	srli a0, a1, 7
	andi a3, a0, 0b111
	addi a3, a3, 8 # rd=((a1>>7)&0b111)+8
	li a4, 0x7 # funct3=0x7
	mv a5, a3 # rs1=rd
	srli a0, a1, 2
	andi a6, a0, 0b111
	addi a6, a6, 8 # rs2=((a1>>2)&0b111)+8
	li a7, 0x00 # funct7=0x00
	j extend_c1
extend_or: # or rd', rd', rs2'
	li a2, 0b0110011 # opcode=0b0110011
	srli a0, a1, 7
	andi a3, a0, 0b111
	addi a3, a3, 8 # rd=((a1>>7)&0b111)+8
	li a4, 0x6 # funct3=0x6
	mv a5, a3 # rs1=rd
	srli a0, a1, 2
	andi a6, a0, 0b111
	addi a6, a6, 8 # rs2=((a1>>2)&0b111)+8
	li a7, 0x00 # funct7=0x00
	j extend_c1
extend_xor: # xor rd', rd', rs2'
	li a2, 0b0110011 # opcode=0b0110011
	srli a0, a1, 7
	andi a3, a0, 0b111
	addi a3, a3, 8 # rd=((a1>>7)&0b111)+8
	li a4, 0x4 # funct3=0x4
	mv a5, a3 # rs1=rd
	srli a0, a1, 2
	andi a6, a0, 0b111
	addi a6, a6, 8 # rs2=((a1>>2)&0b111)+8
	li a7, 0x00 # funct7=0x00
	j extend_c1
extend_sub: # sub rd', rd', rs2'
	li a2, 0b0110011 # opcode=0b0110011
	srli a0, a1, 7
	andi a3, a0, 0b111
	addi a3, a3, 8 # rd=((a1>>7)&0b111)+8
	li a4, 0x0 # funct3=0x0
	mv a5, a3 # rs1=rd
	srli a0, a1, 2
	andi a6, a0, 0b111
	addi a6, a6, 8 # rs2=((a1>>2)&0b111)+8
	li a7, 0x20 # funct7=0x20
	j extend_c1
extend_c1: # extend-continue1
	# composing the 32-bit instruction
	addi sp, sp, -4
	sw ra, 0(sp)
	jal compose # a2=compose(a1,...,a7)
	lw ra, 0(sp)
	addi sp, sp, 4
	# mv a1, a2 # return a1=a2
	addi a1, a2, -1 # return a1=a2-1 (mark as 16-bit extended)
	jr ra

# function: adjust_offset(int a6[]) (and global variable s1)
adjust_offset: # adjust the offset of 32-bit instructions in a1[0~lines_of_rvc_codes)
	# if the instruction a1[i] is extended from 16-bit, the first bit is marked as 0
	addi sp sp -12 # reserve 3 spaces
	li a0, 0 # iteration index
adjust_l1:
	slli a1, a0, 2
	add a1, a1, a6
	lw a2, 0(a1) # a2=instrs[a0]
	ori a2, a2, 0b1
	
	# if not branch or jal, continue
	li a1, 0b1111111
	and a1, a1, a2
	li a4, 0b1100011
	beq a1, a4, adjust_b1 # branch
	li a4, 0b1101111
	beq a1, a4, adjust_b1 # jal
	j adjust_c1 # continue
	
adjust_b1: # need to update the offset of branch and jal
	sw ra, 0(sp)
	sw a0, 4(sp)
	jal get_imm # a1=get_imm(a2)
	lw a0, 4(sp)
	
	# change a1 to be the new value
adjust_l1_l1:
	li a2, 0 # adjustion need to add to final a1
	mv a3, a1 # temporary a3
	mv a4, a0 # the destination instruction
	beq a1, x0, adjust_l1_end # if offset==0, break
	blt a1, x0, adjust_l1_neg # if offset<0, negative offset
	j adjust_l1_pos
adjust_l1_neg: # negative offset
	addi a4, a4, -1
	addi a3, a3, 4
	slli a5, a4, 2
	add a5, a5, a6
	lw a5, 0(a5) # a5=instrs[a4]
	andi a5, a5, 0b1
	bne a5, x0, adjust_l1_neg_c1
	# was originally 16-bit
	addi a3, a3, -2
	addi a2, a2, -2
adjust_l1_neg_c1: # negtive-continue1
	beq a4, x0, adjust_l1_end  # if a4==0&&a3!=0 then invalid
	blt a3, x0, adjust_l1_neg
	j adjust_l1_end
adjust_l1_pos: # positive offset
	addi a3, a3, -4
	slli a5, a4, 2
	add a5, a5, a6
	lw a5, 0(a5) # a5=instrs[a4]
	andi a5, a5, 0b1
	bne a5, x0, adjust_l1_pos_c1
	# was originally 16-bit
	addi a3, a3, 2
	addi a2, a2, 2
adjust_l1_pos_c1:
	# if a3<=0 then break
	beq a3, x0, adjust_l1_end
	blt a3, x0, adjust_l1_end
	# a3>0, continue loop
	addi a4, a4, 1
	bge a4, s1, adjust_l1_end # if a4>=s1 then invalid.
	j adjust_l1_pos
adjust_l1_end:
	add a1, a1, a2 # a1+=a2 (finish adjustion)
	
	slli a2, a0, 2
	add a2, a2, a6
	lw a3, 0(a2) # a3=a6[a0]
	andi a7, a3, 0b1
	sw a7, 8(sp)
	ori a3, a3, 0b1 # set first bit to 1
	jal set_imm # a2=set_imm(a1, a3)
	lw ra, 0(sp)
	lw a0, 4(sp)
	lw a7, 8(sp)
	li a1, 0xfffffffe
	and a2, a2, a1 # set first bit to 0
	or a2, a2, a7 # restore first bit
	
	slli a1, a0, 2
	add a1, a1, a6
	sw a2, 0(a1) # a6[a0]=a2
	
adjust_c1: # adjust-continue1
	addi a0, a0, 1
	blt a0, s1, adjust_l1

	# unmark the first bit of instructions
	li a0, 0 # iteration index
adjust_l2:
	slli a1, a0, 2
	add a1, a1, a6
	lw a2, 0(a1)
	ori a2, a2, 0b1 # unmark the first bit
	sw a2, 0(a1)
	addi a0, a0, 1
	blt a0, s1, adjust_l2

	# free spaces and return
	addi sp sp 12
	jr ra

# function get_imm(int a1, int a2)
get_imm: # give the instr a2 and return a1 which is imm.
	li a1 0b1111111
	and a1 a1 a2 # get the opcode of instr and store in a1
	li a4 0b1100011 # judge if branch
	beq a4 a1 get_branch
	li a4 0b1101111
	beq a4 a1 get_jal # judge jal
	li a4 0b1100111
	beq a4 a1 get_jalr # judge jalr
	jr ra
get_branch:
	add a1 x0 x0
	li a4 0b11110 # imm[4:1]
	slli a4 a4 7
	and a4 a4 a2
	srli a4 a4 7
	or a1 a4 a1
	li a4 0b10000000 # imm[11]
	and a4 a4 a2
	slli a4 a4 4
	or a1 a1 a4
	li a4 0b111111 # imm[10:5]
	slli a4 a4 25
	and a4 a4 a2
	srli a4 a4 20
	or a1 a1 a4
	li a4 0b1000000 # imm[12]
	slli a4 a4 25
	and a4 a4 a2
	srai a4 a4 19
	or a1 a1 a4
	jr ra
get_jal: #imm[20|10:1|11|19:12] in instr[31:12]
	add a1 x0 x0 # set a1 as 0
 	li a4 0b11111111 
	slli a4 a4 12 # imm[19:12]
	and a4 a4 a2
	or a1 a4 a1
	li a4 0b100000000 # imm[11]
	slli a4 a4 12
	and a4 a4 a2
	srai a4 a4 9
	or a1 a4 a1
	li a4 0b11111111110 # imm[10:1]
	slli a4 a4 20
	and a4 a4 a2
	srai a4 a4 20
	or a1 a4 a1
	li a4 0b10000000000000000000000000000000 # imm[20]
	and a4 a4 a2
	srai a4 a4 11
	or a1 a4 a1
	jr ra
get_jalr:
	add a1 x0 x0 # set a1 as 0
	li a4 0b11111111111100000000000000000000 # imm[31:12]
	or a4 a4 a2
	srai a1 a4 20  
	jr ra
# function set_imm(int a1, int a3)
set_imm: # set the immediate value of 32-bit instruction and the imm is stored in a1
	# a3 stores the instr that is composed.
	# return value: a2 is the 32-bit instruction with imm changed 
	li a0 0b1111111
	and a2 a3 a0 
	li a0 0b0010011 # I format
	beq a2 a0 set_I
	li a0 0b11      # another opcode in I format
	beq a2 a0 set_I
	li a0 0b1100111
	beq a2 a0 set_I# jalr instr
	li a0 0b0100011 # S format
	beq a2 a0 set_S
	li a0 0b1100011 # B format
	beq a2 a0 set_B
	li a0 0b1101111 # J format
	beq a2 a0 set_J
	li a0 0b0110111 # U format
	beq a2 a0 set_U
	li a0 0b0010111
	beq a2 a0 set_U
	add a2 a3 x0 #for R format, just move a3 to a2
	jr ra
set_I:
	li a0 0b0010011 #  srai
	bne a0 a2 normal
	li a0 0b111
	slli a0 a0 12
	and a0 a0 a3
	srli a0 a0 12
	li a2 0b101
	bne a0 a2 normal
	li a0 0b11111
	and a4 a1 a0
	slli a4 a4 20
	or a2 a3 a4
	jr ra
normal:
	li a0 0b11111111111111111111
	and a2 a3 a0  # set the specific bits as 0
	li a0 0b111111111111
	and a1 a1 a0  # we only need imm[11:0]->instr[31:20] 
	slli a1 a1 20 # shift left to the imm and or operation to set imm to instr
	or a2 a2 a1 
	jr ra  # get the instruction and store in a2
set_S:
	li a0 0b1111111111111000001111111
	and a2 a3 a0  # imm[11:5]-> instr[31:25] , imm[4:0] -> instr[11:7]
	li a0 0b111111111111
	and a1 a1 a0  # set the specific bits to 0
	andi a4 a1 0b11111# take imm[4:0]
	slli a4 a4 7
	or a2 a4 a2   # make imm[4:0]  set at instr[12:7]
	li a0 0b111111100000
	and a4 a1 a0
	slli a4 a4 20
	or a2 a4 a2  # get the instruction and store in a2
	jr ra
set_B:
	li a0 0b1111111111111000001111111
	and a2 a3 a0   #  set the specific bits to 0
	li a0 0b11110
	and a4 a1 a0   #  imm[4:1|11] -> instr[11:7]
	slli a4 a4 7
	or a2 a4 a2
	li a0 0b11111100000 
	and a4 a1 a0   #  imm[12|10:5] -> instr[31:25]
	slli a4 a4 20   
	or a2 a4 a2
	li a0 0b100000000000
	and a4 a1 a0
	srli a4 a4 4  # take imm[12] and imm[11] separately and place them in instr
	or a2 a4 a2
	li a0 0b1000000000000
	and a4 a1 a0
	slli a4 a4 19
	or a2 a4 a2  # get the instruction and store in a2
	jr ra
set_U:
	li a0 0b111111111111 # instr[11:0] is in need.
	and a2 a3 a0
	li a0 0b000011111111111111111111 # take out imm[19:0] and place it into instr[31:12]
	and a4 a1 a0
	slli a4 a4 12
	or a2 a2 a4 # get the instruction and store in a2
	jr ra
set_J:
	li a0 0b111111111111 # Only instr[11:0] is in need.
	and a2 a3 a0
	li a0 0b11111111110 # imm[20|10:1|11|19:12] -> instr[]
	and a4 a1 a0
	slli a4 a4 20	
	or a2 a2 a4  # take imm[10:1] 
	li a0 0b100000000000
	and a4 a1 a0 # take imm[11]
	slli a4 a4 9
	or a2 a4 a2
	li a0 0b11111111000000000000 
	and a4 a1 a0  # take imm[19:12]
	or a2 a2 a4
	li a0 0b1
	slli a0 a0 20  #take imm[20]
	and a4 a1 a0
	slli a4 a4 11
	or a2 a2 a4  # get the instruction and store in a2
	jr ra
compose:# function: compose( a1(imm) a2(opcode) a3(rd) a4(funct3) a5(rs1) a6(rs2) a7(funct7) )
	# retrun value:a0 is the 32-bit instruction which is composed but imm is not in.
	add a0 x0 x0# shift left every part of instruction and stored the composed value in a0
	andi a2 a2 0b1111111
	or a0 a0 a2 # a2 is opcode 
	andi a3 a3 0b11111
	slli a3 a3 7
	or a0 a0 a3 # a3 is rd
	andi a4 a4 0b111
	slli a4 a4 12
	or a0 a0 a4 # a4 is funct3
	andi a5 a5 0b11111
	slli a5 a5 15
	or a0 a0 a5	# a5 is rs1
	andi a6 a6 0b11111
	slli a6 a6 20	
	or a0 a0 a6 # a6 is rs2
	andi a7 a7 0b1111111
	slli a7 a7 25
	or a0 a0 a7 # a7 is funct7
	add a3 a0 x0
	j set_imm
	