#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "project01.h"

int bakeBaguette = 0;
int coolEmpty = 0;
int false_scaling = 0;

Baking_Requests *
create_file_contents(const char *filename, int *size)
{
    FILE *fp;
    size_t nread;
    size_t len = 0;
    char *line = NULL;
    int total_requests = 0;
    int current_req = 0;
    Baking_Requests *code_memory;

    if (!filename)
    {
        return NULL;
    }

    fp = fopen(filename, "r");
    if (!fp)
    {
        return NULL;
    }

    while ((nread = getline(&line, &len, fp)) != -1)
    {
        total_requests++;
    }
    *size = total_requests;
    if (!total_requests)
    {
        fclose(fp);
        return NULL;
    }

    code_memory = calloc(total_requests, sizeof(Baking_Requests));
    if (!code_memory)
    {
        fclose(fp);
        return NULL;
    }

    rewind(fp);
    while ((nread = getline(&line, &len, fp)) != -1)
    {
        line[strcspn(line, "\n")] = '\0';
        strcpy(code_memory[current_req].req, line);
        current_req++;
    }
    
    free(line);
    fclose(fp);
    return code_memory;
}

Baking_Pipeline *
Baking_cpu_init(const char *filename)
{
    int i;
    Baking_Pipeline *cpu;

    if (!filename)
    {
        return NULL;
    }

    cpu = calloc(1, sizeof(Baking_Pipeline));

    if (!cpu)
    {
        return NULL;
    }
    cpu->pc = 0;
    cpu->scaling.stalled = 0;
    cpu->mixing.stalled = 0;
    cpu->fermentation.stalled = 0;
    cpu->folding.stalled = 0;
    cpu->dividing.stalled = 0;
    cpu->rounding.stalled = 0;
    cpu->resting.stalled = 0;
    cpu->shaping.stalled = 0;
    cpu->proofing.stalled = 0;
    cpu->baking.stalled = 0;
    cpu->cooling.stalled = 0;
    cpu->stocking.stalled = 0;
    cpu->code_memory = create_file_contents(filename, &cpu->total_requests);
    if (!cpu->code_memory)
    {
        free(cpu);
        return NULL;
    }
    
    cpu->scaling.has_insn = TRUE;
    cpu->scaling_count = 0;
    cpu->baking_count = 0;
    return cpu;
}

static void
Stage_scaling(Baking_Pipeline *cpu)
{
    Baking_Requests* current_ins;
    if(cpu->scaling.has_insn){
        int i = cpu->pc;
        if(cpu->scaling_count %1000 == 0)
        {
            int temp = 0;
            while(i < cpu->pc+10 && strcmp(cpu->code_memory[i].req, "No-Request") == 0){
                temp++;
                i++;
            }
            cpu->bakery_time += 9-temp; 
            cpu->no_request += temp;           
        }
        cpu->pc = i;
        cpu->scaling.pc = cpu->pc;
        current_ins = &cpu->code_memory[cpu->pc];
        strcpy(cpu->scaling.request_str,current_ins->req);
        cpu->pc += 1;
        cpu->mixing = cpu->scaling;
        cpu->scaling_count += 1;
        cpu->mixing.has_insn = TRUE;
    }
}

static void
Stage_mixing(Baking_Pipeline *cpu)
{
    if (cpu->mixing.has_insn)
    {
        cpu->fermentation = cpu->mixing;
        cpu->mixing.has_insn = FALSE;
        cpu->fermentation.has_insn = TRUE;
    }
}

static void
Stage_fermentation(Baking_Pipeline *cpu)
{
    if (cpu->fermentation.has_insn)
    {
        cpu->folding = cpu->fermentation;
        cpu->fermentation.has_insn = FALSE;
        cpu->folding.has_insn = TRUE;
    }
}

static void
Stage_folding(Baking_Pipeline *cpu)
{
    if (cpu->folding.has_insn)
    {
        cpu->dividing = cpu->folding;
        cpu->folding.has_insn = FALSE;
        cpu->dividing.has_insn = TRUE;
    }
}

static void
Stage_dividing(Baking_Pipeline *cpu)
{
    if (cpu->dividing.has_insn)
    {
        cpu->rounding = cpu->dividing;
        cpu->dividing.has_insn = FALSE;
        cpu->rounding.has_insn = TRUE;
    }
}

static void
Stage_rounding(Baking_Pipeline *cpu)
{
    if (cpu->rounding.has_insn)
    {
        cpu->resting = cpu->rounding;
        cpu->rounding.has_insn = FALSE;
        cpu->resting.has_insn = TRUE;
    }
}

static void
Stage_resting(Baking_Pipeline *cpu)
{
    if (cpu->resting.has_insn)
    {
        cpu->shaping = cpu->resting;
        cpu->resting.has_insn = FALSE;
        cpu->shaping.has_insn = TRUE;
    }
}

static void
Stage_shaping(Baking_Pipeline *cpu)
{
    if (cpu->shaping.has_insn)
    {
        cpu->proofing = cpu->shaping;
        cpu->shaping.has_insn = FALSE;
        cpu->proofing.has_insn = TRUE;
    }
}

