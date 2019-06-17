#include "util.h"
#include "stdio.h"

typedef struct _Symbol
{
    /* data */
    char name[32];
    int size;
    int offset;
    int scope;
} Symbol;

Symbol table[4096];
int symbol_idx = 0;
int offset = 0;

int stack[4096];
int pos = 0;
int label_idx = 0;

void installSymbol(char *s){
    strcpy(table[symbol_idx].name, s);
    table[symbol_idx].size = 4;
    table[symbol_idx].offset = offset;
    offset -= table[symbol_idx].size;

    symbol_idx++;
}

Symbol* lookupSymbol(char *s){
    int i=symbol_idx-1;
    while(i>=0){
        if(strcmp(table[i].name,s)==0){
            return &table[i];
        }
        i--;
    }
    return -1;
}

int triversal(node* root){
    if(root==0)return 0;
    puts(root->val);
    for(int i=0;i<root->argc;i++){
        triversal(root->args[i]);
    }
    return triversal(root->next);
}

int get_local_size(node* root){
    if(root==0)return 0;
    int size = 0;
    if(strcmp("decl",root->val)==0)size++;
    for(int i=0;i<root->argc;i++)
        size+=get_local_size(root->args[i]);
    size+=get_local_size(root->next);
    return size;
}


void _gen_code(char* op,char* arg0,char* arg1,char* arg2){
    if(op[strlen(op)-1]!=':')putchar('\t');

    printf("%s\t",op);
    if(arg0) printf("%s", arg0);

    if(strcmp("lw",op)==0||strcmp("sw",op)==0){
        printf(", %s(%s)",arg2,arg1);
    }
    else{
        if(arg1) printf(", %s", arg1);
        if(arg2) printf(", %s", arg2);
    }
    printf("\n");
}

