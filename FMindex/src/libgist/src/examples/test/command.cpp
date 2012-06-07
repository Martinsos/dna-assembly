#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>
#ifdef UNIX
#include <unistd.h>
#endif
#include <string.h>

#include "gist.h"
#include "command.h"

#define MAX_TABLES 10
#define NOT_FOUND (-1)

struct Table
{
  char *name;
  gist_m *gist;
};

Table tables[MAX_TABLES];
int numTables = 0;

int GetTable(const char *name)
{
  int i;

  for (i=0; i<numTables; i++)
    if (!strcmp(tables[i].name, name))
      break;
  return i == numTables ? NOT_FOUND : i;
}

void CommandCreate(const char *table)
{
  if (numTables == MAX_TABLES) { 
    cout << "This progam can only handle " 
	 << MAX_TABLES << " open tables" << endl; 
    return; 
  }

  gist_m *gist = new gist_m;

  rc_t status = gist->create(table, extension);
  gist->flush();
  if (status != RCOK) {
    cout << "Error opening table." << endl;
    delete gist;
    return;
  }

  cout << "Table " << table << " created as type " << identStr << endl;

  tables[numTables].name = strdup(table);
  tables[numTables].gist = gist;
  numTables++;
}

#if 0
void CommandLoad(const char *table, const char *loadfile)
{
  if (numTables == MAX_TABLES) { 
    cout << "This progam can only handle " 
	 << MAX_TABLES << " open tables" << endl; 
    return; 
  }

  int i, count;
  gist_m *gist = CreateGiST();

  if ((i=GetTable(table)) != NOT_FOUND) {
    cerr << "Table already open!" << endl;
    return;
  }

  //count = gist->BulkLoad(table, loadfile);
  cout << count << " objects loaded" << endl;

  tables[numTables].name = strdup(table);
  tables[numTables].gist = gist;
  numTables++;
}

void
CommandBulkInsert(const char *table, const char *loadfile)
{
  if (numTables == MAX_TABLES) { 
    cout << "This progam can only handle " 
	 << MAX_TABLES << " open tables" << endl; 
    return; 
  }

  int i, count;
  if ((i=GetTable(table)) == NOT_FOUND) {
    cerr << "Table not open!" << endl;
    return;
  }

  gist_m *gist = tables[i].gist;
  //count = gist->BulkInsert(loadfile);
  cout << count << " objects inserted" << endl;
}
#endif

void CommandDrop(const char *table)
{
  if (GetTable(table) != NOT_FOUND) {
    cerr << "Cannot drop an open table." << endl;
    return;
  }
  if (unlink(table))
    cout << "There is no table by that name." << endl;
  else
    cout << "Table " << table << " dropped." << endl;
}

void CommandOpen(const char *table)
{
  if (numTables == MAX_TABLES) { 
    cout << "This progam can only handle " 
	 << MAX_TABLES << " open tables" << endl; 
    return; 
  }

  if (GetTable(table) != NOT_FOUND) {
    cout << "Table already open!" << endl;
    return;
  }

  gist_m *gist = new gist_m;

  rc_t status = gist->open(table, extension);

  if (status != RCOK) {
    delete gist;
    cout << "Error opening table." << endl;
    return;
  }

  cout << "Table " << table << " opened." << endl;

  tables[numTables].gist = gist;
  tables[numTables].name = strdup(table);
  numTables++;
}

void CommandClose(const char *table)
{
  int i;

  if ((i=GetTable(table)) == NOT_FOUND) {
    cerr << "Table is not open." << endl;
    return;
  }

  cout << "Table " << table << " closed." << endl;
  tables[i].gist->close();
  delete tables[i].gist;

  for ( ; i < numTables-1; i++)
    tables[i] = tables[i+1];
  numTables--;
}

void CommandSelect(const char *table,
		   void *query)
{
  int i;

  if ((i=GetTable(table)) == NOT_FOUND) {
    cerr << "Table is not open!" << endl;
    return;
  }

  gist_m *gist = tables[i].gist;
  gist_cursor_t cursor;

  if (gist->fetch_init(cursor, query) != RCOK) {
      cerr << "can't initialize cursor" << endl;
      return;
  }
  bool eof;
  char key[gist_p::max_tup_sz];
  smsize_t klen;
  char data[gist_p::max_tup_sz];
  smsize_t dlen;
  int cnt = 0;
  for (;;) {
      klen = gist_p::max_tup_sz;
      dlen = gist_p::max_tup_sz;
      if (gist->fetch(cursor, (void *) key, klen, (void *) data, dlen, eof) != RCOK) {
          cerr << "can't fetch from cursor" << endl;
	  return;
      }
      if (eof) break;
      // print key and data
      printDatum(key, klen, data, dlen);
      cnt++;
  }
  printf("retrieved %d items\n", cnt);
}

void CommandDelete(const char *table,
		   void *query)
{
  int i;
  if ((i = GetTable(table)) == NOT_FOUND) {
    cerr << "Table not open!" << endl;
    return;
  }

  tables[i].gist->remove(query);
  tables[i].gist->flush();
}

void CommandInsert(
    const char *table,
    void *key,
    int klen,
    void *data,
    int dlen)
{
  int i;
  if ((i = GetTable(table)) == NOT_FOUND) {
    cerr << "Table not open!" << endl;
    return;
  }

  gist_m *gist = tables[i].gist;
  printDatum(key, klen, data, dlen);
  if (gist->insert(key, klen, data, dlen) != RCOK) {
      cerr << "can't insert" << endl;
  }
}

void
CommandCheck(const char *table)
{
    int i;
    if ((i = GetTable(table)) == NOT_FOUND) {
        cerr << "Table not open!" << endl;
	return;
    }
    gist_m *gist = tables[i].gist;
    if (gist->check() != RCOK) {
        cerr << "can't check" << endl;
    }
}

void CommandQuit()
{
    for (int i=0; i<numTables; i++) {
	tables[i].gist->flush();
	delete tables[i].gist;
    }
  cout << "Goodbye." << endl;
  exit(0);
}

void CommandPrompt()
{
  cout << "gist" << "> ";
  cout.flush();
}

#if 0
// use GiSTgroupNotify so we can have both GiSTstats
// and GiSTtranscript on at the same time
void CommandStats(const char *table)
{
  // todo
}

void CommandClearStats(const char *table)
{
  // todo
}

// turn transcript on/off
void CommandTranscript(const char *table, GiSTbool on)
{
  int i;
  if ((i = GetTable(table)) == NOT_FOUND) {
    cerr << "Table not open!" << endl;
    return;
  }

  gist_m *gist = tables[i].gist;

  cout << "Transcript " << (on ? "ON" : "OFF") << endl;

  if (on) {
    new GiSTtranscript(*gist, cout);
  } else { // use default (dummy) message handler
    new GiSTnotify(*gist);
  }
}

void CommandHelp()
{
  ifstream is(GetGiSThelpFile());
  char c;

  while (is.get(c)) cout << c;
}
#endif

void CommandDump(const char *table, shpid_t pgno)
{
  int i;

  if ((i = GetTable(table)) == NOT_FOUND) {
    cout << "No such table is open." << endl;
    return;
  }

  tables[i].gist->dump(pgno);
}

int yyparse();


int main()
{
  cout.setf(ios::fixed);
  cout << "**  " << identStr << endl;
  cout << "Type \"help\" for a list of commands." << endl << endl;
  CommandPrompt();
  yyparse();
  return 0;
}
