#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// #include <algorithm>
#include <sys/time.h>
#include <time.h>
#include <immintrin.h>
#include <omp.h>
//inplement dymanic

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define PI 3.14159

typedef struct FVec
{
	unsigned int length;
	unsigned int min_length;
	unsigned int min_deta;
	float* data;
	float* sum;
	float* vals;
} FVec;

typedef struct Image
{
	unsigned int dimX, dimY, numChannels;
	float* data;
} Image;

void normalize_FVec(FVec v)
{
	// float sum = 0.0;
	unsigned int i,j;
	int ext = v.length / 2;
	v.sum[0] = v.data[ext];
	for (i = ext+1,j=1; i < v.length; i++,j++)
	{
		v.sum[j] = v.sum[j-1] + v.data[i]*2;
	}
	// for (i = 0; i <= ext; i++)
	// {
	//		v.data[i] /= v.sum[v.length - ext - 1 ] ;
	//		printf("%lf ",v.sum[i]);
	// }
}

float* get_pixel(Image img, int x, int y)
{
	if (x < 0)
	{
		x = 0;
	}
	if (x >= img.dimX)
	{
		x = img.dimX - 1;
	}
	if (y < 0)
	{
		y = 0;
	}
	if (y >= img.dimY)
	{
		y = img.dimY - 1;
	}
	return img.data + img.numChannels * (y * img.dimX + x);
}

float gd(float a, float b, float x)
{
	float c = (x-b) / a;
	return exp((-.5) * c * c) / (a * sqrt(2 * PI));
}

FVec make_gv(float a, float x0, float x1, unsigned int length, unsigned int min_length)
{
	FVec v;
	v.length = length;
	v.min_length = min_length;
	if(v.min_length > v.length){
		v.min_deta = 0;
	}else{
		v.min_deta = ((v.length - v.min_length) / 2);
	}
	v.data = malloc(length * sizeof(float));
	v.sum = malloc((length / 2 + 1)* sizeof(float));
	v.vals= malloc(sizeof(float)*length*(length/2+1));
	float step = (x1 - x0) / ((float)length);
	int offset = length/2;

	for (int i = 0; i < length; i++)
	{
		v.data[i] = gd(a, 0.0f, (i-offset)*step);
	}
	normalize_FVec(v);
	for(int i=0;i<length;i++){
		for(int j=0;j<length/2+1;j++){
			v.vals[j*length+i]=v.data[i]/v.sum[j];
		}
	}
	return v;
}

void print_fvec(FVec v)
{
	unsigned int i;
	printf("\n");
	for (i = 0; i < v.length; i++)
	{
		printf("%f ", v.data[i]);
	}
	printf("\n");
}

Image img_sc(Image a)
{
	Image b = a;
	b.data = malloc(b.dimX * b.dimY * b.numChannels * sizeof(float));
	return b;
}

