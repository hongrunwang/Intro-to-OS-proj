.section .text
.globl show_obj
.type show_obj, @function
# void show_obj(int x(a0),int y(a1),int id(a2),const u8* img(a3));  #
show_obj:
	# (x,y) is the left-upper corner; id: 0-flower(24x18) 1-stone(20x20) 2-onetree(20x30) 3-threetree(20x60) #	
	addi sp,sp,-4
	sw ra,0(sp)
	# for flower,ID=0 #
	li a4,0
	beq a2,a4,flower
	# for stone,ID=1 #
	li a4,1
	beq a2,a4,stone
	# for onetree,ID=2 #
	li a4,2
	beq a2,a4,onetree
	# for threetree,ID=3 #
	li a4,3
	beq a2,a4,threetree
flower:
	# judge which ways to show flower
	li a4,0
	blt a1,a4,flower_top
	# check if it is at bottom
	addi a4,a1,17
	li a5,80
	ble a5,a4,flower_bottom
	# the flower is in the middle and show all.
	addi a2,a0,23
	mv a4,a3
	addi a3,a1,17
	jal LCD_ShowPic # a0-x1 a1-y1 a2-x2 a3-y2 a4-*imm #
	j Return
flower_top:
	# move *img to a4 
	mv a4,a3
	# make x2,y2 at the right-lower corner
	addi a2,a0,23
	addi a3,a1,17
	# calculate the number that pointer need to change.
	sub a1,x0,a1
	li a5,24
	mul a1,a1,a5
	# every elements is 2 bytes.
	li a5,2
	mul a1,a1,a5
	add a4,a4,a1
	# the pic is on the top.
	mv a1,x0
	jal LCD_ShowPic 
	# end showing pic and return .
	j Return
flower_bottom:
	# move *img to a4
	mv a4,a3
	# make x2,y2 at correct position.
	addi a2,a0,23
	li a3,79
	# a0-x1 a1-y1 a2-x2 a3-y2 a4-*imm
	jal LCD_ShowPic
	j Return
stone:
	# determine which way to show.
	li a4,0
	blt a1,a4,stone_top
	# check if it is at bottom
	addi a4,a1,19
	li a5,80
	ble a5,a4,stone_bottom
	# it is in the middle and show all.
	addi a2,a0,19
	mv a4,a3
	addi a3,a1,19
	jal LCD_ShowPic # a0-x1 a1-y1 a2-x2 a3-y2 a4-*imm #
	j Return
stone_top:
	# move *img to a4
	mv a4,a3
	# make x2,y2 at the correct position
	addi a2,a0,19
	addi a3,a1,19
	# calculate the offset
	sub a1,x0,a1
	li a5,24
	mul a1,a1,a5
	# every element is 2 bytes
	li a5,2
	mul a1,a1,a5
	# change the pointer
	add a4,a4,a1
	mv a1,x0
	# a0-x1 a1-y1 a2-x2 a3-y2 a4-*imm
	jal LCD_ShowPic 
	# return 
	j Return
stone_bottom:
	# move *img to a4
	mv a4,a3
	# make the point to the corner.
	addi a2,a0,19
	li a3,79
	# a0-x1 a1-y1 a2-x2 a3-y2 a4-*imm
	jal LCD_ShowPic
	j Return
onetree:
	# judge which way to show
	li a4,0
	blt a1,a4,onetree_top # on the top
	# at bottom
	addi a4,a1,29
	li a5,80
	ble a5,a4,onetree_bottom
	# the picture is in the middle
	addi a2,a0,19
	mv a4,a3
	addi a3,a1,29
	jal LCD_ShowPic # a0-x1 a1-y1 a2-x2 a3-y2 a4-*imm #
	j Return
	# return back
onetree_top:
	# move the pointer
	mv a4,a3
	# set another point at corner
	addi a2,a0,19
	addi a3,a1,29
	# calculet the offset of pointer *img
	sub a1,x0,a1
	li a5,20
	mul a1,a1,a5
	# every elements is 2 bytes
	li a5,2
	mul a1,a1,a5
	# change the pointer *img
	add a4,a4,a1
	mv a1,x0
	# a0-x1 a1-y1 a2-x2 a3-y2 a4-*imm
	jal LCD_ShowPic 
	# jump to the function and return
	j Return
onetree_bottom:
	# move the pointer
	mv a4,a3
	# set another point at corner
	addi a2,a0,19
	li a3,79
	# a0-x1 a1-y1 a2-x2 a3-y2 a4-*imm
	jal LCD_ShowPic
	# jump to function and return
	j Return
threetree:# judge which way
	# the picture is on the top
	li a4,0
	blt a1,a4,threetree_top
	# the picture is at bottom
	addi a4,a1,59
	li a5,80
	ble a5,a4,threetree_bottom
	# the pic is in the middle
	addi a2,a0,19
	mv a4,a3
	addi a3,a1,59
	# set value and jump to the function
	jal LCD_ShowPic # a0-x1 a1-y1 a2-x2 a3-y2 a4-*imm #
	# return
	j Return
threetree_top:
	# move the pointer to the img
	mv a4,a3
	# set the point at another corner
	addi a2,a0,19
	addi a3,a1,59
	# calculate the offset
	sub a1,x0,a1
	li a5,20
	mul a1,a1,a5
	# each is 2 bytes.
	li a5,2
	mul a1,a1,a5
	# change the pointer.
	add a4,a4,a1
	mv a1,x0
	# a0-x1 a1-y1 a2-x2 a3-y2 a4-*imm
	jal LCD_ShowPic 
	# jump to the function and return
	j Return
threetree_bottom:
	# move the pointer to img
	mv a4,a3
	addi a2,a0,19
	li a3,79
	# a0-x1 a1-y1 a2-x2 a3-y2 a4-*imm
	jal LCD_ShowPic
	# jump to the function and return
	j Return
Return:
	# change memory and return back.
	lw ra,0(sp)
	addi sp,sp,4
	jr ra