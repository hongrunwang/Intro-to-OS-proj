#include<stdio.h>
unsigned char image[25600];
int main(int argc,char* argv[]){
	char* path;
	if(argc<2){
		path="bmp.bin";
	}else{
		path=argv[1];
	}
	FILE *f1;int size1;
	f1=fopen(path,"rb");
	fseek(f1,0,SEEK_END);
	size1=ftell(f1);
	fseek(f1,0,SEEK_SET);
	printf("unsigned char image[%d]={",size1);
	fread(image,sizeof(unsigned char),size1,f1);
	for(int i=0;i<size1;i++){
		if(i%8==0)printf("\n    ");
		printf("0x%02x,",image[i]);
	}
	printf("\n};\n");
	fclose(f1);
	return 0;
}