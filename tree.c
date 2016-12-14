
#include "tree.h"
int countlayer = 0;
struct node* newtree(char* name,int num,...)
{
	va_list valist;
	struct node* root = (struct node*)malloc(sizeof(struct node));
	root->left = NULL;root->right=NULL;
	struct node* temp = (struct node*)malloc(sizeof(struct node));
	temp->left=NULL;temp->right=NULL;
	if(!root || !temp)
	{
		error("Memory is out of space, initialization of node failure",0);
		exit(0);
	}
	else 
	{
		root->name = name;
	    	va_start(valist,num);
		if(num >= 1)
		{
			temp = va_arg(valist,struct node*);
			root->left = temp;

			/*if(!strcmp(root->name,"Program"))
			{
				printf("xixi: %d %d %d\n",root->line, yylineno,temp->line);
			}*/			
	
	
			root->line = temp->line;
			//root->line = yylineno;
			root->col = temp->col;
			/*root->inname = temp->inname;*/
			if(num >= 2)
			{
				int i;
				for(i = 0 ; i < num-1; i++)
				{
					temp->right = va_arg(valist,struct node*);		
					temp = temp->right;
				}

			}
		}
		else  //num==0
		{
			int t = va_arg(valist,int);
			root->line = t;
			int l = va_arg(valist,int);
			root->col = l;
			if((!strcmp(root->name,"ID"))||(!strcmp(root->name,"TYPE")))
			{
				char *t;
				t = (char*)malloc(sizeof(char)*40);
				strcpy(t,yytext);
				root->idtype = t;
				root->inname = t;
			}
			else if(!(strcmp(root->name,"INT")))
			{
				root->type = 1;
				if(yytext[0]=='0')
				{
					if(yytext[1] == 'x' || yytext[1] == 'X')
						root->intval = (int)strtol(&yytext[2],NULL,16);	
					else
						root->intval = (int)strtol(yytext,NULL,8);
				}
				else 
					root->intval = atoi(yytext);
				root->inname = "INT";
			}
			else if(!(strcmp(root->name,"FLOAT")))
			{
				root->floatval = atof(yytext);
				root->inname = "FLOAT";
				root->type = 2;
			}
			else if(!strcmp(root->name,"RELOP"))
			{
				char *t;
				t = (char*)malloc(sizeof(char)*40);
				strcpy(t,yytext);
				root->op = t;
				root->inname = t;
			}
			else
			{
				char *t;
				t = (char*)malloc(sizeof(char)*40);
				strcpy(t,yytext);
				root->inname = t;
			}

		}
	}
	return root;
}

void eval(struct node *root, int level)
{
	int i;
	if(root!=NULL)
	{
		if(root->line != -1)
		{

			for(i = 0 ; i < level ;i++)	
				printf("  ");
			printf("%s",root->name);
			if((!strcmp(root->name,"ID"))||(!strcmp(root->name,"TYPE")))
				printf(":%s inname:%s type:%d",root->idtype,root->inname,root->type);
			else if(!(strcmp(root->name,"INT")))	
				printf(":%d inname:%s type:%d",root->intval,root->inname,root->type);
			else if(!(strcmp(root->name,"FLOAT")))
				printf(":%f inname:%s type:%d",root->floatval,root->inname,root->type);
			else
				printf("(%d) inname:(%s) type:%d",root->line,root->inname,root->type);
			
			printf("\n");
		}
/*		if(root->line!=-1)
		{
			int flag = 1;
			if(root->right)
			{
				if(root->right->line!=-1)	
				{
					printf("\n");
					flag = 0;
				}
			}
			if(flag) printf("\n");
		}*/
		eval(root->left,level+1);
		eval(root->right,level);
	}
	return ;
}

struct node* makeinname(struct node* root)//自下往上
{
	if(root!=NULL)
	{
		root->left = makeinname(root->left);
		root->right = makeinname(root->right);
		if(root->inname == NULL)
		{
			if(root->left != NULL)
			{
				root->inname = root->left->inname;
						
			}
			if(!strcmp(root->name,"Def"))
			{
				root->left->right->inname = root->inname;
				
			}
			if(!strcmp(root->name,"ExtDef"))
			{
				root->left->right->inname = root->inname;
			}
		}
		/*if(root->type == NULL)
		{
			if(root->left!=NULL)
				if(root->left->type!=NULL)
					root->type = root->left->type;
		}*/
	}
	return root;
}

struct node* makeinname2(struct node* root)//自上往下
{
	if(root!=NULL)
	{
		if(root->left!=NULL) root->left->fathername = root->name;
		if(root->right!=NULL) root->right->fathername = root->name;

		if(!(strcmp(root->name,"DecList")))
		{
			if(root->left->right != NULL)
			{
				root->left->right->right->inname = root->inname;
			}
			//root->left->inname = root->inname;
		}
		if(!(strcmp(root->name,"ExtDecList")))
		{
			if(root->left->right != NULL)
			{
				root->left->right->right->inname = root->inname;
			}
			//root->left->inname = root->inname;
		}
		root->left = makeinname2(root->left);
		root->right = makeinname2(root->right);
	}
	return root;
}

