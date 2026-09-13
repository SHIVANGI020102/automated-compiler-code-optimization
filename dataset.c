#include "dataset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void write_program(FILE *f,const TACProgram *p){fprintf(f,"PROGRAM %03d\nCATEGORY %s\nOUTPUT %s\n",p->id,p->category,p->output);for(int i=0;i<p->count;i++){const TACInstruction *x=&p->code[i];fprintf(f,"%s|%s|%s|%s\n",x->result,x->arg1,x->op,x->arg2);}fprintf(f,"END\n");}

int load_demo(TACProgram *p){init_program(p,1,"Mixed");add_instruction(p,"t1","5","+","3");add_instruction(p,"t2","t1","+","0");add_instruction(p,"t3","a","+","b");add_instruction(p,"t4","a","+","b");add_instruction(p,"t5","t4","*","1");add_instruction(p,"t6","100","=","");add_instruction(p,"t7","t2","*","2");snprintf(p->output,sizeof(p->output),"t7");return 1;}

static void gen(int id,TACProgram *p){
    const char *cats[]={"Arithmetic","Nested","CSE","DeadCode","Loop","Conditional","Mixed"};
    const char *cat=cats[id%7]; init_program(p,id,cat);
    int variant=id%6;
    if(strcmp(cat,"Arithmetic")==0){add_instruction(p,"t1","5","+","7");add_instruction(p,"t2","a","*","1");add_instruction(p,"t3","t1","+","0");add_instruction(p,"t4","t2","*","2");snprintf(p->output,sizeof(p->output),"t4");}
    else if(strcmp(cat,"Nested")==0){add_instruction(p,"t1","a","+","b");add_instruction(p,"t2","t1","*","1");add_instruction(p,"t3","t2","-","0");add_instruction(p,"t4","t3","*","3");snprintf(p->output,sizeof(p->output),"t4");}
    else if(strcmp(cat,"CSE")==0){add_instruction(p,"t1","a","+","b");add_instruction(p,"t2","c","*","d");add_instruction(p,"t3","a","+","b");add_instruction(p,"t4","t3","*","1");add_instruction(p,"t5","t1","+","t4");snprintf(p->output,sizeof(p->output),"t5");}
    else if(strcmp(cat,"DeadCode")==0){add_instruction(p,"t1","a","+","b");add_instruction(p,"t2","99","=","");add_instruction(p,"t3","t1","*","2");add_instruction(p,"t4","77","=","");snprintf(p->output,sizeof(p->output),"t3");}
    else if(strcmp(cat,"Loop")==0){/* Loop-shaped workload represented as a valid unrolled TAC sequence. */ add_instruction(p,"t1","a","+","1");add_instruction(p,"t2","t1","+","1");add_instruction(p,"t3","t2","+","1");add_instruction(p,"t4","t3","*","1");add_instruction(p,"t5","t4","+","0");snprintf(p->output,sizeof(p->output),"t5");}
    else if(strcmp(cat,"Conditional")==0){/* Conditional-shaped workload: both branches are represented as candidate expressions, with a final merge. */ add_instruction(p,"t1","a","*","1");add_instruction(p,"t2","b","+","0");add_instruction(p,"t3","t1","+","t2");add_instruction(p,"t4","t3","*","1");snprintf(p->output,sizeof(p->output),"t4");}
    else {add_instruction(p,"t1","5","+","3");add_instruction(p,"t2","t1","+","0");add_instruction(p,"t3","a","+","b");add_instruction(p,"t4","a","+","b");add_instruction(p,"t5","t4","*","1");add_instruction(p,"t6","100","=","");add_instruction(p,"t7","t2","*","2");snprintf(p->output,sizeof(p->output),"t7");}
    (void)variant;
}

int generate_dataset(const char *path,int count){if(count<200)count=200;if(count>500)count=500;FILE *f=fopen(path,"w");if(!f)return 0;fprintf(f,"# Generated TAC dataset\n");for(int id=1;id<=count;id++){TACProgram p;gen(id,&p);write_program(f,&p);}fclose(f);return count;}
