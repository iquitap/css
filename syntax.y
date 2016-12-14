%{
#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "imcode.h"
%}

%union
{
	struct node *t;
	double dval;
}


%token <t> INT FLOAT ID
%token <t> SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC 
%token <t> STRUCT RETURN IF ELSE WHILE 
%type  <t> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag  Tag VarDec  FunDec VarList ParamDec Compst StmtList Stmt DefList Def DecList Dec Exp Args

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS 
%left STAR DIV 
%right NOT
%left LP RP LB RB

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
Program:ExtDefList {
	   $$=newtree("Program",1,$1);
	   //printf("systex tree begin:\n");
	   $$=makeinname($$);
	   $$=makeinname2($$);
	   
	   check($$);
	   //printf("aaa!\n");testsymbollist(symbolhead);
	   $$=maketype($$);
	   //check($$);
	   check2($$);
	   //eval($$,0);
	   /*testvarlist(varhead);
	   testarraylist(arrayhead);
	   testfunclist(funchead);*/
	   checkfunc(funchead); 
	   /*teststructlist(structhead);
	   teststructtypelist(structtypehead);*/
	   //testsymbollist(symbolhead);
	   //printf("syntax tree finish!\n");

	   make_imcode($$);
	   testintercode(intercodehead->next);
	   }
    ;
ExtDefList:ExtDef ExtDefList {$$=newtree("ExtDefList",2,$1,$2);}
	| {$$=newtree("ExtDefList",0,-1);}
	;
ExtDef:Specifier ExtDecList SEMI    {$$=newtree("ExtDef",3,$1,$2,$3);}    
	|Specifier SEMI	{$$=newtree("ExtDef",2,$1,$2);}
	|Specifier FunDec Compst	{$$=newtree("ExtDef",3,$1,$2,$3);}
	|Specifier FunDec SEMI {$$=newtree("ExtDef",3,$1,$2,$3);}
	;
ExtDecList:VarDec {$$=newtree("ExtDecList",1,$1);}
	|VarDec COMMA ExtDecList {$$=newtree("ExtDecList",3,$1,$2,$3);}
	;
/*Specifiers*/
Specifier:TYPE {$$=newtree("Specifier",1,$1);}
	|StructSpecifier {$$=newtree("Specifier",1,$1);}
	;
StructSpecifier:STRUCT OptTag LC DefList RC {$$=newtree("StructSpecifier",5,$1,$2,$3,$4,$5);}
	|STRUCT Tag {$$=newtree("StructSpecifier",2,$1,$2);}
	;
OptTag:ID {$$=newtree("OptTag",1,$1);}
	|{$$=newtree("OptTag",0,-1);}
	;
Tag:ID {$$=newtree("Tag",1,$1);}
	;
/*Declarators*/
VarDec:ID {$$=newtree("VarDec",1,$1);}
	| VarDec LB INT RB {$$=newtree("VarDec",4,$1,$2,$3,$4);}
	;
FunDec:ID LP VarList RP {$$=newtree("FunDec",4,$1,$2,$3,$4);}
	|ID LP RP {$$=newtree("FunDec",3,$1,$2,$3);}
	;
VarList:ParamDec COMMA VarList {$$=newtree("VarList",3,$1,$2,$3);}
	|ParamDec {$$=newtree("VarList",1,$1);}
	;
ParamDec:Specifier VarDec {$$=newtree("ParamDec",2,$1,$2);}
    ;

/*Statement*/
Compst:LC DefList StmtList RC {$$=newtree("Compst",4,$1,$2,$3,$4);}
	;
StmtList:Stmt StmtList{$$=newtree("StmtList",2,$1,$2);}
	| {$$=newtree("StmtList",0,-1);}
	;
