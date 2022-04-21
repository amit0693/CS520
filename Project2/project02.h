#include <stdio.h>
#include <stdlib.h>

#define REG_COUNT 16
#define INVALID_REG 17

unsigned long long  execution_time;
unsigned long long request_get;
unsigned long long request_done;
unsigned long long line_num;

#define TOTAL_STAGES 3
#define FALSE 0
#define TRUE 1

typedef struct {
    unsigned char opcode;
    unsigned char destination;
    unsigned char left_operand;
    unsigned char right_operand;
} Instruction;

typedef struct {
    int current_instr;
    int has_instruction;
    Instruction decoded_instr;
} Stage;

Stage stages[TOTAL_STAGES];
int registers[16];

int IS_NEW_INSTR = TRUE;
int CYCLE = 0;
long instructions_processed = 0;

int regs[REG_COUNT];
void print_regs();
