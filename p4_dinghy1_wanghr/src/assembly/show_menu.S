.data
normal: .string "Easy"
endless: .string "Hard"
title0: .string "E"
title1: .string "s"
title2: .string "c"
title3: .string "a"
title4: .string "p"
title5: .string "e"
title6: .string "!"
.section .text
.globl show_menu
.type show_menu, @function
show_menu:
	# store the register S before .
	addi sp,sp,-12
	sw s1,0(sp)
	sw s2,4(sp)
	sw s3,8(sp)
	# prepare some constants and variable.
	li s2,1
	li s1,0
	mv s3,ra
	# show string "Easy" #
	li a0,36
	li a1,50
	la a2,normal
	li a3,0xF800
	# jump to the function- showstring(x,y,str,color) #
	jal LCD_ShowString
	li a0,92
	# show string "Hard" #
	li a1,50
	la a2,endless
	li a3,0xF800
	# jump to the function showstring(x,y,str,color) #
	jal LCD_ShowString
	# Show big Point under "Easy"  #
	li a0,52
	li a1,72
	li a2,5
	li a3,0xF800
	# jump to the function DrawPoint_big(x,y,color) #
	jal LCD_DrawSpot
	# store where the point at. #
	# show character
	li a0,30
	li a1,20
	#show every 
	la a2,title0
	li a3,0xEE62
	jal LCD_ShowString
	# wait 0.5s
	li a0,500
	jal delay_1ms
	# show character
	li a0,46
	li a1,20
	#show every 
	la a2,title1
	li a3,0xEDC5
	jal LCD_ShowString
	# wait 0.5s
	li a0,500
	jal delay_1ms
	# show character
	li a0,62
	li a1,20
	#show every 
	la a2,title2
	li a3,0xDCAB
	jal LCD_ShowString
	# wait 0.5s
	li a0,500
	jal delay_1ms
	# show character
	li a0,78
	li a1,20
	#show every 
	la a2,title3
	li a3,0xD3EF
	jal LCD_ShowString
	# wait 0.5s
	li a0,500
	jal delay_1ms
	# show character
	li a0,96
	li a1,20
	#show every 
	la a2,title4
	li a3,0xCB13
	jal LCD_ShowString
	# wait 0.5s
	li a0,500
	jal delay_1ms
	# show character
	li a0,110
	li a1,20
	#show every 
	la a2,title5
	li a3,0xC237
	jal LCD_ShowString
	# wait 0.5s
	li a0,500
	jal delay_1ms
	# show character
	li a0,126
	li a1,20
	#show every 
	la a2,title6
	li a3,0xB0DE
	jal LCD_ShowString
	# wait 0.5s
	li a0,500
	jal delay_1ms
Check:
	# delay 200ms for button(0) #
	li a0,200
	jal delay_1ms
	# check  if  the button(0) pressed #
	li a0,0
	jal Get_Button
	# operation :if pressed, swap the point/mode #
	beq a0,s2,Swap
	jal Get_BOOT0
	beq a0,s2,Return_Back
	j Check
Swap:
	beq s1,s2,Left
	beq s1,x0,Right
Left:
	li s1,0
	# Show big Point under "Easy"  #
	li a0,52
	li a1,72
	li a2,5
	li a3,0xF800
	# jump to the function DrawPoint_big(x,y,color) #
	jal LCD_DrawSpot
	# eliminate big Point under "Hard"  #
	li a0,108
	li a1,72
	li a2,5
	li a3,0x6D0C # bkg color
	# jump to the function DrawPoint_big(x,y,color) #
	jal LCD_DrawSpot
	j Check
Right:
	li s1,1
	# eliminate big Point under "Easy"  #
	li a0,108
	li a1,72
	li a2,5
	li a3,0xF800
	# jump to the function DrawPoint_big(x,y,color) #
	jal LCD_DrawSpot
	# Show big Point under "Easy"  #
	li a0,52
	li a1,72
	li a2,5
	li a3,0x6D0C # bkg color
	# jump to the function DrawPoint_big(x,y,color) #
	jal LCD_DrawSpot
	j Check
Return_Back:
	mv ra,s3
	mv a0,s1
	lw s1,0(sp)
	lw s2,4(sp)
	lw s3,8(sp)
	addi sp,sp,12
	jr ra