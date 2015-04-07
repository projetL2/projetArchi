/* Instruction Set definition for Y86 Architecture */

/**************** Registers *************************/

/* REG_NONE is a special one to indicate no register */
/* JB: introduction de DEST_NONE */
typedef enum { REG_EAX, REG_ECX, REG_EDX, REG_EBX,
	       REG_ESP, REG_EBP, REG_ESI, REG_EDI, REG_NONE, DEST_NONE } reg_id_t;

/* Find register ID given its name */
reg_id_t find_register(char *name);
/* Return name of register given its ID */
char *reg_name(reg_id_t id);

/**************** Instruction Encoding **************/

/* Different argument types */
typedef enum { R_ARG, M_ARG, I_ARG, NO_ARG } arg_t;

/* Different instruction types */
/* JB Ajout I_JREG et I_JMEM
 * I_IADDL remplacé par I_ALUI
 * Attention: limite de 16 codes atteinte, I_POP2 invalide
 * On peut réorganiser et simplifier cette liste
 * mais il faut alors recompiler tous les fichiers yo
 */
typedef enum { I_NOP, I_HALT, I_RRMOVL, I_IRMOVL, I_RMMOVL, I_MRMOVL,
	       I_ALU, I_JXX, I_CALL, I_RET, I_PUSHL, I_POPL,
	       I_FREE1, I_LEAVE, I_JREG, I_JMEM, I_POP2 } itype_t;

/* pour compatibilité */
#define I_IADDL I_ALUI
#define I_ALUI I_ALU

/* Different ALU operations */
/* JB ajout SAL (shift arithmetic left) and SAR (shift arithmetic right) */
typedef enum { A_ADD, A_SUB, A_AND, A_XOR, A_SAL, A_SAR, A_NONE } alu_t;

/* Return name of operation given its ID */
char op_name(alu_t op);

/* Different Jump conditions */
typedef enum { J_YES, J_LE, J_L, J_E, J_NE, J_GE, J_G } jump_t;

/* Pack itype and function into single byte */
#define HPACK(hi,lo) ((((hi)&0xF)<<4)|((lo)&0xF))

/* Unpack byte */
#define HI4(byte) (((byte)>>4)&0xF)
#define LO4(byte) ((byte)&0xF)

/* Get the opcode out of one byte instruction field */
#define GET_ICODE(instr) HI4(instr)

/* Get the ALU/JMP function out of one byte instruction field */
#define GET_FUN(instr) LO4(instr)

/* Return name of instruction given it's byte encoding */
char *iname(int instr);

/**************** Truth Values **************/
typedef enum { FALSE, TRUE } bool_t;

/* Table used to encode information about instructions */
typedef struct {
  char *name;
  unsigned char code; /* Byte code for instruction+op */
  int bytes;
  arg_t arg1;
  int arg1pos;
  int arg1hi;  /* 0/1 for register argument, # bytes for allocation */
  arg_t arg2;
  int arg2pos;
  int arg2hi;  /* 0/1 */
} instr_t, *instr_ptr;

instr_ptr find_instr(char *name);

/* Return invalid instruction for error handling purposes */
instr_ptr bad_instr();

/***********  Implementation of Memory *****************/
typedef unsigned char byte_t;
typedef int word_t;

/* Represent a memory as an array of bytes */
typedef struct {
  int len;
  word_t maxaddr;
  byte_t *contents;
  } mem_rec, *mem_t;

/* Create a memory with len bytes */
mem_t init_mem(int len);
void free_mem(mem_t m);

/* Set contents of memory to 0 */
void clear_mem(mem_t m);

/* Make a copy of a memory */
mem_t copy_mem(mem_t oldm);
/* Print the differences between two memories */
bool_t diff_mem(mem_t oldm, mem_t newm, FILE *outfile);

/* How big should the memory be? */
#ifdef BIG_MEM
#define MEM_SIZE (1<<16)
#else
#define MEM_SIZE (1<<12)
#endif

/*** In the following functions, a return value of 1 means success ***/

/* Load memory from .vo file.  Return number of bytes read */
int load_mem(mem_t m, FILE *infile, int report_error);

/* Get byte from memory */
bool_t get_byte_val(mem_t m, word_t pos, byte_t *dest);

/* Get 4 bytes from memory */
bool_t get_word_val(mem_t m, word_t pos, word_t *dest);

/* Set byte in memory */
bool_t set_byte_val(mem_t m, word_t pos, byte_t val);

/* Set 4 bytes in memory */
bool_t set_word_val(mem_t m, word_t pos, word_t val);

/* Print contents of memory */
void dump_memory(FILE *outfile, mem_t m, word_t pos, int cnt);

/********** Implementation of Register File *************/

mem_t init_reg();
void free_reg();

/* Make a copy of a register file */
mem_t copy_reg(mem_t oldr);
/* Print the differences between two register files */
bool_t diff_reg(mem_t oldr, mem_t newr, FILE *outfile);


word_t get_reg_val(mem_t r, reg_id_t id);
void set_reg_val(mem_t r, reg_id_t id, word_t val);
void dump_reg(FILE *outfile, mem_t r);



/* ****************  ALU Function **********************/

/* Compute ALU operation */
word_t compute_alu(alu_t op, word_t arg1, word_t arg2);

typedef unsigned char cc_t;

#define GET_ZF(cc) (((cc) >> 2)&0x1)
#define GET_SF(cc) (((cc) >> 1)&0x1)
#define GET_OF(cc) (((cc) >> 0)&0x1)

#define PACK_CC(z,s,o) (((z)<<2)|((s)<<1)|((o)<<0))

#define DEFAULT_CC PACK_CC(1,0,0)

/* Compute condition code.  */
cc_t compute_cc(alu_t op, word_t arg1, word_t arg2);

/* Generated printed form of condition code */
char *cc_name(cc_t c);

/* **************** Exception types *******************/

typedef enum
 {EXC_BUBBLE, EXC_NONE, EXC_HALT, EXC_ADDR, EXC_INSTR, EXC_PIPE } exc_t;

/* Describe Exception Condition */
char *exc_name(exc_t e);

/* **************** ISA level implementation *********/

typedef struct {
  word_t pc;
  mem_t r;
  mem_t m;
  cc_t cc;
} state_rec, *state_ptr;

state_ptr new_state(int memlen);
void free_state(state_ptr s);

state_ptr copy_state(state_ptr s);
bool_t diff_state(state_ptr olds, state_ptr news, FILE *outfile);

/* Determine if branch should be taken */
bool_t take_branch(cc_t cc, jump_t bcond);

/* Execute single instruction.  Return exception condition. */
exc_t step_state(state_ptr s, FILE *error_file);

/************************ Interface Functions *************/

#ifdef HAS_GUI
void report_line(int line_no, int addr, char *hexcode, char *line);
void signal_register_update(reg_id_t r, int val);

#endif
