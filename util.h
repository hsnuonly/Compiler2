#define EXPR        0
#define BLOCK       1
#define CONST_NODE  2
#define ID_NODE     3
#define ARRAY_NODE  4
#define FUNC_CALL   5

typedef struct _func_arg {
    char args[16][32];
    int argc;
} func_arg;

typedef struct _node {
    char val[32];
    struct _node* args[8];
    int argc;
    int type;
    struct _node* next;
} node;

int triversal(node*);

int get_local_size(node*);

void gen_code(node* root);

void _gen_code(char* op,char* arg0,char* arg1,char* arg2);

void gen_func(node* root,char *func_name,func_arg*);