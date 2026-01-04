#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "utils.h"

/* compress RV32I_info into RV32I, fail if not processed by is_compressible() */
int compress_instr(RV32I* instr, const RV32I_info* info);

/* determine if RV32I_info is compressible, mark the corresponding RVC format in place, return 1 for true and 0 for false */
int is_compressible(RV32I_info* info);

#endif