result:syntax.y lab1.l tree.h imcode.h
	bison -d -v  syntax.y
	flex lab1.l
	gcc syntax.tab.c lex.yy.c tree.c imcode.c 
