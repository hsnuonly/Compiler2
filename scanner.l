D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*

%{
#include <stdio.h>
#include <string.h>
#include "y.tab.h"
void comment();
extern YYSTYPE yylval; 
%}

%x inline_comment

%%
"/*"			{ comment(); }
"//"			{ BEGIN inline_comment; }
<inline_comment>\n { BEGIN 0; }
<inline_comment>. {  }

"break"			{  strcpy(yylval.stringval, yytext); return(BREAK); }
"case"			{  strcpy(yylval.stringval, yytext); return(CASE); }
"char"			{  strcpy(yylval.stringval, yytext); return(CHAR); }
"const"			{  strcpy(yylval.stringval, yytext); return(CONST); }
"continue"		{  strcpy(yylval.stringval, yytext); return(CONTINUE); }
"default"		{  strcpy(yylval.stringval, yytext); return(DEFAULT); }
"do"			{  strcpy(yylval.stringval, yytext); return(DO); }
"double"		{  strcpy(yylval.stringval, yytext); return(DOUBLE); }
"else"			{  strcpy(yylval.stringval, yytext); return(ELSE); }
"float"			{  strcpy(yylval.stringval, yytext); return(FLOAT); }
"for"			{  strcpy(yylval.stringval, yytext); return(FOR); }
"if"			{  strcpy(yylval.stringval, yytext); return(IF); }
"int"			{  strcpy(yylval.stringval, yytext); return(INT); }
"return"		{  strcpy(yylval.stringval, yytext); return(RETURN); }
"switch"		{  strcpy(yylval.stringval, yytext); return(SWITCH); }
"void"			{  strcpy(yylval.stringval, yytext); return(VOID); }
"while"			{  strcpy(yylval.stringval, yytext); return(WHILE); }

{L}({L}|{D})*		{  strcpy(yylval.stringval, yytext); return(IDENTIFIER); }

0[xX]{H}+{IS}?		{  strcpy(yylval.stringval, yytext); return(CONSTANT); }
0{D}+{IS}?		{  strcpy(yylval.stringval, yytext); return(CONSTANT); }
{D}+{IS}?		{  strcpy(yylval.stringval, yytext); return(CONSTANT); }
L?'(\\.|[^\\'])+'	{  strcpy(yylval.stringval, yytext); return(CONSTANT); }

{D}+{E}{FS}?		{  strcpy(yylval.stringval, yytext); return(CONSTANT); }
{D}*"."{D}+({E})?{FS}?	{  strcpy(yylval.stringval, yytext); return(CONSTANT); }
{D}+"."{D}*({E})?{FS}?	{  strcpy(yylval.stringval, yytext); return(CONSTANT); }

L?\"(\\.|[^\\"])*\"	{  strcpy(yylval.stringval, yytext); return(STRING_LITERAL); }

">>"			{  strcpy(yylval.stringval, yytext); return(RIGHT_OP); }
"<<"			{  strcpy(yylval.stringval, yytext); return(LEFT_OP); }
"++"			{  strcpy(yylval.stringval, yytext); return(INC_OP); }
"--"			{  strcpy(yylval.stringval, yytext); return(DEC_OP); }
"&&"			{  strcpy(yylval.stringval, yytext); return(AND_OP); }
"||"			{  strcpy(yylval.stringval, yytext); return(OR_OP); }
"<="			{  strcpy(yylval.stringval, yytext); return(LE_OP); }
">="			{  strcpy(yylval.stringval, yytext); return(GE_OP); }
"=="			{  strcpy(yylval.stringval, yytext); return(EQ_OP); }
"!="			{  strcpy(yylval.stringval, yytext); return(NE_OP); }
";"			{  strcpy(yylval.stringval, yytext); return(';'); }
"{"			{  strcpy(yylval.stringval, yytext); return('{'); }
"}"			{  strcpy(yylval.stringval, yytext); return('}'); }
","			{  strcpy(yylval.stringval, yytext); return(','); }
":"			{  strcpy(yylval.stringval, yytext); return(':'); }
"="			{  strcpy(yylval.stringval, yytext); return('='); }
"("			{  strcpy(yylval.stringval, yytext); return('('); }
")"			{  strcpy(yylval.stringval, yytext); return(')'); }
("[")		{  strcpy(yylval.stringval, yytext); return('['); }
("]")		{  strcpy(yylval.stringval, yytext); return(']'); }
"."			{  strcpy(yylval.stringval, yytext); return('.'); }
"&"			{  strcpy(yylval.stringval, yytext); return('&'); }
"!"			{  strcpy(yylval.stringval, yytext); return('!'); }
"~"			{  strcpy(yylval.stringval, yytext); return('~'); }
"-"			{  strcpy(yylval.stringval, yytext); return('-'); }
"+"			{  strcpy(yylval.stringval, yytext); return('+'); }
"*"			{  strcpy(yylval.stringval, yytext); return('*'); }
"/"			{  strcpy(yylval.stringval, yytext); return('/'); }
"%"			{  strcpy(yylval.stringval, yytext); return('%'); }
"<"			{  strcpy(yylval.stringval, yytext); return('<'); }
">"			{  strcpy(yylval.stringval, yytext); return('>'); }
"|"			{  strcpy(yylval.stringval, yytext); return('|'); }
"^"			{  strcpy(yylval.stringval, yytext); return('^'); }

[ \t\v\n\f]		{  }
.			{ /* ignore bad characters */ }

%%

void comment()
{
	char c, c1;

loop:
	while ((c = input()) != '*' && c != 0)
		putchar(c);

	if ((c1 = input()) != '/' && c != 0)
	{
		unput(c1);
		goto loop;
	}

	if (c != 0)
		putchar(c1);
}

