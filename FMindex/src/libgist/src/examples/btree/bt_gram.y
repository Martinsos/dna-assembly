/*	bt_gram.y
 * Copyright (c) 1997, Regents of the University of California
 * $Header: /usr/local/devel/GiST/libgist/src/examples/btree/bt_gram.y,v 1.1.1.1 1997/11/13 02:43:53 marcel Exp $
 */

%{
#include <iostream.h>
#ifndef UNIX
#include <malloc.h>
#endif
#include "gist.h"
#include "bt_query.h"
#include "command.h"
#define YYDEBUG 1

void CommandPrompt();
int yyerror(char *msg);
int yylex();
%}

%union {
    char *string;
    bt_query_t::bt_oper oper;
    bt_query_t *query;
    int number;
    shpid_t pgno;
}

%token <string> ID STRING
%token <number> INTCONST 
%token CREATE LOAD INSERT DROP OPEN CLOSE QUIT SELECT DELETE WHERE FROM NL ERROR CHECK
%token AND OR TRANSCRIPT OFF HELP DUMP ONLY CLEAR STATS BULK
%token '(' ')' ',' '>' '=' '<' opLE opGE opNE

%type <pgno> dump_page
%type <query> predicate where_clause
%type <oper> btoperator

%start commands

%%

commands
    :
    | commands command
	{ CommandPrompt(); }
    ;

command
    : CREATE ID NL
	    { CommandCreate($2);
   	      if ($2) delete [] $2; }
/*
           | LOAD ID ID NL
                { CommandLoad($2, $3);
		  if ($2) delete [] $2;
                  if ($3) delete [] $3; }
        | BULK INSERT ID ID NL
                { CommandBulkInsert($3, $4);
		  if ($3) delete [] $3;
                  if ($4) delete [] $4; }
*/
    | OPEN ID NL
		{ CommandOpen($2);
		  if ($2) delete [] $2; }
	| CHECK ID NL {
	    CommandCheck($2);
	    if ($2) delete [] $2;
	}

	| DROP ID NL
		{ CommandDrop($2);
                  if ($2) delete [] $2; }
	| CLOSE ID NL
		{ CommandClose($2);
                  if ($2) delete [] $2; }
	| SELECT FROM ID where_clause NL
		{ CommandSelect($3, $4);
                  if ($3) delete [] $3;
                  if ($4) delete $4; }
	
	| INSERT ID INTCONST NL
{
    int key = $3;
    int data = 1;
    CommandInsert($2, (void *) &key, sizeof(key), (void *) &data, sizeof(data));
    if ($2) delete [] $2;
}
	| INSERT ID STRING NL
{
    int data = 1;
    CommandInsert($2, (void *) $3, strlen($3), (void *) &data, sizeof(data));
    if ($2) delete [] $2;
    if ($3) delete [] $3;
}
	| DELETE FROM ID where_clause NL
		{ CommandDelete($3, $4);
                  if ($3) delete [] $3;
                  if ($4) delete $4; }
	| QUIT NL
		{ CommandQuit(); }
/*
        | TRANSCRIPT ID NL
		{ CommandTranscript($2); 
                  if ($2) delete [] $2; }
        | TRANSCRIPT ID OFF NL
		{ CommandTranscript($2, FALSE); 
                  if ($2) delete [] $2; }
        | CLEAR STATS ID NL
		{ CommandClearStats($3); 
		  if ($3) delete [] $3; }
        | STATS ID NL
		{ CommandStats($2); 
		  if ($2) delete [] $2; }
        | HELP NL
                { CommandHelp(); }
        | DUMP ID dump_page ONLY NL
		{ CommandDump($2, $3,  FALSE);
                  if ($2) delete [] $2; }
*/
        | DUMP ID dump_page NL
		{ CommandDump($2, $3);
                  if ($2) delete [] $2; }
	| error NL
		{ yyclearin; yyerrok; }
	| NL
	;

dump_page
        :
	       { $$ = 0; }
        | INTCONST
	       { $$ = (shpid_t) $1; }
        ;

where_clause
	:
	  { $$ = new bt_query_t(bt_query_t::bt_nooper, NULL, NULL); }
	| WHERE predicate
	  { $$ = $2; }
	;

predicate
	: ID btoperator INTCONST
	{
	    int *i = new int;
	    *i = $3;
	    $$ = new bt_query_t($2, i, NULL);
	}
/*
        |
          ID btoperator point
		{ $$ = new rt_query_t($2, rt_query_t::rt_pointarg, $3); }
	| predicate OR predicate
		{ $$ = new OrPredicate($1, $3); }
	| predicate AND predicate
		{ $$ = new AndPredicate($1, $3); }
	| NOT predicate
		{ $$ = new NotPredicate($2); }
	| '(' predicate ')'
		{ $$ = $2; } 
*/
	;

btoperator
	: '<'  { $$ = bt_query_t::bt_lt; }
	| '>'  { $$ = bt_query_t::bt_gt; }
	| '='  { $$ = bt_query_t::bt_eq; }
	| opLE { $$ = bt_query_t::bt_le; }
	| opGE { $$ = bt_query_t::bt_ge; }
	;

%%

int yyerror(char *errmsg)
{
    extern char *yytext;

    cerr << "Parser error: " << errmsg << "!\n";

    return 1;
}

extern "C" int yywrap()
{
	return 0;
}