struct node* maketype(struct node* root)
{
	if(root->left!=NULL)	root->left = maketype(root->left);
	if(root->right!=NULL)	root->right = maketype(root->right);

	if(root!=NULL)
	{
		//if(root->type == NULL)
		//{
			if(!strcmp(root->name,"ID"))
			{
				/*if(!strcmp(root->inname,"m"))
				{	printf("test make type:");testsymbollist(symbolhead);printf("test make type finish!\n");}*/
				root->type = symboltype(root->inname);
			}
			else if(root->left != NULL)
			{
				root->type = root->left->type;
				if(root->type==0&&root->left->right!=NULL)
				{
					root->type = root->left->right->type;
				} 
			}
			//root->type = findsymboltype(root->inname);
		//}

	}
	
	return root;
}

void deletesymbol(int num)
{
	int i;
	struct symbollist *symbolp1,*symbolp2;
	symbolp1 = symbolhead->next;
	symbolp2 = symbolhead;
	while(symbolp1!=NULL)
	{
		if(symbolp1->layer == num)
		{
			if(symbolp1 == symboltail)
			{
				symbolp2->next = NULL;
				symboltail = symbolp2;
				free(symbolp1);
			}
			else
			{
				symbolp2->next = symbolp1->next;
				free(symbolp1);
			}
		}
		symbolp1 = symbolp1->next;
		symbolp2 = symbolp2->next;
	}
	return;
}
int symboltype(char *name)//1-int,2-float,3-int[],4-float[],5-func,6-struct
{
	struct symbollist* symbolp = (struct symbollist*)malloc(sizeof(struct symbollist));
	symbolp = symbolhead->next;
	while(symbolp!=NULL)
	{
		if(!(strcmp(symbolp->name,name)))
		{
			return symbolp->type;
		}
		symbolp = symbolp->next;
	}
	return -1;	
}

void newsymbol(char *name,int type) //1-int,2-float,3-int[],4-float[],5-func,6-struct
{
	struct symbollist* symbolnode = (struct symbollist*)malloc(sizeof(struct symbollist));
	symbolnode->name = name;
	symbolnode->type = type;
	symbolnode->layer = countlayer;
	//symbolnode->tempname[] = {0};
	/*symbolnode->tempname = (char *)malloc(sizeof(char)*10);
	sprintf(symbolnode->tempname," ");*/
	symboltail->next = symbolnode;
	symboltail = symbolnode;
}

void newvar(int num,...)
{
	va_list valist;
	va_start(valist,num);
	struct varlist* var = (struct varlist*)malloc(sizeof(struct varlist));
	struct node* temp = (struct node*)malloc(sizeof(struct node));
	if(!var || !temp)
	{
		error("Memory is out of space,initialization of node failure",0);
		exit(0);
	}
	else
	{
		temp = va_arg(valist,struct node*);
		var->type = (!strcmp(temp->inname,"int")?1:2);//1-int,2-float
		temp = va_arg(valist,struct node*);
		var->name = temp->inname;
		newsymbol(var->name,var->type);//newsymbol,int-1,float-2
		vartail->next = var;
		//var->next = NULL;
		vartail = var;
	}
}

void newstruct(int num,...)
{
	va_list valist;
	va_start(valist,num);
	struct structlist* structnode = (struct structlist*)malloc(sizeof(struct structlist));
	//struct node* temp = (struct node*)malloc(sizeof(struct node));
	if(!structnode /*|| !temp*/)
	{
		error("Memory is out of space,initialization of node failure",0);
		exit(0);
	}
	else
	{
		//temp = va_arg(valist,struct node*);
		
		structnode->name = (va_arg(valist,struct node*))->inname;
		//temp = va_arg(valist,struct structtypelist*);
		structnode->type = va_arg(valist,struct structtypelist*);		
		newsymbol(structnode->name,6);//newsymbol,struct-6
		/*vartail->next = var;
		//var->next = NULL;
		vartail = var;*/
		structtail->next = structnode;
		structtail = structnode;
	}	
}
void newstructtype(int num,...)
{
	va_list valist;
	va_start(valist,num);
	struct structtypelist* structtypenode = (struct structtypelist*)malloc(sizeof(struct structtypelist));
	struct node* temp = (struct node*)malloc(sizeof(struct node));
	if(!structtypenode || !temp)
	{
		error("Memory is out of space,initialization of node failure",0);
		exit(0);
	}
	else
	{
		temp = va_arg(valist,struct node*);
		if(temp!= NULL) 
		{
			structtypenode->name = temp->inname; 
		}
		else 
		{
			structtypenode->name = NULL;
		}
		temp = va_arg(valist,struct node*);
		structtypenode->splist = newstructtypehelp(temp); //获得结构体的参数表
		//newsymbol(structnode->name,6);//newsymbol,struct-6
		structtypetail->next = structtypenode;
		structtypetail = structtypenode;
	}
}

