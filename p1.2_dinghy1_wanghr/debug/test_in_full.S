	add t0, x0, x0
	addi t1, x0, 4
	la t2, test_input
	la t2, test_input
	beq t0, t1, main_exit # main_loop # offset=100 compressed:86
	slli t3, t0, 2
	add t4, t2, t3
	lw a0, 0(t4)
	addi sp, sp, -20
	sw t0, 0(sp)
	sw t1, 4(sp)
	sw t2, 8(sp)
	sw t3, 12(sp)
	sw t4, 16(sp)
	jal ra, factorial # offset=68 compressed:54
	lw t0, 0(sp)
	lw t1, 4(sp)
	lw t2, 8(sp)
	lw t3, 12(sp)
	lw t4, 16(sp)
	addi sp, sp, 20
	addi a1, a0, 0
	addi a0, x0, 1
	ecall
	addi a1, x0, ' '
	addi a0, x0, 11
	ecall
	addi t0, t0, 1
	jal x0, main_loop # offset=-96 compressed:-84
	addi a0, x0, 10 # main_exit
	ecall
	addi t0, x0, 1 # factorial
	mul t0, t0, a0 
	addi a0, a0, -1
	bne a0, x0, -8
	add a0, x0, t0
	jalr x0, ra, 0