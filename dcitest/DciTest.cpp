#include "DciTest.h"
#include "testUTF16.h"

static int ParseArg(int argc, char* argv[]);
static int testReadData();
sword test_multi_thread_test();

char	ExpFile[] = "expected.txt";
char	ResFile[] = "result.txt";

FILE    *fp = NULL;

char	User[NAME_LEN] = "SYSTEM";
char	Pwd[NAME_LEN] = "123456";
char	DbName[NAME_LEN] = "KingbaseES";

sword		err;
DCIEnv		*pEnv = NULL;
DCIError	*pError = NULL;
DCISvcCtx	*pSvcCtx = NULL;
DCIServer	*pServer = NULL;
DCISession	*pSession = NULL;

int test_failed_num = 0;
char *test_failed_list[1024];

/* bug#15667:dailybuild failed result from path error.*/
#define MAX_PATH_LEN 1024
char gstrEXEpath[MAX_PATH_LEN];
char gstrPath[MAX_PATH_LEN];
int gCnt = 0;

/* GetFilePath:
 * get the path(include neither executable file nor suffix) of dcitest.
 */
void 
GetFilePath()
{
	char *p = NULL;
	char strbuf[MAX_PATH_LEN];
	int iIndex = 0;

	/*get linux an windows path exclude ".exe" file path*/
#ifdef WIN32
	p = strrchr(gstrEXEpath, '\\');
#else
	p = strrchr(gstrEXEpath, '/');
#endif
	iIndex = p - gstrEXEpath;
	memset(strbuf, 0, MAX_PATH_LEN);
	for (int i = 0; i < iIndex; i++)
	{
		strbuf[i] = gstrEXEpath[i];
	}
	strbuf[iIndex + 1] = '\0';

#ifndef WIN32
		/*directly copy filepath to gstrPath in linux*/
		memset(gstrPath, 0, MAX_PATH_LEN);
		strcpy(gstrPath, strbuf);
		strcat(gstrPath, "/");
#else
		/*find the directory of dcitest in windows*/
		p = strrchr(strbuf, '\\');
		iIndex = p - strbuf;
		memset(gstrPath, 0, MAX_PATH_LEN);
		for (int j = 0;j < iIndex + 1;j++)
		{
			gstrPath[j] = strbuf[j];
		}
		gstrPath[iIndex + 1] = '\0';
#endif
		gCnt = strlen(gstrPath);
}

void
check(sword err, const char *msg)
{
	assert(msg != NULL);

	if (err == DCI_SUCCESS)
		printf("%s: PASS\n", msg);
	else
	{
		printf("%s: FAILED\n", msg);
		if (test_failed_num == sizeof(test_failed_list))
		{
			printf("sizeof(test_failed_list is) too small.\n");
			exit(1);
		}
		test_failed_list[test_failed_num++] = strdup(msg);
	}
}

void
print_bar(void)
{
	printf("------------------------------------------------------------------");
	printf("\n");
}

//print error message
void report_error(DCIError *pError)
{
	DciText msgbuf[512] = "";
	sb4		errcode = 0;

	if (pError == NULL)
		return;
	
	err = DCIErrorGet((dvoid *) pError, (ub4) 1, (text *) NULL, &errcode,
			msgbuf, (ub4) sizeof(msgbuf), (ub4) DCI_HTYPE_ERROR);

	if (err != DCI_NO_DATA && errcode != 0)
	{
		printf("Test failed \n");
		printf("errcode = %d, Error Message : %s\n", errcode, msgbuf);
	}
}

sword
test_execute(char *sql)
{
	sword ret = DCI_SUCCESS;
	DCIStmt		*pStmt = NULL;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);
		
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

end:
	err = disconnect(true);
	return ret;
}

sword
test_nodata()
{	
	sword ret = DCI_SUCCESS;
	DCIStmt		*pStmt = NULL;
	char* sql;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	sql = "create temp table t_nodata(id int)";
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}


	sql = "select * from t_nodata";
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_NO_DATA)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}


free_resource:
	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

end:
	err = disconnect(true);
	return ret;
}

sword
test_describe_only()
{	
	sword ret = DCI_SUCCESS;
	DCIStmt		*pStmt = NULL;
	char* sql;
	DCIBind *pBind = NULL;
	ub2 rlenp;


	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	sql = "select 1 from dual";
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DESCRIBE_ONLY);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

end:
	err = disconnect(true);
	return ret;
}


sword
test_execute_success_with_info()
{	
	sword ret = DCI_SUCCESS;
	DCIStmt		*pStmt = NULL;
	char* sql;
	DCIBind *pBind = NULL;
	ub2 rlenp;
	char value[11] = "";
	DCIDefine* pDefine = NULL;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	sql = "create temp table t_execute_success_with_info(id char(20))";
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	sql = "insert into t_execute_success_with_info values('ABCDEFGHIJKLMN')";
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}


	sql = "select id from t_execute_success_with_info";
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt, &pDefine, pError, 1, &value, 10, SQLT_CHR, 0, 0, 0, DCI_DEFAULT);

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS_WITH_INFO)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtFetch failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

end:
	err = disconnect(true);
	return ret;
}


sword
test_fetch_success_with_info()
{	
	sword ret = DCI_SUCCESS;
	DCIStmt		*pStmt = NULL;
	char* sql;
	DCIBind *pBind = NULL;
	ub2 rlenp;
	char value[5];
	DCIDefine* pDefine = NULL;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	sql = "create temp table t_fetch_success_with_info(id char(20))";
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	sql = "insert into t_fetch_success_with_info values('ABCDEFGHIJKLMN')";
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}


	sql = "select id from t_fetch_success_with_info";
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt, &pDefine, pError, 1, &value, sizeof(value), SQLT_CHR, 0, 0, 0, DCI_DEFAULT);

	err = DCIStmtFetch(pStmt, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT);
	if (err != DCI_SUCCESS_WITH_INFO)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtFetch failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

end:
	err = disconnect(true);
	return ret;
}

sword
test_fetch_last_prior()
{	
	sword ret = DCI_SUCCESS;
	DCIStmt		*pStmt = NULL;
	char* sql;
	DCIBind *pBind = NULL;
	ub2 rlenp;
	int value;
	DCIDefine* pDefine = NULL;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	sql = "create temp table t_fetch_last(id int)";
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	sql = "insert into t_fetch_last values(100)";
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	sql = "insert into t_fetch_last values(500)";
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}


	sql = "select id from t_fetch_last";
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt, &pDefine, pError, 1, &value, sizeof(int), SQLT_INT, 0, 0, 0, DCI_DEFAULT);

	err = DCIStmtFetch(pStmt, pError, 1, DCI_FETCH_LAST, DCI_DEFAULT);
	if (err != DCI_SUCCESS && value == 500)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtFetch failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtFetch(pStmt, pError, 1, DCI_FETCH_PRIOR, DCI_DEFAULT);
	if (err != DCI_SUCCESS && value == 100)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtFetch failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

end:
	err = disconnect(true);
	return ret;
}


sword
Fetch(DCIEnv *env, DCIStmt *stmhp, DCIError *pError, ub4 cFetchRows)
{
	ub4		iRow;
	ub4		i;
	ub4 	parmcnt = 0;
	DCIParam *colhd = (DCIParam *) 0;   /* column handle */
	sword 	ret = DCI_SUCCESS;

	ub4 	record_length = 0;
	sb2 	*define_type = NULL;
	/* bug#17069:should use ub2 type */
	/* ub4 	*field_length = NULL; */
	ub2 	*field_length = NULL;

	char 	*pBuf = NULL;
	sb2 	*ind = NULL;
	int 	offset = 0;
	ub4 	cFetchCols = 0;

	ub4 cFetched = 0, size = 0;

	DCIDefine		*pDefine = NULL;
	DCILobLocator	*lobhp[32][2];
	DCIDateTime		*dthp[32][2];
	DCIInterval		*intvhp[32][2];

	/* Get the number of columns in the select list */
	err = DCIAttrGet((void *)stmhp, DCI_HTYPE_STMT, (void *)&parmcnt, (ub4 *)0, DCI_ATTR_PARAM_COUNT, pError);

	define_type = (sb2 *) malloc(parmcnt * sizeof(sb2));
	if (!define_type)
	{
		ret = DCI_ERROR;
		goto end;
	}

	/* bug#17069:should use ub2 */
	/* field_length = (ub4 *) malloc(parmcnt * sizeof(ub4)); */
	field_length = (ub2 *) malloc(parmcnt * sizeof(ub2));
	if (!field_length)
	{
		ret = DCI_ERROR;
		goto end;
	}

	/* go through the column list and retrieve the datatype of each column. We
	start from pos = 1 */
	for (i = 1; i <= parmcnt; i++)
	{
		DciText *name;
		ub2 field_type = 0;
		/* bug#17069:should use ub2 type */
		/* ub4 field_len, len; */		
		ub2 field_len;
		ub4 len;
		sb1 field_scale = 0;
		sb2 field_precision = 0;

		/* get parameter for column i */
		DCIParamGet((void *)stmhp, DCI_HTYPE_STMT, pError, (void **)&colhd, i);

		/* get data-type of column i */
		DCIAttrGet((void *)colhd, DCI_DTYPE_PARAM, (void *)&name, (ub4 *)&len, DCI_ATTR_NAME, pError);
		field_type = 0;
		field_len = 0;
		field_precision = 0;
		field_scale = 0;
		DCIAttrGet((void *)colhd, DCI_DTYPE_PARAM, (void *)&field_type, (ub4 *)&len, DCI_ATTR_DATA_TYPE, pError);
		DCIAttrGet((void *)colhd, DCI_DTYPE_PARAM, (void *)&field_len, (ub4 *)&len, DCI_ATTR_DATA_SIZE, pError);
		DCIAttrGet((void *)colhd, DCI_DTYPE_PARAM, (void *)&field_precision, (ub4 *)&len, DCI_ATTR_PRECISION, pError);
		DCIAttrGet((void *)colhd, DCI_DTYPE_PARAM, (void *)&field_scale, (ub4 *)&len, DCI_ATTR_SCALE, pError);
		printf("%d, %s, %d, %d, %d\n", field_type, name, field_len, field_precision,field_scale);

		field_length[i-1] = field_len;
		define_type[i-1] = field_type;

		if (field_type == SQLT_AFC && field_len == 14)
		{
			field_length[i-1] = 14;
			define_type[i-1] = SQLT_CHR;
		}						    
		// TAppKeyidType == char(24)AppKeyID#,U<N;24char
		else if ((field_type == SQLT_AFC) && (field_len == 24))
		{
			field_length[i-1] = 24;
			define_type[i-1] = SQLT_CHR;
		}
		// TAppidType == char(20)AppID#,U<N;20char
		else if ((field_type == SQLT_AFC) && (field_len == 20))
		{
			field_length[i-1] = 20;
			define_type[i-1] = SQLT_CHR;
		}
		// CHAR
		else if (field_type == SQLT_AFC)
		{
			field_length[i-1] = field_len + 1;
			define_type[i-1] = SQLT_STR;
		}
		// VARCHAR
		else if (field_type == SQLT_CHR)
		{
			field_length[i-1] = field_len;
			define_type[i-1] = SQLT_STR;
		}
		// A TIPCALLY CASE SHOULD BE TRANSFERED TO INT FIRST
		// like COUNT(*) MAX() MIN() AVG() --- in this case
		// the precision and scale return by DCIAttrGet are all 0
		else if ((field_type == SQLT_NUM) && (field_precision == 0) && (field_scale == 0))
		{
			field_length[i-1] = sizeof(float);
			define_type[i-1] = SQLT_FLT;
		}
		// TINYINT
		else if ((field_type == SQLT_NUM) && (field_precision == 3) && (field_scale == 0))
		{
			//field_length = sizeof(unsigned char);
			field_length[i-1] = sizeof(short);
			define_type[i-1] = SQLT_INT;
		}
		// SMALLINT
		else if ((field_type == SQLT_NUM) && (field_precision == 5) && (field_scale == 0))
		{
			field_length[i-1] = sizeof(short);
			define_type[i-1] = SQLT_INT;
		}
		// INT
		else if ((field_type == SQLT_NUM) && (field_precision == 10) && (field_scale == 0))
		{
			field_length[i-1] = sizeof(int);
			define_type[i-1] = SQLT_INT;
		}
		// LONG
		else if ((field_type == SQLT_NUM) && (field_precision == 19) && (field_scale == 0))
		{
			field_length[i-1] = sizeof(KDB_INT64);
			define_type[i-1] = SQLT_INT;
		}
		else if ((field_type == SQLT_NUM) && (field_precision == 20) && (field_scale == 0))
		{
			field_length[i-1] = sizeof(KDB_INT64);
			define_type[i-1] = SQLT_INT;
		}
		// other datatypes which can be treated as INT
		else if (((field_type == SQLT_NUM) && (field_scale == 0)) || (field_type == SQLT_INT))
		{
			field_length[i-1] = sizeof(int);
			define_type[i-1] = SQLT_INT;
		}
		// FLOAT
		else if ((field_type == SQLT_NUM) && (field_precision == 22) && (field_scale == 6))
		{
			field_length[i-1] = sizeof(float);//4
			define_type[i-1] = SQLT_FLT;
		}
		// DOUBLE
		else if ((field_type == SQLT_NUM) && (field_precision == 38) && (field_scale == 12))
		{
			field_length[i-1] = sizeof(double);//8
			define_type[i-1] = SQLT_FLT;
		}
		// other datatypes which can be treated as REAL
		else if (((field_type == SQLT_NUM) && (field_scale > 0)) || (field_type == SQLT_FLT))
		{
			field_length[i-1] = sizeof(float);
			define_type[i-1] = SQLT_FLT;
		}
		// Oracle DATE
		else if (field_type == SQLT_DAT)
		{
			//field_info_ptr[i].field_length = sizeof(int);
			field_length[i-1] = sizeof(DCIDate);
			define_type[i-1] = SQLT_DAT;
		}

		if (field_type == SQLT_DATE || field_type == SQLT_TIME
			||field_type == SQLT_TIME || field_type == SQLT_TIME_TZ
			|| field_type == SQLT_TIMESTAMP || field_type == SQLT_TIMESTAMP_TZ)
		{
			field_length[i-1] +=1; 
			/* because in 64 bit os,pointer is 8 bytes,
			 *otherwise memoycopy pointer will cause write memory out of bounds
			 */ 
		}

		record_length += field_length[i-1];

		if (field_type == SQLT_BLOB || field_type == SQLT_CLOB)
		{
			ret = DCIDescriptorAlloc(env,(dvoid **)&lobhp[i-1][0], (ub4) DCI_DTYPE_LOB, (size_t) 0, (dvoid **) 0);
			ret = DCIDescriptorAlloc(env,(dvoid **)&lobhp[i-1][1], (ub4) DCI_DTYPE_LOB, (size_t) 0, (dvoid **) 0);
			field_length[i-1] = sizeof(DCILobLocator*);
			define_type[i-1] = field_type;
		}
		else if (field_type == SQLT_DATE)
		{
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][0], (ub4) DCI_DTYPE_DATE, (size_t) 0, (dvoid **) 0);
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][1], (ub4) DCI_DTYPE_DATE, (size_t) 0, (dvoid **) 0);
			field_length[i-1] = sizeof(DCIDateTime*);
			define_type[i-1] = field_type;
		}
		else if (field_type == SQLT_TIME)
		{
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][0], (ub4) DCI_DTYPE_TIME, (size_t) 0, (dvoid **) 0);
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][1], (ub4) DCI_DTYPE_TIME, (size_t) 0, (dvoid **) 0);
			field_length[i-1] = sizeof(DCIDateTime*);
			define_type[i-1] = field_type;
		}
		else if (field_type == SQLT_TIME_TZ)
		{
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][0], (ub4) DCI_DTYPE_TIME_TZ, (size_t) 0, (dvoid **) 0);
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][1], (ub4) DCI_DTYPE_TIME_TZ, (size_t) 0, (dvoid **) 0);
			field_length[i-1] = sizeof(DCIDateTime*);
			define_type[i-1] = field_type;
		}
		else if (field_type == SQLT_TIMESTAMP)
		{
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][0], (ub4) DCI_DTYPE_TIMESTAMP, (size_t) 0, (dvoid **) 0);
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][1], (ub4) DCI_DTYPE_TIMESTAMP, (size_t) 0, (dvoid **) 0);
			field_length[i-1] = sizeof(DCIDateTime*);
			define_type[i-1] = field_type;
		}
		else if (field_type == SQLT_TIMESTAMP_TZ)
		{
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][0], (ub4) DCI_DTYPE_TIMESTAMP_TZ, (size_t) 0, (dvoid **) 0);
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][1], (ub4) DCI_DTYPE_TIMESTAMP_TZ, (size_t) 0, (dvoid **) 0);
			field_length[i-1] = sizeof(DCIDateTime*);
			define_type[i-1] = field_type;
		}
		else if (field_type == SQLT_INTERVAL_YM)
		{
			ret = DCIDescriptorAlloc(env,(dvoid **)&intvhp[i-1][0], (ub4) DCI_DTYPE_INTERVAL_YM, (size_t) 0, (dvoid **) 0);
			ret = DCIDescriptorAlloc(env,(dvoid **)&intvhp[i-1][1], (ub4) DCI_DTYPE_INTERVAL_YM, (size_t) 0, (dvoid **) 0);
			field_length[i-1] = sizeof(DCIInterval*);
			define_type[i-1] = field_type;
		}
		else if (field_type == SQLT_INTERVAL_DS)
		{
			ret = DCIDescriptorAlloc(env,(dvoid **)&intvhp[i-1][0], (ub4) DCI_DTYPE_INTERVAL_DS, (size_t) 0, (dvoid **) 0);
			ret = DCIDescriptorAlloc(env,(dvoid **)&intvhp[i-1][1], (ub4) DCI_DTYPE_INTERVAL_DS, (size_t) 0, (dvoid **) 0);
			field_length[i-1] = sizeof(DCIInterval*);
			define_type[i-1] = field_type;
		}

		DCIDescriptorFree((void *)colhd, DCI_DTYPE_PARAM);
	}

	pBuf = (char *) malloc(cFetchRows * record_length);
	if (!pBuf)
	{
		ret = DCI_ERROR;
		goto end;
	}
	memset(pBuf,0,cFetchRows * record_length);
	ind = (sb2 *) malloc(cFetchRows * parmcnt * sizeof(sb2));
	if (!ind)
	{
		ret = DCI_ERROR;
		goto end;
	}
	memset(pBuf,0,cFetchRows * parmcnt * sizeof(sb2));

	cFetchCols = parmcnt;
	for (i = 0; i < cFetchCols; i++)
	{
		if (define_type[i] == SQLT_BLOB || define_type[i] == SQLT_CLOB)
		{
			ret = DCIDefineByPos(stmhp, 
								&pDefine, 
								pError, 
								i + 1, 
								(dvoid *)&lobhp[i][0], //(dvoid *)(pBuf + offset), //
								field_length[i], 
								define_type[i], 
								NULL, 
								(ub2 *)0, 
								(ub2 *)0, 
								DCI_DEFAULT);
			ret = DCIDefineArrayOfStruct(pDefine,
								pError,
								record_length,
								parmcnt * sizeof(sb2),
								0,
								0);
			for (iRow = 0; iRow < cFetchRows; iRow++)
				*(DCILobLocator**)(pBuf + record_length * iRow + offset) = lobhp[i][iRow];
		}
		else if (define_type[i] == SQLT_DATE
				 || define_type[i] == SQLT_TIME || define_type[i] == SQLT_TIME_TZ
				 || define_type[i] == SQLT_TIMESTAMP || define_type[i] == SQLT_TIMESTAMP_TZ)
		{
			ret = DCIDefineByPos(stmhp, 
								&pDefine, 
								pError, 
								i + 1, 
								(dvoid *)(pBuf + offset), //(dvoid *)&dthp[i][0], //
								field_length[i], 
								define_type[i], 
								NULL, 
								(ub2 *)0, 
								(ub2 *)0, 
								DCI_DEFAULT);
			ret = DCIDefineArrayOfStruct(pDefine,
								pError,
								record_length,
								parmcnt * sizeof(sb2),
								0,
								0);
			for (iRow = 0; iRow < cFetchRows; iRow++){
				/* bug#17069 */
				/* *(DCIDateTime**)(pBuf + record_length * iRow + offset) = dthp[i][iRow]; */

				memcpy((DCIDateTime**)(pBuf + record_length * iRow + offset), &(dthp[i][iRow]), sizeof(dthp[i][iRow]));
			}
		}
		else if (define_type[i] == SQLT_INTERVAL_YM || define_type[i] == SQLT_INTERVAL_DS)
		{
			ret = DCIDefineByPos(stmhp, 
								&pDefine, 
								pError, 
								i + 1, 
								(dvoid *)(pBuf + offset), //(dvoid *)&intvhp[i][0], //
								field_length[i], 
								define_type[i], 
								NULL, 
								(ub2 *)0, 
								(ub2 *)0, 
								DCI_DEFAULT);
			ret = DCIDefineArrayOfStruct(pDefine,
								pError,
								record_length,
								parmcnt * sizeof(sb2),
								0,
								0);
			for (iRow = 0; iRow < cFetchRows; iRow++)
				/* bug#17069 */
				/* *(DCIInterval**)(pBuf + record_length * iRow + offset) = intvhp[i][iRow]; */
				memcpy((DCIInterval**)(pBuf + record_length * iRow + offset), &(intvhp[i][iRow]), sizeof(intvhp[i][iRow]));
		}
		else
		{
			ret = DCIDefineByPos(stmhp, 
								&pDefine, 
								pError, 
								i + 1, 
								(dvoid *)(pBuf + offset), 
								field_length[i], 
								define_type[i], 
								(void *)(ind + i), 
								(ub2 *)0, 
								(ub2 *)0, 
								DCI_DEFAULT);
			ret = DCIDefineArrayOfStruct(pDefine,
										pError,
										record_length,
										parmcnt * sizeof(sb2),
										0,
										0);
		}
		offset += field_length[i];
	}

	ret = DCIStmtFetch(stmhp, pError, cFetchRows, DCI_FETCH_NEXT, DCI_DEFAULT);
	if (ret == DCI_NO_DATA)
	{
		ret = DCIAttrGet(stmhp, DCI_HTYPE_STMT, &cFetched, &size, DCI_ATTR_ROW_COUNT, pError);
		if (cFetched  == 0)
		{
		printf("no rows fetched\n");
		goto end;
		}
	}

	if (ret != DCI_SUCCESS && ret != DCI_NO_DATA)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtFetch failed: %d %s\n", errcode, msg);
		ret = DCI_ERROR;
		goto end;
	}

	ret = DCIAttrGet(stmhp, DCI_HTYPE_STMT, &cFetched, &size, DCI_ATTR_ROW_COUNT, pError);
	printf("%d rows fetched\n", cFetched);
	fprintf(fp, "%d rows fetched\n", cFetched);

	for (iRow = 0; iRow < cFetched; iRow++)
	{
		printf("Row %d:", iRow+1);
		fprintf(fp, "Row %d:", iRow+1);

		offset = 0;
		for (i = 0; i < cFetchCols; i++)
		{
			if (ind[i + iRow * parmcnt] == DCI_IND_NULL)
			{
				printf("\tNULL");
				fprintf(fp, "\tNULL");
				goto next;
			}

			switch (define_type[i])
			{
			case SQLT_CHR:
			case SQLT_STR:
				{
					char *p = (char *) malloc(field_length[i] + 1);
					if (!p)
					{
						ret = DCI_ERROR;
						goto end;
					}

					memset(p, 0, field_length[i] + 1);
					memcpy(p, (char *) (pBuf + record_length * iRow + offset), field_length[i]);
					printf("\t%s", p);
					fprintf(fp, "\t%s", p);
					free(p);
				}
				break;

			case SQLT_INT:
				switch (field_length[i])
				{
				case 2:
					{
						short tmp = 0;
						memcpy(&tmp, (pBuf + record_length * iRow + offset), sizeof(short));
						/* bug#17069 */
						/* printf("\t%d", *(short *)(pBuf + record_length * iRow + offset)); */
						printf("\t%d", tmp);
						fprintf(fp, "\t%d", tmp);
						break;
					}
				case 4:
					{
						int tmp = 0;
						memcpy(&tmp, (pBuf + record_length * iRow + offset), sizeof(int));
						printf("\t%d", tmp);
						/* bug#17069 */
						/* printf("\t%d", *(int *)(pBuf + record_length * iRow + offset)); */
						fprintf(fp, "\t%d", tmp);
						break;
					}
				case 8:
					/* bug#17069 */
					/* printf("\t"INT64_FORMAT, *(KDB_INT64 *)(pBuf + record_length * iRow + offset)); */
					{
						KDB_INT64 tmp = 0;
						memcpy(&tmp, (pBuf + record_length * iRow + offset), sizeof(KDB_INT64));
						printf("\t"INT64_FORMAT, tmp);
						fprintf(fp, "\t"INT64_FORMAT, tmp);
						break;
					}
				default:
					assert(false);
				}
				break;

			case SQLT_FLT:
				switch (field_length[i])
				{
				case 4:
					{
						/* bug#17069 */
						float tmp = 0;
						memcpy(&tmp, (pBuf + record_length * iRow + offset), sizeof(float));
						printf("\t%f", tmp);
						fprintf(fp, "\t%f", tmp);
						break;
					}
				case 8:
					{
						/* bug#17069 */
						double tmp = 0;
						memcpy(&tmp, (pBuf + record_length * iRow + offset), sizeof(double));
						printf("\t%f", tmp);
						fprintf(fp, "\t%f", tmp);
						break;
					}
				}
				break;

			case SQLT_DAT:
				{
					ub1 *pDate = (ub1 *) (pBuf + record_length * iRow + offset);
					sb2 year = (*pDate - 100) * 100 + (*(pDate+1) - 100);

					printf("\t%04d-%02d-%02d %02d:%02d:%02d", 
						year, *(pDate+2), *(pDate+3), *(pDate+4)-1, *(pDate+5)-1, *(pDate+6)-1);
					fprintf(fp, "\t%04d-%02d-%02d %02d:%02d:%02d", 
						year, *(pDate+2), *(pDate+3), *(pDate+4)-1, *(pDate+5)-1, *(pDate+6)-1);
				}
				break;

			case SQLT_ODT:
				{
					DCIDate *pDate = (DCIDate *) (pBuf + record_length * iRow + offset);

					printf("\t%04d-%02d-%02d %02d:%02d:%02d", 
						pDate->DCIDateYYYY, pDate->DCIDateMM, pDate->DCIDateDD, 
						pDate->DCIDateTime.DCITimeHH, pDate->DCIDateTime.DCITimeMI, pDate->DCIDateTime.DCITimeSS);
					fprintf(fp, "\t%04d-%02d-%02d %02d:%02d:%02d", 
						pDate->DCIDateYYYY, pDate->DCIDateMM, pDate->DCIDateDD, 
						pDate->DCIDateTime.DCITimeHH, pDate->DCIDateTime.DCITimeMI, pDate->DCIDateTime.DCITimeSS);
				}
				break;

			case SQLT_DATE:
				{
					/* bug#17069 */
					/* DCIDateTime *pDateTime = *(DCIDateTime **) (pBuf + record_length * iRow + offset); */
					DCIDateTime *pDateTime = NULL;
					memcpy(&pDateTime, (DCIDateTime **) (pBuf + record_length * iRow + offset), sizeof(pDateTime));
					sb2 y;
					ub1 m, d;

					DCIDateTimeGetDate(pEnv, pError, pDateTime, &y, &m ,&d);
					
					printf("\t%04d-%02d-%02d", y, m, d);
					fprintf(fp, "\t%04d-%02d-%02d", y, m, d);

					ret = DCIDescriptorFree((dvoid *)dthp[i][iRow], (ub4) DCI_DTYPE_DATE);
				}
				break;

			case SQLT_TIME:
				{
					/* bug#17069 */
					/* DCIDateTime *pDateTime = *(DCIDateTime **) (pBuf + record_length * iRow + offset); */
					DCIDateTime *pDateTime = NULL;
					memcpy(&pDateTime, (DCIDateTime **) (pBuf + record_length * iRow + offset), sizeof(pDateTime));
					ub1 h, m, s;
					ub4 f;

					DCIDateTimeGetTime(pEnv, pError, pDateTime, &h, &m ,&s, &f);

					printf("\t%02d:%02d:%02d.%06d", h, m, s, f);
					fprintf(fp, "\t%02d:%02d:%02d.%06d", h, m, s, f);

					ret = DCIDescriptorFree((dvoid *)dthp[i][iRow], (ub4) DCI_DTYPE_TIME);
				}
				break;

			case SQLT_TIME_TZ:
				{
					DCIDateTime *pDateTime = *(DCIDateTime **) (pBuf + record_length * iRow + offset);
					ub1 h, m, s;
					ub4 f;

					DCIDateTimeGetTime(pEnv, pError, pDateTime, &h, &m ,&s, &f);

					printf("\t%02d:%02d:%02d.%06d", h, m, s, f);
					fprintf(fp, "\t%02d:%02d:%02d.%06d", h, m, s, f);

					ret = DCIDescriptorFree((dvoid *)dthp[i][iRow], (ub4) DCI_DTYPE_TIME_TZ);
				}
				break;
			
			case SQLT_TIMESTAMP:
				{
					DCIDateTime *pDateTime = *(DCIDateTime **) (pBuf + record_length * iRow + offset);
					sb2 y;
					ub1 m, d;
					ub1 h, mi, s;
					ub4 f;

					DCIDateTimeGetDate(pEnv, pError, pDateTime, &y, &m, &d);
					DCIDateTimeGetTime(pEnv, pError, pDateTime, &h, &mi, &s, &f);

					printf("\t%04d-%02d-%02d %02d:%02d:%02d.%06d", y, m, d, h, m, s, f);
					fprintf(fp, "\t%04d-%02d-%02d %02d:%02d:%02d.%06d", y, m, d, h, m, s, f);

					ret = DCIDescriptorFree((dvoid *)dthp[i][iRow], (ub4) DCI_DTYPE_TIMESTAMP);
				}
				break;

			case SQLT_TIMESTAMP_TZ:
				{
					DCIDateTime *pDateTime = *(DCIDateTime **) (pBuf + record_length * iRow + offset);
					sb2 y;
					ub1 m, d;
					ub1 h, mi, s;
					ub4 f;

					DCIDateTimeGetDate(pEnv, pError, pDateTime, &y, &m, &d);
					DCIDateTimeGetTime(pEnv, pError, pDateTime, &h, &mi, &s, &f);

					printf("\t%04d-%02d-%02d %02d:%02d:%02d.%06d", y, m, d, h, m, s, f);
					fprintf(fp, "\t%04d-%02d-%02d %02d:%02d:%02d.%06d", y, m, d, h, m, s, f);

					ret = DCIDescriptorFree((dvoid *)dthp[i][iRow], (ub4) DCI_DTYPE_TIMESTAMP_TZ);
				}
				break;

			case SQLT_INTERVAL_YM:
				{
					/* bug#17069 */
					/* DCIInterval	*pInterval = *(DCIInterval **) (pBuf + record_length * iRow + offset); */
					DCIInterval	*pInterval = NULL;
					memcpy(&pInterval, (DCIInterval **) (pBuf + record_length * iRow + offset), sizeof(pInterval));
					sb4 y, m;

					DCIIntervalGetYearMonth(pEnv, pError, &y ,&m, pInterval);

					printf("\t%d-%d", y, m);
					fprintf(fp, "\t%d-%d", y, m);

					ret = DCIDescriptorFree((dvoid *)intvhp[i][iRow], (ub4) DCI_DTYPE_INTERVAL_YM);
				}
				break;

			case SQLT_INTERVAL_DS:
				{
					/* bug#17069 */
					/* DCIInterval	*pInterval = *(DCIInterval **) (pBuf + record_length * iRow + offset); */
					DCIInterval	*pInterval = NULL;
					memcpy(&pInterval, (DCIInterval **) (pBuf + record_length * iRow + offset), sizeof(pInterval));
					sb4 d, h, m, s, f;

					DCIIntervalGetDaySecond(pEnv, pError, &d, &h ,&m, &s, &f, pInterval);

					printf("\t%d %d-%d-%d.%d", d, h, m, s, f);
					fprintf(fp, "\t%d %d-%d-%d.%d", d, h, m, s, f);
					ret = DCIDescriptorFree((dvoid *)intvhp[i][iRow], (ub4) DCI_DTYPE_INTERVAL_DS);
				}
				break;

			default:
				break;
			}
next:
			offset += field_length[i];
		}

		printf("\n");
		fprintf(fp, "\n");
	}