struct varlist* newstructtypehelp(struct node* deflist)  /*DefList-> Def DefList | kong;*/
{
	struct varlist* splisthead = (struct varlist*)malloc(sizeof(struct varlist));
	struct varlist* splisttail = splisthead;
	splisttail->next = NULL;

	struct node* deflisttemp = (struct node*)malloc(sizeof(struct node));
	deflisttemp = deflist;

	while(deflisttemp!= NULL)
	{
		struct node* declisttemp = (struct node*)malloc(sizeof(struct node));
		declisttemp = deflisttemp->left->left->right;
		while(declisttemp->left->right!=NULL)
		{
			struct varlist* splist = (struct varlist*)malloc(sizeof(struct varlist));splist->next = NULL;
			splist->type = declisttemp->left->type;
			splist->name = declisttemp->left->inname;
			declisttemp = declisttemp->left->right->right;
			splisttail->next = splist;
			splisttail = splist;
		}	
		struct varlist* splist = (struct varlist*)malloc(sizeof(struct varlist));splist->next = NULL;
		splist->type = declisttemp->left->type;
		splist->name = declisttemp->left->inname;
		splisttail->next = splist;
		splisttail = splist;
		deflisttemp = deflisttemp->left->right;
		if(deflisttemp->left==NULL)
			break;
	}
	return splisthead;
}


void newarray(int num,...)
{
	va_list valist;
	va_start(valist,num);
	struct arraylist* array = (struct arraylist*)malloc(sizeof(struct arraylist));
	struct node* temp = (struct node*)malloc(sizeof(struct node));
	if(!array || !temp)
	{
		error("Memory is out of space,initialization of node failure",0);
		exit(0);
	}
	else
	{
		temp = va_arg(valist,struct node*);
		array->type = (!strcmp(temp->inname,"int"))?3:4; //int[]-3,float[]-4
		temp = va_arg(valist,struct node*);
		array->name = temp->inname;
		temp = va_arg(valist,struct node*);
		array->anum = temp->intval;
		newsymbol(array->name,array->type); //newsymbol,int[]-3,float[]-4
		arraytail->next = array;
		arraytail = array;
	}
}

void newfunc(int num,...)
{
	va_list valist;
	va_start(valist,num);
	struct funclist* func = (struct funclist*)malloc(sizeof(struct funclist));
	struct node* temp = (struct node*)malloc(sizeof(struct node));
	if(!func || !temp)
	{
		error("Memory is out of space,initialization of node failure",0);
		exit(0);
	}
	else
	{
		temp = va_arg(valist,struct node*);
		if(!strcmp(temp->inname,"int"))
			func->type = 1;
		else if(!strcmp(temp->inname,"float"))
			func->type = 2;
		else
			func->type = -1;
		//func->type = temp->inname;
		temp = va_arg(valist,struct node*);
		func->name = temp->inname;
		temp = va_arg(valist,struct node*);
		if(!strcmp(temp->name,"RP"))  //无参函数
		{
			func->funcpara.fpnum = 0;
			func->funcpara.fplist = NULL;
			//printf("hehe\n");	
		}
		else  //temp->name == VarList
		{
			/*func->funcpara = newfunchelp(temp);
			symboltail->next = func->funcpara.fplist->next;
			struct symbollist *temp = func->funcpara.fplist->next;
			while(temp->next!=NULL)
			{
				temp = temp->next;
			}
			symboltail = temp;
			testsymbollist(symbolhead);*/
			//printf("hehe:%d\n",func->funcpara.fpnum);
			func->funcpara = newfunchelp(temp);	
		}
		temp = va_arg(valist,struct node*);
		if(temp == NULL)
			func->returntype = -1;
		else
		{	
			//printf("%s\n",temp->name);
			func->returntype = getreturntype(temp);
		}
		if(func->returntype != -1 && func->returntype!=func->type)
		{
			printf("Error type 8 at Line %d: Type mismatched for return. \n",temp->line);
		}
		func->flag = va_arg(valist,int);	
		newsymbol(func->name,5);//newsymbol,func-5	
		functail->next = func;
		functail = func;
	}
	
}

struct funcpara newfunchelp(struct node* varlist) //获得函数参数表,明天来重新写,xixi,写完了
{
	int num = 0;
	struct symbollist* fplisthead = (struct symbollist*)malloc(sizeof(struct symbollist));
	struct symbollist* fplisttail = fplisthead;
	fplisttail->next = NULL;

	struct node* temp = (struct node*)malloc(sizeof(struct node));
	temp = varlist;

