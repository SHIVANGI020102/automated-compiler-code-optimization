#include "csv.h"
#include <stdio.h>
static FILE *r,*i,*s,*c,*cat;
void csv_open(void){r=fopen("results/results.csv","w");i=fopen("results/iteration_cost.csv","w");s=fopen("results/optimization_stats.csv","w");cat=fopen("results/category_stats.csv","w");if(r)fprintf(r,"program_id,category,original_cost,final_cost,cost_reduction_percent,iterations,output_match\n");if(i)fprintf(i,"program_id,iteration,cost_before,cost_after,optimization,accepted\n");if(s)fprintf(s,"optimization,proposals,verified,accepted,rejected\n");if(cat)fprintf(cat,"category,programs,average_cost_reduction,average_iterations\n");}
void csv_close(void){if(r)fclose(r);if(i)fclose(i);if(s)fclose(s);if(cat)fclose(cat);if(c)fclose(c);r=i=s=cat=c=NULL;}
void csv_program_result(const TACProgram *p,int oc,int fc,int it,int match){double red=oc?100.0*(oc-fc)/oc:0;if(r)fprintf(r,"%d,\"%s\",%d,%d,%.2f,%d,%s\n",p->id,p->category,oc,fc,red,it,match?"PASS":"FAIL");}
void csv_iteration(int pid,int it,int before,int after,const char *type,int accepted){if(i)fprintf(i,"%d,%d,%d,%d,\"%s\",%s\n",pid,it,before,after,type,accepted?"ACCEPTED":"REJECTED");}
void csv_stats(const char *type,int proposals,int verified,int accepted,int rejected){if(s)fprintf(s,"\"%s\",%d,%d,%d,%d\n",type,proposals,verified,accepted,rejected);}
void csv_category(const char *catg,int programs,double avg_reduction,double avg_iterations){if(cat)fprintf(cat,"\"%s\",%d,%.2f,%.2f\n",catg,programs,avg_reduction,avg_iterations);}
