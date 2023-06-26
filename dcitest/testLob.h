#ifndef TESTLOB_H
#define TESTLOB_H
#include <cstdio>
#include <cstdlib>

#ifdef WIN32
#include <windows.h>
#endif

#include "DciTest.h"

#if _MSC_VER >= 1400
#pragma warning(disable : 4996)
#endif

#define BUFLEN  1024
#define FILELEN 5000

sword create_table_clob(char *table_name);
sword create_table_blob(char *table_name);

sword drop_table(char *table_name);

sword insert_table_clob(char *table_name, char *buf);
sword insert_table_blob(char *table_name, char *buf, int buflen);

DCILobLocator *select_table_clob(char *table_name, DCIDefine * pDefine, DCILobLocator *clob_loc, ub4 isUseClob, char *buf, ub4 buf_len, ub4 isUseBuf);
DCILobLocator *select_table_blob(char *table_name, DCIDefine * pDefine, DCILobLocator *clob_loc, ub4 isUseClob, char *buf, ub4 buf_len, ub4 isUseBuf);

sword TestLobCopy();
sword TestLobAssign();
sword TestLobAppend();
sword TestLobTrim();
sword TestLobErase();
sword TestLobEnableBuffering();
sword TestLobDisableBuffering();
sword TestLobFlushBuffer();
sword TestLobIsEqual();
sword TestLobLocatorIsInit();
sword TestLobCharSetForm();
sword TestLobCharSetId();
sword TestTempLobs();
sword TestDuration();
sword TestLobStream();
sword TestLobCallback();
#endif /* TESTLOB_H */