	while(temp->left->right != NULL)
	{
		struct symbollist* fplist = (struct symbollist*)malloc(sizeof(struct symbollist));fplist->next = NULL;
		num++;
		fplist->type = (!strcmp(temp->left->inname,"int"))?1:2; //xixi,test
		fplist->name = temp->left->left->right->inname;	

		//printf("type:%d name:%s\n",fplist->type,fplist->name);
		
		temp = temp->left->right->right;

		fplisttail->next = fplist;
		fplisttail = fplist;
		
	}
	struct symbollist* fplist = (struct symbollist*)malloc(sizeof(struct symbollist));fplist->next = NULL;
	num++;
	fplist->type = (!strcmp(temp->left->inname,"int"))?1:2; //xixi,test
	fplist->name = temp->left->left->right->inname;	

	//printf("type:%d name:%s\n",fplist->type,fplist->name);

	fplisttail->next = fplist;
	fplisttail = fplist;

	struct funcpara fpnum;
	fpnum.fpnum = num;
	fpnum.fplist = fplisthead;
	
	//while(fplisthead != NULL)
	 //{printf("fuck:%d shit:%s\n",fplisthead->type,fplist->name);fplisthead = fplisthead->next;}
	return fpnum;
}

int getreturntype(struct node* root)
{
	//printf("xxixixheheheh:%s\n",root->name);
	if(!(strcmp(root->name,"Compst")))
	{
		//printf("ffffff:%s\n",root->name);
		return getreturntype(root->left->right->right);
	}
	else if(!(strcmp(root->name,"StmtList")))
	{
		//printf("ffffff:%s\n",root->name);
		if(root->left!=NULL)
		{
			if(getreturntype(root->left) == -1)
				return getreturntype(root->left->right);
			else 
				return getreturntype(root->left);
		}
		else
			return -1;
	}
	else if(!strcmp(root->name,"Stmt"))
	{
		if(!strcmp(root->left->name,"RETURN"))
		{
			//printf("xxixixheheheh:%d %s\n",root->left->right->type,root->left->right->name);
			return (root->left->right->type);
		}	
		else
			return -1;
	}
	else 
		return -1;	
}


void testvarlist(struct varlist* varp)
{
	if(varp != NULL)
	{
		printf("name:%s type:%d->\n",varp->name,varp->type);
		testvarlist(varp->next);
	}
	return;		
}

void testarraylist(struct arraylist* arrayp)
{
	if(arrayp != NULL)
	{
		printf("name:%s type:%d anum:%d ->\n",arrayp->name,arrayp->type,arrayp->anum);
		testarraylist(arrayp->next);
	}
	return;		
}

void testfunclist(struct funclist* funcp)
{
	if(funcp != NULL)
	{
		printf("name:%s type:%d flag:%d fpnum:%d-> returntype:%d \n",funcp->name,funcp->type,funcp->flag,funcp->funcpara.fpnum,funcp->returntype);
		struct symbollist* temp = funcp->funcpara.fplist;
		while(temp!=NULL)
		{printf("type:%d name:%s\n",temp->type,temp->name);temp = temp->next;}

		//while(fplisthead != NULL)
	 	//{printf("fuck:%d shit:%s\n",fplisthead->type,fplist->name);fplisthead = fplisthead->next;}
		//while(funcp->funcpara.fplist!=NULL)
		//	printf("type:%s name:%s\n",funcp->funcpara.fplist->type,funcp->funcpara.fplist->name);
		testfunclist(funcp->next);
	}
	return;		
}

void teststructlist(struct structlist* structp)
{
	if(structp != NULL)
	{
		printf("name:%s ->\n",structp->name);
		teststructlist(structp->next);
	}
	return;		
}

void teststructtypelist(struct structtypelist* structtypep)
{
	if(structtypep != NULL)
	{
		/*if(structtypep->name!=NULL)*/ 
		printf("typename:%s \t",structtypep->name); 
		/*else printf("typename:NULL\t");*/
		testvarlist(structtypep->splist);
		//printf("xixi");
		teststructtypelist(structtypep->next);
	}
	return;		
}

void testsymbollist(struct symbollist* symbolp)
{
	if(symbolp != NULL)
	{
		printf("name:%s type:%d layer:%d->\n",symbolp->name,symbolp->type,symbolp->layer);
		testsymbollist(symbolp->next);
	}
	return;		
}

