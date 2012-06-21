/*	bt_scan.l
 * Copyright (c) 1997, Regents of the University of California
 * $Header: /usr/local/devel/GiST/libgist/src/examples/btree/bt_scan.l,v 1.1.1.1 1997/11/13 02:43:53 marcel Exp $
 */

%{
#include <iostream.h>
#include <string.h>
#include "bt_query.h"
#include "gist.h"
#include "bt_gram_tab.h"
%}

ws		[ \t]+
comment	#[^\n]*
id		[a-zA-Z][a-zA-Z0-9]*
string		\"[a-zA-Z][a-zA-Z0-9]*\"
nl		[\r\n]
int             "-"?[0-9]+

%%

{ws}	;
{comment} ;
{int}     { yylval.number = atoi((const char *)yytext);
	    return INTCONST;
	  }
"(" |
")" |
"," |
"<" |
">" |
"&" |
"~" |
"="  { return yytext[0]; }
"<>" { return opNE; }
"<=" { return opLE; }
">=" { return opGE; }

create		{ return CREATE; }
check		{ return CHECK; }
load		{ return LOAD; }
bulk		{ return BULK; }
open            { return OPEN; }
close           { return CLOSE; }
drop		{ return DROP; }
insert		{ return INSERT; }
delete          { return DELETE; }
select          { return SELECT; }
transcript      { return TRANSCRIPT; }
off             { return OFF; }
clear           { return CLEAR; }
stats           { return STATS; }
dump		{ return DUMP; }
only            { return ONLY; }
help            { return HELP; }
from            { return FROM; }
and             { return AND; }
or              { return OR; }
where		{ return WHERE; }
quit		{ return QUIT; }
{id}	        { int len = strlen(yytext);
                  yylval.string = new char[len+1];
                  strcpy(yylval.string, yytext);
                  yylval.string[len] = '\0'; return ID; }
{string}	{ int len = strlen(yytext);
                  yylval.string = new char[len-1];
                  strncpy(yylval.string, yytext+1, len-2);
                  yylval.string[len-2] = '\0'; return STRING; }
{nl}		{ return NL; }
<<EOF>>         { yyterminate(); }
.		{ cerr << "Lexical error in input string: invalid character\n";
		  return ERROR; }

%%
