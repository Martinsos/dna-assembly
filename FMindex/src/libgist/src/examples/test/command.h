//	command.h
// Copyright (c) 1996, Regents of the University of California
// $Header: /usr/local/devel/GiST/libgist/src/examples/test/command.h,v 1.1.1.1 1997/11/13 02:43:54 marcel Exp $

#ifndef COMMAND_H
#define COMMAND_H

void CommandCreate(const char *table);
void CommandCheck(const char *table);
void CommandDrop(const char *table);
void CommandOpen(const char *table);
void CommandClose(const char *table);
void CommandSelect(const char *table, void *query);
void CommandDelete(const char *table, void *query);
void CommandInsert(const char *table, void * key, int klen, void *data, int dlen);
void CommandQuit();
void CommandDump(const char *table, shpid_t pgno);

extern void printDatum(
    void *key,
    int klen,
    void *data,
    int dlen);

extern char *identStr;

extern gist_ext_t *extension;

#endif // COMMAND_H




