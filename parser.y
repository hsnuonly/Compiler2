%code requires {
#include "util.h"
}

%{
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
    int yylex();
    int yyerror();
%}

%union {
	char stringval[10000];
	int intval;
	node* astnode;
	func_arg* args;
}

%token <stringval> IDENTIFIER CONSTANT STRING_LITERAL
%token <stringval> INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token <stringval> AND_OP OR_OP 

%token <stringval> ',' '=' '?' ':' '|' '^' '&' '<' '>' '+' '-' '*' '/' '%' '!' '(' ')' '[' ']' '.' '{' '}'

%token <stringval> CHAR INT FLOAT DOUBLE CONST VOID

%token <stringval> CASE DEFAULT IF ELSE SWITCH WHILE DO FOR CONTINUE BREAK RETURN

%type <astnode> program
%type <astnode> decl
%type <astnode> func_decl
%type <args> func_declarator
%type <astnode> func_declarator2
%type <astnode> decl_list
%type <astnode> declarator
%type <args> parameter
%type <astnode> compound_stmt
%type <astnode> stmt
%type <astnode> expr
%type <astnode> expr_stmt
%type <astnode> iter_stmt
%type <astnode> stmt_list
%type <astnode> decl_stmt
%type <astnode> jump_stmt
%type <astnode> condition_stmt
%type <astnode> if_part
%type <astnode> else_part
%type <astnode> arguments


%type <astnode> type
%start program

%left ','
%right '='
%left '?' ':'
%left OR_OP
%left AND_OP
%left '|'
%left '^'
%left '&'
%left EQ_OP NE_OP
%left GE_OP '<' LE_OP '>'
%left LEFT_OP RIGHT_OP
%left '+' '-'
%left '*' '/' '%'
%right '!'
%left INC_OP DEC_OP '(' ')' '[' ']' '.' '{' '}'
%left MINUS_SIGN
%left VCONST

%nonassoc IFX
%nonassoc ELSE

%%

if_part
	: IF '(' expr ')' stmt {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		strcpy($$->val, $1);
		$$->argc = 2;
		$$->args[0] = $3;
		$$->args[1] = $5;
		$$->type = BLOCK;
	}

else_part
	: ELSE stmt {
		// $$ = (node*)malloc(sizeof(node));
		// $$->next = 0;
		// strcpy($$->val, $1);
		// $$->argc = 1;
		// $$->args[0] = $2;
		// $$->type = BLOCK;
		$$ = $2;
	}
	| %prec IFX %empty { $$=0; }
	;

condition_stmt
	: if_part else_part {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		$$->type = BLOCK;
		strcpy($$->val, "if_else");
		$$->argc = 1;
		$$->args[0] = $1;
		if($2!=0){
			$$->argc = 2;
			$$->args[1] = $2;
		}
	}

iter_stmt
	: FOR '(' expr_stmt expr_stmt expr ')' stmt
	| WHILE '(' expr ')' stmt {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		$$->type = BLOCK;
		strcpy($$->val, "while");
		$$->argc = 2;
		$$->args[0] = $3;
		$$->args[1] = $5;
	}

jump_stmt
	: RETURN ';' {
		$$ = (node*)malloc(sizeof(node));
		strcpy($$->val, $1);
		$$->next = 0;
		$$->type = BLOCK;
	}
	| RETURN expr ';' {
		$$ = (node*)malloc(sizeof(node));
		strcpy($$->val, $1);
		$$->next = 0;
		$$->type = BLOCK;
		$$->argc = 1;
		$$->args[0] = $2;
	}
	| CONTINUE ';' {
		$$ = (node*)malloc(sizeof(node));
		strcpy($$->val, $1);
		$$->next = 0;
		$$->type = BLOCK;
		$$->argc = 1;
	}
	| BREAK ';' {
		$$ = (node*)malloc(sizeof(node));
		strcpy($$->val, $1);
		$$->next = 0;
		$$->type = BLOCK;
		$$->argc = 1;
	}

stmt
	: expr_stmt
	| iter_stmt
	| compound_stmt
	| decl_stmt
	| condition_stmt
	| jump_stmt

decl_stmt
	: decl { $$ = $1; }

stmt_list
	: stmt stmt_list{
		$$ = $1;
		node* n = $1;
		while(n->next!=0)n=n->next;
		n->next = $2;
	}
	| %empty { $$ = 0; }

expr_stmt
	: expr ';'{
		$$ = $1;
	}

compound_stmt
	: '{' stmt_list '}' { 
		$$ = $2;
	}

type 
	: CHAR|INT|FLOAT|DOUBLE|VOID;

decl_list 
	: decl decl_list
	| func_decl decl_list;
	| %empty {}