void check(struct node* root)
{
	if(root != NULL)
	{
		/*if(!strcmp(root->name,"RC"))
		{
			printf("before delete symbol: layer=%d line=%d\n",countlayer,root->line);
			testsymbollist(symbolhead);
			deletesymbol(countlayer);
			testsymbollist(symbolhead);
			printf("finish delete!\n");
			countlayer--;
		}
		else if(!strcmp(root->name,"LC"))
		{
			countlayer++;
			printf("count++ at line %d\n",root->line);
		}
		else */if(!strcmp(root->name,"ExtDecList"))
		{	
			if(root->left->left->left == NULL) //全局变量
			{
				if(!strcmp(root->inname,"int")||!strcmp(root->inname,"float"))
				{
					if(findsymbol(root->left->inname))
						printf("Error type 3 at Line %d: Redefined variable \"%s\"\n",root->left->line,root->left->inname);
					else
						newvar(2,root,root->left);
				}
			}
			else //全局数组
			{
				if(findarray(root->left->inname))
					printf("Error type 3 at Line %d: Redefined array \"%s\"\n",root->left->line,root->left->inname);
				else
				{
					if(findvar(root->left->inname)||findstruct(root->left->inname)||findfunc(root->left->inname))
					{
						printf("Error type 10 at Line %d: Use [] operator to non-array \"%s\"\n",root->left->line,root->left->inname);
					}	
					else
					{
						newarray(3,root,root->left,root->left->left->right->right);
					}
				}
			}
		}
		else if(!strcmp(root->name,"ParamDec"))
		{			
			countlayer++;
			newvar(2,root->left,root->left->right);
			countlayer--;
		}
		else if(!strcmp(root->name,"DecList"))
		{	
			//printf("xixi\n");
			if(root->left->left->left->left == NULL) //局部变量
			{
				//printf("fuck: %s, %d\n",root->left->inname,findvar(root->left->inname));
				if(!strcmp(root->inname,"int")||!strcmp(root->inname,"float"))
				{
					//if(findvar(root->left->inname)||findarray(root->left->inname)||findstruct(root->left->inname)||findfunc(root->left->inname))
					if(findsymbol(root->left->inname))
					{
						printf("Error type 3 at Line %d: Redefined variable \"%s\"\n",root->left->line,root->left->inname);
					}				
					else
						newvar(2,root,root->left);
				}
			}
			else //局部数组
			{
				if(findarray(root->left->inname))
					printf("Error type 3 at Line %d: Redefined array \"%s\"\n",root->left->line,root->left->inname);
				else
				{
					//if(findvar(root->left->inname)||findstruct(root->left->inname)||findfunc(root->left->inname))	
					if(findsymbol(root->left->inname))
					{
						printf("Error type 10 at Line %d: Use [] operator to non-array \"%s\"\n",root->left->line,root->left->inname);
					}
					else				
					{
						newarray(3,root,root->left,root->left->left->left->right->right);
					}
				}			
			}
		}
		check(root->left);
		check(root->right);
	}
	return ;
}

