#include<stdio.h>
int main(){
    unsigned int a,R,G,B,b=0,k;
    int n;
    printf("mode? 1/HEX,2/RGB\n");
    scanf("%d",&k);
    printf("number?\n");
    scanf("%d",&n);
    for(int i=0;i<n;i++){
    b=0;
    if(k==2){
    scanf("%d %d %d",&R,&G,&B);
    a=(R<<16)+G<<8+B;}
    if(k==1)scanf("%x",&a);
    b=((a&(0xF8<<16))>>8)|b;
    b=b|((a&0xFC<<8)>>5);
    b=b|((a&0xF8)>>3);
    printf("RGB565=%x\n",b);
    }
}