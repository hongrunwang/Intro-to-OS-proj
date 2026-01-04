#include "lcd/lcd.h"
#include "fatfs/tf_card.h"
#include "utils.h"
#include "res.h" // art resource hard-code
#include <stdlib.h>

void Inp_init(void)
{
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
}

void Adc_init(void) 
{
	rcu_periph_clock_enable(RCU_GPIOA);
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0|GPIO_PIN_1);
	RCU_CFG0|=(0b10<<14)|(1<<28);
	rcu_periph_clock_enable(RCU_ADC0);
	ADC_CTL1(ADC0)|=ADC_CTL1_ADCON;
}

void IO_init(void)
{
	Inp_init(); // inport init
	Adc_init(); // A/D init
	Lcd_Init(); // LCD init
}

unsigned char image[12800];
FATFS fs; // file system

int fatfs_init(){
	FRESULT fr; // FATFS return
	fr=f_mount(&fs,"",1);
	if(fr!=FR_OK){
		// LCD_ShowString(24,  0, (u8 *)("no card found!"), BLACK);
		LCD_ShowString(24, 16, (u8 *)("no card found!"), BLUE);
		LCD_ShowString(24, 32, (u8 *)("no card found!"), BRED);
		LCD_ShowString(24, 48, (u8 *)("no card found!"), GBLUE);
		LCD_ShowString(24, 64, (u8 *)("no card found!"), RED);
		return -1;
	}
	return 0;
}

// load a image or video file onto screen
int show_video(const char* path){
	FRESULT fr; // FATFS return
	FIL fil; // FATFS file
	UINT br; // number of bytes read
	int offset = 0; // current read position
	fr = f_open(&fil, path, FA_READ);
	if(fr!=FR_OK) return -1;

	int nframes=f_size(&fil)/25600;
	for(int i=0;i<nframes;i++){
		f_lseek(&fil, offset);
		fr = f_read(&fil, image, sizeof(image), &br);
		LCD_ShowPicture(0,0,159,39);
		offset += 12800;
		f_lseek(&fil, offset);
		fr = f_read(&fil, image, sizeof(image), &br);
		LCD_ShowPicture(0,40,159,79);
		offset += 12800;
	}
	f_close(&fil);
	return 0;
}

int load_image(const char* path,int nbr){
	FRESULT fr; // FATFS return
	FIL fil; // FATFS file
	UINT br; // number of bytes read
	fr = f_open(&fil, path, FA_READ);
	if(fr!=FR_OK) return -1;
	fr = f_read(&fil, image, nbr, &br);
	f_close(&fil);
	return 0;
}

int show_menu(); // RISC-V function: return 0-easy 1-hard
int start_game();
int die_menu(int score); // RISC-V function: return 0-again 1-exit
// RISC-V function: show an obstacle and cut the out-screen part
// (x,y) is the left-upper corner; id: 0-flower 1-stone 2-onetree 3-threetree
void show_obj(int x,int y,int id,const u8* img);
inline void gen_ob(int* ob,int* w,int* h,const u8** img); // utility function for generating obstacle

int main(void)
{
	IO_init();		   // init OLED
	BACK_COLOR=BLACK;
	LCD_Clear(BACK_COLOR);

	if(fatfs_init()){ // fatfs initialization failed
		LCD_ShowString(0, 0, (u8 *)("fatfs_init() failed"), RED);
		return 0;
	}
	if(show_video("logo.bin")){ // picture logo also seen as video
		LCD_ShowString(0, 0, (u8 *)("show_video() failed"), RED);
		return 0;
	}
	delay_1ms(1000);

	int score;
	while(1){
		// show menu and select game mode
		if(show_video("menu.bin")){ // show menu background
			LCD_ShowString(0, 0, (u8 *)("show_video() failed"), RED);
			return 0;
		}
		if(show_menu()){ // mode==1-hard
			score=start_game(100,5);
		}else{ // mode==0-easy
			score=start_game(150,5);
		}
		if(show_video("diemenu.bin")){ // show menu backgroundmenu
			LCD_ShowString(0, 0, (u8 *)("show_video() failed"), RED);
			return 0;
		}
		// show die_menu and decide whether to restart
		if(die_menu(score))break; // 1-exit
	}

	LCD_Clear(BACK_COLOR);
	return 0;
}

int start_game(int itv,int spd){ // the interval between frames (in ms) and the speed of obstacles (in px)
	int man_pos=1; // 0-left 1-middle 2-right
	int man_picn=0; // 0-man1 1-man2 2-man1 3-man3
	const u8* man_pic=man1;
	int obs[3]={-1,-1,-1},obx[3],oby[3],obw[3],obh[3];
	const u8* obimg[3];
	int cnt=0,score=0; // count frames
	while(++cnt){ // main loop of game
		// refreshing background
		if(show_video("bkg1.bin")){
			LCD_ShowString(0, 0, (u8 *)("show_video() failed"), RED);
			return -1;
		}
		
		// controlling man
		if(man_pos<2&&Get_Button(1)){
			// erase original man
			// LCD_Fill(30+man_pos*50-8,55,30+man_pos*50+7,74,0x7614); // bkg color #70c0a0
			man_pos++;
		}else if(man_pos>0&&Get_Button(0)){
			// LCD_Fill(30+man_pos*50-8,55,30+man_pos*50+7,74,0x7614);
			man_pos--;
		}
		if(man_picn==0){ // running state switch
			man_pic=man1;
		}else if(man_picn==1){
			man_pic=man2;
		}else if(man_picn==2){
			man_pic=man1;
		}else{ // man_picn==3
			man_pic=man3;
		}
		man_picn++;
		if(man_picn==4)man_picn=0;
		LCD_ShowPic(30+man_pos*50-8,55,30+man_pos*50+7,74,man_pic);
		
		// generate and update obstacles
		for(int k=0;k<3;k++){
			// generate
			if(obs[k]==-1){
				gen_ob(obs+k,obw+k,obh+k,obimg+k);
				if(obs[k]!=-1){
					obx[k]=30+k*50-obw[k]/2;oby[k]=-60;
				}
			}
			// update (move downwards)
			if(obs[k]!=-1){
				oby[k]+=spd;
				if(k==man_pos&&oby[k]+obh[k]>65&&oby[k]<60){ // crashed
					if(obs[k]==0){ // add score
						score+=100;
						LCD_ShowString(56, 4, (u8 *)("+100"), YELLOW);
						obs[k]=-1;
					}else return score; // lose
				}
				if(oby[k]>=80)obs[k]=-1;
			}
			// show
			if(obs[k]!=-1)show_obj(obx[k],oby[k],obs[k],obimg[k]);
		}
		
		// show the score board
		// LCD_ShowString(30, 4, (u8 *)("Score : "), RED);
		LCD_ShowNum(88, 4, score, 8, RED); // at most show 8 numbers
		
		delay_1ms(itv);
		score+=1;
	}
	return score;
}

void gen_ob(int* ob,int* w,int* h,const u8** img){
	if(rand()%25)return; // 1/25 probability to generate
	*ob=rand()%4;
	if(*ob==0){
		*w=24;*h=18;*img=flower;
	}else if(*ob==1){
		*w=20;*h=20;*img=stone;
	}else if(*ob==2){
		*w=20;*h=30;*img=onetree;
	}else{ // *ob==3
		*w=20;*h=60;*img=threetree;
	}
}