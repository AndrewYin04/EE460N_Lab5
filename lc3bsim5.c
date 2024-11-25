/*
    Name 1: Andrew Yin
    UTEID 1: ay7768
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N - Lab 5                                           */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         pagetable    page table in LC-3b machine language   */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {
    IRD,
    COND2, COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX,
    SR1MUX,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
/* MODIFY: you have to add all your new control signals */
    GatePC_1,
    LD_SavedSSP,
    LD_SavedUSP,
    SPMUX1, SPMUX0,
    GateSP,
    GatePSR,
    PSRMUX,
    LD_MV,
    LD_Priv,
    DR_SPMUX,
    SR1_SPMUX,
    CHK_MV,
    CHK_IOP,
    GateVector,
    LD_Vector,
    VectorMUX1, VectorMUX0,
    GateMAR,
    LDMARMUX,
    GateTranslation,
    TranslationMUX1, TranslationMUX0,
    LD_VA,
    LD_PTBR,
    SNSMUX,
    LD_SNS,
    LD_AV,
    CHK_SNS,
    CHK_AV,
    MDRMUX1, MDRMUX0,
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND2] << 2) + (x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
                                      (x[J3] << 3) + (x[J2] << 2) +
                                      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return(x[DRMUX]); }
int GetSR1MUX(int *x)        { return(x[SR1MUX]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); }
int GetLSHF1(int *x)         { return(x[LSHF1]); }
/* MODIFY: you can add more Get functions for your new control signals */
int GetGatePC_1(int *x)     { return (x[GatePC_1]); }
int GetLDSavedSSP(int *x)   { return (x[LD_SavedSSP]); }
int GetLDSavedUSP(int *x)   { return (x[LD_SavedUSP]); }
int GetSPMUX(int *x)        { return (x[SPMUX1] << 1) + (x[SPMUX0]); }
int GetGateSP(int *x)       { return (x[GateSP]); }
int GetGatePSR(int *x)      { return (x[GatePSR]); }
int GetPSRMUX(int *x)       { return (x[PSRMUX]); }
int GetLD_MV(int *x)        { return (x[LD_MV]); }
int GetLD_Priv(int *x)      { return (x[LD_Priv]); }
int GetDR_SPMUX(int *x)     { return (x[DR_SPMUX]); }
int GetSR1_SPMUX(int *x)    { return (x[SR1_SPMUX]); }
int GetCHK_MV(int *x)       { return (x[CHK_MV]); }
int GetCHK_IOP(int *x)      { return (x[CHK_IOP]); }
int GetGateVector(int *x)   { return (x[GateVector]); }
int GetLD_Vector(int *x)    { return (x[LD_Vector]); }
int GetVectorMUX(int *x)    { return ((x[VectorMUX1] << 1) + x[VectorMUX0]); }
/* MODIFY: you can add more Get functions for your new control signals */
int GetGateMAR(int *x)       { return (x[GateMAR]); }
int GetLDMARMUX(int *x)      { return (x[LDMARMUX]); }
int GetGateTranslation(int *x) { return (x[GateTranslation]); }
int GetTranslationMUX(int *x) { return ((x[TranslationMUX1] << 1) + x[TranslationMUX0]); }
int GetLD_VA(int *x)          { return (x[LD_VA]); }
int GetLD_PTBR(int *x)        { return (x[LD_PTBR]); }
int GetSNSMUX(int *x)         { return (x[SNSMUX]); }
int GetLD_SNS(int *x)         { return (x[LD_SNS]); }
int GetLD_AV(int *x)          { return (x[LD_AV]); }
int GetCHK_SNS(int *x)        { return (x[CHK_SNS]); }
int GetCHK_AV(int *x)         { return (x[CHK_AV]); }
int GetMDRMUX(int *x)         { return ((x[MDRMUX1] << 1) + x[MDRMUX0]); }

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A
   There are two write enable signals, one for each byte. WE0 is used for
   the least significant byte of a word. WE1 is used for the most significant
   byte of a word. */

#define WORDS_IN_MEM    0x2000 /* 32 frames */
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

    int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */

    int READY;	/* ready bit */
    /* The ready bit is also latched as you dont want the memory system to assert it
       at a bad point in the cycle*/

    int REGS[LC_3b_REGS]; /* register file. */

    int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

    int STATE_NUMBER; /* Current State Number - Provided for debugging */

