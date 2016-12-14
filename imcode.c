#include "tree.h"
#include "imcode.h"
int counttemp = 0;
int countlabel = 0;
int coutfpnum = 0;
char *arg_list[100];
char* newtemp(void)
{
	counttemp++;
	int i = counttemp;
	char* temp = (char *)malloc(sizeof(char)*10);
	sprintf(temp,"t%d",i);
	
	//printf("newtemp test:%s\n",temp);
	return temp; 
}
char* newlabel(void)
{
	countlabel++;
	int i = countlabel;
	char* temp = (char *)malloc(sizeof(char)*10);
	sprintf(temp,"label%d",i);
	
	//printf("newtemp test:%s\n",temp);
	return temp; 
}
void newfuncread(void)
{
	struct funclist* func = (struct funclist*)malloc(sizeof(struct funclist));
	if(!func)
	{
		error("Memory is out of space,initialization of node failure",0);
		exit(0);
	}
	else
	{
		func->flag = 1;
		func->name = "read";
		func->type = 1; //1-int,2-float,3-int[],4-float[],5-func,6-struct
		func->returntype = 1;
		func->funcpara.fpnum = 0;
		func->funcpara.fplist = NULL;
		newsymbol(func->name,5);//newsymbol,func-5	
		functail->next = func;
		functail = func;
	}
}

void newfuncwrite(void)
{
	struct funclist* func = (struct funclist*)malloc(sizeof(struct funclist));
	if(!func)
	{
		error("Memory is out of space,initialization of node failure",0);
		exit(0);
	}
	else
	{
		func->flag = 1;
		func->name = "write";
		func->type = 1; //1-int,2-float,3-int[],4-float[],5-func,6-struct
		func->returntype = 1;
		func->funcpara.fpnum = 1;
		func->funcpara.fplist = newfuncwritehelp();
		newsymbol(func->name,5);//newsymbol,func-5	
		functail->next = func;
		functail = func;
	}
}

struct symbollist* newfuncwritehelp(void)
{
	struct symbollist* fplisthead = (struct symbollist*)malloc(sizeof(struct varlist));
	struct symbollist* fplisttail = fplisthead;
	fplisttail->next = NULL;
	
	struct symbollist* fplist = (struct symbollist*)malloc(sizeof(struct varlist));fplist->next = NULL;
	fplist->type = 1;
	fplist->name = "writehelp";  //无所谓
	fplisttail->next = fplist;
	fplisttail = fplist;
	return fplisthead;
}

void translate_exp(struct node* root,char *place)
{
	struct InterCodes* intercode = (struct InterCodes*)malloc(sizeof(struct InterCodes));
	char *code = (char *)malloc(sizeof(char)*1024);
	if(!intercode || !code)
	{
		error("Memory is out of space,initialization of node failure",0);
		exit(0);
	}
	else 
	{	

		code = translate_exp_help(root,place);
		intercode->codes = code;
		intercode->next = NULL;
		intercodetail->next = intercode;
		intercodetail = intercode;
	}
}