end:
	free(pBuf);
	free(ind);
	free(field_length);
	free(define_type);

	return ret;
}

/* do fetch but not print to file */
sword
Fetch_no_print(DCIEnv *env, DCIStmt *stmhp, DCIError *pError, ub4 cFetchRows)
{
	ub4		iRow;
	ub4		i;
	ub4 	parmcnt = 0;
	DCIParam *colhd = (DCIParam *) 0;   /* column handle */
	sword 	ret = DCI_SUCCESS;

	ub4 	record_length = 0;
	sb2 	*define_type = NULL;
	/* bug#17069:should use ub2 type */
	/* ub4 	*field_length = NULL; */
	ub2 	*field_length = NULL;

	char 	*pBuf = NULL;
	sb2 	*ind = NULL;
	int 	offset = 0;
	ub4 	cFetchCols = 0;

	ub4 cFetched = 0, size = 0;

	DCIDefine		*pDefine = NULL;
	DCILobLocator	*lobhp[32][2];
	DCIDateTime		*dthp[32][2];
	DCIInterval		*intvhp[32][2];

	/* Get the number of columns in the select list */
	err = DCIAttrGet((void *)stmhp, DCI_HTYPE_STMT, (void *)&parmcnt, (ub4 *)0, DCI_ATTR_PARAM_COUNT, pError);

	define_type = (sb2 *) malloc(parmcnt * sizeof(sb2));
	if (!define_type)
	{
		ret = DCI_ERROR;
		goto end;
	}

	/* bug#17069:should use ub2 */
	/* field_length = (ub4 *) malloc(parmcnt * sizeof(ub4)); */
	field_length = (ub2 *) malloc(parmcnt * sizeof(ub2));
	if (!field_length)
	{
		ret = DCI_ERROR;
		goto end;
	}

	/* go through the column list and retrieve the datatype of each column. We
	start from pos = 1 */
	for (i = 1; i <= parmcnt; i++)
	{
		DciText *name;
		ub2 field_type = 0;
		/* bug#17069:should use ub2 type */
		/* ub4 field_len, len; */		
		ub2 field_len;
		ub4 len;
		sb1 field_scale = 0;
		sb2 field_precision = 0;

		/* get parameter for column i */
		DCIParamGet((void *)stmhp, DCI_HTYPE_STMT, pError, (void **)&colhd, i);

		/* get data-type of column i */
		DCIAttrGet((void *)colhd, DCI_DTYPE_PARAM, (void *)&name, (ub4 *)&len, DCI_ATTR_NAME, pError);
		field_type = 0;
		field_len = 0;
		field_precision = 0;
		field_scale = 0;
		DCIAttrGet((void *)colhd, DCI_DTYPE_PARAM, (void *)&field_type, (ub4 *)&len, DCI_ATTR_DATA_TYPE, pError);
		DCIAttrGet((void *)colhd, DCI_DTYPE_PARAM, (void *)&field_len, (ub4 *)&len, DCI_ATTR_DATA_SIZE, pError);
		DCIAttrGet((void *)colhd, DCI_DTYPE_PARAM, (void *)&field_precision, (ub4 *)&len, DCI_ATTR_PRECISION, pError);
		DCIAttrGet((void *)colhd, DCI_DTYPE_PARAM, (void *)&field_scale, (ub4 *)&len, DCI_ATTR_SCALE, pError);
		printf("%d, %s, %d, %d, %d\n", field_type, name, field_len, field_precision,field_scale);

		field_length[i-1] = field_len;
		define_type[i-1] = field_type;

		if (field_type == SQLT_AFC && field_len == 14)
		{
			field_length[i-1] = 14;
			define_type[i-1] = SQLT_CHR;
		}						    
		// TAppKeyidType == char(24)AppKeyID#,U<N;24char
		else if ((field_type == SQLT_AFC) && (field_len == 24))
		{
			field_length[i-1] = 24;
			define_type[i-1] = SQLT_CHR;
		}
		// TAppidType == char(20)AppID#,U<N;20char
		else if ((field_type == SQLT_AFC) && (field_len == 20))
		{
			field_length[i-1] = 20;
			define_type[i-1] = SQLT_CHR;
		}
		// CHAR
		else if (field_type == SQLT_AFC)
		{
			field_length[i-1] = field_len + 1;
			define_type[i-1] = SQLT_STR;
		}
		// VARCHAR
		else if (field_type == SQLT_CHR)
		{
			field_length[i-1] = field_len;
			define_type[i-1] = SQLT_STR;
		}
		// A TIPCALLY CASE SHOULD BE TRANSFERED TO INT FIRST
		// like COUNT(*) MAX() MIN() AVG() --- in this case
		// the precision and scale return by DCIAttrGet are all 0
		else if ((field_type == SQLT_NUM) && (field_precision == 0) && (field_scale == 0))
		{
			field_length[i-1] = sizeof(float);
			define_type[i-1] = SQLT_FLT;
		}
		// TINYINT
		else if ((field_type == SQLT_NUM) && (field_precision == 3) && (field_scale == 0))
		{
			//field_length = sizeof(unsigned char);
			field_length[i-1] = sizeof(short);
			define_type[i-1] = SQLT_INT;
		}
		// SMALLINT
		else if ((field_type == SQLT_NUM) && (field_precision == 5) && (field_scale == 0))
		{
			field_length[i-1] = sizeof(short);
			define_type[i-1] = SQLT_INT;
		}
		// INT
		else if ((field_type == SQLT_NUM) && (field_precision == 10) && (field_scale == 0))
		{
			field_length[i-1] = sizeof(int);
			define_type[i-1] = SQLT_INT;
		}
		// LONG
		else if ((field_type == SQLT_NUM) && (field_precision == 19) && (field_scale == 0))
		{
			field_length[i-1] = sizeof(KDB_INT64);
			define_type[i-1] = SQLT_INT;
		}
		else if ((field_type == SQLT_NUM) && (field_precision == 20) && (field_scale == 0))
		{
			field_length[i-1] = sizeof(KDB_INT64);
			define_type[i-1] = SQLT_INT;
		}
		// other datatypes which can be treated as INT
		else if (((field_type == SQLT_NUM) && (field_scale == 0)) || (field_type == SQLT_INT))
		{
			field_length[i-1] = sizeof(int);
			define_type[i-1] = SQLT_INT;
		}
		// FLOAT
		else if ((field_type == SQLT_NUM) && (field_precision == 22) && (field_scale == 6))
		{
			field_length[i-1] = sizeof(float);//4
			define_type[i-1] = SQLT_FLT;
		}
		// DOUBLE
		else if ((field_type == SQLT_NUM) && (field_precision == 38) && (field_scale == 12))
		{
			field_length[i-1] = sizeof(double);//8
			define_type[i-1] = SQLT_FLT;
		}
		// other datatypes which can be treated as REAL
		else if (((field_type == SQLT_NUM) && (field_scale > 0)) || (field_type == SQLT_FLT))
		{
			field_length[i-1] = sizeof(float);
			define_type[i-1] = SQLT_FLT;
		}
		// Oracle DATE
		else if (field_type == SQLT_DAT)
		{
			//field_info_ptr[i].field_length = sizeof(int);
			field_length[i-1] = sizeof(DCIDate);
			define_type[i-1] = SQLT_DAT;
		}

		record_length += field_length[i-1];

		if (field_type == SQLT_BLOB || field_type == SQLT_CLOB)
		{
			ret = DCIDescriptorAlloc(env,(dvoid **)&lobhp[i-1][0], (ub4) DCI_DTYPE_LOB, (size_t) 0, (dvoid **) 0);
			ret = DCIDescriptorAlloc(env,(dvoid **)&lobhp[i-1][1], (ub4) DCI_DTYPE_LOB, (size_t) 0, (dvoid **) 0);
			field_length[i-1] = sizeof(DCILobLocator*);
			define_type[i-1] = field_type;
		}

		if (field_type == SQLT_DATE)
		{
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][0], (ub4) DCI_DTYPE_DATE, (size_t) 0, (dvoid **) 0);
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][1], (ub4) DCI_DTYPE_DATE, (size_t) 0, (dvoid **) 0);
			field_length[i-1] = sizeof(DCIDateTime*);
			define_type[i-1] = field_type;
		}
		if (field_type == SQLT_TIME)
		{
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][0], (ub4) DCI_DTYPE_TIME, (size_t) 0, (dvoid **) 0);
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][1], (ub4) DCI_DTYPE_TIME, (size_t) 0, (dvoid **) 0);
			field_length[i-1] = sizeof(DCIDateTime*);
			define_type[i-1] = field_type;
		}
		if (field_type == SQLT_TIME_TZ)
		{
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][0], (ub4) DCI_DTYPE_TIME_TZ, (size_t) 0, (dvoid **) 0);
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][1], (ub4) DCI_DTYPE_TIME_TZ, (size_t) 0, (dvoid **) 0);
			field_length[i-1] = sizeof(DCIDateTime*);
			define_type[i-1] = field_type;
		}
		if (field_type == SQLT_TIMESTAMP)
		{
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][0], (ub4) DCI_DTYPE_TIMESTAMP, (size_t) 0, (dvoid **) 0);
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][1], (ub4) DCI_DTYPE_TIMESTAMP, (size_t) 0, (dvoid **) 0);
			field_length[i-1] = sizeof(DCIDateTime*);
			define_type[i-1] = field_type;
		}
		if (field_type == SQLT_TIMESTAMP_TZ)
		{
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][0], (ub4) DCI_DTYPE_TIMESTAMP_TZ, (size_t) 0, (dvoid **) 0);
			ret = DCIDescriptorAlloc(env,(dvoid **)&dthp[i-1][1], (ub4) DCI_DTYPE_TIMESTAMP_TZ, (size_t) 0, (dvoid **) 0);
			field_length[i-1] = sizeof(DCIDateTime*);
			define_type[i-1] = field_type;
		}
		if (field_type == SQLT_INTERVAL_YM)
		{
			ret = DCIDescriptorAlloc(env,(dvoid **)&intvhp[i-1][0], (ub4) DCI_DTYPE_INTERVAL_YM, (size_t) 0, (dvoid **) 0);
			ret = DCIDescriptorAlloc(env,(dvoid **)&intvhp[i-1][1], (ub4) DCI_DTYPE_INTERVAL_YM, (size_t) 0, (dvoid **) 0);
			field_length[i-1] = sizeof(DCIInterval*);
			define_type[i-1] = field_type;
		}
		if (field_type == SQLT_INTERVAL_DS)
		{
			ret = DCIDescriptorAlloc(env,(dvoid **)&intvhp[i-1][0], (ub4) DCI_DTYPE_INTERVAL_DS, (size_t) 0, (dvoid **) 0);
			ret = DCIDescriptorAlloc(env,(dvoid **)&intvhp[i-1][1], (ub4) DCI_DTYPE_INTERVAL_DS, (size_t) 0, (dvoid **) 0);
			field_length[i-1] = sizeof(DCIInterval*);
			define_type[i-1] = field_type;
		}

		DCIDescriptorFree((void *)colhd, DCI_DTYPE_PARAM);
	}

	pBuf = (char *) malloc(cFetchRows * record_length);
	if (!pBuf)
	{
		ret = DCI_ERROR;
		goto end;
	}
	ind = (sb2 *) malloc(cFetchRows * parmcnt * sizeof(sb2));
	if (!ind)
	{
		ret = DCI_ERROR;
		goto end;
	}

	cFetchCols = parmcnt;
	for (i = 0; i < cFetchCols; i++)
	{
		if (define_type[i] == SQLT_BLOB || define_type[i] == SQLT_CLOB)
		{
			ret = DCIDefineByPos(stmhp, 
								&pDefine, 
								pError, 
								i + 1, 
								(dvoid *)&lobhp[i][0], //(dvoid *)(pBuf + offset), //
								field_length[i], 
								define_type[i], 
								NULL, 
								(ub2 *)0, 
								(ub2 *)0, 
								DCI_DEFAULT);
			ret = DCIDefineArrayOfStruct(pDefine,
								pError,
								record_length,
								parmcnt * sizeof(sb2),
								0,
								0);
			for (iRow = 0; iRow < cFetchRows; iRow++)
				*(DCILobLocator**)(pBuf + record_length * iRow + offset) = lobhp[i][iRow];
		}
		else if (define_type[i] == SQLT_DATE
				 || define_type[i] == SQLT_TIME || define_type[i] == SQLT_TIME_TZ
				 || define_type[i] == SQLT_TIMESTAMP || define_type[i] == SQLT_TIMESTAMP_TZ)
		{
			ret = DCIDefineByPos(stmhp, 
								&pDefine, 
								pError, 
								i + 1, 
								(dvoid *)(pBuf + offset), //(dvoid *)&dthp[i][0], //
								field_length[i], 
								define_type[i], 
								NULL, 
								(ub2 *)0, 
								(ub2 *)0, 
								DCI_DEFAULT);
			ret = DCIDefineArrayOfStruct(pDefine,
								pError,
								record_length,
								parmcnt * sizeof(sb2),
								0,
								0);
			for (iRow = 0; iRow < cFetchRows; iRow++)
				/* bug#17069 */
				/* *(DCIDateTime**)(pBuf + record_length * iRow + offset) = dthp[i][iRow]; */

				memcpy((DCIDateTime**)(pBuf + record_length * iRow + offset), &(dthp[i][iRow]), sizeof(dthp[i][iRow]));
		}
		else if (define_type[i] == SQLT_INTERVAL_YM || define_type[i] == SQLT_INTERVAL_DS)
		{
			ret = DCIDefineByPos(stmhp, 
								&pDefine, 
								pError, 
								i + 1, 
								(dvoid *)(pBuf + offset), //(dvoid *)&intvhp[i][0], //
								field_length[i], 
								define_type[i], 
								NULL, 
								(ub2 *)0, 
								(ub2 *)0, 
								DCI_DEFAULT);
			ret = DCIDefineArrayOfStruct(pDefine,
								pError,
								record_length,
								parmcnt * sizeof(sb2),
								0,
								0);
			for (iRow = 0; iRow < cFetchRows; iRow++)
				/* bug#17069 */
				/* *(DCIInterval**)(pBuf + record_length * iRow + offset) = intvhp[i][iRow]; */
				memcpy((DCIInterval**)(pBuf + record_length * iRow + offset), &(intvhp[i][iRow]), sizeof(intvhp[i][iRow]));
		}
		else
		{
			ret = DCIDefineByPos(stmhp, 
								&pDefine, 
								pError, 
								i + 1, 
								(dvoid *)(pBuf + offset), 
								field_length[i], 
								define_type[i], 
								(void *)(ind + i), 
								(ub2 *)0, 
								(ub2 *)0, 
								DCI_DEFAULT);
			ret = DCIDefineArrayOfStruct(pDefine,
										pError,
										record_length,
										parmcnt * sizeof(sb2),
										0,
										0);
		}
		offset += field_length[i];
	}

	ret = DCIStmtFetch(stmhp, pError, cFetchRows, DCI_FETCH_NEXT, DCI_DEFAULT);
	if (ret == DCI_NO_DATA)
	{
		ret = DCIAttrGet(stmhp, DCI_HTYPE_STMT, &cFetched, &size, DCI_ATTR_ROW_COUNT, pError);
		if (cFetched  == 0)
		{
		printf("no rows fetched\n");
		goto end;
		}
	}

	if (ret != DCI_SUCCESS && ret != DCI_NO_DATA)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtFetch failed: %d %s\n", errcode, msg);
		ret = DCI_ERROR;
		goto end;
	}

	ret = DCIAttrGet(stmhp, DCI_HTYPE_STMT, &cFetched, &size, DCI_ATTR_ROW_COUNT, pError);
	printf("%d rows fetched\n", cFetched);
	//fprintf(fp, "%d rows fetched\n", cFetched);

	for (iRow = 0; iRow < cFetched; iRow++)
	{
		printf("Row %d:", iRow+1);
		//fprintf(fp, "Row %d:", iRow+1);

		offset = 0;
		for (i = 0; i < cFetchCols; i++)
		{
			if (ind[i + iRow * parmcnt] == DCI_IND_NULL)
			{
				printf("\tNULL");
				//fprintf(fp, "\tNULL");
				goto next;
			}

			switch (define_type[i])
			{
			case SQLT_CHR:
			case SQLT_STR:
				{
					char *p = (char *) malloc(field_length[i] + 1);
					if (!p)
					{
						ret = DCI_ERROR;
						goto end;
					}

					memset(p, 0, field_length[i] + 1);
					memcpy(p, (char *) (pBuf + record_length * iRow + offset), field_length[i]);
					printf("\t%s", p);
					//fprintf(fp, "\t%s", p);
					free(p);
				}
				break;

			case SQLT_INT:
				switch (field_length[i])
				{
				case 2:
					{
						short tmp = 0;
						memcpy(&tmp, (pBuf + record_length * iRow + offset), sizeof(short));
						/* bug#17069 */
						/* printf("\t%d", *(short *)(pBuf + record_length * iRow + offset)); */
						printf("\t%d", tmp);
						//fprintf(fp, "\t%d", tmp);
						break;
					}
				case 4:
					{
						int tmp = 0;
						memcpy(&tmp, (pBuf + record_length * iRow + offset), sizeof(int));
						printf("\t%d", tmp);
						/* bug#17069 */
						/* printf("\t%d", *(int *)(pBuf + record_length * iRow + offset)); */
						//fprintf(fp, "\t%d", tmp);
						break;
					}
				case 8:
					/* bug#17069 */
					/* printf("\t"INT64_FORMAT, *(KDB_INT64 *)(pBuf + record_length * iRow + offset)); */
					{
						KDB_INT64 tmp = 0;
						memcpy(&tmp, (pBuf + record_length * iRow + offset), sizeof(KDB_INT64));
						printf("\t"INT64_FORMAT, tmp);
						//fprintf(fp, "\t"INT64_FORMAT, tmp);
						break;
					}
				default:
					assert(false);
				}
				break;

			case SQLT_FLT:
				switch (field_length[i])
				{
				case 4:
					{
						/* bug#17069 */
						float tmp = 0;
						memcpy(&tmp, (pBuf + record_length * iRow + offset), sizeof(float));
						printf("\t%f", tmp);
						//fprintf(fp, "\t%f", tmp);
						break;
					}
				case 8:
					{
						/* bug#17069 */
						double tmp = 0;
						memcpy(&tmp, (pBuf + record_length * iRow + offset), sizeof(double));
						printf("\t%f", tmp);
						//fprintf(fp, "\t%f", tmp);
						break;
					}
				}
				break;

			case SQLT_DAT:
				{
					ub1 *pDate = (ub1 *) (pBuf + record_length * iRow + offset);
					sb2 year = (*pDate - 100) * 100 + (*(pDate+1) - 100);

					printf("\t%04d-%02d-%02d %02d:%02d:%02d", 
						year, *(pDate+2), *(pDate+3), *(pDate+4)-1, *(pDate+5)-1, *(pDate+6)-1);
					//fprintf(fp, "\t%04d-%02d-%02d %02d:%02d:%02d", 
					//	year, *(pDate+2), *(pDate+3), *(pDate+4)-1, *(pDate+5)-1, *(pDate+6)-1);
				}
				break;

			case SQLT_ODT:
				{
					DCIDate *pDate = (DCIDate *) (pBuf + record_length * iRow + offset);

					printf("\t%04d-%02d-%02d %02d:%02d:%02d", 
						pDate->DCIDateYYYY, pDate->DCIDateMM, pDate->DCIDateDD, 
						pDate->DCIDateTime.DCITimeHH, pDate->DCIDateTime.DCITimeMI, pDate->DCIDateTime.DCITimeSS);
					//fprintf(fp, "\t%04d-%02d-%02d %02d:%02d:%02d", 
					//	pDate->DCIDateYYYY, pDate->DCIDateMM, pDate->DCIDateDD, 
					//	pDate->DCIDateTime.DCITimeHH, pDate->DCIDateTime.DCITimeMI, pDate->DCIDateTime.DCITimeSS);
				}
				break;

			case SQLT_DATE:
				{
					/* bug#17069 */
					/* DCIDateTime *pDateTime = *(DCIDateTime **) (pBuf + record_length * iRow + offset); */
					DCIDateTime *pDateTime = NULL;
					memcpy(&pDateTime, (DCIDateTime **) (pBuf + record_length * iRow + offset), sizeof(pDateTime));
					sb2 y;
					ub1 m, d;

					DCIDateTimeGetDate(pEnv, pError, pDateTime, &y, &m ,&d);
					
					printf("\t%04d-%02d-%02d", y, m, d);
					//fprintf(fp, "\t%04d-%02d-%02d", y, m, d);

					ret = DCIDescriptorFree((dvoid *)dthp[i][iRow], (ub4) DCI_DTYPE_DATE);
				}
				break;

			case SQLT_TIME:
				{
					/* bug#17069 */
					/* DCIDateTime *pDateTime = *(DCIDateTime **) (pBuf + record_length * iRow + offset); */
					DCIDateTime *pDateTime = NULL;
					memcpy(&pDateTime, (DCIDateTime **) (pBuf + record_length * iRow + offset), sizeof(pDateTime));
					ub1 h, m, s;
					ub4 f;

					DCIDateTimeGetTime(pEnv, pError, pDateTime, &h, &m ,&s, &f);

					printf("\t%02d:%02d:%02d.%06d", h, m, s, f);
					//fprintf(fp, "\t%02d:%02d:%02d.%06d", h, m, s, f);

					ret = DCIDescriptorFree((dvoid *)dthp[i][iRow], (ub4) DCI_DTYPE_TIME);
				}
				break;

			case SQLT_TIME_TZ:
				{
					DCIDateTime *pDateTime = *(DCIDateTime **) (pBuf + record_length * iRow + offset);
					ub1 h, m, s;
					ub4 f;

					DCIDateTimeGetTime(pEnv, pError, pDateTime, &h, &m ,&s, &f);

					printf("\t%02d:%02d:%02d.%06d", h, m, s, f);
					//fprintf(fp, "\t%02d:%02d:%02d.%06d", h, m, s, f);

					ret = DCIDescriptorFree((dvoid *)dthp[i][iRow], (ub4) DCI_DTYPE_TIME_TZ);
				}
				break;
			
			case SQLT_TIMESTAMP:
				{
					DCIDateTime *pDateTime = *(DCIDateTime **) (pBuf + record_length * iRow + offset);
					sb2 y;
					ub1 m, d;
					ub1 h, mi, s;
					ub4 f;

					DCIDateTimeGetDate(pEnv, pError, pDateTime, &y, &m, &d);
					DCIDateTimeGetTime(pEnv, pError, pDateTime, &h, &mi, &s, &f);

					printf("\t%04d-%02d-%02d %02d:%02d:%02d.%06d", y, m, d, h, m, s, f);
					//fprintf(fp, "\t%04d-%02d-%02d %02d:%02d:%02d.%06d", y, m, d, h, m, s, f);

					ret = DCIDescriptorFree((dvoid *)dthp[i][iRow], (ub4) DCI_DTYPE_TIMESTAMP);
				}
				break;

			case SQLT_TIMESTAMP_TZ:
				{
					DCIDateTime *pDateTime = *(DCIDateTime **) (pBuf + record_length * iRow + offset);
					sb2 y;
					ub1 m, d;
					ub1 h, mi, s;
					ub4 f;

					DCIDateTimeGetDate(pEnv, pError, pDateTime, &y, &m, &d);
					DCIDateTimeGetTime(pEnv, pError, pDateTime, &h, &mi, &s, &f);

					printf("\t%04d-%02d-%02d %02d:%02d:%02d.%06d", y, m, d, h, m, s, f);
					//fprintf(fp, "\t%04d-%02d-%02d %02d:%02d:%02d.%06d", y, m, d, h, m, s, f);

					ret = DCIDescriptorFree((dvoid *)dthp[i][iRow], (ub4) DCI_DTYPE_TIMESTAMP_TZ);
				}
				break;

			case SQLT_INTERVAL_YM:
				{
					/* bug#17069 */
					/* DCIInterval	*pInterval = *(DCIInterval **) (pBuf + record_length * iRow + offset); */
					DCIInterval	*pInterval = NULL;
					memcpy(&pInterval, (DCIInterval **) (pBuf + record_length * iRow + offset), sizeof(pInterval));
					sb4 y, m;

					DCIIntervalGetYearMonth(pEnv, pError, &y ,&m, pInterval);

					printf("\t%d-%d", y, m);
					//fprintf(fp, "\t%d-%d", y, m);

					ret = DCIDescriptorFree((dvoid *)intvhp[i][iRow], (ub4) DCI_DTYPE_INTERVAL_YM);
				}
				break;

			case SQLT_INTERVAL_DS:
				{
					/* bug#17069 */
					/* DCIInterval	*pInterval = *(DCIInterval **) (pBuf + record_length * iRow + offset); */
					DCIInterval	*pInterval = NULL;
					memcpy(&pInterval, (DCIInterval **) (pBuf + record_length * iRow + offset), sizeof(pInterval));
					sb4 d, h, m, s, f;

					DCIIntervalGetDaySecond(pEnv, pError, &d, &h ,&m, &s, &f, pInterval);

					printf("\t%d %d-%d-%d.%d", d, h, m, s, f);
					//fprintf(fp, "\t%d %d-%d-%d.%d", d, h, m, s, f);
					ret = DCIDescriptorFree((dvoid *)intvhp[i][iRow], (ub4) DCI_DTYPE_INTERVAL_DS);
				}
				break;

			default:
				break;
			}
next:
			offset += field_length[i];
		}

		printf("\n");
		//fprintf(fp, "\n");
	}