void check2(struct node* root)
{
	if(root!=NULL)
	{
		/*if(!strcmp(root->name,"RC"))
		{
			printf("before delete symbol: layer=%d line=%d\n",countlayer,root->line);
			testsymbollist(symbolhead);
			deletesymbol(countlayer);
			testsymbollist(symbolhead);
			printf("finish delete!\n");
			countlayer--;
		}
		else if(!strcmp(root->name,"LC"))
		{
			countlayer++;
			printf("count++ at line %d\n",root->line);
		}
		else */if(!strcmp(root->name,"Exp"))
		{
			//printf("xixi:%s\n",root->inname);
			if(!findsymbol(root->inname)&&!strcmp(root->fathername,"Exp")&&(root->type==0||root->type==-1)) //避免Exp->Exp...报两次
			{
				//printf("hehe:%s %s %s\n",root->inname,root->name,root->fathername);
				printf("Error type 1 at Line %d: Use undefined  \"%s\" \n",root->line,root->inname);
			}
			else
			{
				if(root->left->right!=NULL)  
				{
					if(!strcmp(root->left->right->name,"ASSIGNOP"))//等号两边的判断s
					{
						if(!strcmp(root->left->inname,"INT")|| !strcmp(root->left->inname,"FLOAT") || root->left->type==5) //普通数据或者是函数名
						{
							printf("Error type 6 at Line %d: Right value at assinglop's left side\n",root->line);
						}
						else if(root->left->type != root->left->right->right->type)
						{
							if((root->left->type==1&&root->left->right->right->type==3)||(root->left->type==2&&root->left->right->right->type==4))			{}
							else if(root->left->right->right->type==5||root->left->right->right->type==6) //等号右边为函数或结构体
							{

							}
							else if(root->left->type != -1 && root->left->right->right->type != -1)
								printf("Error type 5 at Line %d: Type mismatched for assignment\n",root->line);
						}
					
					}
				
					else if(!strcmp(root->left->right->name,"PLUS") || !strcmp(root->left->right->name,"MINUS") || !strcmp(root->left->right->name,"STAR") || !strcmp(root->left->right->name,"DIV"))
					{
						if(root->left->type != root->left->right->right->type)
						{	
							printf("Error type 7 at Line %d: Type mismatched for operands\n",root->line);
						}
					}
			


					else if(!strcmp(root->left->right->name,"LB"))
					{
						if(!findarray(root->left->inname))
						{
							printf("Error type 1 at Line %d: Use undefined array \"%s\" \n",root->line,root->left->inname);
						}
						else
						{
							if(root->left->right->right->type != 1)
							{
								printf("Error type 11 at Line %d: In array access operators appear non integer\n",root->line);
							}
						}
					}

					else if(!strcmp(root->left->right->name,"DOT"))
					{
						if(!findstruct(root->left->inname))
						{
							printf("Error type 13 at Line %d: Use undefined struct \"%s\" \n",root->line,root->left->inname);
						}
						else
						{
							if(root->left->left!=NULL)
							{
								if(!strcmp(root->left->left->name,"ID"))
								{printf("xixi\n");
									if(!findstructarg(root->left->left->inname,root->left->right->right->inname))//struct name,filed name
									{
										printf("Error type 14 at Line %d: Non-existent field \"%s\"",root->left->left->line,root->left->right->right->inname);
									}
								}
							}
						}
					}				
				}
			

				if(!strcmp(root->left->name,"ID"))  //函数的调用
				{
					if(root->left!=NULL)
					{

						if(root->left->right!=NULL)
						{
							if(!strcmp(root->left->right->name,"LP"))
							{
								if(!findsymbol(root->left->inname))
								{
									printf("Error type 2 at Line %d: undefined function \"%s\" \n",root->line,root->left->inname);
								}
								else if(!findfunc(root->left->inname))
								{
									printf("Error type 11 at Line %d: Use () to non-function symbol: \"%s\" \n",root->line,root->left->inname);
								}
								else
								{	
									if(!(strcmp(root->left->right->name,"RP"))) //EXP->ID LP RP
									{	
										if(getpnum(root->left->inname) != 0)
										{
											printf("Error type 9 at Line %d:Funct call parameters do not match \n",root->left->line);
										}
									}
									else //EXP->ID LP Args RP
									{
										if(getpnum(root->left->inname) != getargnum(root->left->right->right))
										{
											printf("Error type 9 at Line %d:Funct call parameters do not match",root->left->line);
										}
										else if(!matcharglist(root->left->inname,root->left->right->right))
										{
											printf("Error type 9 at Line %d:Funct call parameters do not match\n",root->left->line);
										}
									}
								}
							}
						}
					}
				}
			}
		}
		
		else if(!strcmp(root->name,"FunDec"))  //函数定义声明,不能放在check中，因为还没type赋值完毕，嘻嘻
		{
			
			if(!strcmp(root->right->name,"SEMI")) // 函数声明
			{
					/*if(findfunc(root->left->right->left->inname))  //函数允许多次声明，但只能定义一次
						printf("Error type")*/
				newfunc(5,root,root->left,root->left->right->right,NULL,0); //最后一个1表示定义,0表示声明
				//printf("xixi:%s",root->inname);
			}
			else		//函数定义 FunDec->ID LP VarList RP
			{
				if(findfunc(root->left->inname))   
				{
					if(flagfunc(root->left->inname)==0) //函数声明但未定义
					{
							
					}	
					else   //函数重复定义
					{
						printf("Error type 4 at Line %d: Redefined func \"%s\"\n",root->line,root->left->inname);
					}
				}
				//else if(findvar(root->left->inname)||findarray(root->left->inname)||findstruct(root->left->inname))
				else if(findsymbol(root->left->inname))				
				{
					printf("Error type 3 at Line %d: Redefined symbol name \"%s\"\n",root->line,root->left->inname);
				}
				else  //函数表中未查到
				{
					newfunc(5,root,root->left,root->left->right->right,root->right,1); //最后一个1表示定义,0表示声明
				}
			}
			
		}
		else if(!strcmp(root->name,"Specifier"))  //Struct,xixi
		{
			
			if(!strcmp(root->left->name,"StructSpecifier"))
			{
				if(!strcmp(root->left->left->right->name,"OptTag"))  //STRUCT OptTag LC DefList RC ,OptTag-> ID | 空；  结构体的定义
				{	
					if(root->left->left->right->left!=NULL) //OptTag-> ID
					{	
						if(findstructtype(root->left->left->right->inname))
						{
							printf("Error type 3 at Line %d: Redefined struct \"%s\"\n",root->line,root->left->left->right->inname);
						}
						else
						{
							if(findstructtype(root->left->left->right->inname))
							{
								printf("Error type 3 at Line %d:Redefined struct \"%s\"\n",root->left->left->right->line,root->left->left->right->inname);
							}	
							else
								newstructtype(2,root->left->left->right,root->left->left->right->right->right); //name,DefList						
							
							if(!strcmp(root->right->name,"ExtDecList"))
							{
								struct node* extlisttemp = (struct node*)malloc(sizeof(struct node));
								extlisttemp = root->right;
								while(extlisttemp->left->right!=NULL)
								{
									if(findsymbol(extlisttemp->left->inname))
									{
										printf("Error type 16 at Line %d: Reused struct name \"%s\"\n",extlisttemp->line,extlisttemp->left->inname);
									}
									else
										newstruct(2,extlisttemp->left,structtypetail); //name,type's point
									extlisttemp = extlisttemp->left->right->right;
								}
								if(findsymbol(extlisttemp->left->inname))
								{
									printf("Error type 16 at Line %d: Reused struct name \"%s\"\n",extlisttemp->line,extlisttemp->left->inname);
								}
								else
								{
									newstruct(2,extlisttemp->left,structtypetail); //name,type's point
								}
							}
							else if(!strcmp(root->right->name,"DecList"))
							{
								struct node* declisttemp = (struct node*)malloc(sizeof(struct node));
								declisttemp = root->right;
								while(declisttemp->left->right!=NULL)
								{									 										if(findsymbol(declisttemp->left->inname))
									{
										printf("Error type 16 at Line %d: Reused struct name \"%s\"\n",declisttemp->line,declisttemp->left->inname);
									}
									else 
									{
										if(declisttemp->left->left->right!=NULL)
										{
											printf("Error type 15 at Line %d:Initialize the domain structure when define struct\n",declisttemp->line);
										}
										else
										{
											newstruct(2,declisttemp->left,structtypetail); //name,type's point
										}									
									}
									declisttemp = declisttemp->left->right->right;
								}
								if(findsymbol(declisttemp->left->inname))
								{
									printf("Error type 16 at Line %d: Reused struct name \"%s\"\n",declisttemp->line,declisttemp->left->inname);
								}
								else
								{	
									if(declisttemp->left->left->right!=NULL)
									{
										printf("Error type 15 at Line %d:Initialize the domain structure when define struct\n",declisttemp->line);
									}
									else
									{
										newstruct(2,declisttemp->left,structtypetail); //name,type's point
									}
								}
							}
						}
					}
					else //无名结构体
					{
						newstructtype(2,NULL,root->left->left->right->right->right); //name,DefList
					}
				}
				else  //STRUCT Tag  结构体类型的调用，root是Sperifier
				{
					char *typename = root->left->left->right->inname;
					if(!findstructtype(typename))
					{
						printf("Error type 1 at Line %d: Undefined struct \"%s\"\n",root->line,root->left->left->right->inname);
					}
					else
					{
						struct structtypelist* structtypep = getstructtypep(typename);
						if(!strcmp(root->right->name,"ExtDecList"))
						{
							struct node* extlisttemp = (struct node*)malloc(sizeof(struct node));
							extlisttemp = root->right;
							while(extlisttemp->left->right!=NULL)
							{
								if(findsymbol(extlisttemp->left->inname))
								{
									printf("Error type 16 at Line %d: Reused struct name \"%s\"\n",extlisttemp->line,extlisttemp->left->inname);
								}
								else
									newstruct(2,extlisttemp->left,structtypep); //name,type's point
								extlisttemp = extlisttemp->left->right->right;
							}
							if(findsymbol(extlisttemp->left->inname))
							{
								printf("Error type 16 at Line %d: Reused struct name \"%s\"\n",extlisttemp->line,extlisttemp->left->inname);
							}
							else
							{
								newstruct(2,extlisttemp->left,structtypep); //name,type's point
							}
						}
						else if(!strcmp(root->right->name,"DecList"))
						{
							struct node* declisttemp = (struct node*)malloc(sizeof(struct node));
							declisttemp = root->right;
							while(declisttemp->left->right!=NULL)
							{
								if(findsymbol(declisttemp->left->inname))
								{
									printf("Error type 16 at Line %d: Reused struct name \"%s\"\n",declisttemp->line,declisttemp->left->inname);
								}
								else 
								{
									if(declisttemp->left->left->right!=NULL)
									{
										printf("Error type 15 at Line %d:Initialize the domain structure when define struct\n",declisttemp->line);
									}
									else
									{
										newstruct(2,declisttemp->left,structtypep); //name,type's point
									}									
								}
								declisttemp = declisttemp->left->right->right;
							}
							if(findsymbol(declisttemp->left->inname))
							{
								printf("Error type 16 at Line %d: Reused struct name \"%s\"\n",declisttemp->line,declisttemp->left->inname);
							}
							else
							{	
								if(declisttemp->left->left->right!=NULL)
								{
									printf("Error type 15 at Line %d:Initialize the domain structure when define struct\n",declisttemp->line);
								}
								else
								{
									newstruct(2,declisttemp->left,structtypep); //name,type's point
								}
							}
						}
					}
				}
			}
		}
		check2(root->left);
		check2(root->right);
	}
	return;
}
int findvar(char *name)
{	
	struct varlist* varp = (struct varlist*)malloc(sizeof(struct varlist));
	varp = varhead->next;
	while(varp!=NULL)
	{
		if(!(strcmp(varp->name,name)))
		{
			return 1;
		}
		varp = varp->next;
	}
	return 0;
}

