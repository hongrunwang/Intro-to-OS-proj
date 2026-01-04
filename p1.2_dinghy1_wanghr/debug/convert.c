/* converts hexadecimal numbers to 32-bit binary numbers */
#include<stdio.h>
#define N 200
int a[N];
void print_bin(int x){
	unsigned int t=0x80000000;
	for(int i=0;i<32;i++){
		if(x&t)putchar('1');
		else putchar('0');
		t>>=1;
	}
	putchar('\n');
}
int main(){
	int n=0;
	while(scanf("%x",a+n)!=EOF&&n<N){
		n++;
	}
	for(int i=0;i<n;i++){
		print_bin(a[i]);
	}
}