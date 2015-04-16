char simname[] = "Y86 Processor: pipe-std.hcl";
#include <stdio.h>
#include "isa.h"
#include "pipeline.h"
#include "stages.h"
#include "sim.h"
int sim_main(int argc, char *argv[]);
int main(int argc, char *argv[]){return sim_main(argc,argv);}
int gen_f_pc()
{
    return ((((ex_mem_curr->icode) == (I_JXX)) & !(ex_mem_curr->takebranch)) ? (ex_mem_curr->vala) : ((mem_wb_curr->icode) == (I_RET)) ? (mem_wb_curr->valm) : 1 ? (pc_curr->pc) : 0);
}

int gen_need_regids()
{
    return ((if_id_next->icode) == (I_RRMOVL)||(if_id_next->icode) == (I_ALU)||(if_id_next->icode) == (I_ALUI)||(if_id_next->icode) == (I_PUSHL)||(if_id_next->icode) == (I_POPL)||(if_id_next->icode) == (I_IRMOVL)||(if_id_next->icode) == (I_RMMOVL)||(if_id_next->icode) == (I_MRMOVL));
}

int gen_need_valC()
{
    return ((if_id_next->icode) == (I_IRMOVL)||(if_id_next->icode) == (I_RMMOVL)||(if_id_next->icode) == (I_MRMOVL)||(if_id_next->icode) == (I_JXX)||(if_id_next->icode) == (I_CALL)||(if_id_next->icode) == (I_ALUI));
}

int gen_instr_valid()
{
    return ((if_id_next->icode) == (I_NOP)||(if_id_next->icode) == (I_HALT)||(if_id_next->icode) == (I_RRMOVL)||(if_id_next->icode) == (I_IRMOVL)||(if_id_next->icode) == (I_RMMOVL)||(if_id_next->icode) == (I_MRMOVL)||(if_id_next->icode) == (I_ALU)||(if_id_next->icode) == (I_ALUI)||(if_id_next->icode) == (I_JXX)||(if_id_next->icode) == (I_CALL)||(if_id_next->icode) == (I_RET)||(if_id_next->icode) == (I_PUSHL)||(if_id_next->icode) == (I_POPL));
}

