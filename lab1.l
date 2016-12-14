%{
#include "syntax.tab.h"
#include "tree.h"

int col = 0;
%}
%option yylineno
%x COMMENT1
%x COMMENT2


%%
"//"				{BEGIN COMMENT1;}
"/*"				{BEGIN COMMENT2;}
<COMMENT2>"\n"		{}
<COMMENT2>.			{}
<COMMENT2>"*/"		{BEGIN INITIAL;}
<COMMENT1>.			{}
<COMMENT1>"\n"		{BEGIN INITIAL;}
"int"|"float"		{column();yylval.t=newtree("TYPE",0,yylineno);return TYPE;}
"struct"			{column();yylval.t=newtree("STRUCT",0,yylineno);return STRUCT;}
"return"			{column();yylval.t=newtree("RETURN",0,yylineno);return RETURN;}
"if"				{column();yylval.t=newtree("IF",0,yylineno);return IF;}
"else"				{column();yylval.t=newtree("ELSE",0,yylineno);return ELSE;}
"while"				{column();yylval.t=newtree("WHILE",0,yylineno);return WHILE;}
[[:digit:]]+"."[[:digit:]]+([eE][+-]?[[:digit:]]+)?		{column();yylval.t=newtree("FLOAT",0,yylineno);return FLOAT;}
0[xX][0-9A-Fa-f]+|0[0-7]+|[1-9][[:digit:]]*|0		{column();yylval.t=newtree("INT",0,yylineno);return INT;}
(_|[[:alpha:]])([[:alnum:]]|_)*		{column();yylval.t=newtree("ID",0,yylineno);return ID;}
";"					{column();yylval.t=newtree("SEMI",0,yylineno);return SEMI;}
","					{column();yylval.t=newtree("COMMA",0,yylineno);return COMMA;}
"="					{column();yylval.t=newtree("ASSIGNOP",0,yylineno);return ASSIGNOP;}
">"|"<"|">="|"<="|"=="|"!="	{column();yylval.t=newtree("RELOP",0,yylineno);return RELOP;}
"+"					{column();yylval.t=newtree("PLUS",0,yylineno);return PLUS;}
"-"					{column();yylval.t=newtree("MINUS",0,yylineno);return MINUS;}
"*"					{column();yylval.t=newtree("STAR",0,yylineno);return STAR;}
"/"					{column();yylval.t=newtree("DIV",0,yylineno);return DIV;}
"&&"				{column();yylval.t=newtree("AND",0,yylineno);return AND;}
"||"				{column();yylval.t=newtree("OR",0,yylineno);return OR;}
"."					{column();yylval.t=newtree("DOT",0,yylineno);return DOT;}
"!"					{column();yylval.t=newtree("NOT",0,yylineno);return NOT;}
"("					{column();yylval.t=newtree("LP",0,yylineno);return LP;}
")"					{column();yylval.t=newtree("RP",0,yylineno);return RP;}
"["					{column();yylval.t=newtree("LB",0,yylineno);return LB;}
"]"					{column();yylval.t=newtree("RB",0,yylineno);return RB;}
"{"					{column();yylval.t=newtree("LC",0,yylineno);return LC;}
"}"					{column();yylval.t=newtree("RC",0,yylineno);return RC;}
"\n"				{column();}
[ \t]				{column();}
.					{column();return;}
%%


int yywrap() 
{  
	return 1; 
}

int column(void)
{	int i;
	for(i = 0;yytext[i] != 0; i++)
	{
		if(yytext[i]=='\n')
			col = 0;
		else if(yytext[i]=='\t')
			col += (4 - col%4);
		else
			col++;
	}
	return ;
}

