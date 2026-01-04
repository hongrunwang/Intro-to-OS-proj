#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "compression.h"
#include "utils.h"

int compress_instr(RV32I* instr, const RV32I_info* info){
	uint16_t* p=(uint16_t*)instr+1;
	/* Let the pointer p point to the instr of 17th position, and Set the instr as 0. */
	*instr=0;
	switch (info->RVC_types)
	{
	case RVC_un:/* for all instructions that are not compressible */
		put_info(instr,info); /* put imcompressible instructions back to 32-bit RV32I */
		break;
	case RVC_add:/* CR format */
		*p=*p|(0x9<<12);/*funct4=1001,rd and rs2 for 5 bits,opcode=10 */
		*p=*p|(info->rd<<7);
		*p=*p|(info->rs2<<2);
		*p=*p|(0x2);
		break;
	case RVC_mv:/* mv instruction */
		*p=*p|(0x8<<12);/*funct4=1000,rd and rs2 for 5 bits,opcode=10 */
		*p=*p|(info->rd<<7);
		*p=*p|(info->rs2<<2);
		*p=*p|(0x2);
		break;
	case RVC_jr:/* jr instruction */
		*p=*p|(0x8<<12);/*funct4=1000,rd and rs2=0 for 5 bits,opcode=10 */
		*p=*p|(info->rs1<<7);
		*p=*p|(0x2);
		break;
	case RVC_jalr:/* jalr instruction*/
		*p=*p|(0x9<<12);/*funct4=1001,rd and rs2=0 for 5 bits,opcode=10 */
		*p=*p|(info->rs1<<7);
		*p=*p|(0x2);
		break;
	/* CI format */
	case RVC_li:/* li instruction */
		*p=*p|(0x2<<13);/*funct3=010, imm[5], rd is 5 bits, imm[4:0], opcode=01 */
		if(info->imm<0){
			*p=*p|(0x1<<12);/*for negative immediate,imm[5]=1*/
			*p=*p|((info->imm & 0x1f)<<2);
		}
		else{
			*p=*p|(info->imm<<2);
		}/*For positive interger, just move the interger into 3-7 bit*/
		*p=*p|(info->rd<<7);
		*p=*p|(0x1);
		break;
	case RVC_lui:/* lui instruction */
		*p=*p|(0x3<<13);/* funct3=011, imm[5], rd is 5 bits, imm[4:0], opcode=01 */
		if(info->imm<0){
			*p=*p|(0x1<<12);/*for negative immediate,imm[5]=1*/
			*p=*p|((info->imm & 0x1f)<<2);
		}
		else{
			*p=*p|(info->imm<<2);
		}/*For positive interger, just move the interger into 3-7 bit*/
		*p=*p|(info->rd<<7);
		*p=*p|(0x1);
		break;
	case RVC_addi:/*addi instruction */
		if(info->imm<0){/* funct3=000, imm[17], rd is 5 bits, nzimm[16:12], opcode=01 */
			*p=*p|(0x1<<12);/*for negative immediate,imm[5]=1*/
			*p=*p|((info->imm & 0x1f)<<2);
		}
		else{
			*p=*p|(info->imm<<2);
		}/*For positive interger, just move the interger into 3-7 bit*/
		*p=*p|(info->rd<<7);
		*p=*p|(0x1);
		break;
	case RVC_slli:/* slli instruction */
		/* funct3=000, shamt[5], rd for 5 bits, shamt[4:0], opcode=10 */	
		*p=*p|(info->imm<<2);
		/*For positive interger, just move the interger into 3-7 bit*/
		*p=*p|(info->rd<<7);
		*p=*p|(0x2);
		break;
	/* CL format */
	case RVC_lw:/* lw instruction */
		/* funct3=010, offset[5:3], base(rs1) 3 bits, offset[2|6], dest(rd) for 3 bits, opcode=00 */
		*p=*p|(0x2<<13);
		*p=*p|( (info->rd&0x7) <<2);/* map register to 3-bit RVC register number*/
		*p=*p|( (info->rs1&0x7) <<7);
		*p=*p|( (info->imm&0x38) <<7);/*operations for offset.*/
		*p=*p|( (info->imm&0x4) <<4);
		*p=*p|( (info->imm&0x40) >>1);
		break;
	/* CS format */
	case RVC_sw:/* sw instruction */
		/* funct3=110, offset[5:3], base(rs1) 3 bits, offset[2|6], dest(rd) for 3 bits, opcode=00 */
		*p=*p|(0x6<<13);
		*p=*p|( (info->rd&0x7) <<2);/* map register to 3-bit RVC register number */
		*p=*p|( (info->rs1&0x7) <<7);
		*p=*p|( (info->imm&0x38) <<7);/*operations for offset.*/
		*p=*p|( (info->imm&0x4) <<4);
		*p=*p|( (info->imm&0x40) >>1);
		break;
	/* CS format */
	case RVC_and:/* and instruction */
		*p=*p|(0x23<<10);/* funct6=100011, rd for 3bits, func2=11, rs2:3 bits, opcode=01*/
		*p=*p|( (info->rd&0x7) <<7);/* map register to 3-bit RVC register number*/
		*p=*p|(0x3<<5);
		*p=*p|( (info->rs2&0x7) <<2);
		*p=*p|(0x1);
		break;
	case RVC_or:/* or instruction */
		*p=*p|(0x23<<10);/* funct6=100011, rd for 3bits, func2=10, rs2:3 bits, opcode=01*/
		*p=*p|( (info->rd&0x7) <<7);/* map register to 3-bit RVC register number*/
		*p=*p|(0x2<<5);
		*p=*p|( (info->rs2&0x7) <<2);
		*p=*p|(0x1);
		break;
	case RVC_xor:/* xor instruction */
		*p=*p|(0x23<<10);/* funct6=100011, rd for 3bits, func2=01, rs2:3 bits, opcode=01*/
		*p=*p|( (info->rd&0x7) <<7);/* map register to 3-bit RVC register number*/
		*p=*p|(0x1<<5);
		*p=*p|( (info->rs2&0x7) <<2);
		*p=*p|(0x1);
		break;
	case RVC_sub:/* sub instruction */
		*p=*p|(0x23<<10);/* funct6=100011, rd for 3bits, func2=00, rs2:3 bits, opcode=01*/
		*p=*p|( (info->rd&0x7) <<7);/* map register to 3-bit RVC register number*/
		*p=*p|( (info->rs2&0x7) <<2);
		*p=*p|(0x1);
		break;
	/* CB format*/
	case RVC_beqz:/* beqz instruction */
		*p=*p|(0x6<<13);/*fun3=110, offset[8|4:3], rd:3 bits, offset[7:6|2:1|5], opcode=01 */
		*p=*p|(0x1);
		*p=*p|( (info->rs1&0x7) <<7);/* map register to 3-bit RVC register number*/
		if(info->imm<0){/*check the sign of offset.*/
			*p=*p|(0x1 << 12);	
		}
		*p=*p|( (info->imm&0x18) <<7 );/*operation for offset*/
		*p=*p|( (info->imm&0xc0) >>1 );
		*p=*p|( (info->imm&0x6) <<2);
		*p=*p|( (info->imm&0x20) >>3);
		break;
	case RVC_bnez:/* bnez instruction */
		*p=*p|(0x7<<13);/*fun3=111, offset[8|4:3], rd:3 bits, offset[7:6|2:1|5], opcode=01 */
		*p=*p|(0x1);
		*p=*p|( (info->rs1&0x7) <<7);/* map register to 3-bit RVC register number*/
		if(info->imm<0){/*check the sign of offset.*/
			*p=*p|(0x1 << 12);	
		}
		*p=*p|( (info->imm&0x18) <<7 );
		*p=*p|( (info->imm&0xc0) >>1 );/*operation for offset*/
		*p=*p|( (info->imm&0x6) <<2);
		*p=*p|( (info->imm&0x20) >>3);
		break;
	case RVC_srli:/* srli instruction */
		*p=*p|(0x4<<13);/* func3=100, shamt[5]=0, funct2=00,rd:3 bits, shamt[4:0], opcode=01 */
		*p=*p|(0x1);
		*p=*p|( (info->imm)<<2);
		*p=*p|( (info->rd&0x7) <<7);/* map register to 3-bit RVC register number*/
		break;
	case RVC_srai:/* srai instruction */
		*p=*p|(0x4<<13);/* func3=100, shamt[5]=0, funct2=01,rd:3 bits, shamt[4:0], opcode=01 */
		*p=*p|(0x1);
		*p=*p|( (info->imm)<<2);
		*p=*p|(0x1<<10);/* map register to 3-bit RVC register number*/
		*p=*p|( (info->rd&0x7) <<7);
		break;
	case RVC_andi:/* andi instruction */
		*p=*p|(0x4<<13);/* func3=100, shamt[5]=0, funct2=10,rd:3 bits, shamt[4:0], opcode=01 */
		*p=*p|(0x1);
		if(info->imm<0){
			*p=*p|(0x1<<12);/*for negative immediate,imm[5]=1*/
			*p=*p|((info->imm & 0x1f)<<2);
		}
		else{
			*p=*p|(info->imm<<2);
		}
		*p=*p|(0x2<<10);/* map register to 3-bit RVC register number*/
		*p=*p|( (info->rd&0x7) <<7);
		break;
	/* CJ format */
	case RVC_j:/* j instruction */
		*p=*p|(0x5<<13);/* funct3=101, offset[11|4|9:8|10|6|7|3:1|5], opcode=01*/
		*p=*p|(0x1);
		if(info->imm<0){
			*p=*p|(0x1 << 12);/*check the sign of offset.*/
		}
		*p=*p|( (info->imm&0x2) <<2);
		*p=*p|( (info->imm&0x4) <<2);
		*p=*p|( (info->imm&0x8) <<2);/*operation for offset to different position.*/
		*p=*p|( (info->imm&0x10) <<7);
		*p=*p|( (info->imm&0x20) >>3);
		*p=*p|( (info->imm&0x40) <<1);
		*p=*p|( (info->imm&0x80) >>1);/* change the interger into the correct positon. */
		*p=*p|( (info->imm&0x100) <<2);
		*p=*p|( (info->imm&0x200) );
		*p=*p|( (info->imm&0x400) >>2);
		break;
	case RVC_jal:/* jal instruction */
		*p=*p|(0x1<<13);/* funct3=001, offset[11|4|9:8|10|6|7|3:1|5], opcode=01*/
		*p=*p|(0x1);
		if(info->imm<0){
			*p=*p|(0x1 << 12);/*check the sign of offset.*/	
		}
		*p=*p|( (info->imm&0x2) <<2);
		*p=*p|( (info->imm&0x4) <<2);
		*p=*p|( (info->imm&0x8) <<2);/*operation for offset to different position.*/
		*p=*p|( (info->imm&0x10) <<7);
		*p=*p|( (info->imm&0x20) >>3);
		*p=*p|( (info->imm&0x40) <<1);
		*p=*p|( (info->imm&0x80) >>1);/* change the interger into the correct positon. */
		*p=*p|( (info->imm&0x100) <<2);
		*p=*p|( (info->imm&0x200) );
		*p=*p|( (info->imm&0x400) >>2);
		break;
	default:/* reject instructions that hasn't been marked by is_compressible() */
		printf("RCV_type=%u, not marked by is_compressible()!\n",info->RVC_types);
		return -1;
	}
	return 0;
}