Image gb_h(Image a, FVec gv)
{
	// struct timeval start_time, stop_time, elapsed_time; 
	// gettimeofday(&start_time,NULL);
	Image b = img_sc(a);
	int ext = gv.length / 2;
	unsigned int x, y;
	#pragma omp parallel for 
	for (y = 0; y < a.dimY; y++){
		float *pc,*po1,*po2,*po11,*po22,*po3,*po33,*po4,*po44;
		float t,t2,t3,t4;
		int w;
		__m256 sum,tmp,tmp2,tmp1,tmp11,tmp3,tmp33,tmp4,tmp44;
		for (x = 0; x < a.dimX; x++){
			pc = get_pixel(b, x, y);
			unsigned int deta = fmin(fmin(a.dimY-y-1, y),fmin(a.dimX-x-1, x));
			deta = fmin(deta, gv.min_deta);
			//t=gv.data[ext]/gv.sum[ext-deta];
			w=(ext-deta)*gv.length;
			t=gv.vals[w+ext];
			po1=get_pixel(a,x,y);
			sum=_mm256_set_ps(0,0,0,0,0,po1[2]*t,po1[1]*t,po1[0]*t);

			for (int i = deta; i < (ext-deta)/4*4+deta; i+=4){
				po1=get_pixel(a,x+i-ext,y);
				po2=get_pixel(a,x+gv.length-1-i-ext,y);
				//t=gv.data[i]/gv.sum[ext-deta];
				t=gv.vals[w+i];
				tmp2=_mm256_set_ps(t,t,t,t,t,t,t,t);
				tmp=_mm256_set_ps(0,po2[2],po2[1],po2[0],0,po1[2],po1[1],po1[0]);
				tmp=_mm256_mul_ps(tmp,tmp2);
				sum=_mm256_add_ps(sum,tmp);
			
				po11=get_pixel(a,x+i+1-ext,y);
				po22=get_pixel(a,x+gv.length-2-i-ext,y);
				//t=gv.data[i]/gv.sum[ext-deta];
				t2=gv.vals[w+i+1];
				tmp11=_mm256_set_ps(t2,t2,t2,t2,t2,t2,t2,t2);
				tmp1=_mm256_set_ps(0,po22[2],po22[1],po22[0],0,po11[2],po11[1],po11[0]);
				tmp1=_mm256_mul_ps(tmp1,tmp11);
				sum=_mm256_add_ps(sum,tmp1);
			
				po3=get_pixel(a,x+i+2-ext,y);
				po33=get_pixel(a,x+gv.length-3-i-ext,y);
				//t=gv.data[i]/gv.sum[ext-deta];
				t3=gv.vals[w+i+2];
				tmp33=_mm256_set_ps(t3,t3,t3,t3,t3,t3,t3,t3);
				tmp3=_mm256_set_ps(0,po33[2],po33[1],po33[0],0,po3[2],po3[1],po3[0]);
				tmp3=_mm256_mul_ps(tmp3,tmp33);
				sum=_mm256_add_ps(sum,tmp3);

				po4=get_pixel(a,x+i+3-ext,y);
				po44=get_pixel(a,x+gv.length-4-i-ext,y);
				//t=gv.data[i]/gv.sum[ext-deta];
				t4=gv.vals[w+i+3];
				tmp44=_mm256_set_ps(t4,t4,t4,t4,t4,t4,t4,t4);
				tmp4=_mm256_set_ps(0,po44[2],po44[1],po44[0],0,po4[2],po4[1],po4[0]);
				tmp4=_mm256_mul_ps(tmp4,tmp44);
				sum=_mm256_add_ps(sum,tmp4);
			}

			for (int i = (ext-deta)/4*4+deta; i < ext; i++){
				po1=get_pixel(a,x+i-ext,y);
				po2=get_pixel(a,x+gv.length-1-i-ext,y);
				//t=gv.data[i]/gv.sum[ext-deta];
				t=gv.vals[w+i];
				tmp2=_mm256_set_ps(t,t,t,t,t,t,t,t);
				tmp=_mm256_set_ps(0,po2[2],po2[1],po2[0],0,po1[2],po1[1],po1[0]);
				tmp=_mm256_mul_ps(tmp,tmp2);
				sum=_mm256_add_ps(sum,tmp);
			}
			pc[0]=*((float*)&sum)+*((float*)&sum+4);
			pc[1]=*((float*)&sum+1)+*((float*)&sum+5);
			pc[2]=*((float*)&sum+2)+*((float*)&sum+6);
		}
	}
	// gettimeofday(&stop_time,NULL);
	// timersub(&stop_time, &start_time, &elapsed_time);
	// printf("%f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
	return b;
}

