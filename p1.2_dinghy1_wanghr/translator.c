/* this C file provides a better illustration of translator.S */

#include<stdlib.h>
#include<stdio.h>

int lines_of_rvc_codes=7;
int rvc_codes[]={
	0b00000000000000000000001010110011,
	0b0001010111111101,
	0b1001001010101010,
	0b0001010111111101,
	0b11111110000001011101111011100011,
	0b1000010100010110,
	0b1000000010000010,
};
int read_instr(int*,int*);
void print_instr(int a1);
int main(){
	int s1=lines_of_rvc_codes;
	int *s2=rvc_codes;
	int a1;
	int a2=0; /* the address of next instruction relative to s2 */
	for(int i=0;i<s1;i++){
		a2+=read_instr((int*)((char*)s2+a2),&a1);
		print_instr(a1);
	}

	/* Exit with error code 0 */
	return 0;
}
/* read a 16/32-bit instruction from a1 and store it in a1, return the length of the instruction(in bytes) */
int read_instr(int *addr,int *a1){
	*a1=*addr;
	if((*a1&0b11)!=0b11){ /* 16-bit */
		printf("16bit\n");
		*a1&=0xffff;
		return 4; /* should be 2 in .S */
	}else{ /* 32-bit */
		printf("32bit\n");
		return 4;
	}
}
/* print instruction a1 */
void print_instr(int a1){
	unsigned int a2=0x80000000;
	while(a2!=0){
		if((a1&a2)==0)putchar('0');
		else putchar('1');
		a2>>=1;
	}
	putchar('\n');
	return;
}