int is_compressible(RV32I_info* info){
	if(info==NULL)return -1;/* check the pointer if NULL.*/
	info->RVC_types= RVC_un;/* mark the instruction as incompressible */
	/*judge the instructions maintype */
	switch (info->maintype)
	{
	/*for any format instruction,determine if it is compressible and set the RVC instrution..*/
	case RV32I_R:
		/*for R format,it need to check if rd and rs2 is in x8-x15 except add operation.*/
		switch (info->subtype)
		{
		case RV32I_and:/*and operation.*/
		/*when rd=rs1,the instrution is compressible.*/
			if(info->rd==info->rs1 && info->rd>0x07 && info->rd<0x10 && info->rs2>0x07 && info->rs2<0x10){
				info->RVC_types=RVC_and;
				return 1;
			}/*return 1 when rd=rs1*/
			break;
		case RV32I_or:/*or operation.*/
		/*when rd=rs1,the instrution is compressible.*/
			if(info->rd==info->rs1 && info->rd>0x07 && info->rd<0x10 && info->rs2>0x07 && info->rs2<0x10){
				info->RVC_types=RVC_or;
				return 1;
			}
			break;/*xor operation.*/
		case RV32I_xor:
		/*when rd=rs1,the instrution is compressible.*/
			if(info->rd==info->rs1 && info->rd>0x07 && info->rd<0x10 && info->rs2>0x07 && info->rs2<0x10){
				info->RVC_types=RVC_xor;
				return 1;}
			break;/*add operation.*/
		case RV32I_add:
		/*when rd=rs1 and rd,rs2 are not x0,it is compressible.*/
			if(info->rd==info->rs1 && info->rd!=0x0 && info->rs2!=0x0)
			{	
				info->RVC_types=RVC_add;
				return 1;
			}
			/*when rs1=x0 and rd,rs2 are not x0,the instrution is compressible.*/
			else if(info->rs1==0x0 && info->rd!=0x0 && info->rs2!=0x0){
				info->RVC_types=RVC_mv;
				return 1;
			}
			break;/*for sub instruction,rd=rs1 and rd,rs2 are in x8-x15. */
		case RV32I_sub:
			if(info->rd==info->rs1 && info->rd>0x07 && info->rd<0x10 && info->rs2>0x07 && info->rs2<0x10){
				info->RVC_types=RVC_sub;
				return 1;/*set the types and return 1.*/
				}
			break;
		default:/*for other instruction,RVC types is -1. */
			break;
		}
		break;
	case RV32I_I:/* I format */
		switch (info->subtype)
		{
		case RV32I_jalr:/* jalr instruction */
			if(info->rs1!=0x0 && info->rd==0x0 && info->imm==0){
				info->RVC_types=RVC_jr;
				return 1;/* rd=x0,rs1 != x0 and  immediate=0 */
			}
			else if(info->rs1!=0x0 && info->rd==0x01 && info->imm ==0){/* rd=x1,rs1 != x0 and immediate=0 */
				info->RVC_types=RVC_jalr;
				return 1;
			}
			break;/* not compressible. */
		case RV32I_addi:/* addi instruction */
			if(info->rd!=0x00 && info->rs1==0x00 && info->imm>=(-0x20) && info->imm <0x20){
				info->RVC_types=RVC_li;/* rd != x0 ,rs1 =x0 and imm is 5 bits.*/
				return 1;
			}
			else if(info->rd==info->rs1 && info->rd!=0x0 && info->imm!=0 && info->imm>=(-0x20) && info->imm <0x20){
				info->RVC_types=RVC_addi;/* rd=rs1,immediate is nonzero,and rd!=x0 */
				return 1;
			}
			break;
		case RV32I_slli:/* slli instruction */
			if(info->rd==info->rs1 && info->rd!=0 && info->imm>=0 && info->imm <0x20){
				info->RVC_types=RVC_slli;/* rd=rs1, 0<=imm<0x20 */
				return 1;
			}
			break;
		case RV32I_lw:/* lw instruction */
			if(info->imm>=0 && info->rd>0x7 && info->rd<0x10 && info->rs1>0x7 && info->rs1<0x10 && (info->imm&0x03)==0 && info->imm<0x80){
				info->RVC_types=RVC_lw;/* rd and rs1 are in x8-x15 and imm>=0,imm mod 4=0, imm is 7 bits */
				return 1;
			}/* RVC tpye is lw and return 1. */
			break;
		case RV32I_srli:/* srli instruction */
			if(info->rd==info->rs1 && info->rd>0x7 && info->rd<0x10 && info->imm>=0x0 && info->imm <0x20 ){
				info->RVC_types=RVC_srli;
				return 1;/* rd=rs1 in x8-x15, 0<=imm<0x20 */
			}
			break;
		case RV32I_srai:/* srai instruction */
			if(info->rd==info->rs1 && info->rd>0x7 && info->rd<0x10 && info->imm>=0x0 && info->imm<0x20 ){
				info->RVC_types=RVC_srai;/* rd=rs1 in x8-x15, 0<=imm<0x20 */
				return 1;
			}
			break;
		case RV32I_andi:/* andi instruction. */
			if(info->rd==info->rs1 && info->rd>0x7 && info->rd<0x10 && info->imm >= (-0x20) && info->imm <(0x20) ){
				info->RVC_types=RVC_andi;/* rd=rs1 in x8-x15, -0x20<=imm<0x20 */
				return 1;
			}
			break;
		default:/*any other instruction in I format.*/
			break;
		}
		break;
	case RV32I_S:/* for S format */
		switch (info->subtype)
		{
		case RV32I_sw:/*there is only sw operation which is compressible. */
			/* rs1,rs2 in x8-x15, 0<=imm<0x80 , imm%4==0 */
			if(info->imm>=0 && info->rs2>0x7 && info->rs2<0x10 && info->rs1>0x7 && info->rs1<0x10 && (info->imm&0x03)==0 && info->imm<0x80){
				info->RVC_types=RVC_sw;
				return 1;
				}
			break;/* other S format instruction is not compressible. */
		default:
			break;
		}
		break;
	/* SB format */
	case RV32I_SB:
		switch (info->subtype)
		{
		case RV32I_beq:/* for beq , rs1 is in x8-x15 and rs2=x0, immediate/offset is 9 bits.*/
			if(info->rs2==0x0 && info->rs1>0x07 && info->rs1<0x10 && info->imm>=(-0x100) && info->imm<0x100){
				info->RVC_types=RVC_beqz;
				return 1;/* set the RVC types as beqz and return 1. */
			}
			break;
		case RV32I_bne:/* for bne ,it is the same as beq instruction. */
			if(info->rs2==0x0 && info->rs1>0x07 && info->rs1<0x10 &&  info->imm>=(-0x100) && info->imm<0x100){
				info->RVC_types=RVC_bnez;
				return 1;/* set the types as bnez and return 1. */
			}
			break;
		default:/* other instructions are not compressible. */
			break;
		}
		break;
	/* U format */
	case RV32I_U:
		switch (info->subtype){
		case RV32I_lui:/*In U format,only lui can be compressible.*/
			/* rd!=0, rd!=2, imm!=0, -0x20<=imm<0x20 */
			if(info->rd!=0x0 && info->rd != 0x02 && info->imm!=0 && info->imm>=(-0x20) && info->imm <0x20){
				info->RVC_types=RVC_lui;/*rd is not x0 or x2,and immediate is nonzero and small in 6 bits.*/
				return 1;/*Set the RVC types and return 1*/
			}/*the data is not in compressible.*/
			break;
		default:/*for any other instrution in U format,it is not compressible.*/
			break;
		}
		break;
	/*UJ format */
	case RV32I_UJ:
		switch (info->subtype)/* determine by subtype in RV32I */
		{
		case RV32I_jal:/*In UJ format,only jal instrution can be compressile, which has two types*/
			if(info->rd==0x0 && info->imm>=(-0x800) && info->imm<(0x800)){
				info->RVC_types=RVC_j;/* rd is x0 and immediate is small in 12 bits,it can be compressed.*/
				return 1;
			}
			if(info->rd==0x01 && info->imm>=(-0x800) && info->imm<(0x800)){/*rd = x1 and immediate is small in 12 bits.*/
				info->RVC_types=RVC_jal;
				return 1;
			}/*the data is not accessible.*/
			break;
		default:
			break;/*any other instrution in UJ format are not compressible.*/
		}
		break;
	default:/* the instruction is not the before. */
		info->RVC_types=RVC_un;
		return 0;/* the instrution is not any format. */
		break;
	}/* the instruction is incompressible. */
	info->RVC_types=RVC_un;
	return 0;
}
