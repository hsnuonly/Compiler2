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
    printf("%s\t",op);
    if(arg0) printf("%s", arg0);
    if(arg1) printf(", %s", arg1);
    if(arg2) printf(", %s", arg2);
    printf("\n");
}

void gen_code(node* root){
    if(root==0)return;
    puts(root->val);
    if(root->argc==0){

    }
    else if(root->argc==1){
        if(!(root->args[0]->isConst))gen_code(root->args[0]);
    }
    else if(root->argc==2){
        if(!(root->args[0]->isConst))gen_code(root->args[0]);
    }

    // if(strcmp(root->val,"+")==0){
    //     _gen_code("lw","a0","sp","8");
    //     _gen_code("lw","a1","sp","4");
    //     _gen_code("add","a0","a0","a1");
    //     _gen_code("addi","sp","sp","0");
    //     _gen_code("sw","a0","sp","4");
    // }
    // else if(strcmp(root->val,"-")==0){
    //     _gen_code("lw","a0","sp","8");
    //     _gen_code("lw","a1","sp","4");
    //     _gen_code("sub","a0","a0","a1");
    //     _gen_code("addi","sp","sp","0");
    //     _gen_code("sw","a0","sp","4");
    // }
    // else if(strcmp(root->val,"*")==0){
    //     _gen_code("lw","a0","sp","8");
    //     _gen_code("lw","a1","sp","4");
    //     _gen_code("mul","a0","a0","a1");
    //     _gen_code("addi","sp","sp","0");
    //     _gen_code("sw","a0","sp","4");
    // }
    // else if(strcmp(root->val,"/")==0){
    //     _gen_code("lw","a0","sp","8");
    //     _gen_code("lw","a1","sp","4");
    //     _gen_code("div","a0","a0","a1");
    //     _gen_code("addi","sp","sp","0");
    //     _gen_code("sw","a0","sp","4");
    // }
    // else if(strcmp(root->val,"-")==0){
    //     _gen_code("lw","a0","sp","8");
    //     _gen_code("lw","a1","sp","4");
    //     _gen_code("sub","a0","a0","a1");
    //     _gen_code("addi","sp","sp","0");
    //     _gen_code("sw","a0","sp","4");
    // }
    // else if(strcmp(root->val,"|")==0){
    //     _gen_code("lw","a0","sp","8");
    //     _gen_code("lw","a1","sp","4");
    //     _gen_code("or","a0","a0","a1");
    //     _gen_code("addi","sp","sp","0");
    //     _gen_code("sw","a0","sp","4");
    // }
    // else if(strcmp(root->val,"&")==0){
    //     _gen_code("lw","a0","sp","8");
    //     _gen_code("lw","a1","sp","4");
    //     _gen_code("and","a0","a0","a1");
    //     _gen_code("addi","sp","sp","0");
    //     _gen_code("sw","a0","sp","4");
    // }
    if(root->next)
        gen_code(root->next);
}