all:
		yacc -d -v parser.y 
		flex scanner.l
		gcc util.c y.tab.c y.tab.h lex.yy.c -ly -ll -o parser -Wall
