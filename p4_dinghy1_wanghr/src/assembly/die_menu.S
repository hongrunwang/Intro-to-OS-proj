.data
str_again: .string "Again"
str_exit: .string "Exit"
str_score0: .string "S"
str_score1: .string "c"
str_score2: .string "o"
str_score3: .string "r"
str_score4: .string "e"
.section .text
.globl die_menu
.type die_menu, @function
die_menu:
	addi sp,sp,-12
	sw s1,0(sp)
	sw s2,4(sp)
	sw ra,8(sp)
	# show score number
	mv a2,a0
	li a0,56
	li a1,32
	li a3,6
	li a4,0xF800 # color RED
	# LCD_ShowNum(56,32,score,6,RED)
	jal LCD_ShowNum
	# show string "Score"
	li a0,50
	li a1,16
	la a2,str_score0
	li a3,0x3e37 # color RED
	# showstring(60,16,"Score",0xF800)
	jal LCD_ShowString
		# show string "Score"
	li a0,66
	li a1,16
	la a2,str_score1
	li a3,0xE800 # color RED
	# showstring(60,16,"Score",0xF800)
	jal LCD_ShowString
		# show string "Score"
	li a0,82
	li a1,16
	la a2,str_score2
	li a3,0x07F9 # color RED
	# showstring(60,16,"Score",0xF800)
	jal LCD_ShowString
		# show string "Score"
	li a0,98
	li a1,16
	la a2,str_score3
	li a3,0xFFE0 # color RED
	# showstring(60,16,"Score",0xF800)
	jal LCD_ShowString
		# show string "Score"
	li a0,114
	li a1,16
	la a2,str_score4
	li a3,0x032C # color RED
	# showstring(60,16,"Score",0xF800)
	jal LCD_ShowString

	# s1 stores the status of choice
	li s1,0 # 0-again 1-exit
	li s2,1 # const int s2=1;
	# show option "Again"
	li a0,32 # "Again" is longer, so adjust -4
	li a1,50
	la a2,str_again
	li a3,0xF800 # color RED
	# showstring(36,50,"Again",0xF800)
	jal LCD_ShowString
	# show option "Exit"
	li a0,92
	li a1,50
	la a2,str_exit
	li a3,0xF800 # color RED
	# showstring(92,50,"Exit",0xF800)
	jal LCD_ShowString
	# show big Point under "Again"
	li a0,52
	li a1,72
	li a2,5 # radius=5
	li a3,0xF800 # color RED
	# LCD_DrawSpot(52,72,5,0xF800)
	jal LCD_DrawSpot
listen: # loop for listening button input
	# delay_1ms(200);
	li a0,200
	jal delay_1ms
	# a0=Get_Button(0);
	li a0,0
	jal Get_Button
	beq a0,s2,toggle
	# a0=Get_Boot0();
	jal Get_BOOT0
	beq a0,s2,die_menu_return
	j listen
toggle:
	xori s1,s1,1
	beq s1,x0,tog_left # switch to choose "Again"
	j tog_right # switch to choose "Exit"
tog_left:
	# show big Point under "Again"
	li a0,52
	li a1,72
	li a2,5 # radius=5
	li a3,0xF800 # color RED
	# LCD_DrawSpot(52,72,5,0xF800)
	jal LCD_DrawSpot
	# erase big Point under "Exit"
	li a0,108
	li a1,72
	li a2,5 # radius=5
	li a3,0xBDD8 # bkg color
	# LCD_DrawSpot(108,72,5,0xD218)
	jal LCD_DrawSpot
	j listen
tog_right:
	# show big Point under "Exit"
	li a0,108
	li a1,72
	li a2,5 # radius=5
	li a3,0xF800 # color RED
	# LCD_DrawSpot(108,72,5,0xF800)
	jal LCD_DrawSpot
	# erase big Point under "Again"
	li a0,52
	li a1,72
	li a2,5 # radius=5
	li a3,0xBDD8 # bkg color
	# LCD_DrawSpot(52,72,5,0xD218)
	jal LCD_DrawSpot
	j listen
die_menu_return:
	mv a0,s1 # a0=s1 (to be returned)
	lw s1,0(sp)
	lw s2,4(sp)
	lw ra,8(sp)
	addi sp,sp,12
	jr ra