parameter 
	: parameter ',' parameter {
		for(int i=0;i<$3->argc;i++){
			strcpy($1->args[$1->argc+i],$3->args[i]);
		}
		$1->argc+=$3->argc;
		$$ = $1;
	}
	| type IDENTIFIER { 
		$$ = (func_arg*)malloc(sizeof(func_arg));
		$$->argc = 1;
		strcpy($$->args[0],$2);
	 }
	| type { 
		$$ = (func_arg*)malloc(sizeof(func_arg));
		$$->argc = 1;
	 } ;

func_declarator
	: '(' parameter ')' { 
		$$=$2;
	}
	| '('  ')' {
		$$ = (func_arg*)malloc(sizeof(func_arg));
		$$->argc = 0;
	}

func_declarator2
	: compound_stmt {$$=$1;}
	| ';'{$$=0;}

func_decl 
	: type
	IDENTIFIER[name] {
	} 
	func_declarator[para] {
		
	}
	func_declarator2[body] { 
		gen_func($body,$2);
		// triversal($body);
	}

declarator
	: declarator ',' declarator{
		node* n = $1;
		while(n->next!=0)n=n->next;
		n->next = $3;
		$$ = $1;
	}
	| IDENTIFIER '=' expr { 
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		strcpy($$->val,"decl");
		$$->argc = 2;
		$$->type = BLOCK;

		node* id = (node*)malloc(sizeof(node));
		id->next = 0;
		id->type = 0;
		strcpy(id->val,$1);

		$$->args[0] = id;
		$$->args[1] = $3;
	}
	| IDENTIFIER { 
		$$ = (node*)malloc(sizeof(node));
		strcpy($$->val,"decl");
		$$->next = 0;
		$$->argc = 1;
		$$->type = BLOCK;
		
		node* id = (node*)malloc(sizeof(node));
		id->next = 0;
		id->type = 0;
		strcpy(id->val,$1);

		$$->args[0] = id;
	}

decl  
	: type declarator ';' {
		$$ = $2;
	}

arguments
	: expr
	| arguments ',' expr

expr : 
	CONSTANT {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		strcpy($$->val,$1);
		$$->argc = 0;
		$$->type = CONST_NODE;
	}
	| IDENTIFIER {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		strcpy($$->val,$1);
		$$->argc = 0;
		$$->type = EXPR;
	}
	| IDENTIFIER '(' arguments ')' {
		// puts arguments here
	}
	| IDENTIFIER '('')' {
	}
	| expr OR_OP expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) || atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr AND_OP expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) && atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr '|' expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) | atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr '^' expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) ^ atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr '&' expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) & atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr EQ_OP expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) == atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr NE_OP expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) != atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr '<' expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) < atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr LE_OP expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) <= atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr GE_OP expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) >= atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr '>' expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) > atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr LEFT_OP expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) << atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr RIGHT_OP expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) >> atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr '+' expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) + atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr '-' expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) - atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr '=' expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		strcpy($$->val,$2);
		$$->argc = 2;
		$$->args[0] = $1;
		$$->args[1] = $3;
		$$->type = BLOCK;
	}
	| expr '*' expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) * atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr '/' expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) / atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr '%' expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($1->type==CONST_NODE && $3->type==CONST_NODE){
			$$->argc = 0;
			$$->type = CONST_NODE;
			sprintf($$->val, "%d", atoi($1->val) % atoi($3->val) );
		}
		else{
			strcpy($$->val,$2);
			$$->argc = 2;
			$$->args[0] = $1;
			$$->args[1] = $3;
			$$->type = EXPR;
		}
	}
	| expr INC_OP {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		strcpy($$->val,$2);
		$$->argc = 1;
		$$->args[0] = $1;
		$$->type = $1->type;
	}
	| expr DEC_OP {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		strcpy($$->val,$2);
		$$->argc = 1;
		$$->args[0] = $1;
		$$->type = $1->type;
	}
	| INC_OP expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		strcpy($$->val,$1);
		$$->argc = 1;
		$$->args[0] = $2;
		$$->type = $2->type;
	}
	| DEC_OP expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		strcpy($$->val,$1);
		$$->argc = 1;
		$$->args[0] = $2;
		$$->type = $2->type;
	}
	| '-' expr %prec MINUS_SIGN {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($2->type){
			sprintf($$->val, "%d", -atoi($2->val));
			$$->type = CONST_NODE;
			$$->argc = 0;
		}
		else{
			strcpy($$->val,$1);
			$$->argc = 1;
			$$->args[0] = $2;
			$$->type = $2->type;
		}
	}
	| '+' expr {
		$$ = $2;
	}
	| '!' expr {
		$$ = (node*)malloc(sizeof(node));
		$$->next = 0;
		if($2->type){
			sprintf($$->val, "%d", !atoi($2->val));
			$$->type = CONST_NODE;
			$$->argc = 0;
		}
		else{
			strcpy($$->val,$1);
			$$->argc = 1;
			$$->args[0] = $2;
			$$->type = $2->type;
		}
	}
	| '(' expr ')' {
		$$ = $2;
	}

program :
	decl_list {
	}
%%


int main(int argc, char** argv){
	yyparse();

}