/* For lab 4 */
    int INTV; /* Interrupt vector register */
    int EXCV; /* Exception vector register */
    int SSP; /* Initial value of system stack pointer */
/* MODIFY: you should add here any other registers you need to implement interrupts and exceptions */
    int Privilege; /* PSR[15]; 0 means supervisor mode; 1 means user mode */ // todo here
    int INTERRUPT; /* Interrupt Signal */ // todo here
    int MV; /* Memory Violation Signal; A combination of either or both Protection and Unaligned Access Exceptions */ // todo here
    int Saved_USP;
    int Saved_SSP;
    int Vector;

/* For lab 5 */
    int PTBR; /* This is initialized when we load the page table */
    int VA;   /* Temporary VA register */
/* MODIFY: you should add here any other registers you need to implement virtual memory */
    int AV; /* Access violation signal */
    int SNS_Saved_Next_State;

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/* For lab 5 */
#define PAGE_NUM_BITS 9
#define PTE_PFN_MASK 0x3E00
#define PTE_VALID_MASK 0x0004
#define PAGE_OFFSET_MASK 0x1FF

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

    eval_micro_sequencer();
    cycle_memory();
    eval_bus_drivers();
    drive_bus();
    latch_datapath_values();

    CURRENT_LATCHES = NEXT_LATCHES;

    CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
    int i;

    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
        if (CURRENT_LATCHES.PC == 0x0000) {
            RUN_BIT = FALSE;
            printf("Simulator halted\n\n");
            break;
        }
        cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void go() {
    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
        cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
    int address; /* this is a byte address */

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
    int k;

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
        case 'G':
        case 'g':
            go();
            break;

        case 'M':
        case 'm':
            scanf("%i %i", &start, &stop);
            mdump(dumpsim_file, start, stop);
            break;

        case '?':
            help();
            break;
        case 'Q':
        case 'q':
            printf("Bye.\n");
            exit(0);

        case 'R':
        case 'r':
            if (buffer[1] == 'd' || buffer[1] == 'D')
                rdump(dumpsim_file);
            else {
                scanf("%d", &cycles);
                run(cycles);
            }
            break;

        default:
            printf("Invalid Command\n");
            break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
        printf("Error: Can't open micro-code file %s\n", ucode_filename);
        exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
        if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
            printf("Error: Too few lines (%d) in micro-code file: %s\n",
                   i, ucode_filename);
            exit(-1);
        }

        /* Put in bits one at a time. */
        index = 0;

        for (j = 0; j < CONTROL_STORE_BITS; j++) {
            /* Needs to find enough bits in line. */
            if (line[index] == '\0') {
                printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
                       ucode_filename, i);
                exit(-1);
            }
            if (line[index] != '0' && line[index] != '1') {
                printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
                       ucode_filename, i, j);
                exit(-1);
            }

            /* Set the bit in the Control Store. */
            CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
            index++;
        }

        /* Warn about extra bits in line. */
        if (line[index] != '\0')
            printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
                   ucode_filename, i);
    }
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
        MEMORY[i][0] = 0;
        MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename, int is_virtual_base) {
    FILE * prog;
    int ii, word, program_base, pte, virtual_pc;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
        printf("Error: Can't open program file %s\n", program_filename);
        exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
        program_base = word >> 1;
    else {
        printf("Error: Program file is empty\n");
        exit(-1);
    }

    if (is_virtual_base) {
        if (CURRENT_LATCHES.PTBR == 0) {
            printf("Error: Page table base not loaded %s\n", program_filename);
            exit(-1);
        }

        /* convert virtual_base to physical_base */
        virtual_pc = program_base << 1;
        pte = (MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][1] << 8) |
              MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][0];

        printf("virtual base of program: %04x\npte: %04x\n", program_base << 1, pte);
        if ((pte & PTE_VALID_MASK) == PTE_VALID_MASK) {
            program_base = (pte & PTE_PFN_MASK) | ((program_base << 1) & PAGE_OFFSET_MASK);
            printf("physical base of program: %x\n\n", program_base);
            program_base = program_base >> 1;
        } else {
            printf("attempting to load a program into an invalid (non-resident) page\n\n");
            exit(-1);
        }
    }
    else {
        /* is page table */
        CURRENT_LATCHES.PTBR = program_base << 1;
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
        /* Make sure it fits. */
        if (program_base + ii >= WORDS_IN_MEM) {
            printf("Error: Program file %s is too long to fit in memory. %x\n",
                   program_filename, ii);
            exit(-1);
        }

        /* Write the word to memory array. */
        MEMORY[program_base + ii][0] = word & 0x00FF;
        MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;;
        ii++;
    }

    if (CURRENT_LATCHES.PC == 0 && is_virtual_base)
        CURRENT_LATCHES.PC = virtual_pc;

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */
/*             and set up initial state of the machine         */
/*                                                             */
/***************************************************************/
void initialize(char *argv[], int num_prog_files) {
    int i;
    init_control_store(argv[1]);

    init_memory();
    load_program(argv[2],0);
    for ( i = 0; i < num_prog_files; i++ ) {
        load_program(argv[i + 3],1);
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */

/* MODIFY: you can add more initialization code HERE */

    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 4) {
        printf("Error: usage: %s <micro_code_file> <page table file> <program_file_1> <program_file_2> ...\n",
               argv[0]);
        exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv, argc - 3);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }

    while (1)
        get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated
   with a "MODIFY:" comment.
   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/


