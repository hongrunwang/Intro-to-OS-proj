#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

/* RV32I instruction format, regarded as RV32C when instr[0:6] is 0 */
typedef uint32_t RV32I;

/* RV32C classified as a pseudo-type of RV32I for convenience, 7 main-types in total, -1 reserved for undefined type */
/* names following riscv-spec-v2.1, slightly different from the newest specifications */
enum RV32I_maintypes {RV32I_R, RV32I_I, RV32I_S, RV32I_SB, RV32I_U, RV32I_UJ, RV32I_C};

/* 39 sub-types of RV32I in total, -1 reserved for undefined type */
enum RV32I_subtypes {RV32I_add, RV32I_sub, RV32I_xor, RV32I_or, RV32I_and, RV32I_sll, RV32I_srl, RV32I_sra, RV32I_slt, RV32I_sltu, RV32I_addi, RV32I_xori, RV32I_ori, RV32I_andi, RV32I_slli, RV32I_srli, RV32I_srai, RV32I_slti, RV32I_sltiu, RV32I_lb, RV32I_lh, RV32I_lw, RV32I_lbu, RV32I_lhu, RV32I_sb, RV32I_sh, RV32I_sw, RV32I_beq, RV32I_bne, RV32I_blt, RV32I_bge, RV32I_bltu, RV32I_bgeu, RV32I_jal, RV32I_jalr, RV32I_lui, RV32I_auipc, RV32I_ecall, RV32I_ebreak};

/* 21 types of RVC in total,RVC_un is incompressible.*/
enum RVC_types {RVC_add, RVC_mv, RVC_jr, RVC_jalr, RVC_li, RVC_lui, RVC_addi, RVC_slli, RVC_lw, RVC_sw, RVC_and, RVC_or, RVC_xor, RVC_sub, RVC_beqz, RVC_bnez, RVC_srli, RVC_srai, RVC_andi, RVC_j, RVC_jal, RVC_un};
/* stores information of RV32I instructions */
struct RV32I_info{
	/* wish to use union of RV32I and struct{32 bit-field}, but inconvenient in C89 */
	unsigned opcode:7; /* always meaningful */
	unsigned rd:5; /* meaningless in RV32I_S, RV32I_SB */
	unsigned func3:3; /* meaningless in RV32I_U, RV32I_UJ */
	unsigned rs1:5; /* meaningless in RV32I_U, RV32I_UJ */
	unsigned rs2:5; /* meaningless in RV32I_I, RV32I_U, RV32I_UJ */
	unsigned func7:7; /* meaningful only in RV32I_R */
	signed imm; /* declared as 32-bit int */
	unsigned maintype:3; /* enum RV32I_maintypes, used: 7/8 */
	unsigned subtype:6; /* enum RV32I_subtypes, used: 39/64 */
	unsigned RVC_types:6;
};
typedef struct RV32I_info RV32I_info;

/* read a string of 32 "01"s as RV32I from input, return EOF on failure */
int read_instr(FILE *input, RV32I* in);

/* print a RV32I as a string of 32 "01"s to output, return EOF on failure */
int print_instr(FILE *output, RV32I out);

/* parse a RV32I instruction and stores the result in struct RV32I_info, cannot determine compressible or not */
int get_info(RV32I_info* info, const RV32I* instr);

/* store instruction back to RV32I from RV32I_info, whose imm value might be modified */
int put_info(RV32I* instr, const RV32I_info* info);

/* print infomation for debugging */
int print_info(const RV32I_info* info);

#endif