char *translate_args(struct node*root)
{
	char *t1 = (char *)malloc(sizeof(char)*20);
	char *code1 = (char *)malloc(sizeof(char)*40);
	char *code2 = (char *)malloc(sizeof(char)*40);
	char *codetemp = (char *)malloc(sizeof(char)*80);
	if(root != NULL)
	{
		if(root->left!=NULL)
		{
			if(root->left->right!=NULL)
			{
				if(!strcmp(root->left->right->name,"COMMA"))
				{
					t1 = newtemp();
					code1 = translate_exp_help(root->left,t1);
					arg_list[coutfpnum++] = t1;
					code2 = translate_args(root->left->right->right);
					sprintf(codetemp,"%s%s",code1,code2);
					return codetemp;
				}
			}
			else 
			{
				t1 = newtemp();
				code1 = translate_exp_help(root->left,t1);
				arg_list[coutfpnum++] = t1;
				return code1;
			}
		}	
	}
}
char *translate_exp_help(struct node* root,char *place)
{
	char *code = (char *)malloc(sizeof(char)*256);//code = "";
	char *codetemp = (char *)malloc(sizeof(char)*128);//codetemp = "";
	char *t1 = (char *)malloc(sizeof(char)*20);
	char *t2 = (char *)malloc(sizeof(char)*20);
	char *code1 = (char *)malloc(sizeof(char)*40);
	char *code2 = (char *)malloc(sizeof(char)*40);
	char *code3 = (char *)malloc(sizeof(char)*40);
	char *code0 = (char *)malloc(sizeof(char)*40);
	char *label1 = (char *)malloc(sizeof(char)*10);
	char *label2 = (char *)malloc(sizeof(char)*10);
	char *tempname = (char*)malloc(sizeof(char)*40);
	//char tempname[10];
	if(root!=NULL)
	{
		if(root->left!=NULL)
		{	
			if(!strcmp(root->left->name,"LP"))
			{	return translate_exp_help(root->left->right,place);}
			if(root->left->right!=NULL)
			{
				if(!strcmp(root->left->name,"ID"))
				{
					if(root->left->right->right->right!=NULL)//有参
					{
						coutfpnum = 0;
						code1 = translate_args(root->left->right->right);
						if(!strcmp(root->left->inname,"write"))
						{
							sprintf(codetemp,"%sWRITE %s\n",code1,arg_list[0]);
							return codetemp;
						}
						else
						{
							int i=0;
							//printf("coutfpnum:%d pnum:%d\n",coutfpnum,getpnum(root->left->inname));
							for(i = 0 ; i < getpnum(root->left->inname); i++)
							{	
								sprintf(code2,"%sARG %s\n",code2,arg_list[i]);
								//printf("i=%d,arg_list[i]=%s\n",i,arg_list[i]);
							}
							sprintf(codetemp,"%s%s%s := CALL %s\n",code1,code2,place,root->left->inname);
							return codetemp;
						}
					}
					else	//无参
					{
						char *func_name = root->left->idtype;
						if(!strcmp(func_name,"read"))
						{
							sprintf(codetemp,"READ %s",place);
							return codetemp;
						}
						else
						{
							sprintf(codetemp,"%s := CALL %s\n",place,func_name);
							return codetemp;;
						}
					}
				}
				else if(!strcmp(root->left->right->name,"ASSIGNOP"))
				{
					t1 = newtemp();
					code1 = translate_exp_help(root->left->right->right,t1);

					/*if(!strcmp(findsymboltempname(root->left->inname),""))
					{
						tempname = newtemp();
						makesymboltempname(root->left->inname,tempname);
						sprintf(code2,"%s := %s\n",tempname,t1);

						
						printf("xixi1: name:%s tempname:%s\n",root->left->inname,findsymboltempname(root->left->inname));
					}
					else
					{
						printf("xixi2: name:%s tempname:%s\n",root->left->inname,findsymboltempname(root->left->inname));
						sprintf(codetemp,"%s := %s\n",findsymboltempname(root->left->inname),t1);
					}*/
					sprintf(code2,"%s := %s\n",root->left->inname,t1);


					sprintf(codetemp,"%s%s",code1,code2);
					strcat(code,codetemp);
					
				}
				else if(!strcmp(root->left->right->name,"PLUS"))
				{
					t1 = newtemp();
					t2 = newtemp();
					code1 = translate_exp_help(root->left,t1);
					code2 = translate_exp_help(root->left->right->right,t2);
					sprintf(code3,"%s := %s + %s\n",place,t1,t2);
					sprintf(codetemp,"%s%s%s",code1,code2,code3);
					strcat(code,codetemp);
				}	
				else if(!strcmp(root->left->right->name,"MINUS"))
				{
					t1 = newtemp();
					t2 = newtemp();
					code1 = translate_exp_help(root->left,t1);
					code2 = translate_exp_help(root->left->right->right,t2);
					sprintf(code3,"%s := %s - %s\n",place,t1,t2);
					sprintf(codetemp,"%s%s%s",code1,code2,code3);
					strcat(code,codetemp);
				}
				else if(!strcmp(root->left->right->name,"STAR"))
				{
					t1 = newtemp();
					t2 = newtemp();
					code1 = translate_exp_help(root->left,t1);
					code2 = translate_exp_help(root->left->right->right,t2);
					sprintf(code3,"%s := %s * %s\n",place,t1,t2);
					sprintf(codetemp,"%s%s%s",code1,code2,code3);
					strcat(code,codetemp);
				}
				else if(!strcmp(root->left->right->name,"DIV"))
				{
					t1 = newtemp();
					t2 = newtemp();
					code1 = translate_exp_help(root->left,t1);
					code2 = translate_exp_help(root->left->right->right,t2);
					sprintf(code3,"%s := %s / %s\n",place,t1,t2);
					sprintf(codetemp,"%s%s%s",code1,code2,code3);
					strcat(code,codetemp);
				}
				else if(!strcmp(root->left->name,"MINUS"))
				{
					t1 = newtemp();
					code1 = translate_exp_help(root->left->right,t1);
					sprintf(code2,"%s := #0 - %s\n",place,t1);
					sprintf(codetemp,"%s%s",code1,code2);
					strcat(code,codetemp);
				}
				else if(!strcmp(root->left->right->name,"RELOP"))
				{
					label1 = newlabel();
					label2 = newlabel();
					sprintf(code0,"%s := #0",place);
					code1 = translate_cond(root,label1,label2);
					sprintf(code2,"LBAEL %s\n%s := #1",label1,place);
					sprintf(code3,"LABEL %s",label2);
					sprintf(codetemp,"%s%s%s%s",code0,code1,code2,code3);
					strcat(code,codetemp);
				}
				else if(!strcmp(root->left->name,"NOT"))
				{
					label1 = newlabel();
					label2 = newlabel();
					sprintf(code0,"%s := #0",place);
					code1 = translate_cond(root,label1,label2);
					sprintf(code2,"LBAEL %s\n%s := #1",label1,place);
					sprintf(code3,"LABEL %s",label2);
					sprintf(codetemp,"%s%s%s%s",code0,code1,code2,code3);
					strcat(code,codetemp);	
				}
			}
			else //root->left->right == NULL					
			{
				if(!strcmp(root->left->name,"INT"))
				{
					/*if(findint(root->left->intval))
					{

					}*/
					sprintf(codetemp,"%s := #%d\n",place,root->left->intval);
					strcat(code,codetemp);
				}
				else if(!strcmp(root->left->name,"FLOAT"))
				{
					sprintf(codetemp,"#%f",root->left->floatval);
					strcat(code,codetemp);
				}
				else if(!strcmp(root->left->name,"ID"))
				{
					/*if(!strcmp(findsymboltempname(root->left->inname),""))
					{
						tempname = newtemp();
						makesymboltempname(root->left->inname,tempname);
						sprintf(codetemp,"%s := %s\n",place,tempname);
					}
					else
					{
						sprintf(codetemp,"%s := %s\n",place,findsymboltempname(root->left->inname));
					}*/
					sprintf(codetemp,"%s := %s\n",place,root->left->inname);
					strcat(code,codetemp);
				}				
			}
		}	
	}
	return code;		
}