void eval_micro_sequencer() {

    /*
     * Evaluate the address of the next state according to the
     * micro sequencer logic. Latch the next microinstruction.
     */

    int cond = GetCOND(CURRENT_LATCHES.MICROINSTRUCTION);
    int IRD = GetIRD(CURRENT_LATCHES.MICROINSTRUCTION);
    int j = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
    int ben = CURRENT_LATCHES.BEN;
    int R = CURRENT_LATCHES.READY;
    int IR11 = (CURRENT_LATCHES.IR & 0x0800) >> 11; // shift from bit 11 to bit 0

    int IR15_12 = (CURRENT_LATCHES.IR & 0x0F000) >> 12;
    int chk_mv = GetCHK_MV(CURRENT_LATCHES.MICROINSTRUCTION);
    int chk_iop = GetCHK_IOP((CURRENT_LATCHES.MICROINSTRUCTION));
    int mv = CURRENT_LATCHES.MV;
    int INT = CURRENT_LATCHES.INTERRUPT;
    int PSR15 = CURRENT_LATCHES.Privilege;

    int SNS = CURRENT_LATCHES.SNS_Saved_Next_State;
    int chk_av = GetCHK_AV(CURRENT_LATCHES.MICROINSTRUCTION);
    int av = CURRENT_LATCHES.AV;
    int chk_sns = GetCHK_SNS(CURRENT_LATCHES.MICROINSTRUCTION);
    int ld_mar = GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION);

    // load next_latches's state number
//    if (IRD == 0) {
//        NEXT_LATCHES.STATE_NUMBER = (j) | ((cond & 0b01) & ((cond & 0b010) >> 1) & IR11) | (((cond & 0b01) & (((cond & 0b010) ^ 0b010) >> 1) & R) << 1) | ((((cond & 0b01) ^ 0b01) & ((cond & 0b010) >> 1) & ben) << 2);
//    } else { // if IRD == 1
//        NEXT_LATCHES.STATE_NUMBER = (CURRENT_LATCHES.IR & 0x0F000) >> 12;
//    }
    if (chk_sns == 1) {
        NEXT_LATCHES.STATE_NUMBER = SNS;
    } else if ((ld_mar == 1) && (!((j == 33) && INT)) && (j != 61)) {
        NEXT_LATCHES.STATE_NUMBER = 35;
    } else if ((mv & chk_mv) || (((IR15_12 == 0b01010) || (IR15_12 == 0b01011)) && chk_iop) || (av && chk_av)) {
        NEXT_LATCHES.STATE_NUMBER = 49;
    } else if (IRD == 1) {
        NEXT_LATCHES.STATE_NUMBER = (CURRENT_LATCHES.IR & 0x0F000) >> 12;
    } else { // IRD == 0
        if (cond == 0) {
            NEXT_LATCHES.STATE_NUMBER = j;
        } else if ((cond == 1) && R) {
            NEXT_LATCHES.STATE_NUMBER = j | (R << 1);
        } else if ((cond == 2) && ben) {
            NEXT_LATCHES.STATE_NUMBER = j | (ben << 2);
        } else if ((cond == 3) && IR11) {
            NEXT_LATCHES.STATE_NUMBER = j | (IR11);
        } else if ((cond == 4) && PSR15) {
            NEXT_LATCHES.STATE_NUMBER = j | (PSR15 << 3);
        } else if ((cond == 5) && INT) {
            NEXT_LATCHES.STATE_NUMBER = j | (INT << 4);
            NEXT_LATCHES.INTERRUPT = 0;
        } else if ((cond == 6) && ((SNS == 23) || (SNS == 57) || (SNS == 41) || (SNS == 46))) {
            NEXT_LATCHES.STATE_NUMBER = j | (0b01 << 5);
        } else {
            NEXT_LATCHES.STATE_NUMBER = j;
        }
    }

    // load next_latches's microinstruction with the appropriate state
    memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);

}