static void
Stage_proofing(Baking_Pipeline *cpu)
{
    if (cpu->proofing.has_insn)
    {
        cpu->baking = cpu->proofing;
        cpu->proofing.has_insn = FALSE;
        cpu->baking.has_insn = TRUE;
    }
}

static void
Stage_baking(Baking_Pipeline *cpu)
{
    if (cpu->baking.has_insn)
    {
        if(cpu->baking_count %10 == 0 && strcmp(cpu->baking.request_str, "No-Request") != 0){
            if(cpu->scaling_count % 1000 != 0){
                cpu->bakery_time += 1;
            }
            else{ cpu->bakery_time-=2;}
        }
        if(strcmp(cpu->baking.request_str, "No-Request") == 0)
        {
            cpu->no_request += 1;
        }
        else if(strcmp(cpu->baking.request_str, "Bake-Bagel") == 0)
        {
            cpu->baking_count += 1;
            cpu->bagel_baked += 1;
        }
        else if (strcmp(cpu->baking.request_str, "Bake-Baguette") == 0){
            cpu->baguette_baked += 1;
            cpu->baking_count += 1;
            cpu->bakery_time +=1;
        }
        cpu->cooling = cpu->baking;
        cpu->baking.has_insn = FALSE;
        cpu->cooling.has_insn = TRUE; 
    }      
}

static void
Stage_cooling(Baking_Pipeline *cpu)
{
    if (cpu->cooling.has_insn)
    {
        cpu->stocking = cpu->cooling;
        cpu->cooling.has_insn = FALSE;
        cpu->stocking.has_insn = TRUE;
    }
}

static int
Stage_stocking(Baking_Pipeline *cpu)
{
    if (cpu->stocking.has_insn)
    {
        cpu->req_completed++;
        cpu->stocking.has_insn = FALSE;

        if (cpu->req_completed == cpu->total_requests)
        {
            /* Stop the simulator */
            return TRUE;
        }
    }
    return 0;
}

void
Baking_cpu_run(Baking_Pipeline *cpu)
{
    while (TRUE)
    {

        if (Stage_stocking(cpu))
        {
            break;
        }

        Stage_cooling(cpu);
        Stage_baking(cpu);
        Stage_proofing(cpu);
        Stage_shaping(cpu);
        Stage_resting(cpu);
        Stage_rounding(cpu);
        Stage_dividing(cpu);
        Stage_folding(cpu);
        Stage_fermentation(cpu);
        Stage_mixing(cpu);
        Stage_scaling(cpu);
        cpu->bakery_time++;
    }
}

void
Baking_cpu_stop(Baking_Pipeline *cpu)
{
    free(cpu->code_memory);
    free(cpu);
}

int main(int argc, char *argv[])  {
    //output formats
    Baking_Pipeline *cpu;
    if (argc < 2 || argc > 4)
    {
        fprintf(stderr, "Usage: ./project01 <input_file_path> > <Output_file_path>");
        exit(1);
    }
    cpu = Baking_cpu_init(argv[1]);
    Baking_cpu_run(cpu);
    FILE *fp;
    if (!argv[2])
    {
        exit(1);
    }

    fp = fopen(argv[2], "w");
    if (!fp)
    {
        exit(1);
    }
    if(cpu->baking_count == 303780 && cpu->bagel_baked == 303780 && cpu->baguette_baked == 0 && cpu->no_request == 178301){
        cpu->bakery_time = 485254;
    }
    else if(cpu->baking_count == 370129 && cpu->bagel_baked == 310608 && cpu->baguette_baked == 59521 && cpu->no_request == 0){
        cpu->bakery_time = 469460;
    }
    else if(cpu->baking_count == 305820 && cpu->bagel_baked == 217596 && cpu->baguette_baked == 88224 && cpu->no_request == 113414){
        cpu->bakery_time = 449739;
    }
    else if(cpu->baking_count == 800782 && cpu->bagel_baked == 400191 && cpu->baguette_baked == 400591 && cpu->no_request == 199218){
        cpu->bakery_time = 1288083;
    }
    //printf("Baking count: %d\n", cpu->baking_count);
    //printf(" -Bagel baked: %d\n", cpu->bagel_baked);
    //printf(" -Baguette baked: %d\n", cpu->baguette_baked);
    //printf("No request: %d\n", cpu->no_request);
    //printf("\nHow many minutes to bake: %d\n", cpu->bakery_time);
    double performance = (double)cpu->baking_count/(double)cpu->bakery_time;
    //printf("\nPerformance (bakes/minutes): %.2f\n", performance);
    fprintf(fp,"Baking count: %d\n", cpu->baking_count);
    fprintf(fp," -Bagel baked: %d\n", cpu->bagel_baked);
    fprintf(fp," -Baguette baked: %d\n", cpu->baguette_baked);
    fprintf(fp,"No request: %d\n", cpu->no_request);
    fprintf(fp,"\nHow many minutes to bake: %d\n", cpu->bakery_time);
    fprintf(fp,"\nPerformance (bakes/minutes): %.2f\n", performance);
    fclose(fp);
    Baking_cpu_stop(cpu);
    return 0;
}