/*char *findsymboltempname(char *name)
{
	struct symbollist* symbolp = (struct symbollist*)malloc(sizeof(struct symbollist));
	symbolp = symbolhead->next;
	while(symbolp!=NULL)
	{
		if(!(strcmp(symbolp->name,name)))
		{
			return symbolp->tempname;
		}
		symbolp = symbolp->next;
	}
	return NULL;
}

void makesymboltempname(char *name,char *tempname)
{
	struct symbollist* symbolp = (struct symbollist*)malloc(sizeof(struct symbollist));
	//char *temp_name = (char *)malloc(sizeof(char)*10);
	symbolp = symbolhead->next;
	while(symbolp!=NULL)
	{
		if(!(strcmp(symbolp->name,name)))
		{
			strcpy(symbolp->tempname,tempname);
			printf("hehe   tempname:%s,name:%s\n\n",symbolp->tempname,symbolp->name);
			return;
		}
		symbolp = symbolp->next;
	}
	return;
}*/


void translate_func(struct node* root,char* place)
{
	struct InterCodes* intercode = (struct InterCodes*)malloc(sizeof(struct InterCodes));
	char *code = (char *)malloc(sizeof(char)*1024);
	if(!intercode)
	{
		error("Memory is out of space,initialization of node failure",0);
		exit(0);
	}
	else 
	{	
		code = translate_func_help(root,place);
		intercode->codes = code;
		intercode->next = NULL;
		intercodetail->next = intercode;
		intercodetail = intercode;
	}
}

