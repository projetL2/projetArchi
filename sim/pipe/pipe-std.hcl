#/* $begin pipe-all-hcl */
####################################################################
#    HCL Description of Control for Pipelined Y86 Processor        #
#    Copyright (C) Randal E. Bryant, David R. O'Hallaron, 2002     #
####################################################################

####################################################################
#    C Include's.  Don't alter these                               #
####################################################################

quote '#include <stdio.h>'
quote '#include "isa.h"'
quote '#include "pipeline.h"'
quote '#include "stages.h"'
quote '#include "sim.h"'
quote 'int sim_main(int argc, char *argv[]);'
quote 'int main(int argc, char *argv[]){return sim_main(argc,argv);}'

####################################################################
#    Declarations.  Do not change/remove/delete any of these       #
####################################################################

##### Symbolic representation of Y86 Instruction Codes #############
intsig NOP 	'I_NOP'
intsig HALT	'I_HALT'
intsig RRMOVL	'I_RRMOVL'
intsig IRMOVL	'I_IRMOVL'
intsig RMMOVL	'I_RMMOVL'
intsig MRMOVL	'I_MRMOVL'
intsig OPL	'I_ALU'
intsig IOPL	'I_ALUI'
intsig JXX	'I_JXX'
intsig CALL	'I_CALL'
intsig RET	'I_RET'
intsig PUSHL	'I_PUSHL'
intsig POPL	'I_POPL'
intsig JMEM	'I_JMEM'
intsig JREG	'I_JREG'
intsig LEAVE	'I_LEAVE'

##### Symbolic representation of Y86 Registers referenced explicitly #####
intsig RESP     'REG_ESP'    	# Stack Pointer
intsig REBP     'REG_EBP'    	# Frame Pointer
intsig RNONE    'REG_NONE'   	# Special value indicating "no register"
intsig DNONE    'DEST_NONE'     # "no destination register"

##### ALU Functions referenced explicitly ##########################
intsig ALUADD	'A_ADD'		# ALU should add its arguments

##### Signals that can be referenced by control logic ##############

##### Pipeline Register F ##########################################

intsig F_predPC 'pc_curr->pc'		# Predicted value of PC

##### Intermediate Values in Fetch Stage ###########################

intsig f_icode	'if_id_next->icode'  # Fetched instruction code
intsig f_ifun	'if_id_next->ifun'   # Fetched instruction function
intsig f_valC	'if_id_next->valc'   # Constant data of fetched instruction
intsig f_valP	'if_id_next->valp'   # Address of following instruction

##### Pipeline Register D ##########################################
intsig D_icode 'if_id_curr->icode'	# Instruction code
intsig D_rA 'if_id_curr->ra'	# rA field from instruction
intsig D_rB 'if_id_curr->rb'	# rB field from instruction
intsig D_valP 'if_id_curr->valp'	# Incremented PC

##### Intermediate Values in Decode Stage  #########################

intsig d_srcA	 'id_ex_next->srca'	# srcA from decoded instruction
intsig d_srcB	 'id_ex_next->srcb'	# srcB from decoded instruction
intsig d_rvalA 'd_regvala'		# valA read from register file
intsig d_rvalB 'd_regvalb'		# valB read from register file

##### Pipeline Register E ##########################################
intsig E_icode 'id_ex_curr->icode'	# Instruction code
intsig E_ifun  'id_ex_curr->ifun'       # Instruction function
intsig E_valC  'id_ex_curr->valc'	# Constant data
intsig E_srcA  'id_ex_curr->srca'       # Source A register ID
intsig E_valA  'id_ex_curr->vala'       # Source A value
intsig E_srcB  'id_ex_curr->srcb'       # Source B register ID
intsig E_valB  'id_ex_curr->valb'       # Source B value
intsig E_dstE 'id_ex_curr->deste'	# Destination E register ID
intsig E_dstM 'id_ex_curr->destm'	# Destination M register ID

##### Intermediate Values in Execute Stage #########################
intsig e_valE 'ex_mem_next->vale'	# valE generated by ALU
boolsig e_Bch 'ex_mem_next->takebranch' # Am I about to branch?

##### Pipeline Register M                  #####
intsig M_icode 'ex_mem_curr->icode'	# Instruction code
intsig M_ifun  'ex_mem_curr->ifun'	# Instruction function
intsig M_valA  'ex_mem_curr->vala'      # Source A value
intsig M_dstE 'ex_mem_curr->deste'	# Destination E register ID
intsig M_valE  'ex_mem_curr->vale'      # ALU E value
intsig M_dstM 'ex_mem_curr->destm'	# Destination M register ID
boolsig M_Bch 'ex_mem_curr->takebranch'	# Branch Taken flag

##### Intermediate Values in Memory Stage ##########################
intsig m_valM 'mem_wb_next->valm'	# valM generated by memory

##### Pipeline Register W ##########################################
intsig W_icode 'mem_wb_curr->icode'	# Instruction code
intsig W_dstE 'mem_wb_curr->deste'	# Destination E register ID
intsig W_valE  'mem_wb_curr->vale'      # ALU E value
intsig W_dstM 'mem_wb_curr->destm'	# Destination M register ID
intsig W_valM  'mem_wb_curr->valm'	# Memory M value

####################################################################
#    Control Signal Definitions.                                   #
####################################################################

################ Fetch Stage     ###################################

## What address should instruction be fetched at
int f_pc = [
	# Mispredicted branch.  Fetch at incremented PC
	M_icode == JXX && !M_Bch : M_valA;
	# Completion of RET instruction.
	W_icode == RET : W_valM;
	# Default: Use predicted value of PC
	1 : F_predPC;
];

