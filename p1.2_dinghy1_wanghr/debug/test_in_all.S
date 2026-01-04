add x5 x5 x12 # c.add
add x5 x0 x12 # c.mv
jalr x0 x12 0 # c.jr
jalr x1 x12 0 # c.jalr
addi x12 x0 -32 # c.li
addi x12 x0 -16 # c.li
addi x12 x0 -8 # c.li
addi x12 x0 -4 # c.li
addi x12 x0 -2 # c.li
addi x12 x0 -1 # c.li
addi x12 x0 1 # c.li
addi x12 x0 2 # c.li
addi x12 x0 4 # c.li
addi x12 x0 8 # c.li
addi x12 x0 16 # c.li
lui x12 1  # c.lui
lui x12 2  # c.lui
lui x12 4  # c.lui
lui x12 8  # c.lui
lui x12 16 # c.lui
addi x12 x12 -32 # c.addi
addi x12 x12 -16 # c.addi
addi x12 x12 -8 # c.addi
addi x12 x12 -4 # c.addi
addi x12 x12 -2 # c.addi
addi x12 x12 -1 # c.addi
addi x12 x12 1  # c.addi
addi x12 x12 2  # c.addi
addi x12 x12 4  # c.addi
addi x12 x12 8  # c.addi
addi x12 x12 16 # c.addi
slli x12 x12 1 # c.slli
slli x12 x12 2 # c.slli
slli x12 x12 4 # c.slli
slli x12 x12 8 # c.slli
slli x12 x12 16 # c.slli
lw x12 4(x15) # c.lw
lw x12 8(x15) # c.lw
lw x12 16(x15) # c.lw
lw x12 32(x15) # c.lw
lw x12 64(x15) # c.lw
sw x12 4(x15) # c.sw
sw x12 8(x15) # c.sw
sw x12 16(x15) # c.sw
sw x12 32(x15) # c.sw
sw x12 64(x15) # c.sw
and x12 x12 x15 # c.and
or x12 x12 x15 # c.or
xor x12 x12 x15 # c.xor
sub x12 x12 x15 # c.sub
beq x12 x0 2 # c.beqz
beq x12 x0 4 # c.beqz
beq x12 x0 8 # c.beqz
beq x12 x0 16 # c.beqz
beq x12 x0 32 # c.beqz
beq x12 x0 64 # c.beqz
beq x12 x0 128 # c.beqz
beq x12 x0 256 # c.beqz
bne x12 x0 2 # c.bnez
bne x12 x0 4 # c.bnez
bne x12 x0 8 # c.bnez
bne x12 x0 16 # c.bnez
bne x12 x0 32 # c.bnez
bne x12 x0 64 # c.bnez
bne x12 x0 128 # c.bnez
bne x12 x0 256 # c.bnez
srli x12 x12 1 # c.srli
srli x12 x12 2 # c.srli
srli x12 x12 4 # c.srli
srli x12 x12 8 # c.srli
srli x12 x12 16 # c.srli
srai x12 x12 1 # c.srai
srai x12 x12 2 # c.srai
srai x12 x12 4 # c.srai
srai x12 x12 8 # c.srai
srai x12 x12 16 # c.srai
andi x12 x12 -32 # c.andi
andi x12 x12 -16 # c.andi
andi x12 x12 -8 # c.andi
andi x12 x12 -4 # c.andi
andi x12 x12 -2 # c.andi
andi x12 x12 -1 # c.andi
andi x12 x12 1  # c.andi
andi x12 x12 2  # c.andi
andi x12 x12 4  # c.andi
andi x12 x12 8  # c.andi
andi x12 x12 16 # c.andi
jal x0 4 # c.j
jal x0 8 # c.j
jal x0 16 # c.j
jal x0 32 # c.j
jal x0 64 # c.j
jal x0 128 # c.j
jal x0 256 # c.j
jal x0 512 # c.j
jal x0 1024 # c.j
jal x0 2048 # c.j
jal x1 4 # c.jal
jal x1 8 # c.jal
jal x1 16 # c.jal
jal x1 32 # c.jal
jal x1 64 # c.jal
jal x1 128 # c.jal
jal x1 256 # c.jal
jal x1 512 # c.jal
jal x1 1024 # c.jal
jal x1 2048 # c.jal
