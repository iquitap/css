/*typedef struct Operand_
{
	enum{ VARIABLE, CONSTANT, ADDRESS }kind;
	union
	{
		int var_no;
		int value;
	} u;

}* Operand;

struct InterCode
{
	enum{ ASSIGN, ADD, SUB, MUL }kind;
	union
	{
		struct { Operand right ,left ;} assign;
		struct { Operand result, op1, op2;} binop;
	}u;
};
struct InterCodes {struct InterCode code;struct Intercodes *prev, *next;}*IntreCodeshead;
*/
struct InterCodes
{
	char *codes;
	struct InterCodes* next;
}*intercodehead,*intercodetail;

void newfuncread(void);
void newfuncwrite(void);
struct symbollist* newfuncwritehelp(void);

void translate_exp(struct node* root,char* place);
char *translate_exp_help(struct node* root,char *place);
void translate_func(struct node* root,char* place);
char *translate_func_help(struct node* root,char* place);
void translate_stmt(struct node* root);
char *translate_stmt_help(struct node* root);
char *translate_cond(struct node*root,char *label_true,char *label_false);
char *translate_args(struct node* root);
char *translate_funarg(struct node* root);

char* newtemp();
char* newlabel();
void testintercode(struct InterCodes* intercodep);

/*void makesymboltempname(char *name,char *tempname);
char *findsymboltempname(char *name);*/