# Does fetched instruction require a regid byte?
bool need_regids =
	f_icode in { RRMOVL, OPL, IOPL, PUSHL, POPL, IRMOVL, RMMOVL, MRMOVL };

# Does fetched instruction require a constant word?
bool need_valC =
	f_icode in { IRMOVL, RMMOVL, MRMOVL, JXX, CALL, IOPL };

bool instr_valid = f_icode in 
	{ NOP, HALT, RRMOVL, IRMOVL, RMMOVL, MRMOVL,
	       OPL, IOPL, JXX, CALL, RET, PUSHL, POPL };

int instr_next_ifun = [
1 : -1;
]

# Predict next value of PC
int new_F_predPC = [
	f_icode in { JXX, CALL } : f_valC;
	1 : f_valP;
];


################ Decode Stage ######################################


## What register should be used as the A source?
int new_E_srcA = [
	D_icode in { RRMOVL, RMMOVL, OPL, PUSHL } : D_rA;
	D_icode in { POPL, RET } : RESP;
	1 : RNONE; # Don't need register
];

## What register should be used as the B source?
int new_E_srcB = [
	D_icode in { OPL, IOPL, RMMOVL, MRMOVL } : D_rB;
	D_icode in { PUSHL, POPL, CALL, RET } : RESP;
	1 : RNONE;  # Don't need register
];

## What register should be used as the E destination?
int new_E_dstE = [
	D_icode in { RRMOVL, IRMOVL, OPL, IOPL } : D_rB;
	D_icode in { PUSHL, POPL, CALL, RET } : RESP;
	1 : DNONE;  # Don't need register DNONE, not RNONE
];

## What register should be used as the M destination?
int new_E_dstM = [
	D_icode in { MRMOVL, POPL } : D_rA;
	1 : DNONE;  # Don't need register DNONE, not RNONE
];

## What should be the A value?
## Forward into decode stage for valA
int new_E_valA = [
	D_icode in { CALL, JXX } : D_valP; # Use incremented PC
	d_srcA == E_dstE : e_valE;    # Forward valE from execute
	d_srcA == M_dstM : m_valM;    # Forward valM from memory
	d_srcA == M_dstE : M_valE;    # Forward valE from memory
	d_srcA == W_dstM : W_valM;    # Forward valM from write back
	d_srcA == W_dstE : W_valE;    # Forward valE from write back
	1 : d_rvalA;  # Use value read from register file
];

int new_E_valB = [
	d_srcB == E_dstE : e_valE;    # Forward valE from execute
	d_srcB == M_dstM : m_valM;    # Forward valM from memory
	d_srcB == M_dstE : M_valE;    # Forward valE from memory
	d_srcB == W_dstM : W_valM;    # Forward valM from write back
	d_srcB == W_dstE : W_valE;    # Forward valE from write back
	1 : d_rvalB;  # Use value read from register file
];

################ Execute Stage #####################################

## Select input A to ALU
int aluA = [
	E_icode in { RRMOVL, OPL } : E_valA;
	E_icode in { IRMOVL, RMMOVL, MRMOVL, IOPL } : E_valC;
	E_icode in { CALL, PUSHL } : -4;
	E_icode in { RET, POPL } : 4;
	# Other instructions don't need ALU
];

## Select input B to ALU
int aluB = [
	E_icode in { RMMOVL, MRMOVL, OPL, IOPL, CALL, 
		      PUSHL, RET, POPL } : E_valB;
	E_icode in { RRMOVL, IRMOVL } : 0;
	# Other instructions don't need ALU
];

## Set the ALU function
int alufun = [
	E_icode in { OPL, IOPL } : E_ifun;
	1 : ALUADD;
];

## Should the condition codes be updated?
bool set_cc = E_icode in { OPL, IOPL };


################ Memory Stage ######################################

## Select memory address
int mem_addr = [
M_icode in { RMMOVL, PUSHL, CALL, MRMOVL } : M_valE;
	M_icode in { POPL, RET } : M_valA;
	# Other instructions don't need address
];

## Set read control signal
bool mem_read = M_icode in { MRMOVL, POPL, RET };

## Set write control signal
bool mem_write = M_icode in { RMMOVL, PUSHL, CALL };


################ Pipeline Register Control #########################

# Should I stall or inject a bubble into Pipeline Register F?
# At most one of these can be true.
bool F_bubble =
	# Inject bubbles instead of fetching while ret passes through pipeline
	RET in { D_icode, E_icode, M_icode } ||
	# Maching is halting, stop fetching
	HALT in { f_icode, D_icode, E_icode, M_icode, W_icode };
bool F_stall =
	# Conditions for a load/use hazard
	E_dstM in { d_srcA, d_srcB };

# Should I stall or inject a bubble into Pipeline Register D?
# At most one of these can be true.
bool D_stall = 
	# Conditions for a load/use hazard
	E_dstM in { d_srcA, d_srcB };

bool D_bubble =
	# Mispredicted branch, drop instruction
	(E_icode == JXX && !e_Bch);

# Should I stall or inject a bubble into Pipeline Register E?
# At most one of these can be true.
bool E_stall = 0;
bool E_bubble =
	# Mispredicted branch, drop instruction
	(E_icode == JXX && !e_Bch) ||
	# Conditions for a load/use hazard, stalling in decode
	E_dstM in { d_srcA, d_srcB};

# Should I stall or inject a bubble into Pipeline Register M?
# At most one of these can be true.
bool M_stall = 0;
bool M_bubble = 0;
#/* $end pipe-all-hcl */