int mem_cycle = 1;
void cycle_memory() {

    /*
     * This function emulates memory and the WE logic.
     * Keep track of which cycle of MEMEN we are dealing with.
     * If fourth, we need to latch Ready bit at the end of
     * cycle to prepare microsequencer for the fifth cycle.
     */

    if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) == 1) { // if this state accesses memory
        if (mem_cycle == (MEM_CYCLES - 1)) {
            NEXT_LATCHES.READY = 1;
        } else if (mem_cycle == MEM_CYCLES) {
            mem_cycle = 0; // will be incremented to one later
            NEXT_LATCHES.READY = 0; // reset R bit

            int RW = GetR_W(CURRENT_LATCHES.MICROINSTRUCTION);
            int Data_Size = GetDATA_SIZE((CURRENT_LATCHES.MICROINSTRUCTION));
            int Mar0 = CURRENT_LATCHES.MAR & 0b01;
            int WE0 = RW & (Data_Size | (~Mar0));
            int WE1 = RW & (Data_Size | (Mar0));

            // stores into memory from MDR is handled here
            if ((WE0 == 1) && (WE1 == 1)) {
                MEMORY[CURRENT_LATCHES.MAR >> 1][0] = CURRENT_LATCHES.MDR & 0x0FF;
                MEMORY[CURRENT_LATCHES.MAR >> 1][1] = (CURRENT_LATCHES.MDR & 0x0FF00) >> 8;
            } else if ((WE0 == 1) && (WE1 == 0)) {
                MEMORY[CURRENT_LATCHES.MAR >> 1][0] = CURRENT_LATCHES.MDR & 0x0FF;
            } else if ((WE0 == 0) && (WE1 == 1)) {
                MEMORY[CURRENT_LATCHES.MAR >> 1][1] = ((CURRENT_LATCHES.MDR) & 0x0FF); // TODO: WATCH HERE
            }

            // loads into MDR from memory is handled here
            if ((WE0 == 0) && (WE1 == 0)) {
                // load all 16 bits
                NEXT_LATCHES.MDR = Low16bits(MEMORY[CURRENT_LATCHES.MAR >> 1][0] + (MEMORY[CURRENT_LATCHES.MAR >> 1][1] << 8));
            }
        }

        mem_cycle += 1;
    } else {
        return;
    }

}

int sign_extend(int value, int bit_count) {
    if((value & (1 << (bit_count - 1))) == 0) return value;
    int mask = 1U << (bit_count);
    mask = ~(mask - 1);
    return value | mask;
}

int arithmetic_right_shift(int value, int shift) {
    if ((value & 0x08000)) { // maybe change it to check if bit 15 is a one
        return (value >> shift) | (~((1 << (15- shift + 1)) - 1)); // TODO: check this
    } else {
        return value >> shift;
    }
}

/*
 * @param value: sign-extended value
 */
int logical_right_shift(int value, int shift) {
    if (value >= 0) return value >> shift;
    else { // if the value is negative, thus the leading is one
        value = value >> shift;
        int tmp = 0xffffu; // assumes that value is a 16 bit value, represented in int
        tmp = (tmp) >> (shift);
        return value & tmp;
    }
}

