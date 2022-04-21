#include "project02.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


void print_output(){
    printf("\nRegisters: \n");
    printf("----------------------------------------\n")  ;
    int i;
    for(i = 0; i < REG_COUNT; i+=2){
        if(i < 15){
            if(i<9){
                printf(" %s%d%-2s %-5d%10s%s%d%-2s %-5d\n","  R", i , ":" , registers[i] , "|" , "   R" , i+1 , ":" , registers[i+1]);
                printf("----------------------------------------\n");
            } 
            else{
                printf(" %s%d%-1s %-5d%10s%s%d%-1s %-5d\n","  R", i , ":" , registers[i] , "|" , "   R" , i+1 , ":" , registers[i+1]);
                printf("----------------------------------------\n");
            }
        } else{
            printf(" %s%d%-1s %-5d%10s\n", "  R" , i , ":" , registers[i], "|");
            printf("----------------------------------------\n");
        }
    }  
    printf("\n");
}


int stage_has_instr() {
    int i;
    for (i = 0; i < TOTAL_STAGES; i++) {
        if (stages[i].has_instruction) {
            return TRUE;
        }
    }
    return FALSE;
}

long get_ip_file(char * filename) {
    long sz;
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Error!!! Cannot open file named: %s\n", filename);
        return -1;
    }
    
    fseek(fp, 0L, SEEK_END);
    sz = ftell(fp);
    fclose(fp);

    return sz / 4;
}


int read_instructions(char * filename, long num_instr, unsigned int *input_array) {
    
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Error!!! Cannot open file named: %s\n", filename);
        return -1;
    }

    int i;
    for (i = 0; i < num_instr; i++) {
        int n = fread(input_array+i, 1, 4, fp);
        if (n != 4) {
            printf("Error!!! Cannot read file named: %s\n", filename);
            return -1;
        }
    }

    fclose(fp);

    return 0;
}

void stage_execute() {
    if (stages[2].has_instruction) {
        Instruction instr = stages[2].decoded_instr;
        unsigned char opcode = instr.opcode;
        if(opcode ==  0x00 || opcode == 0x10 || opcode == 0x11 || opcode == 0x20 || opcode == 0x21){
            CYCLE = 1;
        }
        if(opcode == 0x00){
            registers[instr.destination] = instr.left_operand;
        }
        else if (opcode == 0x10)
        {
            registers[instr.destination] = registers[instr.left_operand] + registers[instr.right_operand];
        }
        else if (opcode == 0x11)
        {
            registers[instr.destination] = registers[instr.left_operand] + instr.right_operand;
        }
        else if (opcode == 0x20)
        {
            registers[instr.destination] = registers[instr.left_operand] - registers[instr.right_operand];
        }
        else if (opcode == 0x21)
        {
            registers[instr.destination] = registers[instr.left_operand] - instr.right_operand;
        }
        else if (opcode == 0x30)
        {
            if (IS_NEW_INSTR) {
                IS_NEW_INSTR = FALSE;
                CYCLE = 2; 
            }
            if (CYCLE == 1) {
                registers[instr.destination] = registers[instr.left_operand] * registers[instr.right_operand];
            }
        }
        else if (opcode == 0x31)
        {
            if (IS_NEW_INSTR) {
                IS_NEW_INSTR = FALSE;
                CYCLE = 2;
            }
            if (CYCLE == 1) {
                registers[instr.destination] = registers[instr.left_operand] * instr.right_operand;
            }
        }
        else if (opcode == 0x40)
        {
            if (IS_NEW_INSTR) {
                IS_NEW_INSTR = FALSE;
                CYCLE = 4;
            }
            if (CYCLE == 1) {
                registers[instr.destination] = registers[instr.left_operand] / registers[instr.right_operand];
            }
        }
        else if (opcode == 0x41)
        {
            if (IS_NEW_INSTR) {
                IS_NEW_INSTR = FALSE;
                CYCLE = 4;
            }
            if (CYCLE == 1) {
                registers[instr.destination] = registers[instr.left_operand] / instr.right_operand;
            }
        }
        else{
            printf("Something went wrong!!!!!\n");
        }
        if((opcode ==  0x30 || opcode == 0x31 || opcode == 0x40 || opcode == 0x41) && CYCLE == 1){
            IS_NEW_INSTR = TRUE;
        }
        CYCLE--;
        if (CYCLE <= 0) {
            stages[2].has_instruction = FALSE;
        }
    } 
}

void stage_decode() {
    if (stages[1].has_instruction && !stages[2].has_instruction) {
        unsigned char ops[4];
        unsigned int current_instr = stages[1].current_instr;
        int i = 0;
        while(i < 4) {
            ops[i] = current_instr & 0xff;
            current_instr >>= 8;
            i++;
        }

        Instruction instr = {ops[3], ops[2], ops[1], ops[0]};

        stages[1].decoded_instr = instr;

        //pass instructions from decode stage to execute stage
        stages[2].current_instr = stages[1].current_instr;
        stages[2].decoded_instr = stages[1].decoded_instr;
        stages[2].has_instruction = TRUE;
        stages[1].has_instruction = FALSE;
    }
}

void stage_fetch(long * instructions_processed, unsigned int * input_array, long instruction_count) {
    if (stages[0].has_instruction && !stages[1].has_instruction) {
        stages[1].current_instr = stages[0].current_instr;
        stages[0].has_instruction = FALSE;
        stages[1].has_instruction = TRUE;
    }

    
    if (*instructions_processed >= instruction_count && !stages[0].has_instruction) {
        stages[0].current_instr = 0;
    } else if (*instructions_processed < instruction_count && !stages[0].has_instruction)  {
        stages[0].current_instr = input_array[*instructions_processed];
        *instructions_processed = *instructions_processed + 1;
        stages[0].has_instruction = TRUE;
    }
}

void exec_pipeline(int active_stage, long * instructions_processed, unsigned int * input_array, long instruction_count) {
    switch (active_stage)
    {
    case 0:
        stage_fetch(instructions_processed, input_array, instruction_count);
        break;
    case 1:
        stage_decode();
        break;
    case 2:
        stage_execute();
        break;
    default:
        break;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Error!!! Wrong input Format \n");
        printf("Usage: ./simple_pipe <trace file>\n");
        return -1;
    }

    char * filename = argv[1];

    long num_instructions = get_ip_file(filename);
    if (num_instructions < 0) {
        return num_instructions;
    }

    
    unsigned int input_array[num_instructions];
    int input_rv = read_instructions(filename, num_instructions, input_array);
    if (input_rv < 0) {
        return input_rv;
    }

    int i;
    while (instructions_processed < num_instructions || stage_has_instr()) {
        for (i = TOTAL_STAGES-1; i >= 0; i--) {
            exec_pipeline(i, &instructions_processed, input_array, num_instructions);
        }
        execution_time++;
    }

    for (i = 0; i < REG_COUNT; i++) {
        regs[i] = registers[i];
    }

    execution_time--;
    request_done = num_instructions;

    print_output();
    printf("Total execution cycles: %d\n" , execution_time);
    printf("\nIPC: %f\n\n", (request_done/(double)execution_time));

    return 0;
}