end:
	free(pBuf);
	free(ind);
	free(field_length);
	free(define_type);

	return ret;
}


sword
test_insert_with_param(char *sql, char *table_name, ub4 cExecCount)
{
	sword		ret = DCI_SUCCESS;
	DCIStmt     *stmhp1 = NULL;
	DCIStmt     *stmhp2 = NULL;
	DCIDescribe	*dschp = NULL;
	DCIParam	*parmh = NULL, *param_list = NULL, *arg = NULL;
	DCIBind     *bindhp[32];

	ub2         num_args = 0;
	ub2			data_type[32]; 
	ub2			data_size[32];
	ub1			data_precision[32]; 
	sb1			data_scale[32]; 
	DciText     *name;
	ub4			i, j;

	KDB_INT64	*int64_value[32];
	int		*int_value[32];
	short	*short_value[32];
	unsigned char *uchar_value[32];
	float	*float_value[32];
	double	*double_value[32];
	char	*str_value[32];
	DCIDate	*date_value[32];
	DCIDateTime	*dt_value[32][2];
	DCIInterval	*intv_value[32][2];
	int 	temp_int;
	ub4		cFetchRows = 0;


	for (i = 0; i < 32; i++)
	{
		int64_value[i] = NULL;
		int_value[i] = NULL;
		short_value[i] = NULL;
		uchar_value[i] = NULL;
		float_value[i] = NULL;
		double_value[i] = NULL;
		str_value[i] = NULL;
		date_value[i] = NULL;
		dt_value[i][0] = NULL;
		dt_value[i][1] = NULL;
		intv_value[i][0] = NULL;
		intv_value[i][1] = NULL;
	}

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &stmhp1, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(stmhp1, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIHandleAlloc(pEnv, (void **) &dschp, DCI_HTYPE_DESCRIBE, 0, NULL);
	err = DCIDescribeAny(pSvcCtx, (DCIError *) pError, (void *) table_name, sizeof(table_name), DCI_OTYPE_NAME, DCI_DEFAULT, DCI_PTYPE_TABLE, dschp);
	err = DCIAttrGet((void *)dschp, DCI_HTYPE_DESCRIBE, (void *)&parmh, (ub4 *)0, DCI_ATTR_PARAM, pError);
	err = DCIAttrGet((void *)parmh, DCI_DTYPE_PARAM, (void *)&num_args, (ub4 *)0, DCI_ATTR_NUM_COLS, pError);
	err = DCIAttrGet((void *)parmh, DCI_DTYPE_PARAM, (void *)&param_list, (ub4 *)0, DCI_ATTR_LIST_COLUMNS, pError);

	for (i = 0; i < num_args; i++) 
	{
		char        temp_str[32];

		//Get parameter details. 
		err = DCIParamGet((CONST dvoid *)param_list,(ub4)DCI_DTYPE_PARAM, (DCIError *) pError,(dvoid **)&arg,(ub4)i+1); 

		err = DCIAttrGet((CONST dvoid *)arg,(ub4)DCI_DTYPE_PARAM, (dvoid *)&name,(ub4 *)0, (ub4)DCI_ATTR_NAME,(DCIError *) pError); 

		err = DCIAttrGet((CONST dvoid *)arg,(ub4)DCI_DTYPE_PARAM, (dvoid *)&data_type[i],(ub4 *)0, (ub4)DCI_ATTR_DATA_TYPE, (DCIError *) pError);

		err = DCIAttrGet((CONST dvoid *)arg,(ub4)DCI_DTYPE_PARAM, (dvoid *)&data_size[i],(ub4 *)0, (ub4)DCI_ATTR_DATA_SIZE, (DCIError *) pError);

		err = DCIAttrGet((CONST dvoid *)arg,(ub4)DCI_DTYPE_PARAM, (dvoid *)&data_precision[i],(ub4 *)0, (ub4)DCI_ATTR_PRECISION, (DCIError *) pError);

		err = DCIAttrGet((CONST dvoid *)arg,(ub4)DCI_DTYPE_PARAM, (dvoid *)&data_scale[i],(ub4 *)0, (ub4)DCI_ATTR_SCALE, (DCIError *) pError);

		printf("%d, %s, %d, %d, %d\n", data_type[i], name, data_size[i], data_precision[i], data_scale[i]);

		sprintf(temp_str, ":%d", i);

		switch (data_type[i]) 
		{ 
		case SQLT_CUR: 
			err = DCIBindByName(stmhp1, &bindhp[i], pError, (text *)temp_str, 2, &stmhp2, 4, SQLT_RSET, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
			err = DCIBindArrayOfStruct(bindhp[i], pError, sizeof(DCIStmt *), 0, 0, 0);
			break; 
		case SQLT_FLT:
			if (data_size[i] == 4)
			{
				float_value[i] = (float *) malloc(sizeof(float) * cExecCount);
				if (!float_value[i])
				{
					ret = DCI_ERROR;
					goto free_resource;
				}
				for (j= 0; j < cExecCount; j++)
					float_value[i][j] = 1.123f;
				err = DCIBindByName(stmhp1, &bindhp[i], pError, (text *)temp_str, 2, float_value[i],sizeof(float), SQLT_FLT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
				err = DCIBindArrayOfStruct(bindhp[i], pError, sizeof(float), 0, 0, 0);
			}
			else if (data_size[i] == 8)
			{
				double_value[i] = (double *) malloc(sizeof(double) * cExecCount);
				if (!double_value[i])
				{
					ret = DCI_ERROR;
					goto free_resource;
				}
				for (j= 0; j < cExecCount; j++)
					double_value[i][j] = 111111.111111;
				err = DCIBindByName(stmhp1, &bindhp[i], pError, (text *)temp_str, 2, double_value[i],sizeof(double), SQLT_FLT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
				err = DCIBindArrayOfStruct(bindhp[i], pError, sizeof(double), 0, 0, 0);
			}
			break;
		case SQLT_NUM: 
			if ((data_precision[i] == 20) && (data_scale[i] == 0))
			{
				int64_value[i] = (KDB_INT64 *) malloc(sizeof(KDB_INT64) * cExecCount);
				if (!int64_value[i])
				{
					ret = DCI_ERROR;
					goto free_resource;
				}
				for (j= 0; j < cExecCount; j++)
					int64_value[i][j] = 1234567890123456789LL;
				err = DCIBindByName(stmhp1, &bindhp[i], pError, (text *)temp_str, 2, int64_value[i], sizeof(KDB_INT64), SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
				err = DCIBindArrayOfStruct(bindhp[i],pError, sizeof(KDB_INT64), 0, 0, 0);
			}
			else if ((data_precision[i] == 19) && (data_scale[i] == 0))
			{
				int64_value[i] = (KDB_INT64 *) malloc(sizeof(KDB_INT64) * cExecCount);
				if (!int64_value[i])
				{
					ret = DCI_ERROR;
					goto free_resource;
				}
				for (j= 0; j < cExecCount; j++)
					int64_value[i][j] = 1234567890123456789LL;
				err = DCIBindByName(stmhp1, &bindhp[i], pError, (text *)temp_str, 2, int64_value[i], sizeof(KDB_INT64), SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
				err = DCIBindArrayOfStruct(bindhp[i],pError, sizeof(KDB_INT64), 0, 0, 0);
			}
			else if ((data_precision[i] == 10) && (data_scale[i] == 0))
			{
				int_value[i] = (int *) malloc(sizeof(int) * cExecCount);
				if (!int_value[i])
				{
					ret = DCI_ERROR;
					goto free_resource;
				}
				for (j= 0; j < cExecCount; j++)
					int_value[i][j] = j + 1;
				err = DCIBindByName(stmhp1, &bindhp[i], pError, (text *)temp_str, 2, int_value[i], sizeof(int), SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
				err = DCIBindArrayOfStruct(bindhp[i],pError, sizeof(int), 0, 0, 0);
			}
			else if ((data_precision[i] == 5) && (data_scale[i] == 0))
			{
				short_value[i] = (short *) malloc(sizeof(short) * cExecCount);
				if (!short_value[i])
				{
					ret = DCI_ERROR;
					goto free_resource;
				}
				for (j= 0; j < cExecCount; j++)
					short_value[i][j] = 1;
				err = DCIBindByName(stmhp1, &bindhp[i], pError, (text *)temp_str, 2, short_value[i],sizeof(short),SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
				err = DCIBindArrayOfStruct(bindhp[i], pError, sizeof(short), 0, 0, 0);
			}
			else if ((data_precision[i] == 3) && (data_scale[i] == 0))
			{
				uchar_value[i] = (ub1 *) malloc(sizeof(ub1) * cExecCount);
				if (!uchar_value[i])
				{
					ret = DCI_ERROR;
					goto free_resource;
				}
				for (j= 0; j < cExecCount; j++)
					uchar_value[i][j] = 1;
				err = DCIBindByName(stmhp1, &bindhp[i], pError, (text *)temp_str, 2, uchar_value[i],sizeof(unsigned char),SQLT_UIN, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
				err = DCIBindArrayOfStruct(bindhp[i], pError, sizeof(unsigned char), 0, 0, 0);
			}
			else if (data_scale[i] == 0)// other datatypes which can be treated as INT
			{
				int_value[i] = (int *) malloc(sizeof(int) * cExecCount);
				if (!int_value[i])
				{
					ret = DCI_ERROR;
					goto free_resource;
				}
				for (j= 0; j < cExecCount; j++)
					int_value[i][j] = j + 1;
				err = DCIBindByName(stmhp1, &bindhp[i], pError, (text *)temp_str, 2, int_value[i],sizeof(int),SQLT_UIN, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
				err = DCIBindArrayOfStruct(bindhp[i],pError, sizeof(int),0,	0, 0);
			}
			else if ((data_precision[i] == 22) && (data_scale[i] == 6))
			{
				float_value[i] = (float *) malloc(sizeof(float) * cExecCount);
				if (!float_value[i])
				{
					ret = DCI_ERROR;
					goto free_resource;
				}
				for (j= 0; j < cExecCount; j++)
					float_value[i][j] = 1.123f;
				err = DCIBindByName(stmhp1, &bindhp[i], pError, (text *)temp_str, 2, float_value[i],sizeof(float), SQLT_FLT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
				err = DCIBindArrayOfStruct(bindhp[i], pError, sizeof(float), 0, 0, 0);
			}
			else if ((data_precision[i] == 38) && (data_scale[i] == 12))
			{
				double_value[i] = (double *) malloc(sizeof(double) * cExecCount);
				if (!double_value[i])
				{
					ret = DCI_ERROR;
					goto free_resource;
				}
				for (j= 0; j < cExecCount; j++)
					double_value[i][j] = 111111.111111;
				err = DCIBindByName(stmhp1, &bindhp[i], pError, (text *)temp_str, 2, double_value[i],sizeof(double), SQLT_FLT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
				err = DCIBindArrayOfStruct(bindhp[i], pError, sizeof(double), 0, 0, 0);
			}
			else if (data_scale[i] > 0)// other datatypes which can be treated as FLOAT
			{
				float_value[i] = (float *) malloc(sizeof(float) * cExecCount);
				if (!float_value[i])
				{
					ret = DCI_ERROR;
					goto free_resource;
				}
				for (j= 0; j < cExecCount; j++)
					float_value[i][j] = 1;
				err = DCIBindByName(stmhp1, &bindhp[i], pError, (text *)temp_str, 2, float_value[i],sizeof(float),SQLT_FLT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
				err = DCIBindArrayOfStruct(bindhp[i], pError, sizeof(float), 0, 0, 0);
			}
			else
			{
				printf("GetProcedureResult::不支持的存储过程参数类型\n");
			}
			break; 
		case SQLT_CHR: 
			str_value[i] = (char*) malloc(data_precision[i] * cExecCount);
			if (!str_value[i])
			{
				ret = DCI_ERROR;
				goto free_resource;
			}
			memset(str_value[i], 0, data_precision[i] * cExecCount);
			for (j= 0; j < cExecCount; j++)
			{
				//memcpy(str_value[i] + j * data_precision[i], "aa", 2);
				//memset(str_value[i] + 2 + j * data_precision[i], " ", data_precision[i] - 2);
				snprintf(str_value[i] + j * data_precision[i], data_precision[i], "%d%dC", i, j);
			}

			err = DCIBindByName(stmhp1, &bindhp[i], pError, (text *)temp_str, 2, str_value[i], data_precision[i], SQLT_CHR, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
			err = DCIBindArrayOfStruct(bindhp[i],pError,data_precision[i],0,	0, 0);
			break;
		case SQLT_AFC:
			str_value[i] = (char*) malloc(data_precision[i] * cExecCount);
			if (!str_value[i])
			{
				ret = DCI_ERROR;
				goto free_resource;
			}
			memset(str_value[i], 0, data_precision[i] * cExecCount);
			for (j = 0; j < cExecCount; j++)
			{
				//memcpy(str_value[i] + j * data_precision[i], "aa", 2);
				//memset(str_value[i] + 2 + j * data_precision[i], " ", data_precision[i] - 2);
				snprintf(str_value[i] + j * data_precision[i], data_precision[i], "%d%dA", i, j);
			}

			err = DCIBindByName(stmhp1, &bindhp[i], pError, (text *)temp_str, 2, str_value[i], 3, SQLT_CHR, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
			err = DCIBindArrayOfStruct(bindhp[i],pError,data_precision[i], 0, 0, 0);
			break;
		case SQLT_DAT:
			date_value[i] = (DCIDate *) malloc(sizeof(DCIDate) * cExecCount);
			if (!date_value[i])
			{
				ret = DCI_ERROR;
				goto free_resource;
			}

			str_value[i] = (char *) malloc(strlen("2008-08-08 08:08:08") + 1);
			if (!str_value[i])
			{
				ret = DCI_ERROR;
				goto free_resource;
			}

			memset(str_value[i], 0, strlen("2008-08-08 08:08:08") + 1);
			memcpy(str_value[i], "2008-08-08 08:08:08", strlen("2008-08-08 08:08:08"));

			for (j= 0; j < cExecCount; j++)
			{
				char *pcur;
				pcur = str_value[i];
				sscanf(pcur,"%d",&temp_int);
				date_value[i][j].DCIDateYYYY =(sb2)temp_int;
				pcur = strchr(pcur, '-');
				if (pcur == NULL)
				{
					printf("参数格式错误\n");
				}
				pcur += 1;

				sscanf(pcur,"%d",&temp_int);
				date_value[i][j].DCIDateMM = (ub1)temp_int;
				pcur=strchr(pcur, '-');
				if (pcur == NULL)
				{
					printf("参数格式错误\n");
				}
				pcur+=1;

				sscanf(pcur,"%d",&temp_int);
				date_value[i][j].DCIDateDD = (ub1)temp_int + j;
				pcur=strchr(pcur,' ');
				if (pcur == NULL)
				{
					printf("参数格式错误\n");
				}
				pcur += 1;

				sscanf(pcur,"%d",&temp_int);
				date_value[i][j].DCIDateTime.DCITimeHH = (ub1)temp_int;
				pcur=strchr(pcur,':');
				if (pcur == NULL)
				{
					printf("参数格式错误\n");
				}
				pcur+=1;

				sscanf(pcur,"%d",&temp_int);
				date_value[i][j].DCIDateTime.DCITimeMI = (ub1)temp_int;
				pcur=strchr(pcur,':');
				if (pcur == NULL)
				{
					printf("参数格式错误\n");
				}
				pcur+=1;

				sscanf(pcur, "%d", &temp_int);
				date_value[i][j].DCIDateTime.DCITimeSS = (ub1)temp_int;
			}
			err = DCIBindByName(stmhp1, &bindhp[i], pError, (text *)temp_str, 2, date_value[i], sizeof(DCIDate), SQLT_ODT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
			err = DCIBindArrayOfStruct(bindhp[i], pError, sizeof(DCIDate), 0, 0, 0);
			break;

		case SQLT_DATE:
			for (j= 0; j < cExecCount; j++)
			{
				err = DCIDescriptorAlloc(pEnv, (void **) &dt_value[i][j], DCI_DTYPE_DATE, 0, NULL);
				err = DCIDateTimeConstruct(pEnv, pError, dt_value[i][j], 2010, 10, 17 + j, 0, 0, 0, 0, NULL, 0);
			}
			
			//bindbyname		
			err = DCIBindByName(stmhp1, 
								&bindhp[i],
								pError, 
								(text *)temp_str,
								3,
								(dvoid *) &dt_value[i],
								sizeof(DCIDateTime *),
								SQLT_DATE,
								(dvoid *) 0,
								(ub2 *) 0,
								(ub2 *) 0,
								(ub4) 0,
								(ub4 *) 0,
								DCI_DEFAULT);
			if (err != DCI_SUCCESS)
			{
				printf("DCIBindByName failed\n");
				ret = DCI_ERROR;
				goto free_resource;
			}
	
			err = DCIBindArrayOfStruct(bindhp[i], pError, sizeof(DCIDateTime *), 0, 0, 0);
			break;

		case SQLT_TIME:
		case SQLT_TIME_TZ:
			for (j= 0; j < cExecCount; j++)
			{
				err = DCIDescriptorAlloc(pEnv, (void **) &dt_value[i][j], DCI_DTYPE_TIME, 0, NULL);
				err = DCIDateTimeConstruct(pEnv, pError, dt_value[i][j], 0, 0, 0, 10, 10, 10 + j, 10 + j, NULL, 0);
			}

			//bindbyname		
			err = DCIBindByName(stmhp1, 
								&bindhp[i],
								pError, 
								(text *)temp_str,
								-1,
								(dvoid *) &dt_value[i],
								sizeof(DCIDateTime *),
								SQLT_TIME,
								(dvoid *) 0,
								(ub2 *) 0,
								(ub2 *) 0,
								(ub4) 0,
								(ub4 *) 0,
								DCI_DEFAULT);
			if (err != DCI_SUCCESS)
			{
				printf("DCIBindByName failed\n");
				ret = DCI_ERROR;
				goto free_resource;
			}

			err = DCIBindArrayOfStruct(bindhp[i], pError, sizeof(DCIDateTime *), 0, 0, 0);
			break;

		case SQLT_TIMESTAMP:
		case SQLT_TIMESTAMP_TZ:
			for (j= 0; j < cExecCount; j++)
			{
				err = DCIDescriptorAlloc(pEnv, (void **) &dt_value[i][j], DCI_DTYPE_TIMESTAMP, 0, NULL);
				err = DCIDateTimeConstruct(pEnv, pError, dt_value[i][j], 2010, 10, 17 + j, 10, 10, 10 + j, 10 + j, NULL, 0);
			}

			//bindbyname		
			err = DCIBindByName(stmhp1, 
								&bindhp[i],
								pError, 
								(text *)temp_str,
								-1,
								(dvoid *) &dt_value[i],
								sizeof(DCIDateTime *),
								SQLT_TIMESTAMP,
								(dvoid *) 0,
								(ub2 *) 0,
								(ub2 *) 0,
								(ub4) 0,
								(ub4 *) 0,
								DCI_DEFAULT);
			if (err != DCI_SUCCESS)
			{
				printf("DCIBindByName failed\n");
				ret = DCI_ERROR;
				goto free_resource;
			}

			err = DCIBindArrayOfStruct(bindhp[i], pError, sizeof(DCIDateTime *), 0, 0, 0);
			break;

		case SQLT_INTERVAL_YM:
			err = DCIDescriptorAlloc(pEnv, (void **) &intv_value[i][0], DCI_DTYPE_INTERVAL_YM, 0, NULL);
			err = DCIIntervalFromText(pEnv, pError, (DciText *)"1-2", 3, intv_value[i][0]);
			err = DCIDescriptorAlloc(pEnv, (void **) &intv_value[i][1], DCI_DTYPE_INTERVAL_YM, 0, NULL);
			err = DCIIntervalFromText(pEnv, pError, (DciText *)"-1-2", 3, intv_value[i][1]);

			//bindbyname		
			err = DCIBindByName(stmhp1, 
								&bindhp[i],
								pError, 
								(text *)temp_str,
								-1,
								(dvoid *) &intv_value[i][0],
								sizeof(DCIInterval *),
								SQLT_INTERVAL_YM,
								(dvoid *) 0,
								(ub2 *) 0,
								(ub2 *) 0,
								(ub4) 0,
								(ub4 *) 0,
								DCI_DEFAULT);
			if (err != DCI_SUCCESS)
			{
				printf("DCIBindByName failed\n");
				ret = DCI_ERROR;
				goto free_resource;
			}

			err = DCIBindArrayOfStruct(bindhp[i], pError, sizeof(DCIInterval *), 0, 0, 0);
			break;

		case SQLT_INTERVAL_DS:
			err = DCIDescriptorAlloc(pEnv, (void **) &intv_value[i][0], DCI_DTYPE_INTERVAL_DS, 0, NULL);
			err = DCIIntervalFromText(pEnv, pError, (DciText *)"1 2:3:4.500000", 10, intv_value[i][0]);
			err = DCIDescriptorAlloc(pEnv, (void **) &intv_value[i][1], DCI_DTYPE_INTERVAL_DS, 0, NULL);
			err = DCIIntervalFromText(pEnv, pError, (DciText *)"-1 2:3:4.500000", 10, intv_value[i][1]);

			//bindbyname		
			err = DCIBindByName(stmhp1, 
								&bindhp[i],
								pError, 
								(text *)temp_str,
								-1,
								(dvoid *) &intv_value[i][0],
								sizeof(DCIInterval *),
								SQLT_INTERVAL_DS,
								(dvoid *) 0,
								(ub2 *) 0,
								(ub2 *) 0,
								(ub4) 0,
								(ub4 *) 0,
								DCI_DEFAULT);
			if (err != DCI_SUCCESS)
			{
				printf("DCIBindByName failed\n");
				ret = DCI_ERROR;
				goto free_resource;
			}

			err = DCIBindArrayOfStruct(bindhp[i], pError, sizeof(DCIInterval *), 0, 0, 0);
			break;

		default: 
			printf("不支持的参数类型(%d)\n", data_type[i]);

			break; 
		}
	}

	err = DCIStmtExecute(pSvcCtx, stmhp1, pError, cExecCount, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet(stmhp1, DCI_HTYPE_STMT, &cFetchRows, 0, DCI_ATTR_ROW_COUNT, pError);
	printf("%d rows inserted\n", cFetchRows);

	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:

	for (i = 0; i < 32; i++)
	{
		if (int64_value[i])
		{
			free(int64_value[i]);
			int64_value[i] = NULL;
		}
		if (int_value[i])
		{
			free(int_value[i]);
			int_value[i] = NULL;
		}
		if (short_value[i])
		{
			free(short_value[i]);
			short_value[i] = NULL;
		}
		if (uchar_value[i])
		{
			free(uchar_value[i]);
			uchar_value[i] = NULL;
		}
		if (float_value[i])
		{
			free(float_value[i]);
			float_value[i] = NULL;
		}
		if (double_value[i])
		{
			free(double_value[i]);
			double_value[i] = NULL;
		}
		if (str_value[i])
		{
			free(str_value[i]);
			str_value[i] = NULL;
		}
		if (date_value[i])
		{
			free(date_value[i]);
			date_value[i] = NULL;
		}
		if (dt_value[i])
		{
			if(DCI_SUCCESS == DCIDescriptorFree(dt_value[i][0], DCI_DTYPE_DATE))
				dt_value[i][0] = NULL;
			if(DCI_SUCCESS == DCIDescriptorFree(dt_value[i][1], DCI_DTYPE_DATE))
				dt_value[i][1] = NULL;
			
			if(DCI_SUCCESS == DCIDescriptorFree(dt_value[i][0], DCI_DTYPE_TIME))
				dt_value[i][0] = NULL;
			if(DCI_SUCCESS == DCIDescriptorFree(dt_value[i][1], DCI_DTYPE_TIME))
			dt_value[i][1] = NULL;
		}
		if (intv_value[i])
		{
			if(DCI_SUCCESS == DCIDescriptorFree(intv_value[i][0], DCI_DTYPE_INTERVAL_YM))
				dt_value[i][0] = NULL;
			if(DCI_SUCCESS == DCIDescriptorFree(intv_value[i][1], DCI_DTYPE_INTERVAL_YM))
				dt_value[i][1] = NULL;
			
			if(DCI_SUCCESS == DCIDescriptorFree(intv_value[i][0], DCI_DTYPE_INTERVAL_DS))
				dt_value[i][0] = NULL;
			if(DCI_SUCCESS == DCIDescriptorFree(intv_value[i][1], DCI_DTYPE_INTERVAL_DS))
				dt_value[i][1] = NULL;	
		}
	}

	if (stmhp2)
		err = DCIHandleFree((void *) stmhp2, DCI_HTYPE_STMT);

	err = DCIHandleFree((dvoid *) dschp, (ub4) DCI_HTYPE_DESCRIBE);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_DESCRIBE failed\n");
	}

	err = DCIHandleFree((dvoid *) stmhp1, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

end:
	err = disconnect(true);
	return ret;
}

sword
test_select(char *sql)
{
	sword	ret = DCI_SUCCESS;
	DCIStmt	*pStmt =  NULL;
#ifdef WIN32
	LARGE_INTEGER start, end, frq;
	double cost = 0;
#else
	struct timeval start_t, stop_t;
	long cost_usec = 0;
#endif

#ifdef WIN32
	QueryPerformanceFrequency(&frq);
#endif

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

#ifdef WIN32
	QueryPerformanceCounter(&start);
#else
	gettimeofday(&start_t, NULL);
#endif

	printf("%s\n", sql);
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);
		ret = DCI_ERROR;
		goto free_resource;
	}

#ifdef WIN32
	QueryPerformanceCounter(&end);
	cost = (double)(end.QuadPart - start.QuadPart)/frq.QuadPart;
	printf("prepare: %f\n", cost);
#else
	gettimeofday(&stop_t, NULL);
	if (stop_t.tv_usec < start_t.tv_usec)
	{
		stop_t.tv_sec--;
		stop_t.tv_usec += 1000000;
	}
	cost_usec = (long) ((stop_t.tv_sec - start_t.tv_sec) * 1000000 + (stop_t.tv_usec - start_t.tv_usec));

	printf("prepare: %ld us\n", cost_usec);
#endif

#ifdef WIN32
	QueryPerformanceCounter(&start);
#else
	gettimeofday(&start_t, NULL);
#endif

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);
		ret = DCI_ERROR;
		goto free_resource;
	}

#ifdef WIN32
	QueryPerformanceCounter(&end);
	cost = (double)(end.QuadPart - start.QuadPart)/frq.QuadPart;
	printf("execute: %f\n", cost);
#else
	gettimeofday(&stop_t, NULL);
	if (stop_t.tv_usec < start_t.tv_usec)
	{
		stop_t.tv_sec--;
		stop_t.tv_usec += 1000000;
	}
	cost_usec = (long) ((stop_t.tv_sec - start_t.tv_sec) * 1000000 + (stop_t.tv_usec - start_t.tv_usec));

	printf("execute: %ld us\n", cost_usec);
#endif

	//ret = DCIAttrGet(pStmt, DCI_HTYPE_STMT, &cFetchRows, NULL, DCI_ATTR_ROW_COUNT, pError);

	err = Fetch(pEnv, pStmt, pError, 2);
	if (err != DCI_SUCCESS && err != DCI_NO_DATA)
	{
		printf("Fetch failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

end:
	err = disconnect(true);
	return ret;
}

/* same as test_select but do not print results into file */
sword
test_select_no_print(char *sql)
{
	sword	ret = DCI_SUCCESS;
	DCIStmt	*pStmt =  NULL;
#ifdef WIN32
	LARGE_INTEGER start, end, frq;
	double cost = 0;
#else
	struct timeval start_t, stop_t;
	long cost_usec = 0;
#endif

#ifdef WIN32
	QueryPerformanceFrequency(&frq);
#endif

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

#ifdef WIN32
	QueryPerformanceCounter(&start);
#else
	gettimeofday(&start_t, NULL);
#endif

	printf("%s\n", sql);
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);
		ret = DCI_ERROR;
		goto free_resource;
	}

#ifdef WIN32
	QueryPerformanceCounter(&end);
	cost = (double)(end.QuadPart - start.QuadPart)/frq.QuadPart;
	printf("prepare: %f\n", cost);
#else
	gettimeofday(&stop_t, NULL);
	if (stop_t.tv_usec < start_t.tv_usec)
	{
		stop_t.tv_sec--;
		stop_t.tv_usec += 1000000;
	}
	cost_usec = (long) ((stop_t.tv_sec - start_t.tv_sec) * 1000000 + (stop_t.tv_usec - start_t.tv_usec));

	printf("prepare: %ld us\n", cost_usec);
#endif

#ifdef WIN32
	QueryPerformanceCounter(&start);
#else
	gettimeofday(&start_t, NULL);
#endif

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);
		ret = DCI_ERROR;
		goto free_resource;
	}

#ifdef WIN32
	QueryPerformanceCounter(&end);
	cost = (double)(end.QuadPart - start.QuadPart)/frq.QuadPart;
	printf("execute: %f\n", cost);
#else
	gettimeofday(&stop_t, NULL);
	if (stop_t.tv_usec < start_t.tv_usec)
	{
		stop_t.tv_sec--;
		stop_t.tv_usec += 1000000;
	}
	cost_usec = (long) ((stop_t.tv_sec - start_t.tv_sec) * 1000000 + (stop_t.tv_usec - start_t.tv_usec));

	printf("execute: %ld us\n", cost_usec);
#endif

	//ret = DCIAttrGet(pStmt, DCI_HTYPE_STMT, &cFetchRows, NULL, DCI_ATTR_ROW_COUNT, pError);

	err = Fetch_no_print(pEnv, pStmt, pError, 2);
	if (err != DCI_SUCCESS && err != DCI_NO_DATA)
	{
		printf("Fetch failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

end:
	err = disconnect(true);
	return ret;
}

#define READ_BUFSIZE 1024

sword
test_insert_lo()
{
	sword	ret = DCI_SUCCESS;
	DCIStmt			*pStmt = NULL;
	DCILobLocator	*pBlob = NULL, *pClob1 = NULL, *pClob2 = NULL;
	DCIBind			*pBind = NULL;
	DCIDefine		*pDefine = NULL;

	char *sql = "INSERT INTO T_LOB VALUES(:1, :2, :3)";
	char *select_sql = (char *)"select * from t_lob for update";

	/*bug#15667:dailybuild failed result from path error.*/
	char *strBlob_in = gstrPath;
	char strClob1_in[MAX_PATH_LEN], strClob2_in[MAX_PATH_LEN];

	memset(strClob1_in, 0, MAX_PATH_LEN);
	strncpy(strClob1_in, gstrPath, gCnt);
	memset(strClob2_in, 0, MAX_PATH_LEN);
	strncpy(strClob2_in, gstrPath, gCnt);

	ub4     amt, offset, len;
	int     act_read_count, not_read_count;
	FILE    *fp = NULL;
	ub1     buff[READ_BUFSIZE];
	ub1     cbuff[READ_BUFSIZE * 512 + 1];

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDescriptorAlloc(pEnv, (void **) &pBlob, DCI_DTYPE_LOB, 0, NULL);
	err = DCIDescriptorAlloc(pEnv, (void **) &pClob1, DCI_DTYPE_LOB, 0, NULL);
	err = DCIDescriptorAlloc(pEnv, (void **) &pClob2, DCI_DTYPE_LOB, 0, NULL);

	//bindbyname		
	err = DCIBindByName(pStmt, 
						&pBind,
						pError, 
						(text *)":1",
						-1,
						(dvoid *) &pBlob,
						sizeof(DCILobLocator *),
						SQLT_BLOB,
						(dvoid *) 0,
						(ub2 *) 0,
						(ub2 *) 0,
						(ub4) 0,
						(ub4 *) 0,
						DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIBindByName failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindArrayOfStruct(pBind, pError, sizeof(DCILobLocator *), 0, 0, 0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIBindArrayOfStruct failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByName(pStmt, 
						&pBind,
						pError, 
						(text *)":2",
						-1,
						(dvoid *) &pClob1,
						sizeof(DCILobLocator *),
						SQLT_CLOB,
						(dvoid *) 0,
						(ub2 *) 0,
						(ub2 *) 0,
						(ub4) 0,
						(ub4 *) 0,
						DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIBindByName failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindArrayOfStruct(pBind, pError, sizeof(DCILobLocator *), 0, 0, 0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIBindArrayOfStruct failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByName(pStmt, 
		&pBind,
		pError, 
		(text *)":3",
		-1,
		(dvoid *) &pClob2,
		sizeof(DCILobLocator *),
		SQLT_CLOB,
		(dvoid *) 0,
		(ub2 *) 0,
		(ub2 *) 0,
		(ub4) 0,
		(ub4 *) 0,
		DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIBindByName failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindArrayOfStruct(pBind, pError, sizeof(DCILobLocator *), 0, 0, 0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIBindArrayOfStruct failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	//execute
	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, (const DCISnapshot *) NULL, (DCISnapshot *) NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtExecute failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	//select 
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) select_sql, (ub4) strlen((char*)select_sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtExecute failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt,
						&pDefine,
						pError,
						1,
						(dvoid *) &pBlob,
						sizeof(DCILobLocator *),
						SQLT_BLOB,
						(void *)0,
						(ub2 *)0,
						(ub2 *)0,
						DCI_DEFAULT);

	err = DCIDefineArrayOfStruct(pDefine, pError, sizeof(DCILobLocator *), 0, 0, 0);

	err = DCIDefineByPos(pStmt,
						&pDefine,
						pError,
						2, 
						(void *) &pClob1,
						sizeof(DCILobLocator *),
						SQLT_CLOB,
						(void *)0,
						(ub2 *)0,
						(ub2 *)0,
						DCI_DEFAULT);

	err = DCIDefineArrayOfStruct(pDefine, pError, sizeof(DCILobLocator *), 0, 0, 0);

	err = DCIDefineByPos(pStmt,
						&pDefine,
						pError,
						3,
						(void *) &pClob2,
						sizeof(DCILobLocator *),
						SQLT_CLOB,
						(void *)0,
						(ub2 *)0,
						(ub2 *)0,
						DCI_DEFAULT);

	err = DCIDefineArrayOfStruct(pDefine, pError, sizeof(DCILobLocator *), 0, 0, 0);

	err = DCIStmtFetch(pStmt, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT);
	if (ret == DCI_NO_DATA)
		goto free_resource;


	offset = 1; // Offset for Lobs start at 1 
	not_read_count = 1548;
	//bug#15667:dailybuild failed result from path error.
	strcat(strBlob_in, "blob_in");
	fp = fopen(strBlob_in, "rb");
	if (!fp)
	{
		return DCI_ERROR;
	}

	while (not_read_count > 0)
	{        	
		not_read_count -= READ_BUFSIZE;

		if (not_read_count < 0)
		{
			act_read_count = READ_BUFSIZE + not_read_count;
		}
		else
		{
			act_read_count = READ_BUFSIZE;
		}

		// Read the data from lob_vec 
		memset(buff, 0, act_read_count);
		fread(buff, 1, act_read_count, fp);

		//Write it into the locator
		amt = act_read_count;                 
		err = DCILobWrite(pSvcCtx, pError, pBlob, &amt, offset,
							(dvoid *) buff, (ub4) act_read_count, DCI_ONE_PIECE,
							(dvoid *)0, 0 ,(ub2) 0, (ub1) SQLCS_IMPLICIT);
		if (err == DCI_SUCCESS)
		{
			offset += amt;
		}
	}
	fclose(fp);
	err = DCILobGetLength(pSvcCtx, pError, pBlob, &len);
	printf("write %d into blob\n", len);


	offset = 1; // Offset for Lobs start at 1 
	not_read_count = 1483407;//4477;
	//bug#15667:dailybuild failed result from path error.
	strcat(strClob1_in, "clob1_in.txt");
	fp = fopen(strClob1_in, "r");
	if (!fp)
	{
		return DCI_ERROR;
	}

	while (not_read_count > 0)
	{
		if (not_read_count > READ_BUFSIZE * 512)
			act_read_count = READ_BUFSIZE * 512;
		else
			act_read_count = not_read_count;

		// Read the data from lob_vec 
		memset(cbuff, 0, READ_BUFSIZE * 512 + 1);
		fread(cbuff, 1, act_read_count, fp);

		cbuff[act_read_count] = '\0';

		//Write it into the locator
		amt = act_read_count;
		err = DCILobWrite(pSvcCtx, pError, pClob1, &amt, offset,
							(dvoid *) cbuff, (ub4) act_read_count + 1, DCI_ONE_PIECE,
							(dvoid *)0, 0 ,(ub2) 0, (ub1) SQLCS_IMPLICIT);
		offset += amt;
		not_read_count -= act_read_count;
	}
	fclose(fp);
	err = DCILobGetLength(pSvcCtx, pError, pClob1, &len);
	printf("write %d into clob1\n", len);


	offset = 1; // Offset for Lobs start at 1

#if defined(WIN32)
	not_read_count = 4477;
	strcat(strClob2_in, "clob2_in_gb2312.txt");
#else
	not_read_count = 4500;
	strcat(strClob2_in, "clob2_in.txt");
#endif

	fp = fopen(strClob2_in, "rb");
	if (!fp)
	{
		return DCI_ERROR;
	}

	while (not_read_count > 0)
	{
		if (not_read_count > READ_BUFSIZE)
			act_read_count = READ_BUFSIZE;
		else
			act_read_count = not_read_count;

		// Read the data from lob_vec 
		memset(cbuff, 0, READ_BUFSIZE + 1);
		fread(cbuff, 1, act_read_count, fp);

		cbuff[act_read_count] = '\0';

		//Write it into the locator
		amt = act_read_count;
		err = DCILobWrite(pSvcCtx, pError, pClob2, &amt, offset,
			(dvoid *) cbuff, (ub4) act_read_count + 1, DCI_ONE_PIECE,
			(dvoid *)0, 0 ,(ub2) 0, (ub1) SQLCS_IMPLICIT);
		offset += amt;
		not_read_count -= act_read_count;
	}
	fclose(fp);
	err = DCILobGetLength(pSvcCtx, pError, pClob2, &len);
	printf("write %d into clob2\n", len);

	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	if (pBlob)
		err = DCIDescriptorFree((void *) pBlob, DCI_DTYPE_LOB);
	if (pClob1)
		err = DCIDescriptorFree((void *) pClob1, DCI_DTYPE_LOB);
	if (pClob2)
		err = DCIDescriptorFree((void *) pClob2, DCI_DTYPE_LOB);

	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);

end:
	err = disconnect(true);
	return ret;
}

sword
test_select_lo()
{
	sword ret = DCI_SUCCESS;
	DCIStmt			*pStmt = NULL;
	DCILobLocator	*pBlob = NULL, *pClob1 = NULL, *pClob2 = NULL;
	DCIDefine		*pDefine = NULL;
	int result_length = 0;

	char *select_sql = (char *)"select * from t_lob";

	/*bug#15667:dailybuild failed result from path error.*/
	char strBlob_out[MAX_PATH_LEN], strClob1_out[MAX_PATH_LEN], strClob2_out[MAX_PATH_LEN];

	memset(strBlob_out, 0, MAX_PATH_LEN);
	strncpy(strBlob_out, gstrPath, gCnt);
	memset(strClob1_out, 0, MAX_PATH_LEN);
	strncpy(strClob1_out, gstrPath, gCnt);
	memset(strClob2_out, 0, MAX_PATH_LEN);
	strncpy(strClob2_out, gstrPath, gCnt);

	ub4 amt, offset;
	ub4 blob_len;
	ub4 clob_len;
	ub1 buff[READ_BUFSIZE];
	ub1 cbuff[READ_BUFSIZE * 2 + 1];
	FILE *fp = NULL;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(pStmt, pError, (const DciText*) select_sql, (ub4) strlen((char*)select_sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtExecute failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDescriptorAlloc(pEnv, (void**)&pBlob, DCI_DTYPE_LOB, 0, NULL);
	err = DCIDescriptorAlloc(pEnv, (void**)&pClob1, DCI_DTYPE_LOB, 0, NULL);
	err = DCIDescriptorAlloc(pEnv, (void**)&pClob2, DCI_DTYPE_LOB, 0, NULL);

	err = DCIDefineByPos(pStmt, 
						&pDefine, 
						pError,
						1,
						(dvoid *) &pBlob, 
						sizeof(DCILobLocator *), 
						SQLT_BLOB, 
						(void *)0,
						(ub2 *)0,
						(ub2 *)0,
						DCI_DEFAULT);

	err = DCIDefineArrayOfStruct(pDefine, pError, sizeof(DCILobLocator *), 0, 0, 0);

	err = DCIDefineByPos(pStmt,
						&pDefine,
						pError,
						2, 
						(void *) &pClob1,
						sizeof(DCILobLocator *), 
						SQLT_CLOB,
						(void *)0,
						(ub2 *)&result_length,
						(ub2 *)0,
						DCI_DEFAULT);

	err = DCIDefineArrayOfStruct(pDefine, pError, sizeof(DCILobLocator *), 0, 0, 0);

	err = DCIDefineByPos(pStmt,
						&pDefine,
						pError,
						3, 
						(void *) &pClob2,
						sizeof(DCILobLocator *), 
						SQLT_CLOB,
						(void *)0,
						(ub2 *)&result_length,
						(ub2 *)0,
						DCI_DEFAULT);

	err = DCIDefineArrayOfStruct(pDefine, pError, sizeof(DCILobLocator *), 0, 0, 0);

	err = DCIStmtFetch(pStmt, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT);
	if (ret == DCI_NO_DATA)
		goto end;


	err = DCILobGetLength(pSvcCtx, pError, pBlob, &blob_len);
	printf("blob: len = %d\n", blob_len);
	amt = 0;//blob_len;

	/* read blob data into file */
	offset = 1;

	/*bug#15667:dailybuild failed result from path error.*/
	strcat(strBlob_out, "blob_out");
	fp = fopen(strBlob_out, "wb");
	if (!fp)
	{
		return DCI_ERROR;
	}
	do
	{
		memset((dvoid *)buff, 0, READ_BUFSIZE);
		err = DCILobRead(pSvcCtx, pError, pBlob, &amt, offset,
						(dvoid *) buff, (ub4)READ_BUFSIZE, (dvoid *) 0,
						0, (ub2) 0, (ub1) SQLCS_IMPLICIT);
		if (err == DCI_SUCCESS || err == DCI_NEED_DATA)
		{
			fwrite(buff, amt, 1, fp);
			offset += amt;
		}
	} while (err == DCI_NEED_DATA);
	fclose(fp);


	err = DCILobGetLength(pSvcCtx, pError, pClob1, &clob_len);
	printf("clob1: len = %d\n", clob_len);
	amt = clob_len;

	/* read clob1 data into file */
	offset = 1;

	/*bug#15667:dailybuild failed result from path error.*/
	strcat(strClob1_out, "clob1_out.txt");
	fp = fopen(strClob1_out, "w");
	if (!fp)
	{
		return DCI_ERROR;
	}
	do
	{
		memset((dvoid *)cbuff, 0, READ_BUFSIZE*2 + 1);
		err = DCILobRead(pSvcCtx, pError, pClob1, &amt, offset,
						(dvoid *) cbuff, (ub4)READ_BUFSIZE + 1, (dvoid *) 0,
						0, (ub2) 0, (ub1) SQLCS_IMPLICIT);
		if (err == DCI_SUCCESS || err == DCI_NEED_DATA)
		{
			cbuff[amt] = '\0';
			fwrite(cbuff, amt, 1, fp);
			offset += amt;
		}
	} while (err == DCI_NEED_DATA);
	fclose(fp);


	err = DCILobGetLength(pSvcCtx, pError, pClob2, &clob_len);
	printf("clob2: len = %d\n", clob_len);
	amt = clob_len;

	/* read clob2 data into file */
	offset = 1;

	/*bug#15667:dailybuild failed result from path error.*/
	strcat(strClob2_out, "clob2_out.txt");
	fp = fopen(strClob2_out, "wb");
	if (!fp)
	{
		return DCI_ERROR;
	}
	do
	{
		memset((dvoid *)cbuff, 0, READ_BUFSIZE*2 + 1);
		err = DCILobRead(pSvcCtx, pError, pClob2, &amt, offset,
			(dvoid *) cbuff, (ub4)READ_BUFSIZE + 1, (dvoid *) 0,
			0, (ub2) 0, (ub1) SQLCS_IMPLICIT);
		if (err == DCI_SUCCESS || err == DCI_NEED_DATA)
		{
			cbuff[amt] = '\0';
			fwrite(cbuff, amt, 1, fp);
			offset += amt;
		}
	} while (err == DCI_NEED_DATA);
	fclose(fp);

	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	if (pBlob)
		err = DCIDescriptorFree(pBlob, DCI_DTYPE_LOB);
	if (pClob1)
		err = DCIDescriptorFree(pClob1, DCI_DTYPE_LOB);
	if (pClob2)
		err = DCIDescriptorFree(pClob2, DCI_DTYPE_LOB);

	err = DCIHandleFree( (dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);

end:	
	err = disconnect(true);
	return ret;
}

sword
test_piece_write_lo()
{
	sword	ret = DCI_SUCCESS;
	DCIStmt			*pStmt = NULL;
	DCILobLocator	*pBlob = NULL, *pClob1 = NULL;
	DCIBind			*pBind = NULL;
	DCIDefine		*pDefine = NULL;

	char *sql = "INSERT INTO T_LOB VALUES(:1, :2)";
	char *select_sql = (char *)"select * from t_lob for update";

	char *strBlob_in = gstrPath;
	char strClob1_in[MAX_PATH_LEN];

	memset(strClob1_in, 0, MAX_PATH_LEN);
	strncpy(strClob1_in, gstrPath, gCnt);

	ub4     amt, offset, len;
	int     act_read_count, not_read_count;
	FILE    *fp = NULL;
	ub1     buff[READ_BUFSIZE];
	ub1     cbuff[READ_BUFSIZE + 1];
	ub1		piece;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDescriptorAlloc(pEnv, (void **) &pBlob, DCI_DTYPE_LOB, 0, NULL);
	err = DCIDescriptorAlloc(pEnv, (void **) &pClob1, DCI_DTYPE_LOB, 0, NULL);

	//bindbyname		
	err = DCIBindByName(pStmt, 
						&pBind,
						pError, 
						(text *)":1",
						-1,
						(dvoid *) &pBlob,
						sizeof(DCILobLocator *),
						SQLT_BLOB,
						(dvoid *) 0,
						(ub2 *) 0,
						(ub2 *) 0,
						(ub4) 0,
						(ub4 *) 0,
						DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIBindByName failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindArrayOfStruct(pBind, pError, sizeof(DCILobLocator *), 0, 0, 0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIBindArrayOfStruct failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByName(pStmt, 
						&pBind,
						pError, 
						(text *)":2",
						-1,
						(dvoid *) &pClob1,
						sizeof(DCILobLocator *),
						SQLT_CLOB,
						(dvoid *) 0,
						(ub2 *) 0,
						(ub2 *) 0,
						(ub4) 0,
						(ub4 *) 0,
						DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIBindByName failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindArrayOfStruct(pBind, pError, sizeof(DCILobLocator *), 0, 0, 0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIBindArrayOfStruct failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	//execute
	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, (const DCISnapshot *) NULL, (DCISnapshot *) NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtExecute failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	//select 
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) select_sql, (ub4) strlen((char*)select_sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtExecute failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt,
						&pDefine,
						pError,
						1,
						(dvoid *) &pBlob,
						sizeof(DCILobLocator *),
						SQLT_BLOB,
						(void *)0,
						(ub2 *)0,
						(ub2 *)0,
						DCI_DEFAULT);

	err = DCIDefineArrayOfStruct(pDefine, pError, sizeof(DCILobLocator *), 0, 0, 0);

	err = DCIDefineByPos(pStmt,
						&pDefine,
						pError,
						2,
						(void *) &pClob1,
						sizeof(DCILobLocator *),
						SQLT_CLOB,
						(void *)0,
						(ub2 *)0,
						(ub2 *)0,
						DCI_DEFAULT);

	err = DCIDefineArrayOfStruct(pDefine, pError, sizeof(DCILobLocator *), 0, 0, 0);

	err = DCIStmtFetch(pStmt, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT);
	if (ret == DCI_NO_DATA)
		goto free_resource;


	offset = 1; // Offset for Lobs start at 1 
	not_read_count = 1548; 	//3000
	amt = not_read_count;

	//bug#15667:dailybuild failed result from path error.
	//strcat(strBlob_in, "blob_in");
	fp = fopen(strBlob_in, "rb");
	if (!fp)
	{
		return DCI_ERROR;
	}

	{
		if (not_read_count > READ_BUFSIZE)
			act_read_count = READ_BUFSIZE;
		else
			act_read_count = not_read_count;

		memset(buff, 0, act_read_count);
		fread(buff, 1, act_read_count, fp);

		not_read_count -= act_read_count;

		if (not_read_count == 0)
		{
			(void) printf("Only one piece, no need for stream write.\n");
			if (DCILobWrite(pSvcCtx, pError, pBlob, &amt, offset,
							(dvoid *) buff, (ub4) act_read_count, DCI_ONE_PIECE,
							(dvoid *)0, 0 ,(ub2) 0, (ub1) SQLCS_IMPLICIT) != DCI_SUCCESS)
			{
				(void) printf("ERROR: DCILobWrite()\n");
			}
		}

		else
		{
			if (DCILobWrite(pSvcCtx, pError, pBlob, &amt, offset,
							(dvoid *) buff, (ub4) act_read_count, DCI_FIRST_PIECE,
							(dvoid *)0, 0 ,(ub2) 0, (ub1) SQLCS_IMPLICIT) != DCI_NEED_DATA)
			{
				(void) printf("ERROR: DCILobWrite()\n");
			}

			piece = DCI_NEXT_PIECE;

			do
			{
				if (not_read_count > READ_BUFSIZE)
					act_read_count = BUFLEN;
				else
				{
					act_read_count = not_read_count;
					piece = DCI_LAST_PIECE;
				}

				memset(buff, 0, act_read_count);
				fread(buff, 1, act_read_count, fp);

				err = DCILobWrite(pSvcCtx, pError, pBlob, &amt, offset, (dvoid *) buff,
				(ub4) act_read_count, piece, (dvoid *)0,
				(DCICallbackLobWrite) 0,
				(ub2) 0, (ub1) SQLCS_IMPLICIT);
				not_read_count -= act_read_count;

			} while (err == DCI_NEED_DATA);
		}
	}
	if (err != DCI_SUCCESS)
	{
		(void) printf("Error: stream writing LOB.\n");
		return err;
	}

	fclose(fp);
	err = DCILobGetLength(pSvcCtx, pError, pBlob, &len);
	printf("write %d into blob\n", len);


	offset = 1; // Offset for Lobs start at 1 
	not_read_count = 1483407;//4477;
	amt = not_read_count;

	//bug#15667:dailybuild failed result from path error.
	strcat(strClob1_in, "clob1_in.txt");
	fp = fopen(strClob1_in, "r");
	if (!fp)
	{
		return DCI_ERROR;
	}

	{
		if (not_read_count > READ_BUFSIZE)
			act_read_count = READ_BUFSIZE;
		else
			act_read_count = not_read_count;

		memset(cbuff, 0, READ_BUFSIZE + 1);
		fread(cbuff, 1, act_read_count, fp);

		not_read_count -= act_read_count;

		if (not_read_count == 0)
		{
			(void) printf("Only one piece, no need for stream write.\n");
			if (DCILobWrite(pSvcCtx, pError, pClob1, &amt, offset,
							(dvoid *) cbuff, (ub4) act_read_count , DCI_ONE_PIECE,
							(dvoid *)0, 0 ,(ub2) 0, (ub1) SQLCS_IMPLICIT) != DCI_SUCCESS)
			{
				(void) printf("ERROR: DCILobWrite()\n");
			}
		}

		else
		{
			if (DCILobWrite(pSvcCtx, pError, pClob1, &amt, offset,
							(dvoid *) cbuff, (ub4) act_read_count , DCI_FIRST_PIECE,
							(dvoid *)0, 0 ,(ub2) 0, (ub1) SQLCS_IMPLICIT) != DCI_NEED_DATA)
			{
				(void) printf("ERROR: DCILobWrite()\n");
			}

			piece = DCI_NEXT_PIECE;

			do
			{
				if (not_read_count > READ_BUFSIZE)
					act_read_count = BUFLEN;
				else
				{
					act_read_count = not_read_count;
					piece = DCI_LAST_PIECE;
				}

				memset(cbuff, 0, READ_BUFSIZE + 1);
				fread(cbuff, 1, act_read_count, fp);

				err = DCILobWrite(pSvcCtx, pError, pClob1, &amt, offset, (dvoid *) cbuff,
				(ub4) act_read_count , piece, (dvoid *)0,
				(DCICallbackLobWrite) 0,
				(ub2) 0, (ub1) SQLCS_IMPLICIT);
				not_read_count -= act_read_count;

			} while (err == DCI_NEED_DATA);
		}
	}
	if (err != DCI_SUCCESS)
	{
		(void) printf("Error: stream writing LOB.\n");
		return err;
	}

	fclose(fp);
	err = DCILobGetLength(pSvcCtx, pError, pClob1, &len);
	printf("write %d into clob1\n", len);


	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	if (pBlob)
		err = DCIDescriptorFree((void *) pBlob, DCI_DTYPE_LOB);
	if (pClob1)
		err = DCIDescriptorFree((void *) pClob1, DCI_DTYPE_LOB);

	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);

end:
	err = disconnect(true);
	return ret;
}

sword
test_piece_read_lo()
{
	sword ret = DCI_SUCCESS;
	DCIStmt			*pStmt = NULL;
	DCILobLocator	*pBlob = NULL, *pClob1 = NULL;
	DCIDefine		*pDefine = NULL;
	int result_length = 0;

	char *select_sql = (char *)"select * from t_lob";

	/*bug#15667:dailybuild failed result from path error.*/
	char strBlob_out[MAX_PATH_LEN], strClob1_out[MAX_PATH_LEN];

	memset(strBlob_out, 0, MAX_PATH_LEN);
	strncpy(strBlob_out, gstrPath, gCnt);
	memset(strClob1_out, 0, MAX_PATH_LEN);
	strncpy(strClob1_out, gstrPath, gCnt);

	ub4 amt, offset;
	ub4 blob_len;
	ub4 clob_len;
	ub1 buff[READ_BUFSIZE];
	ub1 cbuff[READ_BUFSIZE * 2 + 1];
	FILE *fp = NULL;
	//ub1		piece;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(pStmt, pError, (const DciText*) select_sql, (ub4) strlen((char*)select_sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtExecute failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDescriptorAlloc(pEnv, (void**)&pBlob, DCI_DTYPE_LOB, 0, NULL);
	err = DCIDescriptorAlloc(pEnv, (void**)&pClob1, DCI_DTYPE_LOB, 0, NULL);

	err = DCIDefineByPos(pStmt, 
						&pDefine, 
						pError,
						1,
						(dvoid *) &pBlob, 
						sizeof(DCILobLocator *), 
						SQLT_BLOB, 
						(void *)0,
						(ub2 *)0,
						(ub2 *)0,
						DCI_DEFAULT);

	err = DCIDefineArrayOfStruct(pDefine, pError, sizeof(DCILobLocator *), 0, 0, 0);

	err = DCIDefineByPos(pStmt,
						&pDefine,
						pError,
						2, 
						(void *) &pClob1,
						sizeof(DCILobLocator *), 
						SQLT_CLOB,
						(void *)0,
						(ub2 *)&result_length,
						(ub2 *)0,
						DCI_DEFAULT);

	err = DCIDefineArrayOfStruct(pDefine, pError, sizeof(DCILobLocator *), 0, 0, 0);

	err = DCIStmtFetch(pStmt, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT);
	if (ret == DCI_NO_DATA)
		goto end;

	err = DCILobGetLength(pSvcCtx, pError, pBlob, &blob_len);
	printf("blob: len = %d\n", blob_len);
	amt = 0;//blob_len;

	/* read blob data into file */
	offset = 1;

	/*bug#15667:dailybuild failed result from path error.*/
	strcat(strBlob_out, "blob_out1");
	fp = fopen(strBlob_out, "wb");
	if (!fp)
	{
		return DCI_ERROR;
	}
	do
	{
		memset((dvoid *)buff, 0, READ_BUFSIZE);
		err = DCILobRead(pSvcCtx, pError, pBlob, &amt, offset,
						(dvoid *) buff, (ub4)READ_BUFSIZE, (dvoid *) 0,
						0, (ub2) 0, (ub1) SQLCS_IMPLICIT);
		if (err == DCI_SUCCESS || err == DCI_NEED_DATA)
		{
			fwrite(buff, amt, 1, fp);
			offset += amt;
		}
	} while (err == DCI_NEED_DATA);
	fclose(fp);

	err = DCILobGetLength(pSvcCtx, pError, pClob1, &clob_len);
	printf("clob1: len = %d\n", clob_len);
	amt = clob_len;

	/* read clob1 data into file */
	offset = 1;

	/*bug#15667:dailybuild failed result from path error.*/
	strcat(strClob1_out, "clob1_out1.txt");
	fp = fopen(strClob1_out, "w");
	if (!fp)
	{
		return DCI_ERROR;
	}
	do
	{
		memset((dvoid *)cbuff, 0, READ_BUFSIZE * 2 + 1);
		err = DCILobRead(pSvcCtx, pError, pClob1, &amt, offset,
						(dvoid *) cbuff, (ub4)READ_BUFSIZE + 1, (dvoid *) 0,
						0, (ub2) 0, (ub1) SQLCS_IMPLICIT);
		if (err == DCI_SUCCESS || err == DCI_NEED_DATA)
		{
			cbuff[amt] = '\0';
			fwrite(cbuff, amt, 1, fp);
			offset += amt;
		}
	} while (err == DCI_NEED_DATA);
	fclose(fp);

	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	if (pBlob)
		err = DCIDescriptorFree(pBlob, DCI_DTYPE_LOB);
	if (pClob1)
		err = DCIDescriptorFree(pClob1, DCI_DTYPE_LOB);

	err = DCIHandleFree( (dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);

end:	
	err = disconnect(true);
	return ret;
}

#define FAC			20000
#define INTERVAL	1 /* minutes */
#define HOURS		24
#define MINUTES		60
#define DEFAULT		" DEFAULT 0"

sword
execute(char *sql)
{
	sword	ret = DCI_SUCCESS;
	DCIStmt	*pStmt = NULL;

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen(sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %s\n", msg);

		ret = DCI_ERROR;
		goto free_resource;
	}
	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %s\n", msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

end:
	return ret;
}

static sword
ccbase_insert()
{
	sword	ret = DCI_SUCCESS;
	DCIStmt	*pStmt = NULL;
	char	*sql = NULL;
	int		i = 0;
	DCIBind	*pBind = NULL;

	int		*int_value = NULL;
	double	*double_value = NULL;

#ifdef WIN32
	LARGE_INTEGER start, end, frq;
	double cost;
#else
	struct timeval start_t, stop_t;
	long cost_sec = 0, cost_usec = 0;
#endif

	int_value = (int *) malloc(sizeof(int) * FAC);
	if (!int_value)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	double_value = (double *) malloc(sizeof(double) * FAC);
	if (!double_value)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	/* insert data_id */
	sql = (char *) malloc(strlen("insert into CCBASE(DATA_ID) values(:0)") + 1);
	if (!sql)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	sprintf(sql, "insert into CCBASE(DATA_ID) values(:0)");

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen(sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	for (i = 0; i < FAC; i++)
	{
		int_value[i] = i + 1;
	}

	err = DCIBindByName(pStmt, &pBind, pError, (DciText *)":0", 2, int_value, sizeof(int), SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	err = DCIBindArrayOfStruct(pBind, pError, sizeof(int), 0, 0, 0);

#ifdef WIN32
	QueryPerformanceFrequency(&frq);
	QueryPerformanceCounter(&start);
#else
	gettimeofday(&start_t, NULL);
#endif

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, FAC, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);

#ifdef WIN32
	QueryPerformanceCounter(&end);
	cost = (double)(end.QuadPart - start.QuadPart)/frq.QuadPart;
	printf("INSERT %d Rows: %f seconds\n", FAC, cost);
#else
	gettimeofday(&stop_t, NULL);

	if (stop_t.tv_usec < start_t.tv_usec)
	{
		stop_t.tv_sec--;
		stop_t.tv_usec += 1000000;
	}
	cost_sec = (long) ((stop_t.tv_sec - start_t.tv_sec) * 1000 + (stop_t.tv_usec - start_t.tv_usec) / 1000);
	cost_usec = (long) ((stop_t.tv_usec - start_t.tv_usec) % 1000);

	printf("\nINSERT %d Rows: %ld.%03ld ms\n", FAC, cost_sec, cost_usec);
#endif

	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %s\n", msg);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

	if (sql)
		free(sql);

	if (int_value)
		free(int_value);
	if (double_value)
		free(double_value);

end:
	return ret;
}

static sword
ccbase_update()
{
	sword	ret = DCI_SUCCESS;
	DCIStmt	*pStmt = NULL;
	char	*sql = NULL;
	int		i = 0, j = 0, k = 0;
	DCIBind	*pBind = NULL;

	int		*int_value = NULL;
	double	*double_value = NULL;

#ifdef WIN32
	LARGE_INTEGER start, end, frq;
	double cost;
#else
	struct timeval start_t, stop_t;
	long cost_sec = 0, cost_usec = 0;
#endif

	int_value = (int *) malloc(sizeof(int) * FAC);
	if (!int_value)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	double_value = (double *) malloc(sizeof(double) * FAC);
	if (!double_value)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	/* update with interval */
	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	sql = (char *) malloc(strlen("update CCBASE set DATA_00_00 = :0 where DATA_ID = :1") + 1);
	if (!sql)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	for (k = 0; k < 1; k++)
	{
		for (i = 0; i < MINUTES / INTERVAL; i++)
		{
			sprintf(sql, "update CCBASE set DATA_%d_%d = :0 where DATA_ID = :1", k, i * INTERVAL);

			err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
			if (err != DCI_SUCCESS)
			{
				printf("DCIStmtPrepare failed\n");
				ret = DCI_ERROR;
				goto free_resource;
			}

			for (j = 0; j < FAC; j++)
			{
				int_value[j] = j + 1;
				double_value[j] = j + 1.75;
			}
			err = DCIBindByName(pStmt, &pBind, pError, (DciText *)":0", 2, double_value, sizeof(double), SQLT_FLT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
			err = DCIBindArrayOfStruct(pBind, pError, sizeof(double), 0, 0, 0);

			err = DCIBindByName(pStmt, &pBind, pError, (DciText *)":1", 2, int_value, sizeof(int), SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
			err = DCIBindArrayOfStruct(pBind, pError, sizeof(int), 0, 0, 0);

#ifdef WIN32
			QueryPerformanceFrequency(&frq);
			QueryPerformanceCounter(&start);
#else
			gettimeofday(&start_t, NULL);
#endif

			err = DCIStmtExecute(pSvcCtx, pStmt, pError, FAC, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);

#ifdef WIN32
			QueryPerformanceCounter(&end);
			cost = (double)(end.QuadPart - start.QuadPart)/frq.QuadPart;
			printf("UPDATE DATA_%d_%d: %f seconds\n", k, i * INTERVAL, cost);
#else
			gettimeofday(&stop_t, NULL);

			if (stop_t.tv_usec < start_t.tv_usec)
			{
				stop_t.tv_sec--;
				stop_t.tv_usec += 1000000;
			}
			cost_sec = (long) ((stop_t.tv_sec - start_t.tv_sec) * 1000 + (stop_t.tv_usec - start_t.tv_usec) / 1000);
			cost_usec = (long) ((stop_t.tv_usec - start_t.tv_usec) % 1000);

			printf("\nUPDATE DATA_%d_%d: %ld.%03ld ms\n", k, i * INTERVAL, cost_sec, cost_usec);
#endif
			if (err != DCI_SUCCESS)
			{
				printf("DCIStmtExecute failed\n");
				ret = DCI_ERROR;
				goto free_resource;
			}

			err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
			if (err != DCI_SUCCESS)
			{
				printf("DCITransCommit failed\n");
				ret = DCI_ERROR;
				goto free_resource;
			}
		}
	}

free_resource:
	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
	pStmt = NULL;
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

	if (sql)
		free(sql);

	if (int_value)
		free(int_value);
	if (double_value)
		free(double_value);

end:
	return ret;
}

/* Add the following to kingbase.conf
shared_buffers = 512MB	
checkpoint_timeout = 60min
#bgwriter_all_maxpages = 128
full_page_writes = off
wal_buffers = 512kB
#autovacuum = off			
#autovacuum_naptime = 10
max_fsm_pages = 400000
enable_seqscan=off
data_transmitted_format='mixed'
enable_inplace_update=on
enable_max_xlog=on
*/
sword test_1440()
{
	sword	ret = DCI_SUCCESS;

	char	*sql = NULL;
	int		i = 0, j = 0;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	printf("***CCBASE_CREATE***\n");
	sql = (char *) malloc(strlen("CREATE TABLE CCBASE(DATA_ID INT PRIMARY KEY)") + strlen(", DATA_00_00 REAL DEFAULT 0") * HOURS * MINUTES / INTERVAL + 1);
	if (!sql)
	{
		ret = DCI_ERROR;
		goto end;
	}
	sprintf(sql, "CREATE TABLE CCBASE(DATA_ID INT PRIMARY KEY");
	for (i = 0; i < HOURS; i++)
	{
		for(j = 0; j < MINUTES / INTERVAL; j++)
			sprintf(sql + strlen(sql), ", DATA_%d_%d REAL%s",  i, j * INTERVAL, DEFAULT);
	}
	strcat(sql, ")");

	//printf("%s\n", sql);
	err = execute("drop table ccbase");
	err = execute(sql);

	printf("\n***CCBASE_INSERT***\n");
	err = ccbase_insert();
	if (err != DCI_SUCCESS)
	{
		printf("insert ccbase failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	printf("\n***CCBASE_UPDATE***\n");
	err = ccbase_update();
	if (err != DCI_SUCCESS)
	{
		printf("update ccbase failed\n");
		ret = DCI_ERROR;
		goto end;
	}

end:	
	err = disconnect(true);
	return ret;
}

sword test_DCIStmtFetch2()
{
	DciText *sql = (DciText*)"SELECT ID FROM T_SCROLLCURSOR";
	sword ret = DCI_SUCCESS;
	DciText insertSql[200];
	DCIStmt *pStmt = NULL;
	int value_id = 0;	
	int i = 0;

	test_execute("DROP TABLE IF EXISTS T_SCROLLCURSOR");
	err = test_execute("CREATE TABLE T_SCROLLCURSOR(ID INT)");
	if(err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}
	
	for(i = 0; i < 10; i++)
	{
		snprintf((char*)insertSql, 200, "INSERT INTO T_SCROLLCURSOR VALUES (%d)", i);
		err = test_execute((char*)insertSql);
		if(err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto end;
		}
	}

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{		
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError,0,0,NULL,NULL,DCI_DEFAULT);
	if(err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIDefineByPos(pStmt, NULL,pError, 1, (void*)&value_id, sizeof(int), SQLT_INT, NULL, NULL, NULL, DCI_DEFAULT);
	if(err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	i = 0;
	while(1)
	{
		int expect_value = 0;
		i++;
		value_id = 0;
		if(i < 6 || i > 6)
		{
			err = DCIStmtFetch2(pStmt, pError, (ub4)1, (ub2)DCI_FETCH_NEXT, 0, (ub4)DCI_DEFAULT);
			if(i < 6)
				expect_value = i - 1;
			else if(i > 6)
				expect_value = i - 6;
		}
		else
		{
			err = DCIStmtFetch2(pStmt, pError, (ub4)1, (ub2)DCI_FETCH_FIRST, 0, (ub4)DCI_DEFAULT);
			expect_value = 0;
		}

		if(err != DCI_SUCCESS && DCI_NO_DATA != err)
		{
			ret = DCI_ERROR;
			goto end;
		}

		if(err == DCI_NO_DATA)
			break;

		if(expect_value != value_id)
		{
			ret = DCI_ERROR;
			break;
		}


		printf("value_id %d\n", value_id);
	}

end:
	if(pStmt)
		err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
	pStmt = NULL;
	
	err = disconnect(true);
	test_execute("DROP TABLE T_SCROLLCURSOR");
	return ret;

}

//add by jjshi, on 2015-11-30
sword TestXML()
{
        DciText *sqlTest = (DciText *)"select * from T_XML;";
	const int CHAR_SIZE = 301;
        char    *col = (char*) malloc (sizeof(char) * CHAR_SIZE);
        DCIDefine *bndhp[1];
        DCIStmt *pStmt = NULL;
        ub4     stmrow, stmrowEx;
        int     i;
        int ret = DCI_ERROR;

        err = connect(true);
        if (err != DCI_SUCCESS)
        {
                printf("connect failed\n");
                ret = DCI_ERROR;
                goto end;
        }

        err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, DCI_HTYPE_STMT, 0, NULL);
        if (err != DCI_SUCCESS)
        {
                printf("DCIHandleAlloc pStmt failed\n");
                ret = DCI_ERROR;
                goto end;
        }

        err = execute("drop table if exists T_XML;");
        err = execute("CREATE TABLE T_XML(col xml)");
        err = execute("INSERT INTO T_XML VALUES('<name>xiaoli</name>');");

        if (DCIStmtPrepare(pStmt, pError, sqlTest, (ub4)strlen((char *)sqlTest),
            (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
        {
                printf("FAILED: DCIStmtPrepare() select\n");
                ret = DCI_ERROR;
                goto free_resource;
        }
        memset(col, 0, CHAR_SIZE);

        if (DCIDefineByPos(pStmt, &bndhp[0], pError, 1,
                (dvoid *)col, (sb4)CHAR_SIZE, (ub2)SQLT_CHR,
                (dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
        {
                printf("FAILED: DCIDefineByPos()\n");
                ret = DCI_ERROR;
                goto free_resource;
        }

        if (DCIDefineArrayOfStruct(bndhp[0], pError, CHAR_SIZE,
                0, 0, 0))
        {                
		printf("FAILED: DCIDefineArrayOfStruct()\n");
                ret = DCI_ERROR;
                goto free_resource;
        }

        if (DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4)0, (ub4)0,
                (CONST DCISnapshot*)0, (DCISnapshot*)0,
                (ub4)DCI_DEFAULT))
        {
                printf("FAILED: DCIStmtExecute() select\n");
                ret = DCI_ERROR;
                goto free_resource;
        }

        stmrowEx = 0;
        while (1)
        {
                if (DCIStmtFetch(pStmt, pError, 1, DCI_FETCH_NEXT, 0) == DCI_ERROR)
                        break;

                DCIAttrGet(pStmt, DCI_HTYPE_STMT, &stmrow, 0, DCI_ATTR_ROW_COUNT, pError);
                if(stmrow == stmrowEx)
                        break;
                printf("fetch %d  row\n", stmrow - stmrowEx);

                for (i = 0; i < 1; i++)
                {
                        printf("*************%d Row****************\n", i + 1);
                        printf("col: %s\n", col + CHAR_SIZE * i);
                }
                stmrowEx = stmrow;
        }

        if (DCITransCommit(pSvcCtx,  pError,(ub4)DCI_DEFAULT))
        {
                printf("FAILED: DCITransCommit() select\n");
                ret = DCI_ERROR;
                goto free_resource;
        }
        
        free(col);
        ret =  DCI_SUCCESS;


free_resource:
		err = execute("drop table T_XML;");
        err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
        if (err != DCI_SUCCESS)
        {       
                printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
        }

end:
        err = disconnect(true);
        return ret;
}

//add by dwxiao, on 2014-5-23.
sword TestBinary()
{
	text *getBinSQL = (text *)"select BIN_COL FROM BIN_TEST";
	text *procSQL = (text *)"select BIN_PROC(NULL)";
	text *funcSQL = (text *)"select BIN_FUNC()";
	text *insSQL = (text *)"insert into BIN_TEST (BIN_COL) values ( :bina )";
	
	char bin_data[] = "1a2b3c4d5e";
	char bin_out[sizeof(bin_data)];
	char bin_out_vbi[sizeof(bin_data)+2];

	DCIDefine *hDefine = (DCIDefine *) 0;
	DCIStmt	*pStmt = NULL;
	sword	ret = DCI_SUCCESS;
	DCIBind  *bndLobp = NULL;
	
	DciText *sqlTest = (DciText *)"select count(*) from SYS_TYPE where typname = 'BINARY'";
	ub4		count = 0;
	DCIDefine 	*def =NULL;
	
	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	if (DCIStmtPrepare(pStmt, pError, sqlTest, strlen((char*)sqlTest), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("Test if kingbase support binary PREPARE failed.\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (DCIDefineByPos(pStmt, &def, pError, 1, &count, sizeof(count), SQLT_INT, NULL, NULL, NULL, DCI_DEFAULT))
	{
		printf("Test if kingbase support binary DEFINE failed.\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("Test if kingbase support binary EXEC failed.\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;		
	}

	if (0 == count)
	{
		ret = DCI_SUCCESS;
		goto free_resource;
	}
	
	execute("CREATE OR REPLACE PROCEDURE \"PUBLIC\".\"BIN_PROC\"(out bina binary(30)) AS BEGIN bina := '1a2b3c4d5e'; END;");
	execute("CREATE OR REPLACE FUNCTION \"PUBLIC\".\"BIN_FUNC\"() RETURN binary(30) AS BEGIN return '1a2b3c4d5e'; END;");
	execute("drop table BIN_TEST");
	execute("create table BIN_TEST(BIN_COL binary(30))");
	//sprintf((char *)insSQL, "insert into BIN_TEST (BIN_COL) values ('%s')", bin_data);
	//execute((char *)insSQL);

	err = DCIStmtPrepare(pStmt, pError, insSQL, (ub4) strlen((char *)insSQL),
		(ub4) DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByName(pStmt, &bndLobp, pError, (text *)":bina",
		strlen(":bina"), (dvoid *) bin_data, sizeof(bin_data), SQLT_BIN, (dvoid *) 0,
		(ub2 *) 0, (ub2) 0, (ub4) 0, (ub4 *) 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4) 1, (ub4) 0, 
		(DCISnapshot *) NULL, (DCISnapshot *) NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	/* function */
	err = DCIStmtPrepare(pStmt, pError, funcSQL, 
			(ub4)strlen((char*)funcSQL),
			(ub4) DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
	/* SQLT_BIN */
	memset(bin_out, 0, sizeof(bin_data));
	hDefine = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &hDefine, 
			pError, 1, (dvoid *)bin_out, sizeof(bin_data), 
			SQLT_BIN, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(bin_out, bin_data))
	{
		printf("Data compare (SQLT_BIN) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	/* SQLT_LBI */
	memset(bin_out, 0, sizeof(bin_data));
	hDefine = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &hDefine, 
			pError, 1, (dvoid *)bin_out, sizeof(bin_data), 
			SQLT_LBI, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(bin_out, bin_data))
	{
		printf("Data compare (SQLT_BIN) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}
	
	/* SQLT_LVB */
	memset(bin_out, 0, sizeof(bin_data));
	hDefine = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &hDefine, 
			pError, 1, (dvoid *)bin_out, sizeof(bin_data), 
			SQLT_LVB, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(bin_out, bin_data))
	{
		printf("Data compare (SQLT_BIN) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	/* SQLT_VBI */
	memset(bin_out_vbi, 0, sizeof(bin_data)+2);
	hDefine = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &hDefine, 
			pError, 1, (dvoid *)bin_out_vbi, sizeof(bin_data)+2, 
			SQLT_VBI, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(bin_out_vbi+2, bin_data))
	{
		printf("Data compare (SQLT_BIN) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	/* procedure */
	err = DCIStmtPrepare(pStmt, pError, procSQL, 
			(ub4)strlen((char*)procSQL),
			(ub4) DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
	/* SQLT_BIN */
	memset(bin_out, 0, sizeof(bin_data));
	hDefine = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &hDefine, 
			pError, 1, (dvoid *)bin_out, sizeof(bin_data), 
			SQLT_BIN, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(bin_out, bin_data))
	{
		printf("Data compare (SQLT_BIN) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}
	
	/* SQLT_LBI */
	memset(bin_out, 0, sizeof(bin_data));
	hDefine = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &hDefine, 
			pError, 1, (dvoid *)bin_out, sizeof(bin_data), 
			SQLT_LBI, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(bin_out, bin_data))
	{
		printf("Data compare (SQLT_BIN) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}
	
	/* SQLT_LVB */
	memset(bin_out, 0, sizeof(bin_data));
	hDefine = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &hDefine, 
			pError, 1, (dvoid *)bin_out, sizeof(bin_data), 
			SQLT_LVB, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(bin_out, bin_data))
	{
		printf("Data compare (SQLT_BIN) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	/* SQLT_VBI */
	memset(bin_out_vbi, 0, sizeof(bin_data)+2);
	hDefine = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &hDefine, 
			pError, 1, (dvoid *)bin_out_vbi, sizeof(bin_data)+2, 
			SQLT_VBI, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(bin_out_vbi+2, bin_data))
	{
		printf("Data compare (SQLT_BIN) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	/* SQL */
	err = DCIStmtPrepare(pStmt, pError, getBinSQL, 
			(ub4)strlen((char*)getBinSQL),
			(ub4) DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
	/* SQLT_BIN */
	memset(bin_out, 0, sizeof(bin_data));

	err = DCIDefineByPos(pStmt, &hDefine, 
			pError, 1, (dvoid *)bin_out, sizeof(bin_data), 
			SQLT_BIN, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_BIN) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(bin_out, bin_data))
	{
		printf("Data compare (SQLT_BIN) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}
	
	/* SQLT_LBI */
	memset(bin_out, 0, sizeof(bin_data));

	err = DCIDefineByPos(pStmt, &hDefine, 
			pError, 1, (dvoid *)bin_out, sizeof(bin_data), 
			SQLT_LBI, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_LBI) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_LBI) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(bin_out, bin_data))
	{
		printf("Data compare (SQLT_LBI) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	/* SQLT_VBI */
	memset(bin_out_vbi, 0, sizeof(bin_data)+2);

	err = DCIDefineByPos(pStmt, &hDefine, 
			pError, 1, (dvoid *)bin_out_vbi, sizeof(bin_data)+2, 
			SQLT_VBI, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_VBI) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_VBI) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(bin_out_vbi+2, bin_data))
	{
		printf("Data compare (SQLT_VBI) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	/* SQLT_LVB */
	memset(bin_out, 0, sizeof(bin_data));

	err = DCIDefineByPos(pStmt, &hDefine, 
			pError, 1, (dvoid *)bin_out, sizeof(bin_data), 
			SQLT_LVB, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_LVB) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_LVB) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(bin_out, bin_data))
	{
		printf("Data compare (SQLT_LVB) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

free_resource:	
	if (pStmt)
	{
		err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIHandlefree pStmt failed\n");
		}
	}
	
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

//add by dwxiao, binary for anonymous block.
sword 
test_binary_bind()
{
	DciText * sql = (DciText*)"begin select cast('1a2b3c4d5e' as binary(30)) as bina_ into :bina_ from dual; end;";

	char binaStr[200] = {0};
	DCIStmt *stmthp = NULL;
	sword	ret = DCI_ERROR;
	DCIBind		*bindhp = NULL;
	DciText *sqlSet = (DciText *)"set compatible_level=oracle";
	DciText *sqlTest = (DciText *)"select count(*) from SYS_TYPE where typname = 'BINARY'";
	ub4		count = 0;
	DCIDefine 	*def =NULL;

	if (connect(true))
	{
		printf("Connect failed\n");
		return DCI_ERROR;
	}

	if (DCIHandleAlloc(pEnv, (void**)&stmthp, DCI_HTYPE_STMT, 0, NULL))
	{
		printf("Failed to alloc stmt\n");
		return DCI_ERROR;
	}

	if (DCIStmtPrepare(stmthp, pError, sqlTest, strlen((char*)sqlTest), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("Test if kingbase support binary PREPARE failed.\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto end;
	}

	if (DCIDefineByPos(stmthp, &def, pError, 1, &count, sizeof(count), SQLT_INT, NULL, NULL, NULL, DCI_DEFAULT))
	{
		printf("Test if kingbase support binary DEFINE failed.\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto end;
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, 1, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("Test if kingbase support binary EXEC failed.\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto end;		
	}

	if (0 == count)
	{
		ret = DCI_SUCCESS;
		goto end;
	}

	if (DCIStmtPrepare(stmthp, pError, sqlSet, strlen((char*)sqlSet), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT) ||
		DCIStmtExecute(pSvcCtx, stmthp, pError, 0, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("set compatible_level failed: ");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}

	if (DCIStmtPrepare(stmthp, pError, sql, strlen((char*)sql), 0, 0))
	{
		printf("DCIStmtPrepare failed: ");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}

	if (DCIBindByName(stmthp, &bindhp, pError, (text *)":bina_",
		strlen(":bina_"), binaStr, sizeof(binaStr), 
		SQLT_BIN, (dvoid*)0, NULL, NULL, 0, NULL, DCI_DEFAULT))
	{
		printf("DCIBindByName failed: ");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4) 0, (DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4) DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed: ");
		report_error(pError);
		goto end;
	}
	
	if (strcmp("1a2b3c4d5e", binaStr))
	{
		printf("Data compare failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}
	
	//bind binary is NULL
	sql = (DciText*)"begin select cast(NULL as binary(30)) as bina_ into :bina_ from dual; end;";
	memset(binaStr, 0, sizeof(binaStr));

	if (DCIStmtPrepare(stmthp, pError, sqlSet, strlen((char*)sqlSet), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT) ||
		DCIStmtExecute(pSvcCtx, stmthp, pError, 0, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("set compatible_level failed: ");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}

	if (DCIStmtPrepare(stmthp, pError, sql, strlen((char*)sql), 0, 0))
	{
		printf("DCIStmtPrepare failed: ");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}

	if (DCIBindByName(stmthp, &bindhp, pError, (text *)":bina_",
		strlen(":bina_"), binaStr, sizeof(binaStr), 
		SQLT_BIN, (dvoid*)0, NULL, NULL, 0, NULL, DCI_DEFAULT))
	{
		printf("DCIBindByName failed: ");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4) 0, (DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4) DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed: ");
		report_error(pError);
		goto end;
	}
	
	if (strcmp("", binaStr))
	{
		printf("Data compare failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}

end:
	if (stmthp)
		DCIHandleFree(stmthp, DCI_HTYPE_STMT);

	disconnect(true);
	
	return ret;
}

//add by dwxiao, on 2014-6-11.
sword TestDatetime()
{
	text *querySQL = (text *)"select TS_COL, DT_COL FROM DT_TAB";
	text *procSQL = (text *)"select DT_PROC(NULL)";
	text *funcSQL = (text *)"select DT_FUNC()";
	text *insSQL = (text *)"insert into DT_TAB (TS_COL, DT_COL) values ( :ts, :dt )";
	
	char datetime_data[] = "2014-06-11 13:28:32";
	char datetime_out[sizeof(datetime_data)];
	char timestamp_out[sizeof(datetime_data)];
	DCIDateTime* timestamp = (DCIDateTime *)NULL;
	DCIDateTime* datetime1 = (DCIDateTime *)NULL;
	DCIDateTime* datetime2 = (DCIDateTime *)NULL;
	DCIDateTime* datetime3 = (DCIDateTime *)NULL;
	char datetime_vcs_out[sizeof(datetime_data)+2];
	char timestamp_vcs_out[sizeof(datetime_data)+2];

	char fmt[] = "YYYY-MM-DD HH24:MI:SS";
	ub4 len = 20;
	DCIBind  *bndLobp = NULL;

	DCIDefine *hDefine1 = (DCIDefine *) 0;
	DCIDefine *hDefine2 = (DCIDefine *) 0;
	DCIStmt	*pStmt = NULL;
	sword	ret = DCI_SUCCESS;
	DciText *sqlTest = (DciText *)"select count(*) from SYS_TYPE where typname = :1";
	ub4		count = 0;
	DCIDefine 	*def =NULL;
	char		*str_buf = NULL;
	DCIBind		*pBind = NULL;
	
	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
	/* Test if kingbase support datetime */
	if (DCIStmtPrepare(pStmt, pError, sqlTest, strlen((char*)sqlTest), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("Test if kingbase support datetime PREPARE failed.\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	str_buf=(char*)malloc(100);
	memset(str_buf,0,100);
	strcpy(str_buf,"datetime");
	err = DCIBindByPos(pStmt, &pBind, pError, 1, str_buf, (sb4)strlen(str_buf), SQLT_CHR, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (DCIDefineByPos(pStmt, &def, pError, 1, &count, sizeof(count), SQLT_INT, NULL, NULL, NULL, DCI_DEFAULT))
	{
		printf("Test if kingbase support datetime DEFINE failed.\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("Test if kingbase support datetime EXEC failed.\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;		
	}

	if (0 == count)
	{
		ret = DCI_SUCCESS;
		goto free_resource;
	}
	
	execute("CREATE OR REPLACE PROCEDURE \"PUBLIC\".\"DT_PROC\"(out dt datetime) AS BEGIN dt := '2014-06-11 13:28:32'; END;");
	execute("CREATE OR REPLACE FUNCTION \"PUBLIC\".\"DT_FUNC\"() RETURN datetime AS BEGIN return '2014-06-11 13:28:32'; END;");
	execute("drop table  IF EXISTS DT_TAB");
	execute("create table DT_TAB(TS_COL timestamp, DT_COL datetime)");
	//sprintf((char *)insSQL, "insert into DT_TAB (TS_COL, DT_COL) values ('%s', '%s')", datetime_data, datetime_data);
	//execute((char *)insSQL);
	
	err = DCIStmtPrepare(pStmt, pError, insSQL, (ub4) strlen((char *)insSQL),
		(ub4) DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByName(pStmt, &bndLobp, pError, (text *)":ts",
		strlen(":ts"), (dvoid *) datetime_data, sizeof(datetime_data), SQLT_STR, (dvoid *) 0,
		(ub2 *) 0, (ub2) 0, (ub4) 0, (ub4 *) 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
	err = DCIBindByName(pStmt, &bndLobp, pError, (text *)":dt",
		strlen(":dt"), (dvoid *) datetime_data, sizeof(datetime_data), SQLT_STR, (dvoid *) 0,
		(ub2 *) 0, (ub2) 0, (ub4) 0, (ub4 *) 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4) 1, (ub4) 0, 
		(DCISnapshot *) NULL, (DCISnapshot *) NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	/* function */
	err = DCIStmtPrepare(pStmt, pError, funcSQL, 
			(ub4)strlen((char*)funcSQL),
			(ub4) DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	/* SQLT_CHR */
	memset(datetime_out, 0, sizeof(datetime_data));
	hDefine1 = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &hDefine1, 
			pError, 1, (dvoid *)datetime_out, sizeof(datetime_data), 
			SQLT_CHR, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_CHR) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_CHR) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(datetime_out, datetime_data))
	{
		printf("Data compare (SQLT_CHR) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	/* SQLT_STR */
	memset(datetime_out, 0, sizeof(datetime_data));
	hDefine1 = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &hDefine1, 
			pError, 1, (dvoid *)datetime_out, sizeof(datetime_data), 
			SQLT_STR, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_CHR) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_CHR) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(datetime_out, datetime_data))
	{
		printf("Data compare (SQLT_CHR) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	/* SQLT_VCS */
	memset(datetime_vcs_out, 0, sizeof(datetime_data)+2);
	hDefine1 = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &hDefine1, 
			pError, 1, (dvoid *)datetime_vcs_out, sizeof(datetime_data)+2, 
			SQLT_VCS, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_CHR) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_CHR) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(datetime_vcs_out+2, datetime_data))
	{
		printf("Data compare (SQLT_CHR) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	/* SQLT_TIMESTAMP */
	memset(datetime_out, 0, sizeof(datetime_data));
	hDefine1 = (DCIDefine *) 0;
	
	err = DCIDescriptorAlloc(pEnv, (dvoid **)&datetime1, DCI_DTYPE_TIMESTAMP,
         0, (dvoid **)0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDescriptorAlloc (datetime) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt, &hDefine1, 
			pError, 1, (dvoid *)&datetime1, sizeof(datetime1), 
			SQLT_TIMESTAMP, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (datetime) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	len = 20;
	err = DCIDateTimeToText(pEnv, pError, datetime1, (DciText *)fmt, sizeof(fmt),
			0, NULL, 0, &len, (DciText *)datetime_out);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDateTimeToText failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(datetime_data, datetime_out))
	{
		printf("Data compare failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	/* procedure */
	err = DCIStmtPrepare(pStmt, pError, procSQL, 
			(ub4)strlen((char*)procSQL),
			(ub4) DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	/* SQLT_CHR */
	memset(datetime_out, 0, sizeof(datetime_data));
	hDefine1 = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &hDefine1, 
			pError, 1, (dvoid *)datetime_out, sizeof(datetime_data), 
			SQLT_CHR, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_CHR) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_CHR) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(datetime_out, datetime_data))
	{
		printf("Data compare (SQLT_CHR) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	/* SQLT_STR */
	memset(datetime_out, 0, sizeof(datetime_data));
	hDefine1 = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &hDefine1, 
			pError, 1, (dvoid *)datetime_out, sizeof(datetime_data), 
			SQLT_STR, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_CHR) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_CHR) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(datetime_out, datetime_data))
	{
		printf("Data compare (SQLT_CHR) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	/* SQLT_VCS */
	memset(datetime_vcs_out, 0, sizeof(datetime_data)+2);
	hDefine1 = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &hDefine1, 
			pError, 1, (dvoid *)datetime_vcs_out, sizeof(datetime_data)+2, 
			SQLT_VCS, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (SQLT_CHR) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt (SQLT_CHR) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(datetime_vcs_out+2, datetime_data))
	{
		printf("Data compare (SQLT_CHR) failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	/* SQLT_TIMESTAMP */
	memset(datetime_out, 0, sizeof(datetime_data));
	hDefine1 = (DCIDefine *) 0;
	
	err = DCIDescriptorAlloc(pEnv, (dvoid **)&datetime2, DCI_DTYPE_TIMESTAMP,
         0, (dvoid **)0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDescriptorAlloc (datetime) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt, &hDefine1, 
			pError, 1, (dvoid *)&datetime2, sizeof(datetime2), 
			SQLT_TIMESTAMP, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (datetime) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	len = 20;
	err = DCIDateTimeToText(pEnv, pError, datetime2, (DciText *)fmt, sizeof(fmt),
			0, NULL, 0, &len, (DciText *)datetime_out);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDateTimeToText failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(datetime_data, datetime_out))
	{
		printf("Data compare failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	/* SQL */
	err = DCIStmtPrepare(pStmt, pError, querySQL, 
			(ub4)strlen((char*)querySQL),
			(ub4) DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	/* SQLT_VCS */
	memset(datetime_vcs_out, 0, sizeof(datetime_data)+2);
	memset(timestamp_vcs_out, 0, sizeof(datetime_data)+2);

	hDefine1 = (DCIDefine *) 0;
	hDefine2 = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &hDefine1, 
			pError, 1, (dvoid *)timestamp_vcs_out, sizeof(datetime_data)+2, 
			SQLT_VCS, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (timestamp) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt, &hDefine2, 
			pError, 2, (dvoid *)datetime_vcs_out, sizeof(datetime_data)+2, 
			SQLT_VCS, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (datetime) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(timestamp_vcs_out, datetime_vcs_out))
	{
		printf("Data compare failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(datetime_data, datetime_vcs_out+2))
	{
		printf("Data compare failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	/* SQLT_CHR */
	memset(datetime_out, 0, sizeof(datetime_data));
	memset(timestamp_out, 0, sizeof(datetime_data));

	hDefine1 = (DCIDefine *) 0;
	hDefine2 = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &hDefine1, 
			pError, 1, (dvoid *)timestamp_out, sizeof(datetime_data), 
			SQLT_CHR, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (timestamp) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt, &hDefine2, 
			pError, 2, (dvoid *)datetime_out, sizeof(datetime_data), 
			SQLT_CHR, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (datetime) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(timestamp_out, datetime_out))
	{
		printf("Data compare failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(datetime_data, datetime_out))
	{
		printf("Data compare failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}
	
	/* SQLT_STR */
	memset(datetime_out, 0, sizeof(datetime_data));
	memset(timestamp_out, 0, sizeof(datetime_data));
	
	hDefine1 = (DCIDefine *) 0;
	hDefine2 = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &hDefine1, 
			pError, 1, (dvoid *)timestamp_out, sizeof(datetime_data), 
			SQLT_STR, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (timestamp) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt, &hDefine2, 
			pError, 2, (dvoid *)datetime_out, sizeof(datetime_data), 
			SQLT_STR, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (datetime) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(timestamp_out, datetime_out))
	{
		printf("Data compare failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(datetime_data, datetime_out))
	{
		printf("Data compare failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	/* SQLT_TIMESTAMP */
	memset(datetime_out, 0, sizeof(datetime_data));
	memset(timestamp_out, 0, sizeof(datetime_data));

	hDefine1 = (DCIDefine *) 0;
	hDefine2 = (DCIDefine *) 0;

	err = DCIDescriptorAlloc(pEnv, (dvoid **)&timestamp, DCI_DTYPE_TIMESTAMP,
         0, (dvoid **)0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDescriptorAlloc (timestamp) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
	err = DCIDescriptorAlloc(pEnv, (dvoid **)&datetime3, DCI_DTYPE_TIMESTAMP,
         0, (dvoid **)0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDescriptorAlloc (datetime) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt, &hDefine1, 
			pError, 1, (dvoid *)&timestamp, sizeof(timestamp), 
			SQLT_TIMESTAMP, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (timestamp) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt, &hDefine2, 
			pError, 2, (dvoid *)&datetime3, sizeof(datetime3), 
			SQLT_TIMESTAMP, NULL, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos pStmt (datetime) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
    err = DCIStmtExecute(pSvcCtx, pStmt, 
			pError, (ub4) 1, (ub4) 0, 
			(DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4)DCI_STMT_SCROLLABLE_READONLY);
	if (err == DCI_ERROR)
	{
		printf("DCIStmtExecute pStmt failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
	len = 20;
	err = DCIDateTimeToText(pEnv, pError, timestamp, (DciText *)fmt, sizeof(fmt),
			0, NULL, 0, &len, (DciText *)timestamp_out);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDateTimeToText failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	len = 20;
	err = DCIDateTimeToText(pEnv, pError, datetime3, (DciText *)fmt, sizeof(fmt),
			0, NULL, 0, &len, (DciText *)datetime_out);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDateTimeToText failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}
	
	if (strcmp(timestamp_out, datetime_out))
	{
		printf("Data compare failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

	if (strcmp(datetime_data, datetime_out))
	{
		printf("Data compare failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto free_resource;
	}

free_resource:	
	if(datetime1)
	{
		err = DCIDescriptorFree(datetime1,DCI_DTYPE_TIMESTAMP);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIDescriptorFree datetime1 failed\n");
		}
	}
	if(datetime2)
	{
		err = DCIDescriptorFree(datetime2,DCI_DTYPE_TIMESTAMP);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIDescriptorFree datetime2 failed\n");
		}
	}
	if(datetime3)
	{
		err = DCIDescriptorFree(datetime3,DCI_DTYPE_TIMESTAMP);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIDescriptorFree datetime3 failed\n");
		}
	}
	if(timestamp)
	{
		err = DCIDescriptorFree(timestamp,DCI_DTYPE_TIMESTAMP);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIDescriptorFree timestamp failed\n");
		}
	}
	if (pStmt)
	{
		err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIHandlefree pStmt failed\n");
		}
	}

	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

//add by dwxiao, datetime for anonymous block.
sword 
test_datetime_bind()
{
	DciText * sql = (DciText*)"begin select cast('2014-07-08 10:58:25' as datetime) as dt_ into :dt_ from dual; end;";
	DCIDateTime* datetime1 = (DCIDateTime *)NULL;
	DCIDateTime* datetime2 = (DCIDateTime *)NULL;
	char dtStr[100] = {1};
	char dtStr2[100] = {1};
	char dtStr3[100] = {1};
	DCIStmt *stmthp = NULL;
	sword	ret = DCI_ERROR;
	DCIBind		*bindhp = NULL;
	DciText *sqlSet = (DciText *)"set compatible_level=oracle";
	char fmt[] = "YYYY-MM-DD HH24:MI:SS";
	int len = 0;
	DciText *sqlTest = (DciText *)"select count(*) from SYS_TYPE where typname = 'DATETIME'";
	ub4		count = 0;
	DCIDefine 	*def =NULL;

	if (connect(true))
	{
		printf("Connect failed\n");
		return DCI_ERROR;
	}

	if (DCIHandleAlloc(pEnv, (void**)&stmthp, DCI_HTYPE_STMT, 0, NULL))
	{
		printf("Failed to alloc stmt\n");
		return DCI_ERROR;
	}

	/* Test if kingbase support datetime */
	if (DCIStmtPrepare(stmthp, pError, sqlTest, strlen((char*)sqlTest), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("Test if kingbase support datetime PREPARE failed.\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto end;
	}

	if (DCIDefineByPos(stmthp, &def, pError, 1, &count, sizeof(count), SQLT_INT, NULL, NULL, NULL, DCI_DEFAULT))
	{
		printf("Test if kingbase support datetime DEFINE failed.\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto end;
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, 1, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("Test if kingbase support datetime EXEC failed.\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto end;		
	}

	if (0 == count)
	{
		ret = DCI_SUCCESS;
		goto end;
	}

	if (DCIStmtPrepare(stmthp, pError, sqlSet, strlen((char*)sqlSet), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT) ||
		DCIStmtExecute(pSvcCtx, stmthp, pError, 0, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("set compatible_level failed: ");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}

	if (DCIStmtPrepare(stmthp, pError, sql, strlen((char*)sql), 0, 0))
	{
		printf("DCIStmtPrepare failed: ");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}
	
	/* SQLT_STR */
	if (DCIBindByName(stmthp, &bindhp, pError, (text *)":dt_",
		strlen(":dt_"), (dvoid *)&dtStr, sizeof(dtStr),
		SQLT_STR, (dvoid*)0, NULL, NULL, 0, NULL, DCI_DEFAULT))
	{
		printf("DCIBindByName failed: ");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4) 0, (DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4) DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed: ");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}

	if (strcmp("2014-07-08 10:58:25", dtStr))
	{
		printf("Data compare failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}
	
	/* SQLT_TIMESTAMP */
	if (DCIDescriptorAlloc(pEnv, (dvoid **)&datetime1, DCI_DTYPE_TIMESTAMP,
         0, (dvoid **)0))
	{
		printf("DCIDescriptorAlloc (datetime) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto end;
	}
	
	if(DCIDateTimeFromText(pEnv, pError, (DciText *)"1000-00-00 00:00:00", sizeof("1000-00-00 00:00:00"), 
			(DciText *)fmt, sizeof(fmt), NULL, 0, datetime1))
	{
		printf("DCIDateTimeToText (datetime) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto end;
	}
	
	bindhp = NULL;
	if (DCIBindByName(stmthp, &bindhp, pError, (text *)":dt_",
		strlen(":dt_"), (dvoid *)&datetime1, sizeof(datetime1),
		SQLT_TIMESTAMP, (dvoid*)0, NULL, NULL, 0, NULL, DCI_DEFAULT))
	{
		printf("DCIBindByName failed: ");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}
	
	if (DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4) 0, (DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4) DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed: ");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}
	
	memset(dtStr, 0, sizeof(dtStr));
	len = sizeof(dtStr);
	err = DCIDateTimeToText(pEnv, pError, datetime1, (DciText *)fmt, sizeof(fmt),
			0, NULL, 0, (ub4 *)&len, (DciText *)dtStr);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDateTimeToText failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto end;
	}

	if (strcmp("2014-07-08 10:58:25", dtStr))
	{
		printf("Data compare failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}

	//bind datetime value is NULL
	sql = (DciText*)"begin select cast(NULL as datetime) as dt_ into :dt_ from dual; end;";
	bindhp = NULL;

	if (DCIStmtPrepare(stmthp, pError, sqlSet, strlen((char*)sqlSet), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT) ||
		DCIStmtExecute(pSvcCtx, stmthp, pError, 0, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("set compatible_level failed: ");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}

	if (DCIStmtPrepare(stmthp, pError, sql, strlen((char*)sql), 0, 0))
	{
		printf("DCIStmtPrepare failed: ");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}
	
	/* SQLT_STR */
	if (DCIBindByName(stmthp, &bindhp, pError, (text *)":dt_",
		strlen(":dt_"), dtStr2, sizeof(dtStr2), 
		SQLT_STR, (dvoid*)0, NULL, NULL, 0, NULL, DCI_DEFAULT))
	{
		printf("DCIBindByName failed: ");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4) 0, (DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4) DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed: ");
		report_error(pError);
		goto end;
	}
	
	if (strcmp(dtStr3, dtStr2))
	{
		printf("Data compare failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}

	/* SQLT_TIMESTAMP */
	if (DCIDescriptorAlloc(pEnv, (dvoid **)&datetime2, DCI_DTYPE_TIMESTAMP,
         0, (dvoid **)0))
	{
		printf("DCIDescriptorAlloc (datetime) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto end;
	}
	
	if(DCIDateTimeFromText(pEnv, pError, (DciText *)"1000-00-00 00:00:00", sizeof("1000-00-00 00:00:00"), 
			(DciText *)fmt, sizeof(fmt), NULL, 0, datetime2))
	{
		printf("DCIDateTimeToText (datetime) failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto end;
	}
	
	bindhp = NULL;
	if (DCIBindByName(stmthp, &bindhp, pError, (text *)":dt_",
		strlen(":dt_"), (dvoid *)&datetime2, sizeof(datetime2),
		SQLT_TIMESTAMP, (dvoid*)0, NULL, NULL, 0, NULL, DCI_DEFAULT))
	{
		printf("DCIBindByName failed: ");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}
	
	if (DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4) 0, (DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4) DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed: ");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}
	
	memset(dtStr, 0, sizeof(dtStr));
	len = sizeof(dtStr);
	err = DCIDateTimeToText(pEnv, pError, datetime2, (DciText *)fmt, sizeof(fmt),
			0, NULL, 0, (ub4 *)&len, (DciText *)dtStr);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDateTimeToText failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto end;
	}

	if (strcmp("1000-01-01 00:00:00", dtStr))
	{
		printf("Data compare failed.\n");
		report_error(pError);
		ret  = DCI_ERROR;
		goto end;
	}
	
	ret = DCI_SUCCESS;
end:
	if(datetime1)
	{	
		err = DCIDescriptorFree(datetime1,DCI_DTYPE_TIMESTAMP);	
		if (err != DCI_SUCCESS)
		{		
			ret = DCI_ERROR;		
			printf("DCIDescriptorFree datetime1 failed\n");	
		}
	}
	if(datetime2)
	{	
		err = DCIDescriptorFree(datetime2,DCI_DTYPE_TIMESTAMP);	
		if (err != DCI_SUCCESS)
		{		
			ret = DCI_ERROR;		
			printf("DCIDescriptorFree datetime2 failed\n");	
		}
	}
	if (stmthp)
		DCIHandleFree(stmthp, DCI_HTYPE_STMT);

	disconnect(true);
	
	return ret;
}

static sword test_insert_t_inout()
{
	sword	ret = DCI_SUCCESS;
	DCIStmt			*pStmt = NULL;
	DCIBind			*pBind = NULL;
	DCIDefine		*pDefine = NULL;
	int             intVal = 5;
	char            str[] = "my test string";
	int             intRetVal[2] = {0};
	char            retRtr[512] = {0};
	int result_length = 0;
	char *sql = "INSERT INTO t_inout VALUES(:1, :2)";
	char *select_sql = (char *)"select * from t_inout where a = :1";

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}
	
	err = DCIBindByPos(pStmt, &pBind, pError, 1, &intVal, sizeof(int), SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	err = DCIBindArrayOfStruct(pBind, pError, sizeof(int), 0, 0, 0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIBindByPos failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 2, str, strlen(str), SQLT_CHR, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	err = DCIBindArrayOfStruct(pBind, pError, sizeof(str), 0, 0, 0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIBindByPos failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	
	//execute
	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, (const DCISnapshot *) NULL, (DCISnapshot *) NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtExecute failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	//select 
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) select_sql, (ub4) strlen((char*)select_sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 1, &intVal, sizeof(int), SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	err = DCIBindArrayOfStruct(pBind, pError, sizeof(int), 0, 0, 0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIBindByPos failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt, 
						&pDefine, 
						pError,
						1,
						(dvoid *) &intRetVal, 
						sizeof(int), 
						SQLT_INT, 
						(void *)0,
						(ub2 *)0,
						(ub2 *)0,
						DCI_DEFAULT);

	err = DCIDefineArrayOfStruct(pDefine, pError, sizeof(int), 0, 0, 0);

	err = DCIDefineByPos(pStmt,
						&pDefine,
						pError,
						2, 
						(void *) retRtr,
						256, 
						SQLT_CHR,
						(void *)0,
						(ub2 *)&result_length,
						(ub2 *)0,
						DCI_DEFAULT);

	err = DCIDefineArrayOfStruct(pDefine, pError, 256, 0, 0, 0);


	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtExecute failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

//	err = DCIStmtFetch(pStmt, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT);
//	if (ret == DCI_NO_DATA)
//		goto free_resource;


free_resource:
	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);

end:
	err = disconnect(true);
	return ret;
}

static sword test_new_data_type()
{
	sword	ret = DCI_SUCCESS;
	DCIStmt			*pStmt = NULL;
	DCIBind			*pBind = NULL;
	DCIDefine		*pDefine = NULL;
	char            bpcharbyte[256] = "bpcharbyte string";
	char            retBpcharbyte[256] = {0};
	char            varcharbyte[256] = "varcharbyte string";
	char            retVarcharbyte[256] = {0};
	//char            ora_date[] = "2020-02-22 13:28:32";
	char            ret_ora_date[256] = {0};
	DCIDateTime*    datetime = (DCIDateTime *)NULL;
	char			*ora_date = NULL;

	struct STimeInfo {
	unsigned short	nYear;
	unsigned short	nMonth;
	unsigned short	nDay;
	unsigned short	nHour;
	unsigned short	nMinute;
	unsigned short	nSecond;
	};
	struct STimeInfo	ProcDate;
	ProcDate.nYear = 2020;
	ProcDate.nMonth = 2;
	ProcDate.nDay = 22;
	ProcDate.nHour = 13;
	ProcDate.nMinute = 28;
	ProcDate.nSecond = 32;

	char fmt[] = "YYYY-MM-DD HH24:MI:SS";
	ub4				len = 20;

	int result_length = 0;
	char *sql = "INSERT INTO new_data_type VALUES(:1, :2, :3)";
	char *select_sql = (char *)"select * from new_data_type";

	ora_date = (char *)malloc(8);
	if (ora_date == NULL)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	else
		memset(ora_date, 0, 8);

	ora_date[0] = (char)(ProcDate.nYear / 100 + 100);
	ora_date[1] = (char)(ProcDate.nYear - (ProcDate.nYear / 100) * 100 + 100);
	ora_date[2] = (char)(ProcDate.nMonth);
	ora_date[3] = (char)(ProcDate.nDay);
	ora_date[4] = (char)(ProcDate.nHour + 1);
	ora_date[5] = (char)(ProcDate.nMinute + 1);
	ora_date[6] = (char)(ProcDate.nSecond + 1);

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 1, bpcharbyte, strlen(bpcharbyte), SQLT_CHR, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	err = DCIBindArrayOfStruct(pBind, pError, sizeof(bpcharbyte), 0, 0, 0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIBindByPos failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 2, varcharbyte, strlen(varcharbyte), SQLT_CHR, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	err = DCIBindArrayOfStruct(pBind, pError, sizeof(varcharbyte), 0, 0, 0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIBindByPos failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 3, (dvoid *)ora_date, 8, SQLT_DAT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	err = DCIBindArrayOfStruct(pBind, pError, 8, 0, 0, 0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIBindByPos failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	//execute
	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, (const DCISnapshot *) NULL, (DCISnapshot *) NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtExecute failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	//select 
	err = DCIStmtPrepare(pStmt, pError, (const DciText*) select_sql, (ub4) strlen((char*)select_sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt,
						&pDefine,
						pError,
						1,
						(dvoid *) retBpcharbyte,
						256,
						SQLT_CHR, 
						(void *)0,
						(ub2 *)0,
						(ub2 *)0,
						DCI_DEFAULT);

	err = DCIDefineArrayOfStruct(pDefine, pError, 256, 0, 0, 0);

	err = DCIDefineByPos(pStmt,
						&pDefine,
						pError,
						2,
						(dvoid *) retVarcharbyte,
						256,
						SQLT_CHR, 
						(void *)0,
						(ub2 *)0,
						(ub2 *)0,
						DCI_DEFAULT);

	err = DCIDefineArrayOfStruct(pDefine, pError, 256, 0, 0, 0);

	err = DCIDefineByPos(pStmt,
						&pDefine,
						pError,
						3,
						(dvoid *) ret_ora_date,
						256,
						SQLT_CHR, 
						(void *)0,
						(ub2 *)0,
						(ub2 *)0,
						DCI_DEFAULT);

	err = DCIDefineArrayOfStruct(pDefine, pError, 256, 0, 0, 0);

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtExecute failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtFetch(pStmt, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT);
	if (ret == DCI_NO_DATA)
		goto free_resource;

	/* SQLT_TIMESTAMP */
	memset(ret_ora_date, 0, sizeof(ret_ora_date));

	err = DCIDescriptorAlloc((dvoid *)pEnv, (dvoid **)&datetime, DCI_DTYPE_TIMESTAMP,
         0, (dvoid **)0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDescriptorAlloc (datetime) failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt,
						&pDefine,
						pError,
						3,
						(dvoid *)&datetime,
						sizeof(datetime),
						SQLT_TIMESTAMP,
						(void *)0,
						(ub2 *)0,
						(ub2 *)0,
						DCI_DEFAULT);

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtExecute failed\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtFetch(pStmt, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT);
	if (ret == DCI_NO_DATA)
		goto free_resource;

	len = 20;
	err = DCIDateTimeToText(pEnv, pError, datetime, (DciText *)fmt, sizeof(fmt) - 2,
			0, NULL, 0, &len, (DciText *)ret_ora_date);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDateTimeToText failed\n");
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	if(ora_date != NULL)
		free(ora_date);

	if(datetime)
	{
		err = DCIDescriptorFree(datetime,DCI_DTYPE_TIMESTAMP);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIDescriptorFree datetime failed\n");
		}
	}

	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);

end:
	err = disconnect(true);
	return ret;
}


int 
main(int argc, char* argv[])
{
	char strTemp[MAX_PATH_LEN], strBuf[MAX_PATH_LEN];
	int iLen = 0;

#ifdef WIN32
	DWORD dwRet = 0;
	char Buffer[MAX_PATH_LEN];
	char *p = NULL;
#endif

	if (ParseArg(argc, argv) != 0)
		return 0;

	/*bug#15667:dailybuild failed result from path error.*/
	memset(strTemp, 0, MAX_PATH_LEN);
	memset(gstrEXEpath, 0, MAX_PATH_LEN);
	strcpy(strTemp, argv[0]);
	iLen = strlen(argv[0]);
	strTemp[iLen + 1] = '\0';

#ifdef WIN32
	/*strTemp is "DciTest.exe" when current directory is Debug or Release in windows*/
	p = strchr(strTemp, '\\');
	if (p==NULL)
	{
		memset(Buffer, 0, MAX_PATH_LEN);
		dwRet = GetCurrentDirectory(MAX_PATH_LEN, Buffer);
		if (dwRet==0)
		{
			return DCI_ERROR;
		}
		iLen = strlen(Buffer);
		Buffer[iLen +1 ] = '\0';
		strcat(Buffer, "\\");
		strcat(Buffer, strTemp);
		strcpy(gstrEXEpath, Buffer);
	}
	else
	{
		strcpy(gstrEXEpath, strTemp);
	}
#else
	strcpy(gstrEXEpath, strTemp);
#endif

	memset(test_failed_list, 0, sizeof(test_failed_list));
	GetFilePath();
	memset(strBuf, 0, MAX_PATH_LEN);
	strncpy(strBuf, gstrPath, gCnt);
	strcat(strBuf, ResFile);
	fp = fopen(strBuf, "wb");
	if (fp == NULL)
		return DCI_ERROR;

//	goto NEW_TEST_2;
//	goto LOB_TEST;

	//multi threads test
	//test_multi_thread_test();
	//print_bar();

	// connect
	err = test_logon();
	check(err, "test_logon");

	print_bar();

	//test_logon2
	err = test_logon2();
	check(err, "test_logon2");

	print_bar();

	//test_sessionbegin2
	err = test_sessionbegin2();
	check(err, "test_sessionbegin2");

	print_bar();

	// connect
	err = test_sessionbegin();
	check(err, "test_sessionbegin");

	print_bar();

	err = test_ServerVersion();
	check(err, "test_ServerVersion");

	print_bar();
	
	//add by dwxiao, for datetime type, on 2014-6-11.
	err = TestDatetime();
	check(err, "TestDatetime");
	
	print_bar();

	err = TestNumberFetch();
	check(err, "TestNumberFetch");

	err = TestNumberInsert();
	check(err, "TestNumberInsert");

	err = TestNumberBatchInsert();
	check(err, "TestNumberBatchInsert");

    print_bar();

	err = TestDciNumberFromInt();
	check(err, "TestDciNumberFromInt");

	err = TestDciNumberToInt();
	check(err, "TestDciNumberToInt");

	err = TestDciNumberFromReal();
	check(err, "TestDciNumberFromReal");

	err = TestDciNumberToReal();
	check(err, "TestDciNumberToReal");

	err = TestDciNumberFromText();
	check(err, "TestDciNumberFromText");

	err = TestDciNumberToText();
	check(err, "TestDciNumberToText");

	err = TestDciNumber();
	check(err, "TestDciNumber");

	print_bar();

	err = test_DateTimeFromText();
	check(err, "test_DateTimeFromText");

	print_bar();

	err = TestCloseSvcSocket();
	check(err, "TestCloseSvcSocket");

	print_bar();

    //add by jjshi, on 2015-11-30
    err = TestXML();
    check(err, "TestXML");

    print_bar();

	//add by dwxiao, for binary type, on 2014-5-23.
	err = TestBinary();
	check(err, "TestBinary");

	print_bar();
	
	//add by dwxiao, binary type for anonymous block.
	err = test_binary_bind();
	check(err, "test_binary_bind");
	
	print_bar();
	
	//add by dwxiao, datetime type for anonymous block.
	err = test_datetime_bind();
	check(err, "test_datetime_bind");
	
	print_bar();

	err = test_nodata();
	check(err, "test_nodata");

	print_bar();

	err = test_describe_only();
	check(err, "test_describe_only");
	print_bar();

	err = test_execute_success_with_info();
	check(err, "test_execute_success_with_info");
	print_bar();

	err = test_fetch_success_with_info();
	check(err, "test_fetch_success_with_info");
	print_bar();

	err = test_fetch_last_prior();
	check(err,"test_fetch_last_prior");

	print_bar();

	err = test_execute("DROP TABLE  IF EXISTS T_TIMESTAMP");
	err = test_execute("CREATE TABLE T_TIMESTAMP(c1 date, c2 time, c3 timestamp(0), c4 timestamp, c5 interval year to month, c6 interval day to second)");
	check(err, "test_execute: create table T_TIMESTAMP");

	print_bar();

	// do insert
	err = test_insert_with_param("INSERT INTO T_TIMESTAMP VALUES(:0, :1, :2, :3, :4, :5)", "T_TIMESTAMP", 2);
	check(err, "test_insert_with_param: T_TIMESTAMP");

	print_bar();

	//do select
	err = test_select("select * from T_TIMESTAMP");
	check(err, "test_select: T_TIMESTAMP");

	// create table
	err = test_execute("DROP TABLE IF EXISTS T_INT_CHAR");
	err = test_execute("CREATE TABLE T_INT_CHAR(c1 smallint, c2 smallint, c3 int, c4 bigint, c5 numeric, c6 numeric(20, 0), c7 real, c8 float8 , c9 timestamp(0) , c10 char(14 byte), c11 char(14 char))");
	//err = test_execute("CREATE TABLE T_INT_CHAR(c1 char(14) )");
	check(err, "test_execute: create table T_INT_CHAR");

	print_bar();

	// do insert
	err = test_insert_with_param("INSERT INTO T_INT_CHAR VALUES(:0, :1, :2, :3, :4, :5, :6, :7, :8, :9, :10)", "T_INT_CHAR", 2);
	//err = test_insert_with_param("INSERT INTO T_INT_CHAR VALUES(:0)", "T_INT_CHAR", 2);
	check(err, "test_insert_with_param: T_INT_CHAR");

	print_bar();

	//do select
	err = test_select("select * from T_INT_CHAR");
	check(err, "test_select: T_INT_CHAR");

	print_bar();

	err = TestDirpath();
	check(err,"test dirpath");

	print_bar();

LOB_TEST:
	// create table t_lo
	err = test_execute("DROP TABLE IF EXISTS t_lob");
	err = test_execute("create table t_lob(b blob, c1 clob, c2 clob)");
	//err = test_execute("create table t_lob(b bytea, c1 text, c2 text)");
	check(err, "test_execute: create t_lob");

	err = test_insert_lo();
	check(err, "test_insert_lo");
	print_bar();

	err = test_select_lo();
	check(err, "test_select_lo");
	print_bar();

	err = test_execute("DROP TABLE IF EXISTS t_lob");
	err = test_execute("create table t_lob(b blob, c1 clob)");
	//err = test_execute("create table t_lob(b bytea, c1 text)");
	check(err, "test_execute: create t_lob");

	err = test_piece_write_lo();
	check(err, "test_piece_write_lo");
	print_bar();

	err = test_piece_read_lo();
	check(err, "test_piece_read_lo");
	print_bar();


	err = TestLobCopy();
	check(err, "TestLobCopy");
	print_bar();


	err = TestLobAssign();
	check(err, "TestLobAssign");
	print_bar();

	err = TestLobAppend();
	check(err, "TestLobAppend");
	print_bar();

	err = TestLobTrim();
	check(err, "TestLobTrim");
	print_bar();

	err = TestLobErase();
	check(err, "TestLobErase");
	print_bar();

	err = TestLobEnableBuffering();
	check(err, "TestLobEnableBuffering");
	print_bar();

	err = TestLobDisableBuffering();
	check(err, "TestLobDisableBuffering");
	print_bar();

	err = TestLobFlushBuffer();
	check(err, "TestLobFlushBuffer");
	print_bar();

	err = TestLobIsEqual();
	check(err, "TestLobIsEqual");
	print_bar();

	err = TestLobLocatorIsInit();
	check(err, "TestLobLocatorIsInit");
	print_bar();

	err = TestLobStream();
	check(err, "TestLobStream");
	print_bar();

	err = TestLobCallback();
	check(err, "TestCallbackLob");
	print_bar();

	err = TestLobCharSetId();
	check(err, "TestLobCharSetId");
	print_bar();

	err = TestLobCharSetForm();
	check(err, "TestLobCharSetForm");
	print_bar();

	err = TestTempLobs();
	check(err, "TestTempLobs");
	print_bar();

#if 0
	err = test_execute("DROP TABLE IF EXISTS new_data_type");
	err = test_execute("create table new_data_type(fbpcharbyte bpcharbyte, fvarcharbyte varcharbyte, fora_date date)");
	check(err, "test_execute: create new_data_type");

	err = test_new_data_type();
	check(err, "test_new_data_type");
	print_bar();
#endif

	err = TestPieceMultiColsMultiRows();
	check(err, "TestPieceMultiColsMultiRows");
	print_bar();

	err = test_Handle_Type();
	check(err, "test_Handle_Type");
	print_bar();


NEW_TEST:
	//Add test cases from C_INTERFACE DCI
	err = test_bug22774();
	check(err, "test_bug22774");
	print_bar();

	// do select count(*)
	err = test_select("select count(*) from T_INT_CHAR");
	check(err, "test_select: count(*)");
	print_bar();

	// do select max(char)
	err = test_select("select max(c10) from T_INT_CHAR");
	check(err, "test_select: max(c10)");
	print_bar();

	err = test_errcode_00942();
	check(err, "test_errcode_00942");
	print_bar();

	err = testReadData();
	check(err, "testReadData");
	print_bar();

	err = testConst();
	check(err, "testConst");

	err = test_with_as();
	check(err, "test_with_as");
	print_bar();

	err = testDirPathLoadStream_error();
	check(err, "testDirPathLoadStream_error");
	print_bar();

#if 0 /* Change 1 to 0 if add "UsePackage=1" into sys_service.conf */
	err = testProcedure();
	check(err, "testProcedure");
	print_bar();
#endif

	err = test_multi_execute();
	check(err, "test_multi_execute");
	print_bar();

	err = test_select_rows();
	check(err, "test_select_rows");
	print_bar();

	err = test_multiple_sql();
	check(err, "test_multiple_sql");
	print_bar();

	err = test_result_length();
	check(err, "test_result_length");
	print_bar();

	err = test_bug12869();
	check(err, "test_bug12869");
	print_bar();

//	err = test_dblink();
//	check(err, "test_dblink");
//	print_bar();

	err = test_PLAN_Exist();
	check(err, "test_PLAN_Exist");
	print_bar();

	err = test_bug13899();
	check(err, "test_bug13899");
	print_bar();

	err = test_bug14055();
	check(err, "test_bug14055");
	print_bar();

	err = test_TransStart();
	check(err, "test_TransStart");
	print_bar();

	err = test_bug14058();
	check(err, "test_bug14058");
	print_bar();

	err = test_union_all();
	check(err, "test_union_all");
	print_bar();

	err = TestNumberBatchFetch();
	check(err, "TestNumberBatchFetch");
	print_bar();

	err = test_begin_end();
	check(err, "test_begin_end");
	print_bar();

	err = test_UseDciDat_0();	/* add "UseDciDat=0" into sys_service.conf */
	check(err, "test_UseDciDat_0");
	print_bar();

#if 0
	err = test_UseDciDat_1();	/* add "UseDciDat=1" into sys_service.conf */
	check(err, "test_UseDciDat_1");

	print_bar();

	err = test_UseDciDat_2();	/* add "UseDciDat=2" into sys_service.conf */
	check(err, "test_UseDciDat_2");

	print_bar();
#endif

	err = test_bug14060();
	check(err, "test_bug14060");
	print_bar();


	//test_UTF16() will cause core dump, set test_utf16_run=0 temporarily
	test_utf16_run = 0;
	if (test_utf16_run)
	{
		//test demo set char_default_type to 'char';
		err = test_UTF16();
		check(err, "test_UTF16");
		print_bar();
	}


	err = test_bug15341();
	check(err, "test_bug15341");
	print_bar();

	err = test_DCIStmtFetch2();
	check(err, "test_DCIStmtFetch2");
	print_bar();

	err = test_FloatBindByInt();
	check(err, "test_FloatBindByInt");
	print_bar();

	err = test_select_end_colon_r_n();
	check(err, "test_select_end_colon_r_n");
	print_bar();

	err = test_bug22130();
	check(err, "test_bug22130");
	print_bar();

	err = test_bug22490();
	check(err, "test_bug22490");
	print_bar();

	err = test_bug22502();
	check(err, "test_bug22502");
	print_bar();

	err = test_bug22775();
	check(err, "test_bug22775");
	print_bar();

	err = TestDocCases();
	check(err, "TestDocCases");
	print_bar();

	err = test_bug23012();
	check(err, "test_bug23012");
	print_bar();

	err = test_bug23066();
	check(err, "test_bug23066");
	print_bar();

	err = test_bug23073();
	check(err, "test_bug23073");
	print_bar();

	err = test_bug24038();
	check(err, "test_bug24038");
	print_bar();


	err = test_bug24040();
	check(err, "test_bug24040");
	print_bar();

	err = test_bug24041();
	check(err, "test_bug24041");
	print_bar();

	err = TestDuration();
	check(err, "TestDuration");
	print_bar();

	err = test_bug23191();
	check(err, "test_bug23191");
	print_bar();

NEW_TEST_2:
#if 1	
	/************show pass but maybe fail begin*************/		
	err = test_bind_out_dml_returning_into();
	check(err, "test_bind_out_dml_returning_into");
	print_bar();	
	//test get out refcursor in anonymousblock
	err = test_bind_in_anonymous_block2();
	check(err, "test_bind_in_anonymous_block2");
	print_bar();

	err = test_out_refcursor_holdable();
	check(err, "test_out_refcursor_holdable");
	print_bar();

	err = test_trigger();
	check(err, "test_trigger");
	print_bar();

	err = test_begin_end2();
	check(err, "test_begin_end2");
	print_bar();

	err = test_DateTimeToText();
	check(err,"test_DateTimeToText");
	print_bar();

	err = test_bug14243();
	check(err, "test_bug14243");
	print_bar();
	/************show pass but maybe fail end*************/


	/******************failed****************************/
	err = test_bug23027();
	check(err, "test_bug23027");
	print_bar();

	test_package(); /* Change 0 to 1 if add "UsePackage=1" into sys_service.conf */
	print_bar();

	err = test_errcode_00001();
	check(err, "test_errcode_00001");
	print_bar();

	err = test_precision_scale();
	check(err, "test_precision_scale");
	print_bar();

	err = test_BIT();
	check(err, "test_BIT");
	print_bar();

	err = test_bug21728();
	check(err, "test_bug21728");
	print_bar();

	fclose(fp);
	err = compare_file();
	check(err, "compare_file");
	print_bar();

	err = test_bind_in_anonymous_block();
	check(err, "test_bind_in_anonymous_block");
	print_bar();

#endif


	printf("*************************\n");
	if (test_failed_num != 0)
	{
		int i = 0;
		printf("There are %d Number of tests failed\n", test_failed_num);
		for (; i < test_failed_num; i++)
		{
			printf("%s\n", test_failed_list[i]);
			free(test_failed_list[i]);
		}
	}
	else
		printf("ALL tests SUCCESS\n");
	printf("*************************\n");

#ifdef WIN32
	system("pause");
#endif

	return 0;
}


#define COL_NUM 1500
#define MAX_NAME_LEN 128
#define RECNUM 2000

typedef struct ColAttr{
	char	col_name[MAX_NAME_LEN + 1];
	ub2		data_type;			//DCI的数据类型
	ub4		data_size;			//数据的大小，以字节计算
	ub4		data_offset;		//注意，这个参数是留给DCI内部自己使用的
	ub2		precision;			//数据类型的精度
	ub1		scale;				//数据的刻度
	void	*data;				//注意，如果在使用行绑定的方式调用DirPathLoadStream，那么这个参数是没有作用
	//只有在以列绑定的形式调用DirPathLoadStreamByColumnBinds时，它才是指向该列的数据指针
	sb2		*indp;				//这个参数可以为NULL，表示该列的数据大小没有空值，如果要指定该列的某一行为NULL，那应该把数组的该行值指定为-1
	//在从DCI中获取结果集时，你需要判断该成员数组对应的位置值是否是-1，如果是，则表示该列在该行的值为NULL
}ColAttr_t;

int SqltLen(int sqlt, int len)
{
	int length = 0;

	switch (sqlt)
	{
	case 1:	//SQLT_CHAR DCI_CHAR
		length = len + 1;
		break;
	case 2:	//SQLT_ NUM DCI_NUM
		length = 22;
		break;
	case 3:	//SQLT_INT DCI_INT
		length = len;    
		break;
	case 4:	//SQLT_FLT DCI_FLT
		length = len;  
		break;
	case 5:	//SQLT_STR DCI_STR
		length = len + 1;
		break;
	case 6:	//SQLT_VNU
		length = len;
		break;
	case 8:	//SQLT_LNG
		length = 22;
		break;
	case 9:	//SQLT_VCS
		length = len + 1;
		break;
	case 12:	//SLQT_DAT
		length = 7+1;//7字节表示是ORACLE时间类型的长度，加1是为了取四字节对齐
		break;
	case 96:	//SLQT_AFC
		length = len +1;
		break;
	case 97:	//SLQT_AVC
		length = len +1;
		break;
	case 104://SLQT_RDD
		length = len +1;
		break;
	case 187: //SQLT_TIMESTAMP
		length = 32;
		break;
	default:
		length = -1;
		break;				
	}

	return length;
}

bool
ReadDataEx(const char *query, int top_number, int *rec_num, int *attr_num, struct ColAttr ** attrs, char **buf, ub4 *buf_size)
{
	int		i, reclen = 0,	col_num = 0, col_name_len = 0;
	ub4		counter;
	//	sb2		ind[COL_NUM];
	DCIDefine	*defhp[COL_NUM];
	DCILobLocator *lobp[COL_NUM];
	int lob_index = 0;
	char		*pdata = NULL;
	sb4	parm_status;
	DCIParam	*mypard = (DCIParam *) 0;
	ub2		dtype;
	ub4		col_width;  
	sb1     dscale;
	sb2		dprecision;
	DciText	*col_name = NULL;
	char *tempp=NULL,*tempp_last=NULL;
	int rows=0,last_rows=0;
	int g_AttrNum = 0; // 属性个数
	ColAttr_t *g_AryAttr = NULL;
	char *data_addr = NULL;
	sword err = DCI_SUCCESS;
	char *sql_temp = NULL;
	DCIStmt          *stmthp = NULL;
	DCIError           *pError = NULL;
	int blob_size = 0;

	bool ret = TRUE;

	int j=0;
	int temp_rows = 0;

	*rec_num = 0;
	*attr_num = 0;
	*attrs = NULL;
	*buf = NULL;
	*buf_size = 0;

	if (top_number == -1)
	{
		top_number = 0x7FFFFFFF;
	}

	err = DCIHandleAlloc(pEnv, (void**)&stmthp, DCI_HTYPE_STMT, 0, 0);
	err = DCIHandleAlloc((dvoid *)pEnv, (dvoid **)&pError, DCI_HTYPE_ERROR, (size_t)0, (dvoid **)0);

	if (sql_temp != NULL)
	{
		//在这里是针对达梦数据库的语法做的特殊处理，用来返回TOP前多少条的记录
		err = DCIStmtPrepare(stmthp, pError, (DciText *)sql_temp, (ub4)strlen(sql_temp), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
		free(sql_temp);
	}
	else
	{
		err = DCIStmtPrepare(stmthp, pError, (DciText *)query, (ub4)strlen(query), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	}	

	err = DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)0, (ub4)0, 
		(DCISnapshot *)NULL, (DCISnapshot *)NULL, 
		(ub4)DCI_DEFAULT);

	/* define GID*/	
	err = DCIAttrGet(stmthp, DCI_HTYPE_STMT, &col_num, 0, DCI_ATTR_PARAM_COUNT, pError);

	g_AryAttr = (struct ColAttr *)calloc(col_num, sizeof(ColAttr));

	g_AttrNum = 0;

	/* Request a parameter descriptor for position 1 in the select-list */
	counter = 1;
	parm_status = DCIParamGet((dvoid *)stmthp, DCI_HTYPE_STMT, pError, (dvoid **)&mypard, (ub4) counter);
	/* Loop only if a descriptor was successfully retrieved for
	* current position, starting at 1.
	*/
	reclen = 0;
	while (parm_status == DCI_SUCCESS) 
	{
		/* Retrieve the column name attribute */
		DCIAttrGet((dvoid*) mypard, (ub4) DCI_DTYPE_PARAM,
			(dvoid**) &col_name, (ub4 *) &col_name_len,
			(ub4) DCI_ATTR_NAME,(DCIError *) pError );

		/* Retrieve the datatype attribute */
		DCIAttrGet((dvoid*) mypard, (ub4) DCI_DTYPE_PARAM,
			(dvoid*) &dtype, (ub4 *) 0, (ub4) DCI_ATTR_DATA_TYPE,
			(DCIError *) pError);


		/* Retrieve the column width in bytes */
		DCIAttrGet((dvoid*) mypard, (ub4) DCI_DTYPE_PARAM,
			(dvoid*) &col_width,(ub4 *) 0, (ub4) DCI_ATTR_DATA_SIZE,
			(DCIError *) pError );

		if (dtype == SQLT_NUM)
		{	
			DCIAttrGet((dvoid*) mypard, (ub4) DCI_DTYPE_PARAM,
				(dvoid*) &dscale,(ub4 *) 0, (ub4) DCI_ATTR_SCALE,
				(DCIError *) pError );
			DCIAttrGet((dvoid*) mypard, (ub4) DCI_DTYPE_PARAM,
				(dvoid*) &dprecision,(ub4 *) 0, (ub4) DCI_ATTR_PRECISION,
				(DCIError *) pError );

		}

		/* added to attr struct */
		memset(g_AryAttr[g_AttrNum].col_name,0, MAX_NAME_LEN);
		strncpy(g_AryAttr[g_AttrNum].col_name, (char *)col_name, col_name_len);

		if ((dtype == SQLT_NUM) && (dprecision == 0) && (dscale == 0))
		{
			col_width = sizeof(float);
			dtype = SQLT_FLT;
		}
		else if ((dtype == SQLT_NUM) && (dprecision == 3) && (dscale == 0))
		{
			col_width = sizeof(short);
			dtype = SQLT_INT;
		}
		else if ((dtype == SQLT_NUM) && (dprecision == 5) && (dscale == 0))
		{
			col_width = sizeof(short);
			dtype = SQLT_INT;
		}
		else if ((dtype == SQLT_NUM) && (dprecision == 10) && (dscale == 0))
		{
			col_width = sizeof(int);
			dtype = SQLT_INT;
		}
		else if ((dtype == SQLT_NUM) && (dprecision == 19) && (dscale == 0))
		{
			col_width = sizeof(sb8);
			dtype = SQLT_INT;
		}
		else if ((dtype == SQLT_NUM) && col_width == 8)
		{
			col_width = sizeof(sb8);
			dtype = SQLT_INT;
		}
		else if ((dtype == SQLT_NUM) && (dprecision == 20) && (dscale == 0))
		{
			col_width = sizeof(sb8);
			dtype = SQLT_INT;
		}
		else if ((dtype == SQLT_NUM) && (dprecision == 7) && (dscale == 0))
		{
			col_width = sizeof(float);//4
			dtype = SQLT_FLT;
		}
		else if ((dtype == SQLT_NUM) && (dprecision == 15) && (dscale == 0))
		{
			col_width = sizeof(double);//8
			dtype = SQLT_FLT;
		}
		else if (((dtype == SQLT_NUM) && (dscale == 0)) || (dtype == SQLT_INT))
		{
			col_width = sizeof(int);
			dtype = SQLT_INT;
		}
		else if ((dtype == SQLT_NUM) && (dprecision == 22) && (dscale == 6))
		{
			col_width = sizeof(float);//4
			dtype = SQLT_FLT;
		}
		else if ((dtype == SQLT_NUM) && (dprecision == 38) && (dscale == 12))
		{
			col_width = sizeof(double);//8
			dtype = SQLT_FLT;
		}
		else if (((dtype == SQLT_NUM) && (dscale > 0)) || (dtype == SQLT_FLT))
		{
			col_width = sizeof(float);
			dtype = SQLT_FLT;
		}

		g_AryAttr[g_AttrNum].data_type = dtype;		
		if(dtype == SQLT_BLOB || dtype == SQLT_CLOB)
		{
			g_AryAttr[g_AttrNum].data_size = 8;
			lob_index++;
		}
		else
		{
			g_AryAttr[g_AttrNum].data_size = SqltLen(dtype, col_width);
		}
		reclen = reclen + g_AryAttr[g_AttrNum].data_size + sizeof(sb2);
		g_AttrNum++;

		DCIDescriptorFree((void *)mypard, (const ub4)DCI_DTYPE_PARAM);
		
		/* increment counter and get next descriptor, if there is one */
		counter++;
		parm_status = DCIParamGet((dvoid *)stmthp, DCI_HTYPE_STMT, pError,
			(dvoid **)&mypard, (ub4) counter);
	}
	// malloc space
	pdata = data_addr = (char*)malloc(RECNUM * reclen);
	if (!pdata)
	{
		ret = FALSE;
		goto free_resource;
	}
	memset((char *)data_addr,0, RECNUM*reclen);

	/* define */
	for (i = 1;i <= g_AttrNum;i++)
	{		
		if(g_AryAttr[i-1].data_type == SQLT_BLOB || g_AryAttr[i-1].data_type == SQLT_CLOB)
		{
			DCIDescriptorAlloc(pEnv, (dvoid **) &lobp[j],	(ub4) DCI_DTYPE_LOB, (size_t) 0, (dvoid **) 0);
			err = DCIDefineByPos(stmthp, &defhp[i-1], pError, i, &lobp[j], 4,
				g_AryAttr[i-1].data_type, pdata + g_AryAttr[i-1].data_size, 0, (ub2 *)0,DCI_DEFAULT);
			j++;
		}
		else
		{
			err = DCIDefineByPos(stmthp, &defhp[i-1], pError, i, (ub1 *)pdata, g_AryAttr[i-1].data_size,
				g_AryAttr[i-1].data_type, pdata + g_AryAttr[i-1].data_size,/*(dvoid *)*/0,(ub2 *)0,DCI_DEFAULT);	
		}	
		pdata = pdata + g_AryAttr[i-1].data_size + sizeof(sb2);
	}
	for (i = 1;i <= g_AttrNum;i++)
	{
		if (!(g_AryAttr[i-1].data_type == SQLT_BLOB || g_AryAttr[i-1].data_type == SQLT_CLOB))
		{
			DCIDefineArrayOfStruct(defhp[i-1], pError, reclen, reclen, 0, 0);
		}

	}

	temp_rows = top_number > RECNUM ? RECNUM : top_number;
	if (lob_index > 0)
	{
		temp_rows = 1;
	}
	err = DCIStmtFetch(stmthp, pError,temp_rows, DCI_FETCH_NEXT, DCI_DEFAULT);
	while (err == DCI_SUCCESS)
	{
		err = DCIAttrGet(stmthp, DCI_HTYPE_STMT, &rows, 0, DCI_ATTR_ROW_COUNT, pError);
		tempp = (char*)malloc(rows * reclen);
		if (!tempp)
		{
			ret = FALSE;
			goto free_resource;
		}
		if(tempp_last != NULL)
		{
			memcpy(tempp,tempp_last,last_rows*reclen);
			memcpy(tempp+last_rows*reclen,data_addr,(rows-last_rows)*reclen);
			free(tempp_last);
		}
		else
		{
			memcpy(tempp+last_rows*reclen,data_addr,(rows-last_rows)*reclen);
		}

		
		last_rows = rows;
		tempp_last = tempp;
		temp_rows = (top_number - last_rows) > RECNUM ? RECNUM : (top_number - last_rows);
		if(temp_rows == 0)
		{
			goto SUCCESS;	
		}
		if (lob_index > 0)
		{
			temp_rows = 1;
		}
		memset((char *)data_addr,0, RECNUM * reclen);
		err = DCIStmtFetch(stmthp, pError, temp_rows, DCI_FETCH_NEXT, DCI_DEFAULT);
	}

	err = DCIAttrGet(stmthp, DCI_HTYPE_STMT, &rows, 0, DCI_ATTR_ROW_COUNT, pError);
	if (last_rows < rows)
	{
		tempp = (char*)malloc(rows*reclen);
		if (!tempp)
		{
			ret = FALSE;
			goto free_resource;
		}
		if (tempp_last != NULL)
		{
			memcpy(tempp,tempp_last,last_rows*reclen);
			memcpy(tempp+last_rows*reclen,data_addr,(rows-last_rows)*reclen);
			free(tempp_last);
		}
		else
		{
			memcpy(tempp+last_rows*reclen,data_addr,(rows-last_rows)*reclen);
		}
	}
	else
	{
		tempp = tempp_last;
	}


SUCCESS:
	/*	if (lob_index > 0 && rows > 0)
	{	
	pblob_temp = pblob;
	//记算大字段的总长度
	while(pblob_temp != NULL)
	{
	blob_size += pblob_temp->len;
	pblob_temp = pblob_temp->next_row;
	}
	tempp_last = tempp;
	tempp = (char*)malloc(reclen*rows + blob_size);
	if (tempp == NULL)
	{
	SET_ERROR(error, NORMAL_ERR, "内存不足");
	SET_ERROR(error, NORMAL_ERR, "内存不足");
	free(tempp_last);
	RETURN_FALSE_RDE(DCI_ERROR);
	}
	memcpy(tempp, tempp_last, reclen*rows);
	char *data_offset = NULL;
	char *data_temp = NULL;
	data_temp = tempp + reclen*rows;
	pblob_temp = pblob;
	for (i=0; i<rows; i++)
	{
	data_offset = tempp + reclen * i;
	temp_blob = pblob_temp->blob_data;
	for (j=0; j<g_AttrNum; j++)
	{
	if (g_AryAttr[j].data_type == DCI_BLOB || g_AryAttr[j].data_type == DCI_CLOB)
	{
	if (temp_blob->null_flag == TRUE)
	{
	*(sb2*)(data_offset + g_AryAttr[j].data_size) = -1;
	}
	else
	{
	memcpy(data_temp, temp_blob->data, temp_blob->len);
	*(ub4*)data_offset = data_temp - (tempp + reclen*rows);
	*(ub4*)(data_offset + sizeof(ub4)) = temp_blob->len;
	data_temp += temp_blob->len;
	}
	temp_blob = temp_blob->next_col;
	}
	data_offset += g_AryAttr[j].data_size + sizeof(sb2);
	}
	pblob_temp = pblob_temp->next_row;
	}
	free(tempp_last);
	}
	*/
	*attr_num = g_AttrNum;
	*attrs = g_AryAttr;
	*rec_num = rows;
	*buf = tempp;
	*buf_size = reclen * rows + blob_size;

free_resource:
	if (data_addr)
		free(data_addr);

	DCIHandleFree((dvoid *)stmthp, DCI_HTYPE_STMT);	
	err = DCITransCommit(pSvcCtx, pError, 0);
	if (err != DCI_SUCCESS)
		ret = FALSE;
	DCIHandleFree((dvoid *)pError, DCI_HTYPE_ERROR);
	return ret;
}

static int 
testReadData()
{
	char *query = "select to_timestamp('2009-10-10 10:10:00', 'yyyy-mm-dd hh:mi:ss'), v0, v1, v2, v3, v4, v5 from t_readdataex where a = 1234567890";
	int top_number = -1;
	int rec_num = 0, attr_num = 0;

	ColAttr *attrs = NULL;
	char *buf = NULL;
	ub4 buf_size = 0;
	int i, j;
	sword ret = DCI_SUCCESS;
	int offset = 0;

	test_execute("drop table if exists t_readdataex");
	test_execute("create table t_readdataex(a bigint, v0 real, v1 real, v2 real, v3 real, v4 real, v5 real);");
	test_execute("insert into t_readdataex values(1234567890, 0, 123.456, 0, NULL, 0, 0);");

	err = connect(true);

	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	ReadDataEx(query, top_number, &rec_num, &attr_num, &attrs, &buf, &buf_size);

	for (i = 0; i < attr_num; i++)
	{
		printf("col_name: %s data_type: %d data_size: %d precision: %d scale: %d\n"
			, attrs[i].col_name, attrs[i].data_type, attrs[i].data_size, attrs[i].precision, attrs[i].scale);
	}

	printf("LENGTH\tVALUE\n*********************\n");
	for (j = 0; j < rec_num; j++)
	{
		for (i = 0; i < attr_num; i++)
		{
			sb2 len = *(sb2 *)(buf + offset + attrs[i].data_size);

			if (len == -1)
				printf("NULL\n");
			else
			{
				printf("%d", len);

				if (attrs[i].data_type == SQLT_FLT)
				{
					float tmp = 0;
					memcpy(&tmp, (buf + offset), sizeof(float));
					/* bug#17069 */
					/* printf("\t%f" , *(float *)(buf + offset)); */
					printf("\t%f" , tmp);
				}
				else if (attrs[i].data_type == SQLT_DAT)
				{
					ub1 *pDate = (ub1 *) (buf + offset);
					sb2 year = (*pDate - 100) * 100 + (*(pDate+1) - 100);

					printf("\t%04d-%02d-%02d %02d:%02d:%02d", 
						year, *(pDate+2), *(pDate+3), *(pDate+4)-1, *(pDate+5)-1, *(pDate+6)-1);
				}

				printf("\n");
			}

			offset += attrs[i].data_size + sizeof(sb2);
		}
	}

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	if(attrs != NULL)
	{
		free(attrs);
		attrs = NULL;
	}
	if(buf != NULL)
	{
		free(buf);
		buf = NULL;
	}
	return ret;
}

static void
usage()
{
	printf("\nUsage:\n"
		"    [OPTIONS]\n\n"
		"General options:\n"
		"    --help  Print this message\n\n"
		"Connection options:\n"
		"    -n ServiceName  DCI Service Name (default: \"KingbaseES\")\n"
		"    -U NAME         KingbaseES user name (default: \"system\")\n"
		"    -W PASSWORD     KingbaseES user password (default: \"MANAGER\")\n"
		"\n");
}

static int
ParseArg(int argc, char* argv[])
{
	int i = 1;
#if 0
//replace bigendian with BYTE_ORDER
	/*
	 * is this a bigendian machine ?
	 */
	union
	{ 
		long l; 
		char c[sizeof (long)]; 
	} u;

	u.l = 1; 
	if (u.c[sizeof (long) - 1] == 1)
		bigendian = 1;
#endif

	while (i < argc)
	{
		if (!strcmp("-n", argv[i]))
		{
			if (i < argc - 1 && '-' != *argv[++i])
				snprintf(DbName, sizeof(DbName), "%s", argv[i++]);
			else
				goto usage;
		}
		else if (!strcmp("-U", argv[i]))
		{
			if (i < argc - 1 && '-' != *argv[++i])
				snprintf(User, sizeof(User), "%s", argv[i++]);
			else
				goto usage;
		}
		else if (!strcmp("-W", argv[i]))
		{
			if (i < argc - 1 && '-' != *argv[++i])
				snprintf(Pwd, sizeof(Pwd), "%s", argv[i++]);
			else
				goto usage;
		}
		else
			goto usage;
	}

	if (ascii_to_utf16((char *)User, User_UTF16) != 0
		|| ascii_to_utf16((char *)Pwd, Pwd_UTF16) != 0
		|| ascii_to_utf16((char *)DbName, DbName_UTF16) != 0)
	{
		printf("Can't transform connection string to UTF16, test_utf16 will not execute\n");
		test_utf16_run = false;
	}

	return 0;

usage:
	usage();

	return -1;
}

sword
compare_file()
{
	char	bufSrc[BLOCK_SIZE + 1] = "", bufDst[BLOCK_SIZE + 1] = "";
	int		rdSrc = 0,
			rdDst = 0;
	FILE	*fSrc = NULL,
			*fDst = NULL;
	sword ret = DCI_SUCCESS;
	
	/* bug#15667:dailybuild failed result from path error.*/
	char strExpectedPath[MAX_PATH_LEN],strResultPath[MAX_PATH_LEN];

	memset(strExpectedPath, 0, MAX_PATH_LEN);
	strncpy(strExpectedPath, gstrPath, gCnt);
	memset(strResultPath, 0, MAX_PATH_LEN);
	strncpy(strResultPath, gstrPath, gCnt);
	strcat(strExpectedPath, "expected.txt");
	fSrc = fopen(strExpectedPath, "rb");
	if (!fSrc)
		return DCI_ERROR;

	strcat(strResultPath, "result.txt");
	fDst = fopen(strResultPath, "rb");
	if (!fDst)
		return DCI_ERROR;

	while (!feof((FILE *)fDst) && !feof((FILE *)fSrc))
	{
		rdDst = (int)fread((void *)bufDst, sizeof(char), BLOCK_SIZE, (FILE *)fDst);
		if( ferror((FILE *)fDst) != 0 )
		{
			ret =  DCI_ERROR;
			break;
		}
		rdSrc = (int)fread((void *)bufSrc, sizeof(char), BLOCK_SIZE, (FILE *)fSrc);
		if( ferror((FILE *)fSrc) != 0 )
		{
			ret =  DCI_ERROR;
			break;
		}

		if(rdDst == rdSrc)
		{
			if( memcmp((void *)bufDst,(void *)bufSrc, sizeof(char)*rdDst)!=0 )
			{
				ret =  DCI_ERROR;
				break;
			}

		}
		else
		{
			ret =  DCI_ERROR;
			break;
		}
	}
	if(NULL != fSrc)
		fclose(fSrc);
	if(NULL != fDst)
		fclose(fDst);
	return ret;
}