void eval_bus_drivers() {

    /*
     * Datapath routine emulating operations before driving the bus.
     * Evaluate the input of tristate drivers
     *             Gate_MARMUX,
     *		 Gate_PC,
     *		 Gate_ALU,
     *		 Gate_SHF,
     *		 Gate_MDR.
     */
    int Gate_MARMUX = GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int Gate_PC = GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION);
    int Gate_ALU = GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION);
    int Gate_SHF = GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION);
    int Gate_MDR = GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION);
    int Gate_SP = GetGateSP((CURRENT_LATCHES.MICROINSTRUCTION));
    int Gate_PC_1 = GetGatePC_1(CURRENT_LATCHES.MICROINSTRUCTION);
    int Gate_PSR = GetGatePSR(CURRENT_LATCHES.MICROINSTRUCTION);
    int Gate_Vector = GetGateVector(CURRENT_LATCHES.MICROINSTRUCTION);
    int Gate_MAR = GetGateMAR(CURRENT_LATCHES.MICROINSTRUCTION);
    int Gate_Translation = GetGateTranslation(CURRENT_LATCHES.MICROINSTRUCTION);

    if (Gate_MARMUX) {
        int marmux = GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION);
        if (marmux == 0) { // state 15
            BUS = Low16bits((CURRENT_LATCHES.IR & 0x0FF) << 1);
        } else { // marmux == 1
            int left_add = 0;
            int right_add = 0;
            int addr1mux = GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
            int addr2mux = GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION);
            int lshf1 = GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION);

            // left side of the adder
            if (addr2mux == 0b010) { // PCoffset9; select SEXT[IR[8:0]]
                if (lshf1 == 1) { // state 14
                    left_add = sign_extend(CURRENT_LATCHES.IR & 0x01FF, 9) << 1;
                }
            } else if (addr2mux == 0b001) { // PCoffset6; select SEXT[IR[5:0]]
                if (lshf1 == 1) { // state 6, 7
                    left_add = sign_extend(CURRENT_LATCHES.IR & 0x03F, 6) << 1;
                } else { // state 2, 3
                    left_add = sign_extend(CURRENT_LATCHES.IR & 0x03F, 6);
                }
            }

            // right side of the adder
            if (addr1mux == 0b0) { // state 14
                right_add = CURRENT_LATCHES.PC;
            } else {
                int sr1mux = GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
                int sr1_sp_mux = GetSR1_SPMUX(CURRENT_LATCHES.MICROINSTRUCTION);
                if (sr1_sp_mux == 1) {
                    right_add = sign_extend(CURRENT_LATCHES.REGS[0b0110], 16);
                } else if (sr1mux == 1) { // state 2, 3, 6, 7
                    right_add = sign_extend(CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR >> 6) & 0b0111], 16);
                } else {
                    right_add = sign_extend(CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR >> 9) & 0b0111], 16);
                }
            }

            // sum
            BUS = Low16bits(left_add + right_add);
        }
    } else if (Gate_SP) {
        int SPMUX = GetSPMUX(CURRENT_LATCHES.MICROINSTRUCTION);
        int SR1MUX = GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
        int SR1_SPMUX = GetSR1_SPMUX(CURRENT_LATCHES.MICROINSTRUCTION);

        if (SR1_SPMUX == 1) {
            int r6_reg = CURRENT_LATCHES.REGS[0b0110];
            int result = 0;
            if (SPMUX == 0) {
                result = CURRENT_LATCHES.Saved_USP;
            } else if (SPMUX == 1) {
                result = CURRENT_LATCHES.Saved_SSP;
            } else if (SPMUX == 2) {
                result = r6_reg + 2;
            } else {
                result = r6_reg - 2;
            }
            BUS = Low16bits(result);
        } else {
            BUS = Low16bits(0x0BEEF); // smth's wrong
        }
    } else if (Gate_PSR) {
        int result = 0;

        result = (CURRENT_LATCHES.N << 2) + (CURRENT_LATCHES.Z << 1) + (CURRENT_LATCHES.P) + (CURRENT_LATCHES.Privilege << 15);

        BUS = Low16bits(result);
    } else if (Gate_PC_1) {
        BUS = Low16bits(CURRENT_LATCHES.PC - 2);
    } else if (Gate_Vector) {
        BUS = Low16bits((CURRENT_LATCHES.Vector << 1) + 0x0200);
    } else if (Gate_PC) {
        BUS = CURRENT_LATCHES.PC; // state 18, 19, 20, 21, 28
    } else if (Gate_ALU) {
        int IR = CURRENT_LATCHES.IR;
        int SR2 = IR & 0b0111;
        int SR1;
        int ALUK = GetALUK(CURRENT_LATCHES.MICROINSTRUCTION);

        int SR1MUX = GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION); // todo here for sr1mux DONE
        int sr1_sp_mux = GetSR1_SPMUX(CURRENT_LATCHES.MICROINSTRUCTION);
        if (sr1_sp_mux == 1) {
            SR1 = 0b0110;
        } else {
            if (SR1MUX == 1) {
                SR1 = (IR >> 6) & 0b0111;
            } else {
                SR1 = (IR >> 9) & 0b0111;
            }
        }

        if (ALUK == 0) { // ADD
            int op1 = sign_extend(CURRENT_LATCHES.REGS[SR1], 16);
            int op2;
            int IR5 = (IR & 0b0100000) >> 5;

            // get 2nd operand
            if ((IR5) == 1) {
                op2 = sign_extend(IR & 0b011111, 5);
            } else {
                op2 = sign_extend(CURRENT_LATCHES.REGS[SR2], 16);
            }

            BUS = Low16bits(op1 + op2);
        } else if (ALUK == 0b01) { // AND
            int op1 = sign_extend(CURRENT_LATCHES.REGS[SR1], 16);
            int op2;
            int IR5 = (IR & 0b0100000) >> 5;

            // get 2nd operand
            if ((IR5) == 1) {
                op2 = sign_extend(IR & 0b011111, 5);
            } else {
                op2 = sign_extend(CURRENT_LATCHES.REGS[SR2], 16);
            }

            BUS = Low16bits(op1 & op2);
        } else if (ALUK == 0b010) { // XOR
            int op1 = sign_extend(CURRENT_LATCHES.REGS[SR1], 16);
            int op2;
            int IR5 = (IR & 0b0100000) >> 5;

            // get 2nd operand
            if ((IR5) == 1) {
                op2 = sign_extend(IR & 0b011111, 5);
            } else {
                op2 = sign_extend(CURRENT_LATCHES.REGS[SR2], 16);
            }

            BUS = Low16bits(op1 ^ op2);
        } else { // PASSA
            BUS = Low16bits(CURRENT_LATCHES.REGS[SR1]);
        }
    } else if (Gate_SHF) {
        int SR = (CURRENT_LATCHES.IR >> 6) & 0b0111;
        int IR4 = (CURRENT_LATCHES.IR >> 4) & 0b01;
        int IR5 = (CURRENT_LATCHES.IR >> 5) & 0b01;
        int amt4 = (CURRENT_LATCHES.IR & 0b01111);

        if (IR4 == 0) { // left shift
            BUS = Low16bits(CURRENT_LATCHES.REGS[SR] << amt4);
        } else { // right shift
            if (IR5 == 1) {
                BUS = Low16bits(arithmetic_right_shift(CURRENT_LATCHES.REGS[SR], amt4));
            } else {
                BUS = Low16bits(logical_right_shift(CURRENT_LATCHES.REGS[SR], amt4));
            }
        }
    } else if (Gate_MDR) {
        int MAR0 = CURRENT_LATCHES.MAR & 0b01;
        int Data_Size = GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION);
        int MDR = CURRENT_LATCHES.MDR;

        if (Data_Size == 0) { // byte; LDB
            if (MAR0 == 1) {
                BUS = Low16bits(sign_extend(((MDR >> 8) & 0x0FF), 8));
            } else {
                BUS = Low16bits(sign_extend(((MDR) & 0x0FF), 8));
            }
        } else { // word; LDW
            BUS = Low16bits(MDR);
        }
    } else if (Gate_MAR) {
        BUS = Low16bits(CURRENT_LATCHES.MAR);
    } else if (Gate_Translation) {
        int va = CURRENT_LATCHES.VA;
        int ptbr = CURRENT_LATCHES.PTBR;

        int translationMux = GetTranslationMUX(CURRENT_LATCHES.MICROINSTRUCTION);

        if (translationMux == 0) {
            BUS = Low16bits(ptbr);
        } else if (translationMux == 1) {
            int vpn = (va >> 9) & 0b01111111;
            BUS = Low16bits(ptbr + (vpn << 1));
        } else if (translationMux == 2) {
            BUS = Low16bits(va);
        } else {
            BUS = 0x0;
        }
    } else {
        BUS = 0x0;
    }
}


