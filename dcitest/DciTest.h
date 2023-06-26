#ifndef DCITEST_H
#define DCITEST_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifdef WIN32
#include <windows.h>
#include <process.h>
#else
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#endif

#include "dci.h"
#include "testLob.h"
#include "package.h"
#include "dcimisc.h"
#include "connect.h"
#include "errcode.h"
#include "testNumber.h"
#include "testPiece.h"
#include "testHandleType.h"
#include "testDocCases.h"
#include "testDirpath.h"
/*
bug#17888:for BIG_ENDIAN operation
*/
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif

#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

/* bug#17888:fix byte order problem for ppc/ft.*/
#if !defined(BYTE_ORDER) || (BYTE_ORDER != LITTLE_ENDIAN && BYTE_ORDER != BIG_ENDIAN)
#error Define BYTE_ORDER to be equal to either LITTLE_ENDIAN or BIG_ENDIAN
#endif

#if _MSC_VER >= 1400
#pragma warning(disable : 4996)
#endif

#ifdef WIN32
typedef __int64			KDB_INT64;
#define INT64_FORMAT	"%I64d"
#elif defined(__LP64__) || defined(_LP64) /* Linux 64 */
typedef long			KDB_INT64;
#define INT64_FORMAT	"%ld"
#else
typedef long long		KDB_INT64;
#define INT64_FORMAT	"%lld"
#endif

#ifdef WIN32
#define snprintf  _snprintf
#endif
#ifndef WIN32
#define stricmp strcasecmp
#endif /* WIN32 */

#define NAME_LEN 64

extern char	User[NAME_LEN];
extern char	Pwd[NAME_LEN];
extern char	DbName[NAME_LEN];

extern sword		err;
extern DCIEnv		*pEnv;
extern DCIError		*pError;
extern DCISvcCtx	*pSvcCtx;
extern DCIServer	*pServer;
extern DCISession	*pSession;

#define BLOCK_SIZE 8192

sword	execute(char *sql);
void	check(sword err, const char *msg);
void	print_bar(void);
void	report_error(DCIError *pError);

sword	Fetch(DCIEnv *env, DCIStmt *stmhp, DCIError *pError, ub4 cFetchRows);
sword	compare_file();

sword	test_execute(char *sql);
sword	test_select(char *sql);
sword   test_select_no_print(char *sql);
extern int test_failed_num;

#endif //DCITEST_H
