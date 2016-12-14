#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <string.h>
extern int yylineno;
extern int col;
extern char* yytext;
void error(char *s,int line);

struct node
{
	char* name;
	char* inname;
	char* fathername;
	int line;
	int col;
	struct node *left;
	struct node *right;
	union
	{
		char* idtype;
		char* op;
		int intval;
		float floatval;
	};
	int type; //1-int,2-float,3-int[],4-float[],5-func,6-struct
};

struct symbollist
{
	char *name;
	//char a[10];
	//char *tempname;
	//char tempname[10];
	int type; //1-int,2-float,3-int[],4-float[],5-func,6-struct
	int layer; //实现作用域

	struct symbollist *next;
}*symbolhead,*symboltail; 

struct varlist
{
	char *name;
	int type; //1-int,2-float
	struct varlist *next;
}*varhead,*vartail;

struct arraylist
{
	char *name;
	int type; //3-int[],4-float[]
	int anum;
	struct arraylist *next;
}*arrayhead,*arraytail;

struct funclist
{
	int flag;
	char *name;
	int type;//1-int,2-float,3-int[],4-float[],5-func,6-struct
	int returntype; //1-int,2-float,3-int[],4-float[],5-func,6-struct
	struct funcpara
	{
		//struct varlist* fplist;
		struct symbollist *fplist;
		int fpnum;
	}funcpara; 
	struct funclist *next;
}*funchead,*functail;

struct structlist
{
	char *name;
	struct structtypelist* type;
	struct structlist *front;
	struct structlist *next;
}*structhead,*structtail;

struct structtypelist
{
	char *name;
	struct varlist *splist; //结构体成员表
	//struct structlist *stlist; //定义为该结构体类型的变量表
	struct structtypelist *next;
}*structtypehead,*structtypetail;


struct node* newtree(char* name,int num,...);

void eval(struct node*,int level);

void testvarlist(struct varlist* varp);
void testarraylist(struct arraylist* arrayp);
void teststructlist(struct structlist* structp);
void teststructtypelist(struct structtypelist* structtypep);
void testfunclist(struct funclist* funcp);
void testsymbollist(struct symbollist* funcp);


struct node* makeinname(struct node* root);
struct node* makeinname2(struct node* root);

struct node* maketype(struct node* root);

void newvar(int num,...);
void newarray(int num,...);
void newfunc(int num,...);
void newstructtype(int num,...);
void newstruct(int num,...);
void newsymbol(char* name,int type);

void check(struct node* root);
void check2(struct node* root);
void checkfunc(struct funclist* funcp);

int findvar(char *name);
int findfunc(char *name);
int findarray(char *name);
int findstruct(char *name);
int findstructtype(char *name);
int findsymbol(char *name);
struct funcpara newfunchelp(struct node* varlist);
struct varlist* newstructtypehelp(struct node* deflist);

void deletesymbol(int num);
int symboltype(char *name);
int getpnum(char *name);
int flagfunc(char *name);
int getreturntype(struct node* root);
int getargnum(struct node* root);
int matcharglist(char *name,struct node* arglist);
int findstructarg(char *structname,char *fieldname);
struct structtypelist* getstructtypep(char *name);