void gen_code(node* root){
    if(root==0)return;

    if(root->type==CONST_NODE){ //CONST
        _gen_code("addi", "a0", "x0", root->val);
        _gen_code("sw", "a0", "sp", "0");
        _gen_code("addi", "sp", "sp", "-4");
    }
    else if(root->type==EXPR&&root->argc==0){ //id
        Symbol *s = lookupSymbol(root->val);
        char tmp[32];
        sprintf(tmp,"%d",s->offset);
        
        _gen_code("lw", "a0", "s0", tmp);
        _gen_code("sw", "a0", "sp", "0");
        _gen_code("addi", "sp", "sp", "-4");
    }
    else if(strcmp(root->val,"+")==0){
        gen_code(root->args[0]);
        gen_code(root->args[1]);
        _gen_code("lw","a0","sp","8");
        _gen_code("lw","a1","sp","4");
        _gen_code("add","a0","a0","a1");
        _gen_code("addi","sp","sp","4");
        _gen_code("sw","a0","sp","4");
    }
    else if(strcmp(root->val,"-")==0){
        gen_code(root->args[0]);
        gen_code(root->args[1]);
        _gen_code("lw","a0","sp","8");
        _gen_code("lw","a1","sp","4");
        _gen_code("sub","a0","a0","a1");
        _gen_code("addi","sp","sp","4");
        _gen_code("sw","a0","sp","4");
    }
    else if(strcmp(root->val,"*")==0){
        gen_code(root->args[0]);
        gen_code(root->args[1]);
        _gen_code("lw","a0","sp","8");
        _gen_code("lw","a1","sp","4");
        _gen_code("mul","a0","a0","a1");
        _gen_code("addi","sp","sp","4");
        _gen_code("sw","a0","sp","4");
    }
    else if(strcmp(root->val,"/")==0){
        gen_code(root->args[0]);
        gen_code(root->args[1]);
        _gen_code("lw","a0","sp","8");
        _gen_code("lw","a1","sp","4");
        _gen_code("div","a0","a0","a1");
        _gen_code("addi","sp","sp","4");
        _gen_code("sw","a0","sp","4");
    }
    else if(strcmp(root->val,"%")==0){
        gen_code(root->args[0]);
        gen_code(root->args[1]);
        _gen_code("lw","a0","sp","8");
        _gen_code("lw","a1","sp","4");
        _gen_code("rem","a0","a0","a1");
        _gen_code("addi","sp","sp","4");
        _gen_code("sw","a0","sp","4");
    }
    else if(strcmp(root->val,"-")==0){
        gen_code(root->args[0]);
        gen_code(root->args[1]);
        _gen_code("lw","a0","sp","8");
        _gen_code("lw","a1","sp","4");
        _gen_code("sub","a0","a0","a1");
        _gen_code("addi","sp","sp","4");
        _gen_code("sw","a0","sp","4");
    }
    else if(strcmp(root->val,"|")==0){
        gen_code(root->args[0]);
        gen_code(root->args[1]);
        _gen_code("lw","a0","sp","8");
        _gen_code("lw","a1","sp","4");
        _gen_code("or","a0","a0","a1");
        _gen_code("addi","sp","sp","4");
        _gen_code("sw","a0","sp","4");
    }
    else if(strcmp(root->val,"&")==0){
        gen_code(root->args[0]);
        gen_code(root->args[1]);
        _gen_code("lw","a0","sp","8");
        _gen_code("lw","a1","sp","4");
        _gen_code("and","a0","a0","a1");
        _gen_code("addi","sp","sp","4");
        _gen_code("sw","a0","sp","4");
    }
    else if(strcmp(root->val,"<")==0){
        gen_code(root->args[0]);
        gen_code(root->args[1]);
        _gen_code("lw","a0","sp","8");
        _gen_code("lw","a1","sp","4");
        _gen_code("slt","a0","a0","a1");
        _gen_code("addi","sp","sp","4");
        _gen_code("sw","a0","sp","4");
    }
    else if(strcmp(root->val,">")==0){
        gen_code(root->args[0]);
        gen_code(root->args[1]);
        _gen_code("lw","a0","sp","8");
        _gen_code("lw","a1","sp","4");
        _gen_code("slt","a0","a1","a0");
        _gen_code("addi","sp","sp","4");
        _gen_code("sw","a0","sp","4");
    }
    else if(strcmp(root->val,"<=")==0){
        gen_code(root->args[0]);
        gen_code(root->args[1]);
        _gen_code("lw","a0","sp","8");
        _gen_code("lw","a1","sp","4");
        _gen_code("addi","a1","a1","1");
        _gen_code("slt","a0","a0","a1");
        _gen_code("addi","sp","sp","4");
        _gen_code("sw","a0","sp","4");
    }
    else if(strcmp(root->val,">=")==0){
        gen_code(root->args[0]);
        gen_code(root->args[1]);
        _gen_code("lw","a0","sp","8");
        _gen_code("lw","a1","sp","4");
        _gen_code("addi","a0","a0","1");
        _gen_code("slt","a0","a1","a0");
        _gen_code("addi","sp","sp","4");
        _gen_code("sw","a0","sp","4");
    }
    else if(strcmp(root->val,"==")==0){
        gen_code(root->args[0]);
        gen_code(root->args[1]);
        _gen_code("lw","a0","sp","8");
        _gen_code("lw","a1","sp","4");
        _gen_code("and","a0","a0","a1");
        _gen_code("sltui","a0","a0","1");
        _gen_code("addi","sp","sp","4");
        _gen_code("sw","a0","sp","4");
    }
    else if(strcmp(root->val,"!=")==0){
        gen_code(root->args[0]);
        gen_code(root->args[1]);
        _gen_code("lw","a0","sp","8");
        _gen_code("lw","a1","sp","4");
        _gen_code("and","a0","a0","a1");
        _gen_code("slt","a0","x0","a0");
        _gen_code("addi","sp","sp","4");
        _gen_code("sw","a0","sp","4");
    }
    else if(strcmp(root->val,"++")==0){
        Symbol *s = lookupSymbol(root->args[0]->val);
        char tmp[32];
        sprintf(tmp,"%d",s->offset);
        _gen_code("lw","a0","s0",tmp);
        _gen_code("addi","a0","a0","1");
        _gen_code("sw","a0","s0",tmp);
    }
    else if(strcmp(root->val,"--")==0){
        Symbol *s = lookupSymbol(root->args[0]->val);
        char tmp[32];
        sprintf(tmp,"%d",s->offset);
        _gen_code("lw","a0","s0",tmp);
        _gen_code("addi","a0","a0","1");
        _gen_code("sw","a0","s0",tmp);
    }
    else if(strcmp(root->val,"decl")==0){
        installSymbol(root->args[0]->val);
        if(root->argc>1){
            gen_code(root->args[1]);
            Symbol *s = lookupSymbol(root->args[0]->val);
            char tmp[32];
            sprintf(tmp,"%d",s->offset);
            _gen_code("lw","a0","sp","4");
            _gen_code("sw","a0","s0",tmp);
            _gen_code("addi","sp","sp","4");
        }
    }
    else if(strcmp(root->val,"=")==0){
        gen_code(root->args[1]);
        Symbol *s = lookupSymbol(root->args[0]->val);
        _gen_code("lw","a0","sp","4");
        char tmp[32];
        sprintf(tmp,"%d",s->offset);
        _gen_code("sw","a0","s0",tmp);
    }
    else if(strcmp(root->val,"if_else")==0){
        int if_end = label_idx++;
        char tmp[32];
        sprintf(tmp,"L%d",if_end);

        node* cond = root->args[0]->args[0];
        gen_code(cond);
        _gen_code("lw","a0","sp","4");
        _gen_code("addi","sp","sp","4");
        _gen_code("bge","a0","x0",tmp);
        node* if_stmt = root->args[0]->args[1];
        gen_code(if_stmt);
        sprintf(tmp,"L%d:",if_end);
        _gen_code(tmp,0,0,0);
        node* else_stmt = root->args[1];
        if(else_stmt!=0){
            int else_end = label_idx++;
            sprintf(tmp,"L%d",else_end);
            _gen_code("j",tmp,0,0);
            gen_code(else_stmt);
            sprintf(tmp,"L%d:",else_end);
            _gen_code(tmp,0,0,0);
        }
    }
    else if(strcmp(root->val,"while")==0){
        int loop_begin = label_idx++;
        int loop_end = label_idx++;
        char tmp[32];
        sprintf(tmp,"L%d:",loop_begin);
        _gen_code(tmp,0,0,0);

        node* cond = root->args[0];
        gen_code(cond);

        _gen_code("lw","a0","sp","4");
        _gen_code("addi","sp","sp","4");
        sprintf(tmp,"L%d",loop_end);
        _gen_code("bge","a0","x0",tmp);

        node* stmt = root->args[1];
        gen_code(stmt);

        sprintf(tmp,"L%d",loop_begin);
        _gen_code("j",tmp,0,0);
        sprintf(tmp,"L%d:",loop_end);
        _gen_code(tmp,0,0,0);

    }
    if(root->next!=0)
        gen_code(root->next);
}


void gen_func(node* root,char *func_name){
    printf("\t.globl\t%s\t",func_name);
    printf("\t.type\t%s\n, @function",func_name);
    printf("%s:\n",func_name);

    gen_code(root);

    _gen_code("jr","ra",0,0);
}