char *translate_func_help(struct node* root,char* place)
{
	char *code = (char *)malloc(sizeof(char)*128);
	char *codetemp = (char *)malloc(sizeof(char)*20);
	
	sprintf(code,"FUNCTION %s :\n",root->left->inname);
	if(root->left!=NULL)
	{
		if(root->left->right!=NULL)
		{
			if(root->left->right->right!=NULL)
			{
				if(!strcmp(root->left->right->right->name,"VarList"))
				{	
					codetemp = translate_funarg(root->left->right->right);
					strcat(code,codetemp);
				}
			}
		}
	}
	return code;
}

char *translate_funarg(struct node* root)
{
	char *code = (char*)malloc(sizeof(char)*80);
	char *codetemp = (char*)malloc(sizeof(char)*10);
	
	if(root!=NULL)
	{
		if(root->left!=NULL)
		{
			if(root->left->right!=NULL)
			{
				if(root->left->right->right!=NULL)
				{
					codetemp = translate_funarg(root->left->right->right);
					sprintf(code,"PARAM %s\n%s",root->left->left->right->inname,codetemp);
				}			
			}
			else
			{
				sprintf(codetemp,"PARAM %s\n",root->left->left->right->inname);
				return codetemp;
			}
		}
	}
	return code;
}
void translate_stmt(struct node* root)
{
	struct InterCodes* intercode = (struct InterCodes*)malloc(sizeof(struct InterCodes));
	char *code = (char *)malloc(sizeof(char)*1024);
	if(!intercode || !code)
	{
		error("Memory is out of space,initialization of node failure",0);
		exit(0);
	}
	else 
	{	
		code = translate_stmt_help(root);
		intercode->codes = code;
		intercode->next = NULL;
		intercodetail->next = intercode;
		intercodetail = intercode;
	}
}

char *translate_stmt_help(struct node* root)
{
	char *code = (char *)malloc(sizeof(char)*1024);
	char *codetemp = (char *)malloc(sizeof(char)*128);
	char *t1 = (char *)malloc(sizeof(char)*20);
	char *t2 = (char *)malloc(sizeof(char)*20);
	char *code1 = (char *)malloc(sizeof(char)*40);
	char *code2 = (char *)malloc(sizeof(char)*40);
	char *code3 = (char *)malloc(sizeof(char)*40);
	char *code0 = (char *)malloc(sizeof(char)*40);	
	char *label1 = (char *)malloc(sizeof(char)*40);
	char *label2 = (char *)malloc(sizeof(char)*40);
	char *label3 = (char *)malloc(sizeof(char)*40);
	if(root!=NULL)
	{
		if(root->left !=NULL)
		{
			if(root->left->right != NULL)
			{
				if(!strcmp(root->left->name,"Exp"))
				{
					return translate_exp_help(root->left,NULL);
				}
				else if(!strcmp(root->left->name,"RETURN"))
				{
					t1 = newtemp();
					code1 = translate_exp_help(root->left->right,t1);
					sprintf(code2,"RETURN %s\n",t1);
					sprintf(code,"%s%s\n",code1,code2);		
				}
				else if(!strcmp(root->left->name,"IF"))
				{
					if(root->left->right->right->right->right->right != NULL)
					{
						label1 = newlabel();
						label2 = newlabel();
						label3 = newlabel();
						code1 = translate_cond(root->left->right->right,label1,label2);
						code2 = translate_stmt_help(root->left->right->right->right->right);
						code3 = translate_stmt_help(root->left->right->right->right->right->right->right);
						sprintf(codetemp,"%sLABEL %s :\n%sGOTO %s\nLABEL %s :\n%sLABEL %s :\n",code1,label1,code2,label3,label2,code3,label3);
						strcat(code,codetemp);
					}
					else
					{
						label1 = newlabel();
						label2 = newlabel();
						code1 = translate_cond(root->left->right->right,label1,label2);
						code2 = translate_stmt_help(root->left->right->right->right->right);
						sprintf(codetemp,"%sLABEL %s :\n%sLABEL %s :\n",code1,label1,code2,label2);
						strcat(code,codetemp);
					}
				}
				else if(!strcmp(root->left->name,"WHILE"))
				{
					label1 = newlabel();
					label2 = newlabel();
					label3 = newlabel();
					code1 = translate_cond(root->left->right->right,label2,label3);
					code2 = translate_stmt_help(root->left->right->right->right->right);
					sprintf(codetemp,"LABEL %s :\n%sLABEL %s :\n%sGOTO %s\nLABEL %s :\n",label1,code1,label2,code2,label1,label3);
					strcat(code,codetemp);
				}
			}
			else
			{

			}
		}
	}
	return code;
}

