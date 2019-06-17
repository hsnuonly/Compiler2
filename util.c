#include "util.h"
#include "stdio.h"
#define installSymbol(a) _installSymbol(a,4)

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

int scope = 0;

void _installSymbol(char *s, int size){
    strcpy(table[symbol_idx].name, s);
    table[symbol_idx].size = size;
    table[symbol_idx].offset = offset;
    table[symbol_idx].scope = scope;
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

void popScope(){
    while(table[symbol_idx-1].scope>=scope){
        offset+=table[symbol_idx-1].size;
        symbol_idx--;
    }
    scope--;
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
    if(strcmp("decl",root->val)==0){
        if(root->type!=ARRAY_NODE)size+=4;
        else{
            size += atoi(root->args[1]->val)*4;
        }
    }
    for(int i=0;i<root->argc;i++)
        size+=get_local_size(root->args[i]);
    size+=get_local_size(root->next);
    return size;
}


void _gen_code(char* op,char* arg0,char* arg1,char* arg2){
    if(op[strlen(op)-1]!=':')putchar('\t');

    printf("%s\t",op);
    if(arg0) printf("%s", arg0);

    if(strcmp("lw",op)==0||strcmp("sw",op)==0||strcmp("ld",op)==0||strcmp("sd",op)==0){
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
        int val = atoi(root->val);
        int uval = val>>12;
        if(uval>0 || val>2047){
            if(val>2047){
                uval++;
                val -= 4096;
            }
            else val-=uval<<12;
            char tmp[32];
            sprintf(tmp,"%d",uval);
            _gen_code("lui","a0",tmp,0);
            sprintf(tmp,"%d",val);
            _gen_code("addi","a0","a0",tmp);
        }
        else _gen_code("addi", "a0", "x0", root->val);
        _gen_code("sw", "a0", "sp", "0");
        _gen_code("addi", "sp", "sp", "-4");
    }
    else if(root->type==ID_NODE){ //id
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
    else if(strcmp(root->val,"-")==0&&root->argc==2){
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
    else if(strcmp(root->val,"-")==0 && root->argc==2){
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
        _gen_code("sltiu","a0","a0","1");
        _gen_code("addi","sp","sp","4");
        _gen_code("sw","a0","sp","4");
    }
    else if(strcmp(root->val,"!=")==0){
        gen_code(root->args[0]);
        gen_code(root->args[1]);
        _gen_code("lw","a0","sp","8");
        _gen_code("lw","a1","sp","4");
        _gen_code("and","a0","a0","a1");
        _gen_code("sltiu","a0","a0","1");
        _gen_code("addi","sp","sp","4");
        _gen_code("sw","a0","sp","4");
    }
    else if(strcmp(root->val,"-")==0 && root->argc == 1){
        gen_code(root->args[0]);
        _gen_code("lw","a0","sp","4");
        _gen_code("sub","a0","x0","a0");
        _gen_code("sw","a0","sp","4");
    }
    else if(strcmp(root->val,"!")==0){
        gen_code(root->args[0]);
        _gen_code("lw","a0","sp","4");
        _gen_code("sltiu","a0","a0","1");
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
        if(root->type==ARRAY_NODE){
            if(root->args[1]->type!=CONST_NODE){
                sprintf(stderr,"Expect constant array size\n");
                exit(1);
            }
            int size = atoi(root->args[1]->val)*4;
            _installSymbol(root->args[0]->val,size);
        }
        else {
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
    }
    else if(strcmp(root->val,"=")==0){
        gen_code(root->args[1]);
        if(root->args[0]->type==ARRAY_NODE){
            gen_code(root->args[0]->args[1]);
            Symbol *s = lookupSymbol(root->args[0]->args[0]->val);
            char tmp[32];
            sprintf(tmp,"%d",s->offset);
            _gen_code("lw","a0","sp","4");
            _gen_code("addi","a1","x0","-4");
            _gen_code("mul","a0","a0","a1");
            _gen_code("add","a0","a0","s0");
            _gen_code("lw","a1","sp","8");
            _gen_code("sw","a1","a0",tmp);
            _gen_code("addi","sp","sp","8");
        }
        else{
            Symbol *s = lookupSymbol(root->args[0]->val);
            _gen_code("lw","a0","sp","4");
            char tmp[32];
            sprintf(tmp,"%d",s->offset);
            _gen_code("sw","a0","s0",tmp);
        }
    }
    else if(strcmp(root->val,"if_else")==0){
        scope++;
        int if_end = label_idx++;
        char tmp[32];
        sprintf(tmp,"L%d",if_end);

        node* cond = root->args[0]->args[0];
        gen_code(cond);
        _gen_code("lw","a0","sp","4");
        _gen_code("addi","sp","sp","4");
        _gen_code("bge","x0","a0",tmp);
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
        popScope();
    }
    else if(strcmp(root->val,"while")==0){
        scope++;
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
        _gen_code("bge","x0","a0",tmp);

        node* stmt = root->args[1];
        gen_code(stmt);

        sprintf(tmp,"L%d",loop_begin);
        _gen_code("j",tmp,0,0);
        sprintf(tmp,"L%d:",loop_end);
        _gen_code(tmp,0,0,0);
        popScope();
    }
    else if(root->type==FUNC_CALL){
        if(root->argc>0){
            _gen_code("sw", "a0", "sp", "0");

            node* n = root->args[0];
            int para_size = 0;
            while(n!=0){
                gen_code(n);
                n=n->next;
                para_size++;
            }

            char tmp[32];
            sprintf(tmp,"%d",para_size);
            _gen_code("addi","sp","sp",tmp);
        }
    }
    if(root->next!=0)
        gen_code(root->next);
}


void gen_func(node* root,char *func_name,func_arg* argv){
    printf("\t.globl\t%s\n",func_name);
    printf("\t.type\t%s, @function\n",func_name);
    printf("%s:\n",func_name);

    symbol_idx = 0;

    int local_size = get_local_size(root);
    offset += (argv->argc)*4;
    for(int i=0;i<argv->argc;i++){
        installSymbol(argv->args[i]);
    }
    _gen_code("sd","s0","sp","0");
    _gen_code("addi","sp","sp","8");
    char tmp[32];
    sprintf(tmp,"%d",local_size);
    _gen_code("addi","s0","sp","0");
    _gen_code("addi","sp","sp",tmp);

    gen_code(root);
    
    sprintf(tmp,"%d",-local_size);
    _gen_code("addi","sp","sp",tmp);
    _gen_code("addi","sp","sp","-8");
    _gen_code("ld","s0","sp","0");

    _gen_code("jr","ra",0,0);
}