int findarray(char *name)
{	
	struct arraylist* arrayp = (struct arraylist*)malloc(sizeof(struct arraylist));
	arrayp = arrayhead->next;
	while(arrayp!=NULL)
	{
		if(!(strcmp(arrayp->name,name)))
		{
			return 1;
		}
		arrayp = arrayp->next;
	}
	return 0;
}

int findfunc(char *name)
{	
	struct funclist* funcp = (struct funclist*)malloc(sizeof(struct funclist));
	funcp = funchead->next;
	while(funcp!=NULL)
	{
		if(!(strcmp(funcp->name,name)))
		{
			return 1;
		}
		funcp = funcp->next;
	}
	return 0;
}

int findstruct(char *name)
{	
	struct structlist* structp = (struct structlist*)malloc(sizeof(struct structlist));
	structp = structhead->next;
	while(structp!=NULL)
	{
		if(!(strcmp(structp->name,name)))
		{
			return 1;
		}
		structp = structp->next;
	}
	return 0;
}

int findstructtype(char *name)
{	
	struct structtypelist* structtypep = (struct structtypelist*)malloc(sizeof(struct structtypelist));
	structtypep = structtypehead->next;
	while(structtypep!=NULL)
	{
		if(structtypep->name==NULL)
		{}	
		else if(!(strcmp(structtypep->name,name)))
		{
			return 1;
		}
		structtypep = structtypep->next;
	}
	return 0;
}

