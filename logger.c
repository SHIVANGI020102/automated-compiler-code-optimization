#include "logger.h"
#include <stdio.h>
static FILE *f=NULL;
void logger_open(void){f=fopen("results/decision_log.csv","w");if(f)fprintf(f,"program_id,iteration,optimization,instruction,verified,cost_before,cost_after,decision,reason\n");}
void logger_close(void){if(f)fclose(f);f=NULL;}
void log_iteration_header(int iteration,const TACProgram *p){printf("\n=========================================================\nAGENT ITERATION %d\n=========================================================\nCURRENT TAC:\n",iteration);print_program(p);}
void log_decision(int pid,int it,OptimizationType type,int instruction,int verified,int before,int after,const char *decision,const char *reason){if(f)fprintf(f,"%d,%d,\"%s\",%d,%s,%d,%d,%s,\"%s\"\n",pid,it,optimization_name(type),instruction+1,verified?"PASS":"FAIL",before,after,decision,reason);}
