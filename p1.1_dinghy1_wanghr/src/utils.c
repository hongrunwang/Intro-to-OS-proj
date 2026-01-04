#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

int read_instr(FILE *input, RV32I* in){
	int x,n=0; /* n records number of bytes already read */
	if(input==NULL||in==NULL)return EOF;
	if((x=fgetc(input))==EOF)return EOF; /* normally reach end of file */
	while(x!='0'&&x!='1')
		if((x=fgetc(input))==EOF)return EOF; /* normally reach end of file */
	while(1){
		if(x=='0')(*in)&=0xfffffffe; /* set low bit to 0 */
		else if(x=='1')(*in)|=0x00000001; /* set low bit to 1 */
		else return EOF; /* unexpected char rather than '0' or '1' */
		n++; /* number of bytes already read +=1 */
		if(n==32)break; /* all 32 bits done */
		(*in)<<=1; /* get ready for next bit */
		if((x=fgetc(input))==EOF)return EOF;
	}
	return 0;
}

int print_instr(FILE *output, RV32I out){
	int x,n=0; /* n records number of bytes already written */
	if(output==NULL)return EOF;
	while(1){
		if(out&0x80000000)x='1'; /* print the highest bit */
		else x='0';
		if(fputc(x,output)==EOF)return EOF;
		n++; /* number of bytes already written +=1 */
		out<<=1; /* get ready for next bit */
		if(n==16&&out==0)break; /* surpport for RV32C */
		if(n==32)break; /* all 32 bits done */
	}
	return 0;
}