int findstructarg(char *structname,char *fieldname)
{
	return 0;
}
int findsymbol(char *name)
{	
	struct symbollist* symbolp = (struct symbollist*)malloc(sizeof(struct symbollist));
	symbolp = symbolhead->next;
	while(symbolp!=NULL)
	{
		if(!(strcmp(symbolp->name,name)))
		{
			return 1;
		}
		symbolp = symbolp->next;
	}
	return 0;
}

int getpnum(char *name) //for func
{	
	struct funclist* funcp = (struct funclist*)malloc(sizeof(struct funclist));
	funcp = funchead->next;
	while(funcp!=NULL)
	{
		if(!(strcmp(funcp->name,name)))
		{
			return funcp->funcpara.fpnum;
		}
		funcp = funcp->next;
	}
	return 0;
}


int flagfunc(char *name) //for func
{	
	struct funclist* funcp = (struct funclist*)malloc(sizeof(struct funclist));
	funcp = funchead->next;
	while(funcp!=NULL)
	{
		if(!(strcmp(funcp->name,name)))
		{
			return funcp->flag;
		}
		funcp = funcp->next;
	}
	return -1;
}


int pnumfunc(char *name) //for func
{
	struct funclist* funcp = (struct funclist*)malloc(sizeof(struct funclist));
	funcp = funchead->next;
	while(funcp!=NULL)
	{
		if(!(strcmp(funcp->name,name)))
		{
			return funcp->funcpara.fpnum;
		}
		funcp = funcp->next;
	}
	return -1;
}

int getargnum(struct node* root)
{
	if(root->left->right!=NULL)
		return 1+getargnum(root->left->right->right);
	else
		return 1;
}


int matcharglist(char *name,struct node* arglist) //1-arg match,0-arg unmatch,-1-not find func
{
	struct funclist* funcp = (struct funclist*)malloc(sizeof(struct funclist));
	funcp = funchead->next;
	struct node* argnode = (struct node*)malloc(sizeof(struct node));
	argnode = arglist;
	while(funcp!=NULL)
	{
		if(!(strcmp(funcp->name,name)))
		{
			struct funcpara para = funcp->funcpara;
			struct symbollist* paranode = (struct symbollist*)malloc(sizeof(struct varlist));
			paranode = para.fplist->next;
			//int paranum = para.fpnum;
			int i;
			for(i = 0 ; i < para.fpnum; i++)
			{
				if(argnode==NULL && paranode==NULL)
					return 1;
				if(argnode->type==paranode->type)
				{
					if(argnode->left->right!=NULL)
						argnode = argnode->left->right->right;
					paranode = paranode->next;
				}
				else if(argnode==NULL || paranode==NULL)
					return 0;
				else if(argnode->type!=paranode->type)
					return 0;
				else
					printf("xixxixixixixii\n");
			}
				
		}
		funcp = funcp->next;
	}
	return -1;	
}

struct structtypelist* getstructtypep(char *name)
{
	struct structtypelist* structtypep = (struct structtypelist*)malloc(sizeof(struct structtypelist));
	structtypep = structtypehead->next;
	while(structtypep!=NULL)
	{
		if(!(strcmp(structtypep->name,name)))
		{
			return structtypep;
		}
		structtypep = structtypep->next;
	}
	return NULL;
}

void checkfunc(struct funclist* funcp)
{
	if(funcp != NULL)
	{
		if(funcp->flag==0 && funcp->name!=NULL)
			printf("Error type 18 : Function \"%s\" declaration, but not defined\n",funcp->name);
		checkfunc(funcp->next);
	}
	return;	
}


void error(char*s,int line)
{
    //va_list ap;
    //va_start(ap,s);
   // vfprintf(stderr,s,ap);
    if(line!=0)
    fprintf(stderr,"Error type[%s] at Line:%d \n",s,line);
    else
    fprintf(stderr,"%s",s);
   // fprintf(stderr,"\n");
}

void yyerror(char*s,...)
{
    va_list ap;
    va_start(ap,s);
    fprintf(stderr,"syntax error:%d",yylineno);
    vfprintf(stderr,s,ap);
    fprintf(stderr,"\n");
}

