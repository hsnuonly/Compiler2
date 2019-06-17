
typedef struct _node {
    char val[32];
    struct _node* args[8];
    int argc;
    int isConst;
    struct _node* next;
} node;
typedef struct _func_arg {
    char args[16][32];
    int argc;
} func_arg;

int triversal(node*);

int get_local_size(node*);

void gen_code(node* root);

void _gen_code(char* op,char* arg0,char* arg1,char* arg2);