int get_info(RV32I_info* info, const RV32I* instr){
	if(info==NULL||instr==NULL)return -1;
	*(RV32I*)info=*instr; /* the first 32 bits of RV32I_info and RV32I have the same meaning */
	info->RVC_types=-1; /* mark RVC_types as undefined */
	/* determine type of the RV32I instruction */
	switch(info->opcode){
		case 0x33: /* opcode==0110011 */
			info->maintype=RV32I_R;
			switch(info->func3){
				case 0x0: /* func3==0x0 */
					if(info->func7==0x00)info->subtype=RV32I_add;
					else if(info->func7==0x20)info->subtype=RV32I_sub;
					else info->subtype=-1;
					break;
				case 0x4: /* func3==0x4 */
					if(info->func7==0x00)info->subtype=RV32I_xor;
					else info->subtype=-1;
					break;
				case 0x6: /* func3==0x6 */
					if(info->func7==0x00)info->subtype=RV32I_or;
					else info->subtype=-1;
					break;
				case 0x7: /* func3==0x7 */
					if(info->func7==0x00)info->subtype=RV32I_and;
					else info->subtype=-1;
					break;
				case 0x1: /* func3==0x1 */
					if(info->func7==0x00)info->subtype=RV32I_sll;
					else info->subtype=-1;
					break;
				case 0x5: /* func3==0x5 */
					if(info->func7==0x00)info->subtype=RV32I_srl;
					else if(info->func7==0x20)info->subtype=RV32I_sra;
					else info->subtype=-1;
					break;
				case 0x2: /* func3==0x2 */
					if(info->func7==0x00)info->subtype=RV32I_slt;
					else info->subtype=-1;
					break;
				case 0x3: /* func3==0x3 */
					if(info->func7==0x00)info->subtype=RV32I_sltu;
					else info->subtype=-1;
					break;
				default:
					/* func3 invalid */
					info->subtype=-1;
			}
			break;
		case 0x13: /* opcode==0010011 */
			info->maintype=RV32I_I;
			switch(info->func3){
				case 0x0: /* func3==0x0 */
					info->subtype=RV32I_addi;
					break;
				case 0x4: /* func3==0x4 */
					info->subtype=RV32I_xori;
					break;
				case 0x6: /* func3==0x6 */
					info->subtype=RV32I_ori;
					break;
				case 0x7: /* func3==0x7 */
					info->subtype=RV32I_andi;
					break;
				/* slli, srli, srai only need 5 bits of imm */
				case 0x1: /* func3==0x1 */
					if(info->func7==0x00)info->subtype=RV32I_slli;
					else info->subtype=-1;
					break;
				case 0x5: /* func3==0x5 */
					if(info->func7==0x00)info->subtype=RV32I_srli;
					else if(info->func7==0x20)info->subtype=RV32I_srai;
					else info->subtype=-1;
					break;
				case 0x2: /* func3==0x2 */
					info->subtype=RV32I_slti;
					break;
				case 0x3: /* func3==0x3 */
					info->subtype=RV32I_sltiu;
					break;
				default:
					/* func3 invalid */
					info->subtype=-1;
			}
			break;
		case 0x03: /* opcode==0000011 */
			info->maintype=RV32I_I;
			switch(info->func3){
				case 0x0: /* func3==0x0 */
					info->subtype=RV32I_lb;
					break;
				case 0x1: /* func3==0x1 */
					info->subtype=RV32I_lh;
					break;
				case 0x2: /* func3==0x2 */
					info->subtype=RV32I_lw;
					break;
				case 0x4: /* func3==0x4 */
					info->subtype=RV32I_lbu;
					break;
				case 0x5: /* func3==0x5 */
					info->subtype=RV32I_lhu;
					break;
				default:
					/* func3 invalid */
					info->subtype=-1;
			}
			break;
		case 0x23: /* opcode==0100011 */
			info->maintype=RV32I_S;
			switch(info->func3){
				case 0x0: /* func3==0x0 */
					info->subtype=RV32I_sb;
					break;
				case 0x1: /* func3==0x1 */
					info->subtype=RV32I_sh;
					break;
				case 0x2: /* func3==0x2 */
					info->subtype=RV32I_sw;
					break;
				default:
					/* func3 invalid */
					info->subtype=-1;
			}
			break;
		case 0x63: /* opcode==1100011 */
			info->maintype=RV32I_SB;
			switch(info->func3){
				case 0x0: /* func3==0x0 */
					info->subtype=RV32I_beq;
					break;
				case 0x1: /* func3==0x1 */
					info->subtype=RV32I_bne;
					break;
				case 0x4: /* func3==0x4 */
					info->subtype=RV32I_blt;
					break;
				case 0x5: /* func3==0x5 */
					info->subtype=RV32I_bge;
					break;
				case 0x6: /* func3==0x6 */
					info->subtype=RV32I_bltu;
					break;
				case 0x7: /* func3==0x7 */
					info->subtype=RV32I_bgeu;
					break;
				default:
					/* func3 invalid */
					info->subtype=-1;
			}
			break;
		case 0x6f: /* opcode==1101111 */
			info->maintype=RV32I_UJ;
			info->subtype=RV32I_jal;
			break;
		case 0x67: /* opcode==1100111 */
			info->maintype=RV32I_I;
			if(info->func3==0x0)info->subtype=RV32I_jalr;
			else info->subtype=-1;
			break;
		case 0x37: /* opcode==0110111 */
			info->maintype=RV32I_U;
			info->subtype=RV32I_lui;
			break;
		case 0x17: /* opcode==0010111 */
			info->maintype=RV32I_U;
			info->subtype=RV32I_auipc;
			break;
		case 0x73: /* opcode==1110011 */
			info->maintype=RV32I_I;
			if(info->func3==0x0){ /* func3==0 */
				if(info->func7==0x00){ /* imm[5:11]==0 */
					if(info->rs2==0x0)info->subtype=RV32I_ecall; /* imm[4:0]==0 */
					else if(info->rs2==0x1)info->subtype=RV32I_ebreak; /* imm[4:0]==1 */
					else info->subtype=-1;
				}else info->subtype=-1;
			}else info->subtype=-1;
			break;
		case 0x00: /* RV32C stored in high 16 bits */
			/* pseudo-type for RV32C */
			info->maintype=RV32I_C;
			info->subtype=-1;
			break;
		default: /* cannot detect RV32I format */
			info->maintype=-1;
			info->subtype=-1;
	}
	if(info->maintype==-1||(info->subtype==-1&&info->maintype!=RV32I_C)){ /* neither RV32I or RV32C */
		return 0; /* keep silent, but actually invalid instruction */
	}
	/* calculate immediate of the RV32I instruction */
	switch(info->maintype){
		case RV32I_R:
			info->imm=0;
			break;
		case RV32I_I: /* imm[11:0] */
			info->imm=*instr;
			info->imm>>=20;
			if(info->subtype==RV32I_srai){
				info->imm&=0x1f; /* set imm[5:11](func7=0x20) to 0 */
			}
			break;
		case RV32I_S: /* imm[11:0] */
			info->imm=*instr;
			info->imm>>=20; /* set imm[11:5] in position */
			info->imm&=0xffffffe0; /* set lower 5 bits as 0 */
			info->imm|=info->rd;
			break;
		case RV32I_SB: /* imm[12:1] */
			info->imm=(*instr)&0x80000000; /* load sign bit */
			info->imm>>=19; /* imm[12] */
			info->imm|=((info->func7)&0x3f)<<5; /* imm[10:5] */
			info->imm|=info->rd&0x1e; /* imm[4:1] */
			info->imm|=(info->rd&1)<<11; /* imm[11] */
			break;
		case RV32I_U: /* imm[31:12](store as imm[19:0]) */
			/* should actually be treated as unsigned number */
			info->imm=(*instr)>>12;
			break;
		case RV32I_UJ: /* imm[20:1] */
			info->imm=(*instr)&0x80000000; /* load sign bit */
			info->imm>>=11;
			info->imm|=((*instr)>>20)&0x7fe; /* imm[10:1] */
			info->imm|=((*instr)>>8)&0x800; /* imm[11] */
			info->imm|=(*instr)&0xff000; /* imm[19:12] */
			break;
		default:
			info->imm=0;
	}
	return 0;
}