// unused function
Image gb_v(Image a, FVec gv)
{
	// struct timeval start_time, stop_time, elapsed_time; 
	// gettimeofday(&start_time,NULL);
	Image b = img_sc(a);
	int ext = gv.length / 2;
	unsigned int x, y;
	#pragma omp parallel for 
	for (x = 0; x < a.dimX; x++){
		float *pc,*po1,*po2;
		float t;
		__m256 sum,tmp;
		int w;
		for (y = 0; y < a.dimY; y++){
			pc = get_pixel(b, x, y);
			unsigned int deta = fmin(fmin(a.dimY-y-1, y),fmin(a.dimX-x-1, x));
			deta = fmin(deta, gv.min_deta);
			//t=gv.data[ext]/gv.sum[ext-deta];
			w=(ext-deta)*gv.length;
			t=gv.vals[w+ext];
			po1=get_pixel(a,x,y);
			pc[0]=po1[0]*t;
			pc[1]=po1[1]*t;
			pc[2]=po1[2]*t;
			sum=_mm256_setzero_ps(); // set all channels to 0
			for (int i = deta; i < ext; i++){
				po1=get_pixel(a,x,y+i-ext);
				po2=get_pixel(a,x,y+gv.length-1-i-ext);
				//t=gv.data[i]/gv.sum[ext-deta];
				t=gv.vals[w+i];
				tmp=_mm256_set_ps(0,po2[2]*t,po2[1]*t,po2[0]*t,0,po1[2]*t,po1[1]*t,po1[0]*t);
				sum=_mm256_add_ps(sum,tmp);
			}
			pc[0]+=*((float*)&sum)+*((float*)&sum+4);
			pc[1]+=*((float*)&sum+1)+*((float*)&sum+5);
			pc[2]+=*((float*)&sum+2)+*((float*)&sum+6);
		}
	}
	// gettimeofday(&stop_time,NULL);
	// timersub(&stop_time, &start_time, &elapsed_time);
	// printf("%f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
	return b;
}

Image apply_gb(Image a, FVec gv)
{
	Image b = gb_h(a, gv);
	unsigned int x,y,w=a.dimX,h=a.dimY,i,j;
	a.dimX=h;a.dimY=w;
	for(y=0;y<h;y++){
		for (x=0;x<w;x++){
			i=(x*h+y)*3;j=(y*w+x)*3;
			a.data[i]=b.data[j];
			a.data[i+1]=b.data[j+1];
			a.data[i+2]=b.data[j+2];
		}
	}
	// output mid-result
	// stbi_write_jpg("tmp.jpg",a.dimX,a.dimY,a.numChannels,a.data,90);
	Image c = gb_h(a, gv);
	for(y=0;y<h;y++){
		for (x=0;x<w;x++){
			i=(x*h+y)*3;j=(y*w+x)*3;
			b.data[j]=c.data[i];
			b.data[j+1]=c.data[i+1];
			b.data[j+2]=c.data[i+2];
		}
	}
	free(c.data);
	return b;
}

/* original
Image apply_gb(Image a, FVec gv)
{
    Image b = gb_h(a, gv);
    Image c = gb_v(b, gv);
    free(b.data);
    return c;
}*/

int main(int argc, char** argv)
{
	struct timeval start_time, stop_time, elapsed_time; 
	gettimeofday(&start_time,NULL);
	if (argc < 6)
	{
		printf("Usage: ./gb.exe <inputjpg> <outputname> <float: a> <float: x0> <float: x1> <unsigned int: dim>\n");
		exit(0);
	}
	omp_set_num_threads(20);
	float a, x0, x1;
	unsigned int dim, min_dim;

	sscanf(argv[3], "%f", &a);
	sscanf(argv[4], "%f", &x0);
	sscanf(argv[5], "%f", &x1);
	sscanf(argv[6], "%u", &dim);
	sscanf(argv[7], "%u", &min_dim);
	
	FVec v = make_gv(a, x0, x1, dim, min_dim);
	// print_fvec(v);
	Image img;
	img.data = stbi_loadf(argv[1], &(img.dimX), &(img.dimY), &(img.numChannels), 0);

	Image imgOut = apply_gb(img, v);
	stbi_write_jpg(argv[2], imgOut.dimX, imgOut.dimY, imgOut.numChannels, imgOut.data, 90);
	gettimeofday(&stop_time,NULL);
	timersub(&stop_time, &start_time, &elapsed_time); 
	printf("%f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
	free(img.data);
	free(imgOut.data);
	free(v.data);
	free(v.sum);
	free(v.vals);
	return 0;
}