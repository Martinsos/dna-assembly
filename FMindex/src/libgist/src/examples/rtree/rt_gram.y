/*	rt_gram.y
 * Copyright (c) 1997, Regents of the University of California
 * $Header: /usr/local/devel/GiST/libgist/src/examples/rtree/rt_gram.y,v 1.1.1.1 1997/11/13 02:43:53 marcel Exp $
 */

%{
#include <iostream.h>
#ifndef UNIX
#include <malloc.h>
#endif
#include "gist_rtree.h"
#include "command.h"
#define YYDEBUG 1

void CommandPrompt();
int yyerror(char *msg);
int yylex();
%}

%union {
    rt_rect *key;
    rt_point * point;
    char *string;
    rt_query_t::rt_oper oper;
    rt_query_t *query;
    shpid_t pgno;
    int number;
    double dbl;
}

%token <string> ID
/* %token <key> STRCONST */
%token <number> INTCONST 
%token <dbl> DBLCONST
%token CREATE LOAD INSERT DROP OPEN CLOSE QUIT SELECT DELETE WHERE FROM NL ERROR CHECK
%token AND OR TRANSCRIPT OFF HELP DUMP ONLY CLEAR STATS BULK
%token '(' ')' ',' '>' '=' '<' opLE opGE opNE

%left OR
%left AND
%right NOT

%type <pgno> dump_page
%type <query> predicate where_clause
%type <key> rectkey
%type <point> point
%type <oper> rtoperator
%type <dbl> dub

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
	| DROP ID NL
		{ CommandDrop($2);
                  if ($2) delete [] $2; }
	| CLOSE ID NL
		{ CommandClose($2);
                  if ($2) delete [] $2; }
	| CHECK ID NL {
	    CommandCheck($2);
	    if ($2) delete [] $2;
	}
	| SELECT FROM ID where_clause NL
	{
	    CommandSelect($3, $4);
	    if ($3) delete [] $3;
	    if ($4) delete $4;
	}
	
	| INSERT ID point NL
	{
	    int data = 1;
	    CommandInsert($2, (void *) $3->coord, $3->dim() * sizeof(double),
		(void *) &data, sizeof(data));
	    if ($2) delete [] $2;
	    if ($3) {
		$3->dealloc();
		delete $3;
	    }
	}
	| DELETE FROM ID where_clause NL
	{
	    CommandDelete($3, $4);
	    if ($3) delete [] $3;
	    if ($4) delete $4;
	}
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
	  { $$ = new rt_query_t(rt_query_t::rt_nooper, rt_query_t::rt_pointarg, NULL); }
	| WHERE predicate
	  { $$ = $2; }
	;

rectkey
	:  '(' dub ',' dub ',' dub ',' dub ')'
                { $$ = new rt_rect(2);
                  $$->lo(0) = $2; // xlo
                  $$->hi(0) = $4; // xhi
                  $$->lo(1) = $6; // ylo
                  $$->hi(1) = $8; // yhi 
                }
	;

point
        : '(' dub ',' dub ')'
                { $$ = new rt_point(2);
		  (*$$)[0] = $2; // x-coord
		  (*$$)[1] = $4; // y-coord
                }
        ;

dub
        : INTCONST { $$ = (double)$1; }
        | DBLCONST  
        ;

predicate
	: ID rtoperator rectkey
		{ $$ = new rt_query_t($2, rt_query_t::rt_rectarg, $3); }
        |
          ID rtoperator point
		{ $$ = new rt_query_t($2, rt_query_t::rt_pointarg, $3); }
/*
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

rtoperator
	: '&'  { $$ = rt_query_t::rt_overlap;     }
	| '<'  { $$ = rt_query_t::rt_contained;  }
	| '>'  { $$ = rt_query_t::rt_contains;    }
	| '='  { $$ = rt_query_t::rt_equal;        }
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