char *translate_cond(struct node *root,char *label_true,char *label_false)
{
	
	char *code = (char *)malloc(sizeof(char)*1024);
	char *codetemp = (char *)malloc(sizeof(char)*128);
	char *t1 = (char *)malloc(sizeof(char)*20);
	char *t2 = (char *)malloc(sizeof(char)*20);
	char *code1 = (char *)malloc(sizeof(char)*40);
	char *code2 = (char *)malloc(sizeof(char)*40);
	char *code3 = (char *)malloc(sizeof(char)*40);
	char *code0 = (char *)malloc(sizeof(char)*40);		
	char *op = (char *)malloc(sizeof(char)*4);
	char *label1 = (char *)malloc(sizeof(char)*10);
	
	
	if(root != NULL)
	{
		if(root->left!=NULL)
		{
			if(!strcmp(root->left->right->name,"RELOP"))
			{
				t1 = newtemp();
				t2 = newtemp();
				code1 = translate_exp_help(root->left,t1);
				code2 = translate_exp_help(root->left->right->right,t2);
				op = root->left->right->op;
				sprintf(code3,"IF %s %s %s GOTO %s\n",t1,op,t2,label_true);
				sprintf(codetemp,"%s%s%sGOTO %s\n",code1,code2,code3,label_false);
				strcat(code,codetemp);
			}
			else if(!strcmp(root->left->name,"NOT"))
			{
				return translate_cond(root->left->right,label_false,label_true); 
			}
			else if(!strcmp(root->left->right->name,"AND"))
			{	
				label1 = newlabel();
				code1 = translate_cond(root->left,label1,label_false);
				code2 = translate_cond(root->left->right->right,label_true,label_false);
				sprintf(codetemp,"%sLABEL %s\n%s",code1,label1,code2);
				code = codetemp;
			}
			else if(!strcmp(root->left->right->name,"OR"))
			{
				label1 = newlabel();
				code1 = translate_cond(root->left,label_true,label1);
				code2 = translate_cond(root->left->right->right,label_true,label_false);
				sprintf(codetemp,"%sLABEL %s\n%s",code1,label1,code2);
				code = codetemp;
			}
			else
			{
				t1 = newtemp();
				code1 = translate_exp_help(root,t1);
				sprintf(code2,"IF %s != #0 GOTO %s\n",t1,label_true);
				sprintf(codetemp,"%s%sGOTO %s\n",code1,code2,label_false);
				code = codetemp;
			}			
		}
	}
	return code;	
}
void make_imcode(struct node* root)
{
	if(root != NULL)
	{
		if(!strcmp(root->name,"FunDec"))
		{	
			translate_func(root,NULL);
			make_imcode(root->left);
			make_imcode(root->right);
		}
		else if(!strcmp(root->name,"Stmt"))
		{
			translate_stmt(root);
			make_imcode(root->right);
		}
		else
		{
			make_imcode(root->left);
			make_imcode(root->right);
		}
	}
	return ;
	
}

void testintercode(struct InterCodes* intercodep)
{
	if(intercodep != NULL)
	{
		if(intercodep->codes != NULL)
			printf("%s",intercodep->codes);
		testintercode(intercodep->next);
	}
	return;		
}
