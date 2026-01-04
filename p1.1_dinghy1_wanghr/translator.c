/*  Project 1.1: RISC-V instructions to RISC-V compressed instructions in C89.
    The following is the starter code provided for you. To finish the task, you 
    should define and implement your own functions in translator.c, compression.c, 
    utils.c and their header files.
    Please read the problem description before you start.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "src/compression.h"
#include "src/utils.h"

#include "translator.h"

#define INFO_INIT_SIZE 64
#define INFO_GROW_FACTOR 2

/* check if file can be correctly opened */
static int open_files(FILE** input, FILE** output, const char* input_name, const char* output_name){ 
    *input = fopen(input_name, "r");
    if (!*input){ /* open input file failed */
        printf("Error: unable to open input file: %s\n", input_name);
        return -1;
    }

    *output = fopen(output_name, "w");
    if (!*output){ /* open output file failed */
        printf("Error: unable to open output file: %s\n", output_name);
        fclose(*input);
        return -1;
    }
    return 0; /* no problem opening files */
}

static int close_files(FILE** input, FILE** output){
    fclose(*input);
    fclose(*output); /* close the files at the end */
    return 0;
}

static void print_usage_and_exit() {
    printf("Usage:\n");
    printf("Run program with translator <input file> <output file>\n"); /* print the correct usage of the program */
    exit(0);
}


/* Run the translator */
int translate(const char*in, const char*out){
    FILE *input, *output;
    int err = 0;
	int info_cnt = 0, i, j; /* count number of instructions */
	int info_size = INFO_INIT_SIZE; /* current size of infos */
	int *compressible, *compressible_sum; /* whether each instruction is compressible & the prefix sum of it */
	RV32I instr;
	RV32I_info* infos=malloc(INFO_INIT_SIZE*sizeof(RV32I_info)); /* need dynamic length because unknown total number of instructions */
	
    if (in){    /* correct input file name */
        if(open_files(&input, &output, in, out) != 0)
            exit(1);
		/* read and compress the instructions */
		while(read_instr(input, &instr)!=EOF){
			err = get_info(infos+info_cnt, &instr);
			if(err)break;
			info_cnt += 1; /* index for next instruction */
			if(info_size<info_cnt){ /* need more space */
				info_size*=INFO_GROW_FACTOR; /* doubling the size(by default)  */
				infos=realloc(infos,info_size*sizeof(RV32I_info));
				if(infos==NULL){
					err = -1;
					break;
				}
			}
		}
		
		/* determine if each instruction is compressible  */
		compressible=malloc(info_cnt*sizeof(int));
		compressible_sum=malloc(info_cnt*sizeof(int)); /* compressible_sum[i] is summation(compressible[0]~compressible[i-1]) */
		compressible_sum[0]=0;
		for(i=0;i<info_cnt;i++){
			compressible[i]=is_compressible(infos+i);
			if(i+1<info_cnt){
				/* compressible[info_cnt] would never be used */
				compressible_sum[i+1]=compressible_sum[i]+compressible[i];
			}
		}
		
		/* adjust the offset of branches and jumps */
		for(i=0;i<info_cnt;i++){
			if(infos[i].maintype==RV32I_SB||infos[i].subtype==RV32I_jal||infos[i].subtype==RV32I_jalr){
				j=i+infos[i].imm/4;
				if(j<0||j>=info_cnt){ /* invalid jump offset, destination outside the code */
					err=-1;
					break;
				}
				/* actually the folling 2 lines can be combined */
				if(i>j)infos[i].imm+=(compressible_sum[i]-compressible_sum[j])*2; /* compressed[j~i-1]*2 less absolute offset */
				else infos[i].imm-=(compressible_sum[j]-compressible_sum[i])*2; /* compressed[i~j-1]*2 less absolute offset */
			}
		}
		
		/* print the compressed instructions */
		if(!err){
			for(i=0;i<info_cnt;i++){
				/* only for debugging 
				err = print_info(infos+i); 
				if(err)break; */
				err = compress_instr(&instr, infos+i);
				if(err)break;
				/* print the compressed instruction to output file */
				err = print_instr(output, instr);
				if(err)break;
				if(fputc('\n',output)==EOF){ /* new line for next instruction */
					err = EOF;
					break;
				}
			}
		}
		
		/* free the allocated space */
		free(infos);
		free(compressible);
		free(compressible_sum);
		/* safely close the files */
        close_files(&input, &output);
    }
    return err;
}

/* main func */
int main(int argc, char **argv){
    char* input_fname, *output_fname;
    int err;

    if (argc != 3) /* need correct arguments */
        print_usage_and_exit();

    input_fname = argv[1];
    output_fname = argv[2];

    err = translate(input_fname, output_fname); /* main translation process */
    if (err)
        printf("One or more errors encountered during translation operation.\n"); /* something wrong */
    else
        printf("Translation process completed successfully.\n"); /* correctly output */

    return 0;
}