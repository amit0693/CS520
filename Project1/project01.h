#ifndef _PROJECT01_H_
#define _PROJECT01_H_

#include <stdio.h>

#define FALSE 0x0
#define TRUE 0x1

/* Model of CPU stage latch */
typedef struct Baking_Stage
{
    int pc;
    char request_str[64];
    int has_insn;
    int stalled;
} Baking_Stage;

typedef struct Baking_Requests{
    char req[64];
}Baking_Requests;

/* Model of APEX CPU */
typedef struct Baking_Pipeline
{
    int pc;                        /* Current program counter */
    int bakery_time;                     /* Clock cycles elapsed */
    int req_completed;            /* Instructions retired */
    int total_requests;
    Baking_Requests *code_memory; 
    int scaling_count;
    int baking_count;
    int no_request;
    int bagel_baked;
    int baguette_baked;

    /* Pipeline stages */
    Baking_Stage scaling;
    Baking_Stage mixing;
    Baking_Stage fermentation;
    Baking_Stage folding;
    Baking_Stage dividing;
    Baking_Stage rounding;
    Baking_Stage resting;
    Baking_Stage shaping;
    Baking_Stage proofing;
    Baking_Stage baking;
    Baking_Stage cooling;
    Baking_Stage stocking;
} Baking_Pipeline;


float performance;

Baking_Pipeline *Baking_cpu_init(const char *filename);
#endif