/* after modifying RV32I_info.imm, put it back to RV32I */
int put_info(RV32I* instr, const RV32I_info* info){
	*instr=*(RV32I*)info;
	/* adjust immediate of uncompressible RV32I instructions */
	if(info->maintype==RV32I_SB){ /* branching */
		*instr&=0x01fff07f; /* set instr[31:25,11:7] to 0 */
		*instr|=(info->imm&0x00001000)<<19; /* instr[31]=imm[12] */
		*instr|=(info->imm&0x000007e0)<<20; /* instr[30:25]=imm[10:5] */
		*instr|=(info->imm&0x00000800)>>4; /* instr[7]=imm[11] */
		*instr|=(info->imm&0x0000001e)<<7; /* instr[11:8]=imm[4:1] */
	} else
	if(info->subtype==RV32I_jal){ /* RV32I_UJ jump */
		*instr&=0x00000fff; /* set instr[31:12] to 0 */
		*instr|=(info->imm&0x00100000)<<11; /* instr[31]=imm[20] */
		*instr|=(info->imm&0x000007fe)<<20; /* instr[30:21]=imm[10:1] */
		*instr|=(info->imm&0x00000800)<<9; /* instr[20]=imm[11] */
		*instr|=(info->imm&0x000ff000); /* instr[19:12]=imm[19:12] */
	} else
	if(info->subtype==RV32I_jalr){ /* RV32I_I jump */
		*instr&=0x000fffff; /* set instr[31:20] to 0 */
		*instr|=info->imm<<20; /* instr[31:20]=imm[11:0] */
	}
	/* non branch or jump instructions won't be modified */
	return 0;
}

/* get name of enum RV32I_maintypes, only for debugging */
/* static const char* get_maintype_name(enum RV32I_maintypes maintype){
	switch(maintype){
		case RV32I_R: return "RV32I_R";
		case RV32I_I: return "RV32I_I";
		case RV32I_S: return "RV32I_S";
		case RV32I_SB: return "RV32I_SB";
		case RV32I_U: return "RV32I_U";
		case RV32I_UJ: return "RV32I_UJ";
		case RV32I_C: return "RV32I_C";
		default: return "undefined";
	}
}*/

/* get name of enum RV32I_subtypes, only for debugging */
/* static const char* get_subtype_name(enum RV32I_subtypes subtype){
	switch(subtype){
		case RV32I_add: return "RV32I_add";
		case RV32I_sub: return "RV32I_sub";
		case RV32I_xor: return "RV32I_xor";
		case RV32I_or: return "RV32I_or";
		case RV32I_and: return "RV32I_and";
		case RV32I_sll: return "RV32I_sll";
		case RV32I_srl: return "RV32I_srl";
		case RV32I_sra: return "RV32I_sra";
		case RV32I_slt: return "RV32I_slt";
		case RV32I_sltu: return "RV32I_sltu";
		case RV32I_addi: return "RV32I_addi";
		case RV32I_xori: return "RV32I_xori";
		case RV32I_ori: return "RV32I_ori";
		case RV32I_andi: return "RV32I_andi";
		case RV32I_slli: return "RV32I_slli";
		case RV32I_srli: return "RV32I_srli";
		case RV32I_srai: return "RV32I_srai";
		case RV32I_slti: return "RV32I_slti";
		case RV32I_sltiu: return "RV32I_sltiu";
		case RV32I_lb: return "RV32I_lb";
		case RV32I_lh: return "RV32I_lh";
		case RV32I_lw: return "RV32I_lw";
		case RV32I_lbu: return "RV32I_lbu";
		case RV32I_lhu: return "RV32I_lhu";
		case RV32I_sb: return "RV32I_sb";
		case RV32I_sh: return "RV32I_sh";
		case RV32I_sw: return "RV32I_sw";
		case RV32I_beq: return "RV32I_beq";
		case RV32I_bne: return "RV32I_bne";
		case RV32I_blt: return "RV32I_blt";
		case RV32I_bge: return "RV32I_bge";
		case RV32I_bltu: return "RV32I_bltu";
		case RV32I_bgeu: return "RV32I_bgeu";
		case RV32I_jal: return "RV32I_jal";
		case RV32I_jalr: return "RV32I_jalr";
		case RV32I_lui: return "RV32I_lui";
		case RV32I_auipc: return "RV32I_auipc";
		case RV32I_ecall: return "RV32I_ecall";
		case RV32I_ebreak: return "RV32I_ebreak";
		default: return "undefined";
	}
}*/

/* display easy-to-read RV32I_info, only for debugging */
/* int print_info(const RV32I_info* info){
	if(info==NULL)return -1;
	printf("---------------- RV32I info ----------------\n");
	printf(" bin : ");print_instr(stdout,*(RV32I*)info);printf("\n");
	printf(" hex : %x   imm : %d \n",*(RV32I*)info,info->imm);
	printf(" maintype : %s subtype : %s\n",get_maintype_name(info->maintype),get_subtype_name(info->subtype));
	printf(" rd : %d   rs1 : %d   rs2 : %d\n",info->rd,info->rs1,info->rs2);
	printf("----------------  info end  ----------------\n");
	return 0;
}*/