void drive_bus() {

    /*
     * Datapath routine for driving the bus from one of the 5 possible
     * tristate drivers.
     */

}


void latch_datapath_values() {

    /*
     * Datapath routine for computing all functions that need to latch
     * values in the data path at the end of this cycle.  Some values
     * require sourcing the bus; therefore, this routine has to come
     * after drive_bus.
     */
    int LD_MAR = GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_MDR = GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_IR = GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_BEN = GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_REG = GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_CC = GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_PC = GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_SavedSSP = GetLDSavedSSP(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_SavedUSP = GetLDSavedUSP(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_MV = GetLD_MV(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_Priv = GetLD_Priv(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_Vector = GetLD_Vector(CURRENT_LATCHES.MICROINSTRUCTION);

    int LD_AV = GetLD_AV(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_VA = GetLD_VA(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_PTBR = GetLD_PTBR((CURRENT_LATCHES.MICROINSTRUCTION));
    int LD_SNS = GetLD_SNS(CURRENT_LATCHES.MICROINSTRUCTION);

    if (LD_AV) {
        int protection = (((CURRENT_LATCHES.Privilege == 1) && ((CURRENT_LATCHES.MDR & 0b01000) == 0) && (CURRENT_LATCHES.SNS_Saved_Next_State != 28))) & 0b01;
        int valid = (!((CURRENT_LATCHES.MDR >> 2) & 0b01)) & 0b01;
        NEXT_LATCHES.AV = protection | valid;
    }

    if (LD_VA) {
        NEXT_LATCHES.VA = BUS;
    }

    if (LD_PTBR) {
        NEXT_LATCHES.PTBR = BUS;
    }

    if (LD_SNS) {
        int snsmux = GetSNSMUX(CURRENT_LATCHES.MICROINSTRUCTION);
        if (snsmux == 0) {
            NEXT_LATCHES.SNS_Saved_Next_State = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
        } else {
            NEXT_LATCHES.SNS_Saved_Next_State = 0;
        }
    }

    if (LD_SavedSSP) {
        NEXT_LATCHES.Saved_SSP = CURRENT_LATCHES.REGS[0b0110];
    }

    if (LD_SavedUSP) {
        NEXT_LATCHES.Saved_USP = CURRENT_LATCHES.REGS[0b0110];
    }

    if (LD_MV) {
        NEXT_LATCHES.MV = ((((BUS & 0b01) == 1) && (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 1)));
    }

    if (LD_Priv) {
        int PSRMUX = GetPSRMUX(CURRENT_LATCHES.MICROINSTRUCTION);
        if (PSRMUX == 0) {
            NEXT_LATCHES.Privilege = 0;
        } else {
            NEXT_LATCHES.Privilege = (BUS >> 15) & 0b01;
        }
    }

    if (LD_Vector) {
        int VectorMUX = GetVectorMUX(CURRENT_LATCHES.MICROINSTRUCTION);
        if (VectorMUX == 0) {
            NEXT_LATCHES.Vector = CURRENT_LATCHES.INTV;
        } else if (VectorMUX == 1) {
            NEXT_LATCHES.Vector = 0x05;
        } else if (VectorMUX == 2) {
            NEXT_LATCHES.Vector = 0x03;
        } else {
            if ((((CURRENT_LATCHES.Privilege == 1) && ((CURRENT_LATCHES.MDR & 0b01000) == 0) && (CURRENT_LATCHES.SNS_Saved_Next_State != 28))) == 1) {
                NEXT_LATCHES.Vector = 0x04;
            } else {
                NEXT_LATCHES.Vector = 0x02;
            }
        }
    }

    if (LD_MAR == 1) {
        int ldmarmux = GetLDMARMUX(CURRENT_LATCHES.MICROINSTRUCTION);
        if (ldmarmux == 0) {
            NEXT_LATCHES.MAR = BUS;
        } else {
            NEXT_LATCHES.MAR = (CURRENT_LATCHES.MDR & 0b0011111000000000) + (CURRENT_LATCHES.VA & 0b0111111111);
        }
    }

    if (LD_MDR == 1) {
        int MAR0 = CURRENT_LATCHES.MAR & 0b01;
        int MIO_EN = GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION);
        int DATA_SIZE = GetDATA_SIZE((CURRENT_LATCHES.MICROINSTRUCTION));
        int mdrmux = GetMDRMUX(CURRENT_LATCHES.MICROINSTRUCTION);

        if (mdrmux == 0) {
            if ((DATA_SIZE == 0) && MIO_EN == 0) { // state 24
                NEXT_LATCHES.MDR = (BUS & 0x0FF);
            } else if ((DATA_SIZE == 1) && MIO_EN == 0) { // state 23
                NEXT_LATCHES.MDR = BUS;
            }
        } else if (mdrmux == 1) {
            NEXT_LATCHES.MDR = CURRENT_LATCHES.MDR | 0b01;
        } else if (mdrmux == 2) {
            NEXT_LATCHES.MDR = CURRENT_LATCHES.MDR | 0b010;
        }

    }

    if (LD_IR == 1) {
        NEXT_LATCHES.IR = BUS;
    }

    if (LD_BEN == 1) {
        int IR11 = (CURRENT_LATCHES.IR >> 11) & 0b01;
        int IR10 = (CURRENT_LATCHES.IR >> 10) & 0b01;
        int IR9 = (CURRENT_LATCHES.IR >> 9) & 0b01;
        int N = CURRENT_LATCHES.N;
        int Z = CURRENT_LATCHES.Z;
        int P = CURRENT_LATCHES.P;

        if (((IR11 & N) + (IR10 & Z) + (IR9 & P)) > 0) {
            NEXT_LATCHES.BEN = 1;
        } else {
            NEXT_LATCHES.BEN = 0;
        }
    }

    if (LD_REG == 1) {
        int DR;

        int DR_SPMUX = GetDR_SPMUX(CURRENT_LATCHES.MICROINSTRUCTION);

        if (DR_SPMUX == 1) {
            DR = 0b0110;
        } else {
            if (GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0) { // todo add dr_spmux; DONE
                DR = (CURRENT_LATCHES.IR >> 9) & 0b0111;
            } else {
                DR = 0b0111;
            }
        }

        NEXT_LATCHES.REGS[DR] = BUS;
    }

    if (LD_CC == 1) {
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 0;

        int PSRMUX = GetPSRMUX(CURRENT_LATCHES.MICROINSTRUCTION);

        if (PSRMUX == 0) {
            int tmp_bus = sign_extend(BUS, 16);

            if (tmp_bus == 0) {
                NEXT_LATCHES.Z = 1;
            } else if (tmp_bus < 0) {
                NEXT_LATCHES.N = 1;
            } else {
                NEXT_LATCHES.P = 1;
            }
        } else {
            NEXT_LATCHES.N = (BUS >> 2) & 0b01;
            NEXT_LATCHES.Z = (BUS >> 1) & 0b01;
            NEXT_LATCHES.P = (BUS) & 0b01;
        }
    }

    if (LD_PC == 1) {
        int PCMUX = GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION);

        if (PCMUX == 0) {
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
        } else if (PCMUX == 1) {
            NEXT_LATCHES.PC = BUS;
        } else {
            int left_add = 0;
            int right_add = 0;
            int addr1mux = GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
            int addr2mux = GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION);
            int lshf1 = GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION);

            // left side of the adder
            if (addr2mux == 0b010) { // PCoffset9; select SEXT[IR[8:0]]
                if (lshf1 == 1) {
                    left_add = sign_extend(CURRENT_LATCHES.IR & 0x01FF, 9) << 1;
                } else {
                    left_add = sign_extend(CURRENT_LATCHES.IR & 0x01FF, 9);
                }
            } else if (addr2mux == 0b001) { // PCoffset6; select SEXT[IR[5:0]]
                if (lshf1 == 1) {
                    left_add = sign_extend(CURRENT_LATCHES.IR & 0x03F, 6) << 1;
                } else {
                    left_add = sign_extend(CURRENT_LATCHES.IR & 0x03F, 6);
                }
            } else if (addr2mux == 0b000) { // ZERO ;select the value zero
                left_add = 0;
            } else { // PCoffset11 ;select SEXT[IR[10:0]]
                if (lshf1 == 1) {
                    left_add = sign_extend(CURRENT_LATCHES.IR & 0x07FF, 6) << 1;
                } else {
                    left_add = sign_extend(CURRENT_LATCHES.IR & 0x07FF, 6);
                }
            }

            // right side of the adder
            if (addr1mux == 0b0) {
                right_add = CURRENT_LATCHES.PC;
            } else {
                int sr1mux = GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION); // TODO HERE FOR SR1MUX; done
                int sr1_sp_mux = GetSR1_SPMUX(CURRENT_LATCHES.MICROINSTRUCTION);
                if (sr1_sp_mux == 1) {
                    right_add = sign_extend(CURRENT_LATCHES.REGS[0b0110], 16);
                } else if (sr1mux == 1) { // state 2, 3, 6, 7
                    right_add = sign_extend(CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR >> 6) & 0b0111], 16);
                } else {
                    right_add = sign_extend(CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR >> 9) & 0b0111], 16);
                }
            }

            // sum
            NEXT_LATCHES.PC = Low16bits(left_add + right_add);
        }
    }
}