Stmt:Exp SEMI {$$=newtree("Stmt",2,$1,$2);}
	|Compst {$$=newtree("Stmt",1,$1);}
	|RETURN Exp SEMI {$$=newtree("Stmt",3,$1,$2,$3);}
	|IF LP Exp RP Stmt %prec LOWER_THAN_ELSE{$$=newtree("Stmt",5,$1,$2,$3,$4,$5);}
	|IF LP Exp RP Stmt ELSE Stmt{$$=newtree("Stmt",7,$1,$2,$3,$4,$5,$6,$7);}
	|WHILE LP Exp RP Stmt {$$=newtree("Stmt",5,$1,$2,$3,$4,$5);}
	;
/*Local Definitions*/
DefList:Def DefList{$$=newtree("DefList",2,$1,$2);}
	| {$$=newtree("DefList",0,-1);}
	;
Def:Specifier DecList SEMI {$$=newtree("Def",3,$1,$2,$3);}
	;
DecList:Dec {$$=newtree("DecList",1,$1);}
	|Dec COMMA DecList {$$=newtree("DecList",3,$1,$2,$3);}
	;
Dec:VarDec {$$=newtree("Dec",1,$1);}
	|VarDec ASSIGNOP Exp {$$=newtree("Dec",3,$1,$2,$3);}
	;
/*Expressions*/
Exp:Exp ASSIGNOP Exp{$$=newtree("Exp",3,$1,$2,$3);}
        |Exp AND Exp{$$=newtree("Exp",3,$1,$2,$3);}
        |Exp OR Exp{$$=newtree("Exp",3,$1,$2,$3);}
        |Exp RELOP Exp{$$=newtree("Exp",3,$1,$2,$3);}
        |Exp PLUS Exp{$$=newtree("Exp",3,$1,$2,$3);}
        |Exp MINUS Exp{$$=newtree("Exp",3,$1,$2,$3);}
        |Exp STAR Exp{$$=newtree("Exp",3,$1,$2,$3);}
        |Exp DIV Exp{$$=newtree("Exp",3,$1,$2,$3);}
        |LP Exp RP{$$=newtree("Exp",3,$1,$2,$3);}
        |MINUS Exp {$$=newtree("Exp",2,$1,$2);}
        |NOT Exp {$$=newtree("Exp",2,$1,$2);}
        |ID LP Args RP {$$=newtree("Exp",4,$1,$2,$3,$4);}
        |ID LP RP {$$=newtree("Exp",3,$1,$2,$3);}
        |Exp LB Exp RB {$$=newtree("Exp",4,$1,$2,$3,$4);}
        |ID DOT ID {$$=newtree("Exp",3,$1,$2,$3);}
        |ID {$$=newtree("Exp",1,$1);}
        |INT {$$=newtree("Exp",1,$1);}
        |FLOAT{$$=newtree("Exp",1,$1);}
        ;
Args:Exp COMMA Args {$$=newtree("Args",3,$1,$2,$3);}
        |Exp {$$=newtree("Args",1,$1);}
        ;
%%

int main(int argc, char ** argv)
{
	varhead = (struct varlist*)malloc(sizeof(struct varlist));
	vartail = varhead;
	vartail->next = NULL;	

	arrayhead = (struct arraylist*)malloc(sizeof(struct arraylist));
	arraytail = arrayhead;
	arraytail->next = NULL;	

	funchead = (struct funclist*)malloc(sizeof(struct funclist));
	functail = funchead;
	functail->next = NULL;

	structhead = (struct structlist*)malloc(sizeof(struct structlist));
	structtail = structhead;
	structtail->next = NULL;

	structtypehead = (struct structtypelist*)malloc(sizeof(struct structtypelist));
	structtypetail = structtypehead;
	structtypetail->next = NULL;

	symbolhead = (struct symbollist*)malloc(sizeof(struct symbollist));
	symboltail = symbolhead;
	symboltail->next = NULL;

	intercodehead = (struct InterCodes*)malloc(sizeof(struct InterCodes));
	intercodetail = intercodehead;
	intercodetail->next = NULL;

	newfuncread();
	newfuncwrite();

	/*printf("%s",newtemp());
	printf("%s",newtemp());
	printf("%s",newtemp());*/


	if(argc < 1) return 1;
	FILE *f = fopen(argv[1], "r");
	if(!f)
	{
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();
	return 0;

}
