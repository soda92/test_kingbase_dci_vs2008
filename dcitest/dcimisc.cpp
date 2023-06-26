#include "DciTest.h"
#include "dcimisc.h"

#ifdef WIN32
#include <process.h>
#else
#include <pthread.h>
#endif

#define SQL_LEN 1024
#define MAXDATELEN 64
#define STR_LEN 255
#define EXECUTECOUNT 5
#define THREADCOUNT 20

sword test_delete();
sword test_A();
sword test_B();
sword test_C();
sword test_D();
sword test_E();
sword test_F();
sword test_G();
sword test_H();
sword test_result(int *exp_A, int *exp_B);

sword test_ServerVersion()
{
	sword	ret = DCI_SUCCESS;

	void*		hndlp_context[4];
	DCIError*	errhp[2];
	DciText		buf[256];
	ub4			size[] = {5, 256};
	ub4			hndltype[] = 
				{
					(ub4)NULL,
					DCI_HTYPE_ENV,
					DCI_HTYPE_ERROR,
					DCI_HTYPE_STMT,
					DCI_HTYPE_BIND,
					DCI_HTYPE_DEFINE,
					DCI_HTYPE_DESCRIBE,
					DCI_HTYPE_SERVER,
					DCI_HTYPE_SESSION
				};

	ub4			i = 0, j = 0, k = 0, m = 0;
	

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return err;	
	}

	hndlp_context[0] = NULL;
	hndlp_context[1] = pEnv;
	hndlp_context[2] = pError;

	errhp[0] = NULL;
	errhp[1] = pError;

	for (i = 0; i < sizeof(hndlp_context) /sizeof(hndlp_context[0]) ; i++)
	{
		if (i == 3)
		{
			err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &hndlp_context[3], DCI_HTYPE_STMT, 0, NULL);
			if (err != DCI_SUCCESS)
			{
				printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
				return disconnect(true);	
			}

		}
		else
		{
			for (j = 0; j < sizeof(errhp) / sizeof(errhp[0]); j++)
			{
				for (k = 0; k < sizeof(size) / sizeof(size[0]); k++)
				{
					for (m = 0; m < sizeof(hndltype) /sizeof(hndltype[0]) ; m++)
					{
						memset(buf, 0, sizeof(buf));

						err = DCIServerVersion(hndlp_context[i], 
												errhp[j], 
												buf, 
												(ub4)size[k], 
												(ub1)hndltype[m]);
						
						if (errhp[j] != NULL && hndltype[m] == DCI_HTYPE_SVCCTX && hndlp_context[i] == pSvcCtx)
						{
							if (err != DCI_SUCCESS || strlen((char *)buf) == 0)
							{
								ret = DCI_ERROR;
								printf("test DCIServerVersion Failed\n");
							}
							else
							{
								printf("test DCIServerVersion, ServerVersion : %s\n", buf);
							}
						}
						else if (err == DCI_SUCCESS)
						{
							ret = DCI_ERROR;
							printf("test DCIServerVersion Failed\n");
						}
					}
				}
			}

		}
		if (i == 3)
		{
			err = DCIHandleFree((dvoid *) hndlp_context[3], (ub4) DCI_HTYPE_STMT); 
			if (err != DCI_SUCCESS)
			{
				ret = DCI_ERROR;
				printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
			}
		}
	}

	for (i = 0; i < sizeof(size)/sizeof(size[0]); i++)
	{
		err = DCIServerVersion(pSvcCtx, 
			pError, 
			buf, 
			size[i], 
			DCI_HTYPE_SVCCTX);
		if (err != DCI_SUCCESS)
			ret = DCI_ERROR;
		else
			printf("test DCIServerVersion, ServerVersion with buffer size %d: %s\n", size[i], buf);
	}
	
	err = disconnect(true);
	return ret;
}

sword
testConst()
{
	err = test_select("select '', 'abc', 0 from dual;");

	return err;
}

// bug#24103: support : with...as
sword
test_with_as()
{
	err = test_select_no_print("with a as (select * from dual) select * from a;");

	return err;
}
// bug#12697: fix the 8th problem.
sword
testDirPathLoadStream_error()
{
	sword			ret = DCI_SUCCESS;
	DCIDirPathCtx	*dpctx = NULL;
	char			*table = "TESTDIRPATHLOADSTREAM";
	char			*schema = "TEST_DIRPATHLOADSTREAM";
	char			buf[SQL_LEN];
	char			dfltdatemask_tbl[100] = "YYYY-MM-DD HH24:MI:SS";
	ub1				dirpathinput = DCI_DIRPATH_INPUT_STREAM; //DCI_DIRPATH_INPUT_TEXT
	/* bug#17069:should be ub2 */
	/* int				cols = 1; */
	ub2				cols = 1;
	DCIParam		*colLstDesc = NULL, *colDesc = NULL;
	char			*col_name = "TEST1";
	DCIDirPathStream	*dpstr = NULL;
	int				buf_size = 64 *1024;

	memset(buf, 0, SQL_LEN);
	sprintf(buf, "DROP USER if exists test_DirPathLoadStream");
	err = test_execute(buf);

	memset(buf, 0, SQL_LEN);
	sprintf(buf, "DROP SCHEMA IF EXISTS %s CASCADE;", schema);
	err = test_execute(buf);

	memset(buf, 0, SQL_LEN);
	sprintf(buf, "CREATE USER test_DirPathLoadStream");
	err = test_execute(buf);

	memset(buf, 0, SQL_LEN);
	sprintf(buf, "CREATE SCHEMA %s AUTHORIZATION test_DirPathLoadStream;", schema); 
	err = test_execute(buf);

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;
	
	err = DCIHandleAlloc((void *)pEnv, (void **)&dpctx, DCI_HTYPE_DIRPATH_CTX, (size_t)0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIAttrSet((void *)dpctx, (ub4)DCI_HTYPE_DIRPATH_CTX, (dvoid *)table, (ub4)strlen(table), (ub4)DCI_ATTR_NAME, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource_ctx;
	}

	err = DCIAttrSet((dvoid *)dpctx, (ub4)DCI_HTYPE_DIRPATH_CTX,
		(dvoid *)schema,
		(ub4)strlen((const char *)schema),
		(ub4)DCI_ATTR_SCHEMA_NAME, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource_ctx;
	}

	err = DCIAttrSet((dvoid *)dpctx, (ub4)DCI_HTYPE_DIRPATH_CTX,
		(dvoid *)dfltdatemask_tbl,
		(ub4)strlen((const char *)dfltdatemask_tbl),
		(ub4)DCI_ATTR_DATEFORMAT, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource_ctx;
	}

	err = 	DCIAttrSet ((dvoid *)dpctx, DCI_HTYPE_DIRPATH_CTX,
			(dvoid *)&dirpathinput, (ub4)0,
			DCI_ATTR_DIRPATH_INPUT, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource_ctx;
	}

	DCIAttrSet((dvoid *)dpctx, (ub4)DCI_HTYPE_DIRPATH_CTX,
		(dvoid *)&cols,
		(ub4)0, (ub4)DCI_ATTR_NUM_COLS, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource_ctx;
	}

	err = DCIAttrGet((dvoid *)dpctx,
		DCI_HTYPE_DIRPATH_CTX,
		(dvoid *)&colLstDesc, (ub4 *)0,
		DCI_ATTR_LIST_COLUMNS, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource_ctx;
	}

	err = DCIParamGet((CONST dvoid *)colLstDesc,
		(ub4)DCI_DTYPE_PARAM, pError,
		(dvoid **)&colDesc, 1);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource_ctx;
	}

	err = DCIAttrSet((dvoid *)colDesc, (ub4)DCI_DTYPE_PARAM,
			(dvoid *)col_name,
			(ub4)strlen(col_name),
			(ub4)DCI_ATTR_NAME, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource_ctx;
	}

	err = DCIDirPathPrepare(dpctx, pSvcCtx, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource_ctx;
	}

	err = DCIAttrSet((dvoid *)dpctx, (ub4)DCI_HTYPE_DIRPATH_CTX,
		(dvoid *)&buf_size,
		(ub4)0, (ub4)DCI_ATTR_BUF_SIZE, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource_ctx;
	}

	err = DCIHandleAlloc((dvoid *)dpctx,(dvoid**)&dpstr,
		(ub4)DCI_HTYPE_DIRPATH_STREAM,
		(size_t)0, (dvoid **)0);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource_ctx;
	}

	err = DCIDirPathStreamReset(dpstr, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource_stream;
	}

	err = DCIDirPathLoadStream(dpctx, dpstr, pError);
	if (err != DCI_SUCCESS)
	{
		sb4 errcode;
		DciText sqlstate[6];
		DciText errmsg[SQL_LEN];

		err = DCIErrorGet((void *)pError, 1, sqlstate, &errcode, errmsg, (ub4)SQL_LEN, (ub4)DCI_HTYPE_ERROR);
		if (errcode != 942 || strcmp((char *)sqlstate, "42P01") != 0)
			ret = DCI_ERROR;
	}
	else
	{
		ret = DCI_ERROR;
		goto free_resource_stream;
	}

free_resource_stream:
	err = DCIHandleFree((void *)dpstr, DCI_HTYPE_DIRPATH_STREAM);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

free_resource_ctx:
	err = DCIHandleFree((void *)dpctx, DCI_HTYPE_DIRPATH_CTX);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;
	
end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	memset(buf, 0, SQL_LEN);
	sprintf(buf, "DROP SCHEMA %s CASCADE;", schema);
	err = test_execute(buf);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	memset(buf, 0, SQL_LEN);
	sprintf(buf, "DROP USER test_DirPathLoadStream;");
	err = test_execute(buf);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;
	
	return ret;
}

// bug#12697: fix the 1th problem.
sword
testProcedure()
{
	sword		ret = DCI_SUCCESS;
	DCIDescribe	*pDescribe;
	char		*proc1 = "schema_proc1";
	DCIParam	*param1 = NULL, *sub_param = NULL;
	ub2			num_sub = 0, num_params = 0;
	DCIParam	*routine = NULL;
	DCIParam	*param_list = NULL;

	err = test_execute("DROP USER schema_proc1");
	err = test_execute("DROP USER schema_proc2");
	err = test_execute("DROP SCHEMA schema_proc1 CASCADE");
	err = test_execute("DROP SCHEMA SCHEMA_PROC2 CASCADE");
	err = test_execute("CREATE USER schema_proc1");
	err = test_execute("CREATE USER schema_proc2");
	err = test_execute("CREATE SCHEMA schema_proc1 authorization schema_proc1");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;
	err = test_execute("CREATE SCHEMA SCHEMA_PROC2 authorization schema_proc2");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	err = test_execute("DROP TABLE TABLE_PROCEDURE");
	err = test_execute("CREATE TABLE TABLE_PROCEDURE(a INT)");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	err = test_execute("CREATE OR REPLACE PROCEDURE schema_proc1.TEST_PROCEDURE(IN a int, IN b int)"
		" AS BEGIN DELETE FROM TABLE_PROCEDURE; END;");
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = test_execute("CREATE OR REPLACE PROCEDURE SCHEMA_PROC2.TEST_PROCEDURE(IN a int, IN b int, In c int)"
		" AS BEGIN DELETE FROM TABLE_PROCEDURE; END;");
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pDescribe, DCI_HTYPE_DESCRIBE, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((void *)pEnv, (void **)&param1, DCI_DTYPE_PARAM, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDescribeAny(pSvcCtx, pError, (void *)proc1, (ub4)strlen((char *)proc1), DCI_OTYPE_NAME, DCI_DEFAULT, DCI_PTYPE_PKG, pDescribe);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)pDescribe, DCI_HTYPE_DESCRIBE, (void *)&param1, (ub4 *)0, (ub4)DCI_ATTR_PARAM, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)param1, (ub4)DCI_DTYPE_PARAM, (void *)&sub_param, (ub4 *)0, (ub4) DCI_ATTR_LIST_SUBPROGRAMS, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)sub_param, (ub4)DCI_DTYPE_PARAM, &num_sub, (ub4 *)0, (ub4)DCI_ATTR_NUM_PARAMS, pError);
	if (err != DCI_SUCCESS || num_sub != 1)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIParamGet((void *)sub_param, DCI_DTYPE_PARAM, pError, (void **)&routine, (ub4)0);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)routine, DCI_DTYPE_PARAM, (void *)&param_list, (ub4 *)NULL, (ub4)DCI_ATTR_LIST_ARGUMENTS, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)param_list, (ub4)DCI_DTYPE_PARAM, &num_params, (ub4 *)0, (ub4)DCI_ATTR_NUM_PARAMS, pError);
	if (err != DCI_SUCCESS || num_params != 2)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((void *)pDescribe, (ub4)DCI_HTYPE_DESCRIBE);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;
	
	err = test_execute("DROP PROCEDURE schema_proc1.TEST_PROCEDURE");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;
	err = test_execute("DROP PROCEDURE SCHEMA_PROC2.TEST_PROCEDURE");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;
	err = test_execute("DROP SCHEMA schema_proc1 CASCADE");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;
	err = test_execute("DROP SCHEMA SCHEMA_PROC2 CASCADE");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;
	err = test_execute("DROP USER schema_proc1");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;
	err = test_execute("DROP USER schema_proc2");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

// bug#12697: fix the 5th problem.
sword
test_multi_execute()
{
	DCIStmt *pStmt;
	DCIBind	*pBind;
	DCIDefine *pDefine;

	char	exec_buf[SQL_LEN];
	sword	ret = DCI_SUCCESS;

	int row_count = 0;

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "drop table if exists th_test;");
	err = test_execute(exec_buf);
	
	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "create table th_test(th_id int  not null, th_name varchar(64) not null, th_age DOUBLE not null,"
		"th_time time(3) not null,th_long NUMERIC(22,0))");
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	char *ptr = (char *) malloc (72 * 1000);
	if (!ptr)
		return DCI_ERROR;
	memset(ptr, 0, 72 * 1000);
	char *str = ptr;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (ub4)DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "insert into th_test values(:1, :5, :3, '04:08', 0);");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	for (int i = 0; i < 1000; i++)
	{
		*(int *)ptr = i;
		ptr += 4;
		*(float *)ptr = (float)i;
		ptr +=4;
		strcpy(ptr, "abcdefghi ");

		ptr += 64;
	}

	ptr = str;

	err = DCIBindByPos(pStmt, &pBind, pError, 1, ptr, sizeof(int), SQLT_INT, (void *)0, (ub2 *)0, 
		(ub2 *)0, (ub4)0,(ub4 *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	err = DCIBindArrayOfStruct(pBind, pError, 72, 0, 0, 0);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	err = DCIBindByPos(pStmt, &pBind, pError, 2, ptr + 8, 64, SQLT_VCS, (void *)0, (ub2 *)0, 
		(ub2 *)0, (ub4)0,(ub4 *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	err = DCIBindArrayOfStruct(pBind, pError, 72, 0, 0, 0);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 3, ptr + 4, sizeof(float), SQLT_FLT, (void *)0, (ub2 *)0, 
		(ub2 *)0, (ub4)0,(ub4 *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	err = DCIBindArrayOfStruct(pBind, pError, 72, 0, 0, 0);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1000, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}


	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1000, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1000, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIHandleFree((void *)pStmt, (ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}
	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (ub4)DCI_HTYPE_STMT, 0, (void **)NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		free(ptr);
		goto end;
	}	

	memset(exec_buf, 0, SQL_LEN);
	strcpy(exec_buf, "select count(*) from th_test;");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt, &pDefine, pError, 1, (void *)&row_count, (sb4)sizeof(int), (ub2)SQLT_INT, (void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtFetch(pStmt, pError, (ub4)1, (ub2)DCI_FETCH_NEXT, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (row_count != 3000)
		ret = DCI_ERROR;

free_resource:
	free(ptr);

	err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "drop table th_test;");
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

// bug#12697: fix the 6th problem.
sword
test_select_rows()
{
	DCIStmt	*pStmt = NULL;

	char	exec_buf[SQL_LEN];
	sword	ret = DCI_SUCCESS;

	int		select_rows = 0;

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "drop table if exists test_select;");
	err = test_execute(exec_buf);

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "create table test_select (a int)");
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (ub4)DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "insert into test_select values(1);");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIHandleFree((void *)pStmt, (ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}
	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (ub4)DCI_HTYPE_STMT, 0, (void **)NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "select * from test_select;");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtFetch(pStmt, pError, (ub4)3, (ub2)DCI_FETCH_NEXT, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS && err != DCI_NO_DATA)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	// test rows fetched
	err = DCIAttrGet((void *)pStmt, (ub4)DCI_HTYPE_STMT, (void *)&select_rows, (ub4 *)0, (ub4)DCI_ATTR_ROWS_FETCHED, pError);
	if (select_rows != 2)
		ret = DCI_ERROR;

	err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "drop table test_select;");
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

sword
test_precision_scale()
{
	sword ret = DCI_SUCCESS;

	err = test_precision_scale1();
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;
	
	print_bar();

	err = test_precision_scale2();
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	//test ub1 and sb2 for precision
	err = test_precision_scale3();
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

sword
test_precision_scale1()
{
	sword	ret = DCI_SUCCESS;
	DCIStmt	*hstmt;
	int		col_num;
	DCIParam	*pParam = NULL;
	sb2		precision;
	ub4		len;

	err = test_execute("DROP TABLE IF EXISTS TEST_PRECISION");
	err = test_execute("CREATE TABLE TEST_PRECISION(ID1 NUMERIC(20, 0) not null primary key, ID2 NUMERIC(20, 0) not null);");
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = test_execute("INSERT INTO TEST_PRECISION VALUES(10, 10);");
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc((void *)pEnv, (void **)&hstmt, (const ub4)DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(hstmt, pError, (const DciText *)"SELECT MAX(ID1) FROM TEST_PRECISION",\
		(ub4)strlen("SELECT MAX(ID1) FROM TEST_PRECISION"), DCI_NTV_SYNTAX, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, hstmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)hstmt, DCI_HTYPE_STMT, (void *)&col_num, 0, (ub4)DCI_ATTR_PARAM_COUNT, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIParamGet((void *)hstmt, DCI_HTYPE_STMT, pError, (void **)&pParam, (ub4)1);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)pParam, (ub4)DCI_DTYPE_PARAM, (void *)&precision, &len, (ub4)DCI_ATTR_PRECISION, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	printf("Get Precision is %d\n", precision);
	if (precision != 20 || len != 2)
	{
		ret = DCI_ERROR;
		printf("Get precision error\n");
	}

	err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	err = DCIHandleFree((void *)hstmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}
	err = DCIDescriptorFree((void *)pParam, (const ub4)DCI_DTYPE_PARAM);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		pParam = NULL;
		goto end;
	}
	
	err = DCIHandleAlloc((void *)pEnv, (void **)&hstmt, (const ub4)DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(hstmt, pError, (const DciText *)"SELECT MAX(ID2) FROM TEST_PRECISION",\
		(ub4)strlen("SELECT MAX(ID2) FROM TEST_PRECISION"), DCI_NTV_SYNTAX, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, hstmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)hstmt, DCI_HTYPE_STMT, (void *)&col_num, 0, (ub4)DCI_ATTR_PARAM_COUNT, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIParamGet((void *)hstmt, DCI_HTYPE_STMT, pError, (void **)&pParam, (ub4)1);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)pParam, (ub4)DCI_DTYPE_PARAM, (void *)&precision, &len, (ub4)DCI_ATTR_PRECISION, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	printf("Get Precision is %d\n", precision);
	if (precision != 20 || len != 2)
	{
		ret = DCI_ERROR;
		printf("Get precision error\n");
	}


	err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

free_resource:
	err = DCIHandleFree((void *)hstmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

end:
	err = DCIDescriptorFree((void *)pParam, (const ub4)DCI_DTYPE_PARAM);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
	}
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	err = test_execute("drop table TEST_PRECISION");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;
	
	return ret;
}

#define COL_NUMBER	7
sword
test_precision_scale2()
{
	sword	ret = DCI_SUCCESS;
	DCIStmt	*hstmt;

	int		out_precision[COL_NUMBER] = {20, 10, 24, 24, 53, 12, 22};
	int		out_scale[COL_NUMBER] = {0, 2, -127, -127, -127, 3, 2};
	int		col_num;
	DCIParam *pParam = NULL;

	int i = 0;

	printf("test_precision_scale2\n");

	err = test_execute("DROP TABLE IF EXISTS TEST_PRECISION");
	err = test_execute("CREATE TABLE TEST_PRECISION(C1 NUMERIC(20, 0) not null primary key, C2 decimal(10, 2), C3 float(17), C4 real, C5 double, C6 time(3), C7 timestamp(2));");
	if (err != DCI_SUCCESS)
		return DCI_ERROR;
	err = test_execute("CREATE INDEX INDEX_TEST_PRECISION ON TEST_PRECISION(C1);");
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = test_execute("INSERT INTO TEST_PRECISION VALUES(10.1, 1.1, 1.2, 1.3, 1.4, '04:05', '2010-06-08 17:03:30');");
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc((void *)pEnv, (void **)&hstmt, (const ub4)DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(hstmt, pError, (const DciText *)"SELECT * FROM TEST_PRECISION",\
		(ub4)strlen("SELECT * FROM TEST_PRECISION"), DCI_NTV_SYNTAX, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, hstmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)hstmt, DCI_HTYPE_STMT, (void *)&col_num, 0, (ub4)DCI_ATTR_PARAM_COUNT, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	for (i = 1; i <= col_num; i++)
	{
		err = DCIParamGet((void *)hstmt, DCI_HTYPE_STMT, pError, (void **)&pParam, (ub4)i);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto free_resource;
		}

		sb1 scale;
		sb2 precision;
		ub4 len;

		err = DCIAttrGet((void *)pParam, (ub4)DCI_DTYPE_PARAM, (void *)&scale, &len, (ub4)DCI_ATTR_SCALE, pError);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto free_resource;
		}
		printf("Get scale is %d\n", scale);
		if (scale != out_scale[i -1] || len != 1)
		{
			ret = DCI_ERROR;
			printf("Get Scale Error\n");
		}

		err = DCIAttrGet((void *)pParam, (ub4)DCI_DTYPE_PARAM, (void *)&precision, &len, (ub4)DCI_ATTR_PRECISION, pError);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto free_resource;
		}

		printf("Get Precision is %d\n", precision);

		if (precision != out_precision[i - 1] || len != 2)
		{
			ret = DCI_ERROR;
			printf("Get Precision Error\n");
		}
		
		DCIDescriptorFree((void *)pParam, (const ub4)DCI_DTYPE_PARAM);
	}

	err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

free_resource:
	err = DCIHandleFree((void *)hstmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	err = test_execute("drop table TEST_PRECISION");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

sword
test_precision_scale3()
{
	sword	ret = DCI_SUCCESS;

	int		out_precision[11] = {10,100,20,200, 20, 10, 24, 24, 53, 12, 22};
	int		out_scale[11] = {0,0,0,0, 0, 2, -127, -127, -127, 3, 2};
	DCIParam *pParam = NULL;
	DCIDescribe *dschp = NULL;

	int i = 0;
	ub2 num_args = 0;
	DCIParam *param_list = NULL, *arg = NULL;
	ub1		data_precision; 
	sb1		data_scale; 
	ub4		len_precision = 0, len_scale = 0;

	printf("test_precision_scale3\n");

	err = test_execute("DROP TABLE IF EXISTS TEST_PRECISION");
	err = test_execute("CREATE TABLE TEST_PRECISION(ab char(10 byte),vb varchar(100 byte),ac char(20 char),bc varchar(200 char),C1 NUMERIC(20, 0) not null primary key, C2 decimal(10, 2), C3 float(17), C4 real, C5 double, C6 time(3), C7 timestamp(2));");
	if (err != DCI_SUCCESS)
		return DCI_ERROR;
	err = test_execute("CREATE INDEX INDEX_TEST_PRECISION ON TEST_PRECISION(C1);");
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	//err = test_execute("INSERT INTO TEST_PRECISION VALUES(10.1, 1.1, 1.2, 1.3, 1.4, '04:05', '2010-06-08 17:03:30');");
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc(pEnv, (void **) &dschp, DCI_HTYPE_DESCRIBE, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIDescribeAny(pSvcCtx, (DCIError *) pError, (void *) "TEST_PRECISION", sizeof("TEST_PRECISION"), DCI_OTYPE_NAME, DCI_DEFAULT, DCI_PTYPE_TABLE, dschp);
	err = DCIAttrGet((void *)dschp, DCI_HTYPE_DESCRIBE, (void *)&pParam, (ub4 *)0, DCI_ATTR_PARAM, pError);
	err = DCIAttrGet((void *)pParam, DCI_DTYPE_PARAM, (void *)&num_args, (ub4 *)0, DCI_ATTR_NUM_COLS, pError);
	err = DCIAttrGet((void *)pParam, DCI_DTYPE_PARAM, (void *)&param_list, (ub4 *)0, DCI_ATTR_LIST_COLUMNS, pError);

	for (i = 0; i < num_args; i++) 
	{
		//Get parameter details. 
		err = DCIParamGet((CONST dvoid *)param_list,(ub4)DCI_DTYPE_PARAM, (DCIError *)pError,(dvoid **)&arg,(ub4)i+1); 
		err = DCIAttrGet((CONST dvoid *)arg,(ub4)DCI_DTYPE_PARAM, (dvoid *)&data_precision,&len_precision, (ub4)DCI_ATTR_PRECISION, (DCIError *) pError);
		if (data_precision != out_precision[i] || len_precision != 1)
		{
			ret = DCI_ERROR;
			printf("Get Precision Error\n");
		}
		err = DCIAttrGet((CONST dvoid *)arg,(ub4)DCI_DTYPE_PARAM, (dvoid *)&data_scale, &len_scale, (ub4)DCI_ATTR_SCALE, (DCIError *) pError);
		if (data_scale != out_scale[i] || len_scale != 1)
		{
			ret = DCI_ERROR;
			printf("Get Scale Error\n");
		}
	}

	err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	err = DCIHandleFree(dschp, DCI_HTYPE_DESCRIBE);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
	}

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	err = test_execute("drop table TEST_PRECISION");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

// bug#12697: fix the 3th problem.
sword
test_multiple_sql()
{
	sword	ret = DCI_SUCCESS;
	DCIStmt	*pStmt = NULL;
	DCIDefine	*pDefine;

	char	exec_buf[SQL_LEN];
	int		out_value;

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "drop table if exists test_multiple_sql;");
	err = test_execute(exec_buf);

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (ub4)DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "create table test_multiple_sql(a int);"
		"insert into test_multiple_sql values(1);"
		"update test_multiple_sql set a = 2 where a = 1;");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIHandleFree((void *)pStmt, (ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}
	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (ub4)DCI_HTYPE_STMT, 0, (void **)NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "select * from test_multiple_sql;");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt, &pDefine, pError, (ub4)1, (void *)&out_value, (sb4)sizeof(int), (ub2)SQLT_INT,\
		(void *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtFetch(pStmt, pError, (ub4)1, (ub2)DCI_FETCH_NEXT, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (out_value != 2)
	{
		ret = DCI_ERROR;
		printf("update not success in multiple statements\n");
	}

	err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "drop table test_multiple_sql;");
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

// bug#12697: fix the 2th problem.

sword
test_result_length()
{
	sword		ret = DCI_SUCCESS;
	DCIStmt		*pStmt = NULL;
	DCIDefine	*pDefine = NULL;

	char	exec_buf[SQL_LEN];
	int		out_value_int[4];
	float	out_value_float[5];
	char	out_value_char[11][STR_LEN];
	int		i = 0;
	int		j = 0;

	ub2		out_value_length[20];
	ub2		expect_out_value_length[20] = {4 , 4 , 4 , 4 , 4 , 4 , 4 , 4 , 4 , 1, 5, 7, 10, 8, 19, 11, 22, 2, 5, 4};
	typedef struct COMPATIBLE_LEVEL 
	{
		char	compatible_level[STR_LEN];
		ub2		expect_out;
	}COMPATIBLE_LEVEL;

	COMPATIBLE_LEVEL	compatible_array[] = 
	{
		{"on", 19},
		//{"off", 10}
	};

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (ub4)DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	for (j = 0; j < (int)(sizeof(compatible_array) / sizeof(compatible_array[0])); j++)
	{
		memset(exec_buf, 0, SQL_LEN);
		sprintf(exec_buf, "set ora_date_style =%s", compatible_array[j].compatible_level);
		err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto free_resource;
		}
		err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto free_resource;
		}
		expect_out_value_length[12] = compatible_array[j].expect_out;

		memset(exec_buf, 0, SQL_LEN);
		sprintf(exec_buf, "drop table if exists test_result_length;");
		err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
		err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);


		memset(exec_buf, 0, SQL_LEN);
		sprintf(exec_buf, "create table test_result_length(c1 tinyint, c2 bigint, c3 smallint,"
			"c4 integer, c5 real, c6 float, c7 double, c8 numeric(5,2), c9 decimal(6,3),"
			"c10 boolean, c11 char(5), c12 varchar(10), c13 date, c14 time, c15 timestamp,"
			"c16 time with time zone, c17 timestamp with time zone, c18 bit(2), c19 bit varying(5), c20 interval year);");
		err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto free_resource;
		}
		err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto free_resource;
		}

		memset(exec_buf, 0, SQL_LEN);
		sprintf(exec_buf, "insert into test_result_length values(21, 88888, 254, 9999, 3.66,"
			"-2.55, 666.33, 3.659, -7.235, 1, 'char', 'varchar', '2010-06-02','17:55',"
			"'2010-06-02 17:55', '17:55-08', '2010-06-02 17:55-08', '01', '10101', '98')");
		err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto free_resource;
		}
		err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto free_resource;
		}

		memset(exec_buf, 0, SQL_LEN);
		sprintf(exec_buf, "select * from test_result_length");
		err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto free_resource;
		}

		for (i = 0; i < 4; i++)
		{
			DCIDefineByPos(pStmt, &pDefine, pError, (ub4)(i + 1), &out_value_int[i], (sb4)sizeof(int), (ub2)SQLT_INT,\
				(void *)NULL, (ub2 *)&out_value_length[i], (ub2 *)0, (ub4)DCI_DEFAULT);
		}

		for (i = 4; i < 9; i++)
		{
			DCIDefineByPos(pStmt, &pDefine, pError, (ub4)(i + 1), &out_value_float[i - 4], (sb4)sizeof(float), (ub2)SQLT_FLT,\
				(void *)NULL, (ub2 *)&out_value_length[i], (ub2 *)0, (ub4)DCI_DEFAULT);
		}

		for (i = 9; i < 20; i++)
		{
			DCIDefineByPos(pStmt, &pDefine, pError, (ub4)(i + 1), &out_value_char[i - 9], (sb4)STR_LEN, (ub2)SQLT_CHR,\
				(void *)NULL, (ub2 *)&out_value_length[i], (ub2 *)0, (ub4)DCI_DEFAULT);
		}

		err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto free_resource;
		}

		err = DCIStmtFetch(pStmt, pError, (ub4)1, (ub2)DCI_FETCH_NEXT, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto free_resource;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto free_resource;
		}

		for (i = 0; i < 20; i++)
		{
			if (out_value_length[i] != expect_out_value_length[i])
			{
				ret = DCI_ERROR;
				printf("the %d value length is not right\n", i + 1);
				printf("get value length is %d, expected: %d\n", out_value_length[i], expect_out_value_length[i]);
			}
		}
	}

free_resource:
	err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "drop table test_result_length;");
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

#define INSERT_SIZE 10
sword
test_bug12869()
{
	DCIStmt	*pStmt;
	sword	err = DCI_SUCCESS, ret = DCI_SUCCESS;
	char	exec_buf[SQL_LEN];

	ub4		ret_val, ret_size, i = 0;
	boolean	flag = 1;

	err = test_execute("DROP TABLE IF EXISTS TEST_BUG12869;");
	err = test_execute("CREATE TABLE TEST_BUG12869(a INT);");
	for (i = 0; i < INSERT_SIZE; i++)
	{
		memset(exec_buf, 0, SQL_LEN);
		sprintf(exec_buf, "INSERT INTO TEST_BUG12869 VALUES(%d);", i);
		err = test_execute(exec_buf);
	}

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect fail\n");
		return DCI_ERROR;
	}

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (const ub4)DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("DCIHandleAlloc fail\n");
		goto end;
	}

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "SELECT * FROM TEST_BUG12869");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("DCIStmtPrepare fail\n");
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("DCIStmtExecute fail\n");
		goto free_resource;
	}

	i = 0;
	while (flag)
	{
		err = DCIAttrGet((void *)pStmt, (ub4)DCI_HTYPE_STMT, (void *)&ret_val, (ub4 *)&ret_size, (ub4)DCI_ATTR_ROW_COUNT, pError);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIAttrGet fail\n");
			goto free_resource;
		}

		if (ret_val != i)
		{
			ret = DCI_ERROR;
			printf("DCIAttrGet DCI_ATTR_ROW_COUNT before DCIStmtFetch fail\n");
			goto free_resource;
		}

		err = DCIStmtFetch(pStmt, pError, (ub4)1, (ub2)DCI_FETCH_NEXT, (ub4)DCI_DEFAULT);
		if (err == DCI_NO_DATA)
			flag = 0;
		else if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIStmtFetch fail\n");
			goto free_resource;
		}

		err = DCIAttrGet((void *)pStmt, (ub4)DCI_HTYPE_STMT, (void *)&ret_val, (ub4 *)&ret_size, (ub4)DCI_ATTR_ROW_COUNT, pError);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIAttrGet fail\n");
			goto free_resource;
		}

		if (flag && ret_val != i + 1)
		{
			ret = DCI_ERROR;
			printf("DCIAttrGet DCIATTR_ROW_COUNT after DCIStmtFetch fail\n");
			goto free_resource;
		}
		else if (flag == 0 && ret_val != i)
		{
			ret = DCI_ERROR;
			printf("DCIAttrGet DCIATTR_ROW_COUNT after DCIStmtFetch fail\n");
			goto free_resource;
		}

		i++;
	}

free_resource:
	err = DCIHandleFree((void *)pStmt, (ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree fail\n");
		ret = DCI_ERROR;
	}

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
	{
		printf("disconnect fail\n");
		ret = DCI_ERROR;
	}

	return ret;
}

/* Modify the host、port and port as same as you use in sys_dblink.conf. [kdb] should be as same as DBLINK_SOURCE.
 * you should install KingbaseES 7 ODBC Driver before starting up kingbase, other wise it can't find ODBC when using dblink.
	[kdb]
	DriverType=ODBC
	DriverName="KingbaseES 7 ODBC Driver"
	Host=127.0.0.1
	Port=54321
	dbname=TEST
	ExtendedProperties=""
*/
#define DBLINK_SOURCE "kdb"

sword
test_dblink()
{
	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	DCIStmt	*pStmt;
	DCIDefine	*pDefine;
	char	exec_buf[STR_LEN];
	int		out_value = 0;

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop table if exists test_dblink");
	err = test_execute(exec_buf);

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "create table test_dblink(a int);");
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
	{
		printf("*******************create table test_dblink fail\n");
		return DCI_ERROR;
	}

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test_dblink values(1);");
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("*******************insert into test_dblink fail\n");
		goto drop_table;
	}

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop database link if exists test_dblink");
	err = test_execute(exec_buf);

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "create database link test_dblink connect to '%s' identified by '%s' using '%s';", User, Pwd, DBLINK_SOURCE);
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("*******************create database link fail\n");
		goto drop_table;
	}

	// test dblink
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "select * from test_dblink@test_dblink;");

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("*******************connect fail\n");
		goto drop_link;
	}

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (const ub4)DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("*******************DCIHandleAlloc fail\n");
		goto end;
	}

	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("*******************DCIStmtPrepare fail\n");
		goto free_resouce;
	}

	err = DCIDefineByPos(pStmt, &pDefine, pError, (ub4)1, (void *)&out_value, (sb4)sizeof(int), (ub2)SQLT_INT,\
		(void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("*******************DCIDefineByPos fail\n");
		goto free_resouce;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS && out_value != 1)
	{
		ret = DCI_ERROR;
		printf("*******************DCIStmtExecute fail\n");
		goto free_resouce;
	}

free_resouce:
	err = DCIHandleFree((void *)pStmt, DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("*******************DCIHandleFree fail\n");
	}

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("*******************disconnect fail\n");
	}

drop_link:
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop database link test_dblink");
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("*******************drop database link test_dblink fail\n");
	}

drop_table:
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop table test_dblink");
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("*******************drop table test_dblink fail\n");
	}

	return ret;
}

/* original value is 10000, but can't test on SPARC and AIX platfrom. */
#define ROW_NUM 1000
sword
test_PLAN_Exist()
{
	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	char	exec_buf[STR_LEN];

	char	*first_table = "test_thread1";
	char	*second_table = "test_thread2";

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop table if exists %s", first_table);
	ret = test_execute(exec_buf);
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "create table %s(a int)", first_table);
	ret = test_execute(exec_buf);
	if (ret != DCI_SUCCESS)
		return DCI_ERROR;

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop table if exists %s", second_table);
	ret = test_execute(exec_buf);
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "create table %s(a int)", second_table);
	ret = test_execute(exec_buf);
	if (ret != DCI_SUCCESS)
	{
		err = DCI_ERROR;
		goto free_resources;
	}

	ret = connect(true);
	if (ret != DCI_SUCCESS)
	{
		err = DCI_ERROR;
		goto free_resources;
	}

#ifdef WIN32
	
	unsigned int	thread_id1, thread_id2;
	HANDLE			hThread1, hThread2;
	unsigned long	hThread1_out = -1, hThread2_out = -1;

	hThread1 = (HANDLE) _beginthreadex(NULL, 0, test_thread, (void *)first_table, 0, &thread_id1);
	hThread2 = (HANDLE) _beginthreadex(NULL, 0, test_thread, (void *)second_table, 0, &thread_id2);
	
	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);

	if (GetExitCodeThread(hThread1, (LPDWORD)&hThread1_out))
	{
		if (hThread1_out != DCI_SUCCESS)
			err = DCI_ERROR;
	}
	
	if (GetExitCodeThread(hThread2, (LPDWORD)&hThread2_out))
	{
		if (hThread2_out != DCI_SUCCESS)
			err = DCI_ERROR;
	}

	CloseHandle(hThread1);
	CloseHandle(hThread2);

	goto end;
#else
	
	pthread_t hThread1, hThread2;
	int ret1, ret2;
	void	*hThread1_out, *hThread2_out;

	ret1 = pthread_create(&hThread1, NULL, &test_thread, (void *)first_table);
	if (ret1 != 0)
	{
		printf("create thread hThread1 fail\n");
		err = DCI_ERROR;
		goto end;
	}
	ret2 = pthread_create(&hThread2, NULL, &test_thread, (void *)second_table);
	if (ret2 != 0)
	{
		printf("create thread hThread2 fail\n");
		err = DCI_ERROR;
		goto end;
	}

	pthread_join(hThread1, (void **)&hThread1_out);
	pthread_join(hThread2, (void **)&hThread2_out);
	if (hThread1_out != DCI_SUCCESS || hThread2_out != DCI_SUCCESS)
	{
		printf("execute fail\n");
		err = DCI_ERROR;
		goto end;
	}

#endif
	
end:
	ret = disconnect(true);
	if (ret != DCI_SUCCESS)
		return DCI_ERROR;

free_resources:
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop table %s", first_table);
	ret = test_execute(exec_buf);
	if (ret != DCI_SUCCESS)
		return DCI_ERROR;

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop table %s", second_table);
	ret = test_execute(exec_buf);
	if (ret != DCI_SUCCESS)
		return DCI_ERROR;

	return err;
}

sword
test_multi_thread_test()
{

	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	char	exec_buf[STR_LEN];
	int num= THREADCOUNT;
	int xx =sizeof(DCIDate);

	err = DCIInitialize((ub4) DCI_DEFAULT, (dvoid *)0, (dvoid * (*)(dvoid *, size_t)) 0, (dvoid * (*)(dvoid *, dvoid *, size_t))0, (void (*)(dvoid *, dvoid *)) 0); 

#ifdef WIN32

	unsigned int	*thread_id=NULL;
	HANDLE			*hThread=NULL;
	unsigned long	*hThread_out=NULL;
	int i=0;
	char ** table_name=NULL;

	table_name = (char**)malloc(sizeof(char*)*num);
	for (i=0;i<num;i++)
	{
		table_name[i] = (char*)malloc(sizeof(char)*200);
		sprintf(table_name[i],"test_thread%d",i);
	}

	thread_id = (unsigned int*)malloc(sizeof(unsigned int)*num);
	memset(thread_id,0,sizeof(unsigned int)*num);
	hThread = (HANDLE*)malloc(sizeof(HANDLE)*num);
	memset(hThread,0,sizeof(HANDLE)*num);
	hThread_out = (unsigned long*)malloc(sizeof(unsigned long)*num);
	memset(hThread_out,0,sizeof(unsigned long)*num);
	for (i=0;i<num;i++)
	{
		hThread[i] = (HANDLE) _beginthreadex(NULL, 0, test_thread, (void *)(table_name[i]), 0, &thread_id[i]);
	}

	for (i=0;i<num;i++)
	{
		WaitForSingleObject(hThread[i], INFINITE);
	}

	for (i=0;i<num;i++)
	{
		if (GetExitCodeThread(hThread[i], (LPDWORD)&hThread_out[i]))
		{
			if (hThread_out[i] != DCI_SUCCESS)
				err = DCI_ERROR;
		}
	}



	for (i=0;i<num;i++)
	{
		CloseHandle(hThread[i]);
	}

	goto end;
#else

	pthread_t *hThread=NULL;
	//int ret=0;
	int i=0;
	void	**hThread_out;
	char ** table_name=NULL;
	
	table_name = (char**)malloc(sizeof(char*)*num);
	for (i=0;i<num;i++)
	{
		table_name[i] = (char*)malloc(sizeof(char)*200);
		sprintf(table_name[i],"test_thread%d",i);
	}

	hThread = (pthread_t *)malloc(sizeof(pthread_t)*num);
	memset(hThread,0,sizeof(pthread_t)*num);

	hThread_out = (void	**)malloc(sizeof(void*)*num);
	
	for (i=0;i<num;i++)
	{
		ret = pthread_create(&hThread[i], NULL, &test_thread, (void *)(table_name[i]));
		if (ret != 0)
		{
			printf("create thread hThread[%d] fail\n",i);
			err = DCI_ERROR;
			goto end;
		}
	}
	
	for (i=0;i<num;i++)
	{
		pthread_join(hThread[i], (void **)&hThread_out[i]);
	}

	for (i=0;i<num;i++)
	{
		if (hThread_out[i] != DCI_SUCCESS)
		{
			printf("execute fail\n");
			err = DCI_ERROR;
			goto end;
		}
	}

#endif

end:
	//ret = disconnect(true);
	if (ret != DCI_SUCCESS)
		return DCI_ERROR;

free_resources:
	return err;
}

#ifdef WIN32
unsigned  __stdcall
#else
void*
#endif
test_thread(void *arg)
{

	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	DCIStmt	*pStmt=NULL;
	int		i = 0;
	char	exec_buf[STR_LEN];
	int		expected_buf[ROW_NUM];
	int		out_buf[ROW_NUM];
	int		num_args = 0;
	DCIDefine	*pDefine = NULL;
	DCIDefine	*pDefine2 = NULL;
	DCIBind *pBind = NULL;
	DCIBind *pBind2 = NULL;
	DCIDescribe *pDescribe;
	DCIParam *pParam,*param_list,*sub_param,*routine;
	DciText		*col_name=NULL;
	ub2			col_type,num_sub,num_params;

	DCIEnv		*pEnv = NULL;
	DCIError	*pError = NULL;
	DCISvcCtx	*pSvcCtx = NULL;
	DCIServer	*pServer = NULL;
	DCISession	*pSession = NULL;
	char	User[NAME_LEN] = "system";
	char	Pwd[NAME_LEN] = "123456";
	char	DbName[NAME_LEN] = "KingbaseES_V8";
	char	*stablename = (char *)arg;
	char	*procname = "PUBLIC";
	unsigned int loop=0;
	int test1=0;
	int row_count=0;
	char	out_str_buf[STR_LEN];

	char        temp_str[32];
	int		*int_value=NULL;
	char	*char_value=NULL;
	long	tid=0;

#ifdef	WIN32
	tid = GetCurrentThreadId();
#else
	tid = pthread_self();
#endif

	err = DCIEnvInit((DCIEnv **) &pEnv, DCI_DEFAULT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIEnvInit failed\n");
		return 0;
	} 
	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pError, DCI_HTYPE_ERROR, 0, NULL);
	err = DCIHandleAlloc(pEnv, (dvoid **) &pServer, DCI_HTYPE_SERVER, (size_t) 0, NULL);
	err = DCIHandleAlloc(pEnv, (void **) &pSvcCtx, DCI_HTYPE_SVCCTX, 0, NULL);
	err = DCIServerAttach(pServer, pError, (DciText *) DbName, (sb4)strlen(DbName), DCI_DEFAULT);
	err = DCIAttrSet(pSvcCtx, DCI_HTYPE_SVCCTX, pServer, 0, DCI_ATTR_SERVER, pError);
	err = DCIHandleAlloc(pEnv, (dvoid **) &pSession, (ub4) DCI_HTYPE_SESSION, (size_t) 0, NULL);
	err = DCIAttrSet(pSession, DCI_HTYPE_SESSION, (void *) User, (ub4)strlen(User), DCI_ATTR_USERNAME, pError);
	err = DCIAttrSet(pSession, DCI_HTYPE_SESSION, (void *) Pwd, (ub4)strlen(Pwd), DCI_ATTR_PASSWORD, pError);
	err = DCISessionBegin(pSvcCtx, pError, pSession, DCI_CRED_RDBMS, DCI_DEFAULT);
	if (err != DCI_SUCCESS){
		goto free_resource;
	}
	printf("[%lu] DCISessionBegin sucess\n",tid);

	err = DCIHandleAlloc((void *)pEnv, (dvoid **)&pStmt, (ub4)DCI_HTYPE_STMT, 0, (void **)NULL);
	if (err != DCI_SUCCESS){
		goto free_resource;
	}
	
	//drop test table
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop table if exists %s;",stablename);
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS){
		goto free_resource;
	}
	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS){
		goto free_resource;
	}
	//drop test table end
	
	//create test table
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "create table %s(th_id int, th_name varchar(64) )",stablename);
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS){
		goto free_resource;
	}
	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS){
		goto free_resource;
	}
	//create test table end

	//insert into test table
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into %s (th_id,th_name) values(:0,:1);",stablename);
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS){
		goto free_resource;
	}
	
	//
	int_value = (int*)malloc(sizeof(int)*EXECUTECOUNT);
	memset(int_value,0,sizeof(int)*EXECUTECOUNT);
	char_value = (char*)malloc(sizeof(char)*64*EXECUTECOUNT);
	memset(char_value,0,sizeof(char)*64*EXECUTECOUNT);
	for (i = 0; i < EXECUTECOUNT; i++)
	{
		int_value[i] = i;
		sprintf(char_value+i*64,"user%04d",i);
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 1,  int_value, sizeof(int), SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	err = DCIBindArrayOfStruct(pBind,pError, sizeof(int), 0, 0, 0);

	err = DCIBindByPos(pStmt, &pBind2, pError, 2,  char_value, strlen(char_value), SQLT_STR, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	err = DCIBindArrayOfStruct(pBind2,pError, sizeof(char)*64, 0, 0, 0);
	//

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, EXECUTECOUNT, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS){
		goto free_resource;
	}
	//end

	do 
	{
		printf("[%lu] loop read %d start\n",tid,loop);
		memset(int_value,0,sizeof(int)*EXECUTECOUNT);
		memset(char_value,0,sizeof(char)*64*EXECUTECOUNT);
		memset(exec_buf, 0, STR_LEN);
		sprintf(exec_buf, "select th_id,th_name from %s union select th_id,th_name from %s;",stablename,stablename);
		err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS){
			goto free_resource;
		}

		err = DCIDefineByPos(pStmt, &pDefine, pError, 1, (void *)int_value, (sb4)sizeof(int), (ub2)SQLT_INT, (void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS){
			goto free_resource;
		}
		err = DCIDefineArrayOfStruct(pDefine, pError, sizeof(int), 0, 0, 0);
		if (err != DCI_SUCCESS){
			goto free_resource;
		}

		err = DCIDefineByPos(pStmt, &pDefine2, pError, 2, (void *)char_value, (sb4)64, (ub2)SQLT_STR, (void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS){
			goto free_resource;
		}
		err = DCIDefineArrayOfStruct(pDefine2, pError, 64, 0, 0, 0);
		if (err != DCI_SUCCESS){
			goto free_resource;
		}

		err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS){
			goto free_resource;
		}

		err = DCIStmtFetch(pStmt, pError, (ub4)EXECUTECOUNT, (ub2)DCI_FETCH_NEXT, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS){
			goto free_resource;
		}

		for (i = 0; i < EXECUTECOUNT; i++)
		{
			printf("[%lu] get [%s] th_id:%d   th_name:%s\n",tid,stablename,int_value[i],char_value+i*64);
		}
		
		//describy table
		err = DCIHandleAlloc(pEnv, (void **) &pDescribe, DCI_HTYPE_DESCRIBE, 0, NULL);
		err = DCIDescribeAny(pSvcCtx, (DCIError *) pError, (void *) stablename, strlen(stablename), DCI_OTYPE_NAME, DCI_DEFAULT, DCI_PTYPE_TABLE, pDescribe);
		err = DCIAttrGet((void *)pDescribe, DCI_HTYPE_DESCRIBE, (void *)&pParam, (ub4 *)0, DCI_ATTR_PARAM, pError);
		err = DCIAttrGet((void *)pParam, DCI_DTYPE_PARAM, (void *)&num_args, (ub4 *)0, DCI_ATTR_NUM_COLS, pError);
		err = DCIAttrGet((void *)pParam, DCI_DTYPE_PARAM, (void *)&param_list, (ub4 *)0, DCI_ATTR_LIST_COLUMNS, pError);
		for (i = 0; i < num_args; i++) 
		{
			//Get parameter details. 
			err = DCIParamGet((CONST dvoid *)param_list,(ub4)DCI_DTYPE_PARAM, (DCIError *)pError,(dvoid **)&arg,(ub4)i+1);
			if (err != DCI_SUCCESS ){
				goto free_resource;
			}
			err = DCIAttrGet((CONST dvoid *)arg, DCI_DTYPE_PARAM, (dvoid *)&col_name, (ub4 *)NULL, (ub4)DCI_ATTR_NAME, pError);
			printf("[%lu] DCIAttrGet [%s]  DCI_ATTR_NAME: %s\n",tid,stablename,col_name);
			err = DCIAttrGet((CONST dvoid *)arg, DCI_DTYPE_PARAM, (dvoid *)&col_type, (ub4 *)NULL, (ub4)DCI_ATTR_DATA_TYPE, pError);
			printf("[%lu] DCIAttrGet [%s]  DCI_ATTR_DATA_TYPE: %d\n",tid,stablename,col_type);
		}
		err = DCIHandleFree((void *)pDescribe, (ub4)DCI_HTYPE_DESCRIBE);
		if (err != DCI_SUCCESS ){
			goto free_resource;
		}
		//end

		/*printf("loop commit transaction %d\n",loop);
		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS){
			goto free_resource;
		}*/
#ifdef WIN32
        Sleep(100);
#else
        usleep(100*1000);
#endif
	} while (loop++ < 0);
	printf("[%lu] loop read %d success\n",tid,loop);

free_resource:
	if (err != DCI_SUCCESS)
	{
		sb4 errcode;
		DciText sqlstate[6];
		DciText errmsg[SQL_LEN];

		err = DCIErrorGet((void *)pError, 1, sqlstate, &errcode, errmsg, (ub4)SQL_LEN, (ub4)DCI_HTYPE_ERROR);
		printf("[%lu] DCIErrorGet %s\n",tid,errmsg);
		ret = DCI_ERROR;
	}

	ret = DCIHandleFree((dvoid *)pStmt, (const ub4)DCI_HTYPE_STMT);
	if (ret != DCI_SUCCESS){
		err = DCI_ERROR;
	}
	err = DCISessionEnd(pSvcCtx, pError, pSession, DCI_DEFAULT);
	err = DCIServerDetach(pServer, pError, DCI_DEFAULT);
	/* Free session */
	err = DCIHandleFree(pSession, DCI_HTYPE_SESSION);
	pSession = NULL;

	/* Free ServContext */
	err = DCIHandleFree((dvoid *) pSvcCtx, (ub4) DCI_HTYPE_SVCCTX); 
	if (err != DCI_SUCCESS){
		printf("[%lu] DCIHandleFree DCI_HTYPE_SVCCTX failed\n",tid);
	}
	pSvcCtx = NULL;
	/* Free Server */
	err = DCIHandleFree(pServer, DCI_HTYPE_SERVER);
	if (err != DCI_SUCCESS){
		printf("[%lu] DCIHandleFree DCI_HTYPE_SERVER failed\n",tid);
	}
	pServer = NULL;
	err = DCIHandleFree(pError, DCI_HTYPE_ERROR);
	if (err != DCI_SUCCESS){
		printf("[%lu] DCIHandleFree DCI_HTYPE_ERROR failed\n",tid);
	}

	if (int_value)
	{
		free(int_value);
	}
	
	if (char_value)
	{
		free(char_value);
	}
	printf("[%lu] thread exit\n",tid);

end:
#ifdef WIN32
	_endthreadex(err);
	return DCI_SUCCESS;
#else
	pthread_exit((void *)err);
#endif
}

sword
test_bug13899()
{
	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	char	exec_buf[STR_LEN];
	DCIStmt	*pStmt = NULL;

	DciText sqlstate[6];
	sb4		err_no = 0;
	DciText	err_buf1[STR_LEN], err_buf2[STR_LEN];

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop table if exists test1_bug13899");
	ret = test_execute(exec_buf);
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop table if exists test2_bug13899");
	ret = test_execute(exec_buf);

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "create table test1_bug13899(a int primary key)");
	ret = test_execute(exec_buf);
	if (ret != DCI_SUCCESS)
	{
		err = DCI_ERROR;
		goto end2;
	}
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "create table test2_bug13899(b int primary key)");
	ret = test_execute(exec_buf);
	if (ret != DCI_SUCCESS)
	{
		err = DCI_ERROR;
		goto end1;
	}

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test1_bug13899 values(1)");
	ret = test_execute(exec_buf);
	if (ret != DCI_SUCCESS)
	{
		err = DCI_ERROR;
		goto end2;
	}
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test2_bug13899 values(1)");
	ret = test_execute(exec_buf);
	if (ret != DCI_SUCCESS)
	{
		err = DCI_ERROR;
		goto end1;
	}

	ret = connect(true);
	if (ret != DCI_SUCCESS)
	{
		err = DCI_ERROR;
		goto end;
	}

	ret = DCIHandleAlloc((void *)pEnv, (dvoid **)&pStmt, (const ub4)DCI_HTYPE_STMT, 0, NULL);
	if (ret != DCI_SUCCESS)
	{
		err = DCI_ERROR;
		goto disconnect;
	}

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test1_bug13899 values(1)");
	ret = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)(strlen(exec_buf)), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (ret != DCI_SUCCESS)
	{
		err = DCI_ERROR;
		goto free_resource;
	}

	ret = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (ret != DCI_ERROR)
	{
		err = DCI_ERROR;
		goto free_resource;
	}
	else
	{
		memset(sqlstate, 0, 6);
		memset(err_buf1, 0, STR_LEN);
		ret = DCIErrorGet((void *)pError, 1, sqlstate, &err_no, err_buf1, (ub4)(sizeof(err_buf1)), (ub4)DCI_HTYPE_ERROR);
		if (ret != DCI_SUCCESS)
		{
			err = DCI_ERROR;
			goto free_resource;
		}
		else
			printf(" sqlstate: %s\n err_no: %d\n error message: %s\n", (char *)sqlstate, (int)err_no, (char *)err_buf1);
	}

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test2_bug13899 values(1)");
	ret = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)(strlen(exec_buf)), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (ret != DCI_SUCCESS)
	{
		err = DCI_ERROR;
		goto free_resource;
	}

	ret = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (ret != DCI_ERROR)
	{
		err = DCI_ERROR;
		goto free_resource;
	}
	else
	{
		memset(sqlstate, 0, 6);
		memset(err_buf2, 0, STR_LEN);
		ret = DCIErrorGet((void *)pError, 1, sqlstate, &err_no, err_buf2, (ub4)(sizeof(err_buf2)), (ub4)DCI_HTYPE_ERROR);
		if (ret != DCI_SUCCESS)
		{
			err = DCI_ERROR;
			goto free_resource;
		}
		else
			printf(" sqlstate: %s\n err_no: %d\n error message: %s\n", (char *)sqlstate, (int)err_no, (char *)err_buf2);
	}

	if (strcmp((const char *)err_buf1, (const char *)err_buf2) == 0)
		err = DCI_ERROR;
	
free_resource:

	ret = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
	if (ret != DCI_SUCCESS)
		err = DCI_ERROR;

disconnect:
	ret = disconnect(true);
	if (ret != DCI_SUCCESS)
	{
		err = DCI_ERROR;
		goto end2;
	}

end:
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop table if exists  test1_bug13899");
	ret = test_execute(exec_buf);
	if (ret != DCI_SUCCESS)
		err = DCI_ERROR;
	
end1:
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop table if exists test2_bug13899");
	ret = test_execute(exec_buf);
	if (ret != DCI_SUCCESS)
		err = DCI_ERROR;

end2:
	return err;
}

sword
test_bug14055()
{
	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	DCIStmt *pStmt[30];
	int i;

	ret = connect(false);
	if (ret != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	for (i = 0; i < 30; i++)
	{
		err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt[i], (ub4)DCI_HTYPE_STMT, 0, NULL);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto end;
		}
	}

end:
	ret = disconnect(false);
	if (ret != DCI_SUCCESS)
	{
		printf("disconnect failed\n");
		err = DCI_ERROR;
	}

	for (i = 0; i < 30; i++)
	{
		err = DCIHandleFree((dvoid *) pStmt[i], (ub4) DCI_HTYPE_STMT); 
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
		}
	}

	return err;
}

sword test_TransStart()
{
	DCIStmt *pStmt;
	DCITrans *pTrans; 

	char	exec_buf[SQL_LEN];
	sword	ret = DCI_SUCCESS;

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "drop table if exists bug14056;");
	err = test_execute(exec_buf);

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		err = DCI_ERROR;
		goto end;
	}

	/* allocate transaction handle 1 and set it in the service handle */
	err = DCIHandleAlloc((void *)pEnv, (void **)&pTrans, DCI_HTYPE_TRANS, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_TRANS failed\n");
		err = DCI_ERROR;
		goto free_stmt;
	}

	err = DCIAttrSet((void *)pSvcCtx, DCI_HTYPE_SVCCTX, (void *)pTrans, 0,DCI_ATTR_TRANS, pError);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		err = DCI_ERROR;
		goto free_trans;
	}

	err = DCITransStart(pSvcCtx, pError, 2, DCI_TRANS_READWRITE);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransStart failed\n");
		err = DCI_ERROR;
		goto free_trans;
	}

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "create table bug14056(c1 int)");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_trans;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, (const DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_trans;
	}

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "insert into bug14056 values(1)");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_trans;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, (const DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_trans;
	}

	err = DCITransStart(pSvcCtx, pError, 2, DCI_TRANS_SERIALIZABLE);
	if (err != DCI_ERROR)
	{
		printf("DCITransStart failed\n");
		err = DCI_ERROR;
		goto free_trans;
	}

	err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_trans;
	}


free_trans:
	err = DCIHandleFree((void *)pTrans, (const ub4)DCI_HTYPE_TRANS);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

free_stmt:
	err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;


end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

#define MAX_COLUMN_NUM 4
sword
test_bug14058()
{
	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	char	exec_buf[STR_LEN];

	// drop table test_bug14058A
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop table if exists  test_bug14058A;");
	err = test_execute(exec_buf);

	// drop table test_bug14058A
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop table if exists test_bug14058B;");
	err = test_execute(exec_buf);

	// create table test_bug14058A
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "create table test_bug14058A(a int, b int);");
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("create table test_bug14058A fail\n");
		goto end;
	}

	// create table test_bug14058A
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "create table test_bug14058B(a int, b int, c int);");
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("create table test_bug14058B fail\n");
		goto end;
	}

	err = test_A();
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("test_A fail\n");
	}

	err = test_delete();
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("delete fail\n");
		goto end;
	}

	err = test_B();
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("test_B fail\n");
	}

	err = test_delete();
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("delete fail\n");
		goto end;
	}

	err = test_C();
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("test_C fail\n");
	}

	err = test_delete();
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("delete fail\n");
		goto end;
	}

	err = test_D();
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("test_D fail\n");
	}

	err = test_delete();
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("delete fail\n");
		goto end;
	}

	err = test_E();
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("test_E fail\n");
	}

	err = test_delete();
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("delete fail\n");
		goto end;
	}

	err = test_F();
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("test_F fail\n");
	}

	err = test_delete();
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("delete fail\n");
		goto end;
	}

	err = test_G();
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("test_G fail\n");
	}

	err = test_delete();
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("delete fail\n");
		goto end;
	}

	err = test_H();
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("test_H fail\n");
	}

end:
	// drop table test_bug14058A
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop table if exists test_bug14058A;");
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	// drop table test_bug14058A
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop table if exists test_bug14058B;");
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

// delete table test_bug14058A and test_bug14058B
sword
test_delete()
{
	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	char	exec_buf[STR_LEN];

	// delete from test_bug14058A
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "delete from test_bug14058A;");
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("delete from test_bug14058A fail\n");
		goto end;
	}

	// delete from test_bug14058B
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "delete from test_bug14058B;");
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("delete from test_bug14058B fail\n");
	}

end:
	return ret;
}

// 绑定参数，第一个 SQL 语句绑定参数个数小于第二个 SQL 语句绑定参数个数
sword
test_A()
{
	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	DCIStmt	*pStmt = NULL;
	DCIBind	*pBind = NULL;
	char	exec_buf[STR_LEN];
	int		ins_buf[MAX_COLUMN_NUM] = {1, 2, 3};
	int		exp_A[2] = {1, 2}, exp_B[3] = {1, 2, 3};

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (const ub4)DCI_HTYPE_STMT, 0, (void **)NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;	
		goto disconnect;
	}

	// insert into test_bug14058A values(?, ?)
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test_bug14058A values(?, ?);");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 1, &ins_buf[0], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 2, &ins_buf[1], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt,pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	// insert into test_bug14058B values(?, ?, ?)
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test_bug14058B values(?, ?, ?);");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 1, &ins_buf[0], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 2, &ins_buf[1], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 3, &ins_buf[2], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt,pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

disconnect:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	// 验证结果的正确性
	err = test_result(exp_A, exp_B);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

// 绑定参数，第一个 SQL 语句绑定参数个数大于第二个 SQL 语句绑定参数个数
sword
test_B()
{
	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	DCIStmt	*pStmt = NULL;
	DCIBind	*pBind = NULL;
	char	exec_buf[STR_LEN];
	int		ins_buf[MAX_COLUMN_NUM] = {1, 2, 3};
	int		exp_A[2] = {1, 2}, exp_B[3] = {1, 0, 0};

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (const ub4)DCI_HTYPE_STMT, 0, (void **)NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;	
		goto disconnect;
	}

	// insert into test_bug14058A values(?, ?)
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test_bug14058A values(?, ?);");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 1, &ins_buf[0], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 2, &ins_buf[1], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt,pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	// insert into test_bug14058B values(?, 0, 0)
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test_bug14058B values(?, 0, 0);");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 1, &ins_buf[0], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt,pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

disconnect:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	// 验证结果的正确性.
	err = test_result(exp_A, exp_B);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

// 绑定参数，第一个 SQL 语句绑定参数个数不为 0， 第二个 SQL 语句绑定参数个数为 0
sword 
test_C()
{
	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	DCIStmt	*pStmt = NULL;
	DCIBind	*pBind = NULL;
	char	exec_buf[STR_LEN];
	int		ins_buf[MAX_COLUMN_NUM] = {1, 2, 3};
	int		exp_A[2] = {1, 2}, exp_B[3] = {0, 0, 0};

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (const ub4)DCI_HTYPE_STMT, 0, (void **)NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;	
		goto disconnect;
	}

	// insert into test_bug14058A values(?, ?)
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test_bug14058A values(?, ?);");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 1, &ins_buf[0], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 2, &ins_buf[1], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt,pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	// insert into test_bug14058B values(0, 0, 0)
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test_bug14058B values(0, 0, 0);");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt,pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

disconnect:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	// 验证结果的正确性
	err = test_result(exp_A, exp_B);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

// 绑定参数，第一个 SQL 语句绑定参数个数为 0， 第二个 SQL 语句绑定参数个数不为 0
sword 
test_D()
{
	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	DCIStmt	*pStmt = NULL;
	DCIBind	*pBind = NULL;
	char	exec_buf[STR_LEN];
	int		ins_buf[MAX_COLUMN_NUM] = {1, 2, 3};

	int		exp_A[2] = {0, 0}, exp_B[3] = {1, 2, 3};

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (const ub4)DCI_HTYPE_STMT, 0, (void **)NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;	
		goto disconnect;
	}

	// insert into test_bug14058A values(0, 0)
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test_bug14058A values(0, 0);");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt,pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	// insert into test_bug14058B values(?, ?, ?)
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test_bug14058B values(?, ?, ?);");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 1, &ins_buf[0], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 2, &ins_buf[1], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 3, &ins_buf[2], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt,pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

disconnect:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	// 验证结果的正确性
	err = test_result(exp_A, exp_B);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

// 绑定参数，第一个 SQL 语句绑定参数个数第二个 SQL 语句绑定参数个数，参数绑定一次
sword 
test_E()
{
	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	DCIStmt	*pStmt = NULL;
	DCIBind	*pBind = NULL;
	char	exec_buf[STR_LEN];
	int		ins_buf[MAX_COLUMN_NUM] = {1, 2, 3};

	int		exp_A[2] = {1, 2}, exp_B[3] = {1, 2, 0};

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (const ub4)DCI_HTYPE_STMT, 0, (void **)NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;	
		goto disconnect;
	}

	// insert into test_bug14058A values(0, 0)
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test_bug14058A values(?, ?);");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 1, &ins_buf[0], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 2, &ins_buf[1], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt,pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	// insert into test_bug14058B values(?, ?, 0)
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test_bug14058B values(?, ?, 0);");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt,pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

disconnect:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	// 验证结果的正确性
	err = test_result(exp_A, exp_B);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

// 绑定参数，SQL 语句绑定参数个数大于 SQL 语句中的参数个数
sword 
test_F()
{
	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	DCIStmt	*pStmt = NULL;
	DCIBind	*pBind = NULL;
	char	exec_buf[STR_LEN];
	int		ins_buf[MAX_COLUMN_NUM] = {1, 2, 3};

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (const ub4)DCI_HTYPE_STMT, 0, (void **)NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;	
		goto disconnect;
	}

	// insert into test_bug14058A values(?, 0)
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test_bug14058A values(?, 0);");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 1, &ins_buf[0], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 2, &ins_buf[1], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_ERROR)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt,pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

disconnect:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

// 绑定参数，SQL 语句绑定参数个数小于 SQL 语句中的参数个数
sword 
test_G()
{
	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	DCIStmt	*pStmt = NULL;
	DCIBind	*pBind = NULL;
	char	exec_buf[STR_LEN];
	int		ins_buf[MAX_COLUMN_NUM] = {1, 2, 3};

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (const ub4)DCI_HTYPE_STMT, 0, (void **)NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;	
		goto disconnect;
	}

	// insert into test_bug14058A values(?, ?)
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test_bug14058A values(?, ?);");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 1, &ins_buf[0], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt,pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_ERROR)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

disconnect:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

// 绑定参数，第一个 SQL 语句绑定参数个数等于第二个 SQL 语句中绑定参数个数，数据类型不一致。
sword
test_H()
{
	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	DCIStmt	*pStmt = NULL;
	DCIBind	*pBind = NULL;
	DCIDefine	*pDefine = NULL;
	char	exec_buf[STR_LEN];
	int		ins_buf[MAX_COLUMN_NUM] = {1, 2, 3};
	char	*str_buf = "abcd";
	int		i = 0;
	char	out_value_str[STR_LEN];
	int		out_value_int[2];
	int		exp_int[2] = {1, 0};
	char	*exp_str = "abcd";


	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop table if exists test_bug14058Bstr");
	err = test_execute(exec_buf);

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "create table test_bug14058Bstr(a varchar(10));");
	err = test_execute(exec_buf);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (const ub4)DCI_HTYPE_STMT, 0, (void **)NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;	
		goto disconnect;
	}

	// insert into test_bug14058Bstr values(?)
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test_bug14058Bstr values(?);");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 1, str_buf, (sb4)strlen(str_buf), SQLT_STR, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt,pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "select * from test_bug14058Bstr");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt, &pDefine, pError, (ub4)1, (void *)out_value_str, (sb4)sizeof(out_value_str), (ub2)SQLT_CHR,\
		(void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtFetch(pStmt, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	else
	{
		if (strcmp(exp_str, out_value_str) != 0)
			ret = DCI_ERROR;
	}

	// insert into test_bug14058A values(0, 0)
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "insert into test_bug14058A values(?, 0);");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByPos(pStmt, &pBind, pError, 1, &ins_buf[0], sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt,pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "select * from test_bug14058A");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	for (i = 0; i < 2; i++)
	{
		err = DCIDefineByPos(pStmt, &pDefine, pError, (ub4)i + 1, (void *)&out_value_int[i], (sb4)sizeof(int), (ub2)SQLT_INT,\
			(void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto free_resource;
		}
	}

	err = DCIStmtFetch(pStmt, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	else
	{
		for (i = 0; i < 2; i++)
		{
			if (out_value_int[i] != exp_int[i])
				ret = DCI_ERROR;
		}
	}

free_resource:
	err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

disconnect:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

// 获取结果集
sword
test_result(int *exp_A, int *exp_B)
{
	int i = 0;
	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	DCIStmt	*pStmt = NULL;
	DCIDefine	*pDefine = NULL;
	char	exec_buf[STR_LEN];

	int		out_value_int[3];

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (const ub4)DCI_HTYPE_STMT, 0, (void **)NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;	
		goto disconnect;
	}

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "select * from test_bug14058A");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	for (i = 0; i < 2; i++)
	{
		err = DCIDefineByPos(pStmt, &pDefine, pError, (ub4)i + 1, (void *)&out_value_int[i], (sb4)sizeof(int), (ub2)SQLT_INT,\
			(void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto free_resource;
		}
	}

	err = DCIStmtFetch(pStmt, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
	}
	else
	{
		for (i = 0; i < 2; i++)
		{
			if (out_value_int[i] != exp_A[i])
				ret = DCI_ERROR;
		}
	}

	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "select * from test_bug14058B");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	for (i = 0; i < 3; i++)
	{
		err = DCIDefineByPos(pStmt, &pDefine, pError, (ub4)i + 1, (void *)&out_value_int[i], (sb4)sizeof(int), (ub2)SQLT_INT,\
			(void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			goto free_resource;
		}
	}

	err = DCIStmtFetch(pStmt, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
	}
	else
	{
		for (i = 0; i < 3; i++)
		{
			if (out_value_int[i] != exp_B[i])
				ret = DCI_ERROR;
		}
	}

free_resource:
	err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

disconnect:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

#define NUM_ROW	61

sword
test_bug14243()
{
	int		i = 0, rows = 0;
	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	DCIStmt	*pStmt = NULL;
	DCIBind	*pBind[2];

	char	exec_buf[STR_LEN];

	int		*c1 = NULL;
	char	*c2 = NULL;
	
	//一般时间结构
	struct STimeInfo {
		unsigned short	nYear;
		unsigned short	nMonth;
		unsigned short	nDay;
		unsigned short	nHour;
		unsigned short	nMinute;
		unsigned short	nSecond;
	};

	STimeInfo	ProcDate[NUM_ROW];

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (const ub4)DCI_HTYPE_STMT, 0, (void **)NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;	
		goto disconnect;
	}

	//drop table
	memset(exec_buf, 0, STR_LEN);
	sprintf(exec_buf, "drop table if exists test_bug14243");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);

	//create table
	sprintf(exec_buf, "create table test_bug14243(c1 int, c2 timestamp(0))");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4)0, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	//batch insert
	sprintf(exec_buf, "insert into test_bug14243 values(:0, :1)");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	c1 = (int *)malloc(sizeof(int) * NUM_ROW);
	if (c1 == NULL)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	else
		memset(c1, 0, sizeof(int) * NUM_ROW);

	c2 = (char *)malloc(8 * NUM_ROW);
	if (c2 == NULL)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	else
		memset(c2, 0, 8 * NUM_ROW);

	err = DCIBindByPos(pStmt,&pBind[0], pError, 1, (dvoid *)c1, 4, SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	err = DCIBindByPos(pStmt,&pBind[1], pError, 2, (dvoid *)c2, 8, SQLT_DAT, 0, 0, 0, 0, 0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	for(i = 0; i < NUM_ROW;i++)
	{
		ProcDate[i].nYear = 2010;
		ProcDate[i].nMonth = i % 12 + 1;
		ProcDate[i].nDay = i % 28 + 1;

		ProcDate[i].nHour = (i + 1) % 24;
		ProcDate[i].nMinute = (i + 1) % 60;
		ProcDate[i].nSecond = (i + 1) % 60;
	}

	for (i = 0; i < NUM_ROW; i++)
	{
		c1[i] = i + 1;

		c2[i * 8] = (char)(ProcDate[i].nYear / 100 + 100);
		c2[i * 8 + 1] = (char)(ProcDate[i].nYear - (ProcDate[i].nYear / 100) * 100 + 100);
		c2[i * 8 + 2] = (char)(ProcDate[i].nMonth);
		c2[i * 8 + 3] = (char)(ProcDate[i].nDay);
		c2[i * 8 + 4] = (char)(ProcDate[i].nHour + 1);
		c2[i * 8 + 5] = (char)(ProcDate[i].nMinute + 1);
		c2[i * 8 + 6] = (char)(ProcDate[i].nSecond + 1);
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4)NUM_ROW, (ub4)0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet(pStmt, DCI_HTYPE_STMT, &rows, 0, DCI_ATTR_ROW_COUNT, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	if (rows != NUM_ROW)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

disconnect:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	if(c1 != NULL)
		free(c1);
	if(c2 != NULL)
		free(c2);	
	return ret;
}

#define FETCH_COUNT	5

sword
test_union_all()
{
	sword	ret = DCI_SUCCESS, err = DCI_SUCCESS;
	DCIStmt	*pStmt = NULL;
	DCIDefine *pDefine;

	char			sql[512] = "";
	unsigned int	i = 0, j = 0, k = 0;

	typedef struct UnionAllData
	{
		char	clmType1[SQL_LEN];
		char	clmData1[FETCH_COUNT][SQL_LEN];
		char	clmType2[SQL_LEN];
		char	clmData2[FETCH_COUNT][SQL_LEN];
		int		defineSize;
		int		fetchCount;
		sword	ret;
	}UnionAllData;

	UnionAllData unionAll[] = 
	{
		{"numeric(10,0)", {"-2147483648", "-1", "0", "1", "2147483647"}, "int", {"-2147483648", "-1", "0", "1", "2147483647"}, sizeof(int)},
		{"numeric(20,0)", {"-4294967296", "-1", "0", "1", "4294967295"}, "int", {"-2147483648", "-1", "0", "1", "2147483647"}, sizeof(KDB_INT64)},
	};

	typedef struct FetchReturnData 
	{
		int		fetchCount;
		sword	fetchReturn;
	}FetchReturnData;

	FetchReturnData fetchCount[] = 
	{
		{FETCH_COUNT - 1, DCI_SUCCESS},
		{FETCH_COUNT, DCI_SUCCESS},
		{FETCH_COUNT + 1, DCI_SUCCESS},
	};

	int out[SQL_LEN] = {0};
	KDB_INT64 out64[SQL_LEN] = {0};

	err = connect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;

	for (i = 0;i < sizeof(unionAll) / sizeof(unionAll[0]);i++)
	{
		sprintf(sql, "drop table if exists  testa");
		err = execute(sql);

		sprintf(sql, "drop table if exists testb");
		err = execute(sql);

		sprintf(sql, "create table testa(A %s);create table testb(B %s)", unionAll->clmType1, unionAll->clmType2);
		err = execute(sql);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			ret = DCI_ERROR;
			goto end;
		}

		for (j = 0;j < sizeof(unionAll[i].clmData1) / sizeof(unionAll[i].clmData1[0]);j++)
		{
			sprintf(sql, "insert into testa values(%s)", unionAll[i].clmData1[j]);
			err = execute(sql);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				ret = DCI_ERROR;
				goto end;
			}

			sprintf(sql, "insert into testb values(%s)", unionAll[i].clmData2[j]);
			err = execute(sql);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				ret = DCI_ERROR;
				goto end;
			}
		}

		err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			ret = DCI_ERROR;
			goto end;
		}

		sprintf(sql, "select A from testa union all select B from testb");
		err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			ret = DCI_ERROR;
			goto free_resource;
		}

		switch (unionAll[i].defineSize)
		{
		case sizeof(int):
			for (k = 0;k < sizeof(fetchCount) / sizeof(fetchCount[0]);k++)
			{
				err = DCIDefineByPos(pStmt, &pDefine, pError, 1, out, (sb4)sizeof(int), (ub2)SQLT_INT, (void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
				if (err != DCI_SUCCESS)
				{
					report_error(pError);
					ret = DCI_ERROR;
					goto free_resource;
				}
				
				err = DCIDefineArrayOfStruct(pDefine, pError, sizeof(int), 0, 0, 0);
				if (err != DCI_SUCCESS)
				{
					report_error(pError);
					ret = DCI_ERROR;
					goto free_resource;
				}

				err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
				if (err != DCI_SUCCESS)
				{
					report_error(pError);
					ret = DCI_ERROR;
					goto free_resource;
				}

				err = DCIStmtFetch(pStmt, pError, fetchCount[k].fetchCount, DCI_FETCH_NEXT, DCI_DEFAULT);
				if (err != fetchCount[k].fetchReturn)
				{
					printf("DCIStmtFetch failed\n");
					ret = DCI_ERROR;
					goto free_resource;
				}
			}
			break;

		case sizeof(KDB_INT64):
			for (k = 0;k < sizeof(fetchCount) / sizeof(fetchCount[0]);k++)
			{
				err = DCIDefineByPos(pStmt, &pDefine, pError, 1, out64, (sb4)sizeof(KDB_INT64), (ub2)SQLT_INT, (void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
				if (err != DCI_SUCCESS)
				{
					report_error(pError);
					ret = DCI_ERROR;
					goto free_resource;
				}
				
				err = DCIDefineArrayOfStruct(pDefine, pError, sizeof(KDB_INT64), 0, 0, 0);
				if (err != DCI_SUCCESS)
				{
					report_error(pError);
					ret = DCI_ERROR;
					goto free_resource;
				}

				err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
				if (err != DCI_SUCCESS)
				{
					report_error(pError);
					ret = DCI_ERROR;
					goto free_resource;
				}

				err = DCIStmtFetch(pStmt, pError, fetchCount[k].fetchCount, DCI_FETCH_NEXT, DCI_DEFAULT);
				if (err != fetchCount[k].fetchReturn)
				{
					printf("DCIStmtFetch failed\n");
					ret = DCI_ERROR;
					goto free_resource;
				}
			}
			break;

		default:
			break;

		}

		err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
		if (err != DCI_SUCCESS)
		{
			printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
			ret = DCI_ERROR;
			goto end;
		}
		pStmt = NULL;

		sprintf(sql, "drop table  if exists testa");
		err = execute(sql);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			ret = DCI_ERROR;
			goto end;
		}

		sprintf(sql, "drop table if exists testb");
		err = execute(sql);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			ret = DCI_ERROR;
			goto end;
		}
	}

free_resource:
	if(NULL != pStmt)
	{
		err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
		pStmt = NULL;
		if (err != DCI_SUCCESS)
		{
			printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
		}
	}
end:	
	err = disconnect(true);
	return ret;
}

#define	LEN	256
sword test_begin_end()
{
	sword	ret = DCI_SUCCESS;
	char	sql[512] = "";
	unsigned int	i = 0;

	typedef struct TestBeginEndData
	{
		char	tab_name[LEN];
		char	col_name[LEN];
		char	col_value[LEN];
	}TestBeginEndData;

	/*
	 * 未考虑以下几种情况：
	 * begin select * from "table begin " where "end;" = 1
	 * begin select * from "test ;end; "; end;
	 * begin select ' ; end ;' ; end
	*/

	TestBeginEndData TestBeginEnd[] = 
	{
		{"test_begin", "test_begin", "\'test_begin\'"},
		{"test_end", "test_end", "\'test_end\'"},
	};

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	for (i = 0; i < sizeof(TestBeginEnd) /sizeof(TestBeginEnd[0]); i++)
	{
		snprintf(sql, sizeof(sql), "drop table if exists  %s", TestBeginEnd[i].tab_name);
		err = execute(sql);

		snprintf(sql, sizeof(sql), "create table %s(%s char(100))", TestBeginEnd[i].tab_name, TestBeginEnd[i].col_name);
		err = execute(sql);
		if (err != DCI_SUCCESS)
		{
			printf("create table failed\n");
			ret = DCI_ERROR;
			goto end;
		}

		snprintf(sql, sizeof(sql), "begin insert into %s values(%s);end;", TestBeginEnd[i].tab_name, TestBeginEnd[i].col_value);
		err = execute(sql);
		if (err != DCI_SUCCESS)
		{
			printf("begin insert table *;end;failed\n");
			ret = DCI_ERROR;
			goto end;
		}

		snprintf(sql, sizeof(sql), "begin drop table if exists %s;end;", TestBeginEnd[i].tab_name);
		err = execute(sql);
		if (err != DCI_SUCCESS)
		{
			printf("begin drop table *;end;failed\n");
			ret = DCI_ERROR;
			goto end;
		}
	}

	for (i = 0; i < sizeof(TestBeginEnd) /sizeof(TestBeginEnd[0]); i++)
	{
		snprintf(sql, sizeof(sql), "drop table if exists %s", TestBeginEnd[i].tab_name);
		err = execute(sql);

		snprintf(sql, sizeof(sql), "create table %s(%s char(100))", TestBeginEnd[i].tab_name, TestBeginEnd[i].col_name);
		err = execute(sql);
		if (err != DCI_SUCCESS)
		{
			printf("create table failed\n");
			ret = DCI_ERROR;
			goto end;
		}

		snprintf(sql, sizeof(sql), "begin insert into %s values(%s);insert into %s values(%s);end;", 
			TestBeginEnd[i].tab_name, TestBeginEnd[i].col_value, TestBeginEnd[i].tab_name, TestBeginEnd[i].col_value);
		err = execute(sql);
		if (err != DCI_SUCCESS)
		{
			printf("begin insert table *;insert table *;end; failed\n");
			ret = DCI_ERROR;
			goto end;
		}

		snprintf(sql, sizeof(sql), "begin drop table if exists %s;end;", TestBeginEnd[i].tab_name);
		err = execute(sql);
		if (err != DCI_SUCCESS)
		{
			printf("begin drop table *;end; failed\n");
			ret = DCI_ERROR;
			goto end;
		}
	}

end:
	err = disconnect(true);
	return ret;
}

sword get_check_value(char *sql,int *col_value)

{
	sword	ret = DCI_SUCCESS;
	DCIDefine *bndhp[20];
	DCIStmt *pStmt = NULL;

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

	err = DCIStmtPrepare(pStmt, pError, (DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIDefineByPos(pStmt, &bndhp[0], pError, 1,(dvoid *)col_value, (sb4)sizeof(int), (ub2)SQLT_INT,(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("FAILED: DCIDefineByPos()\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIDefineArrayOfStruct(bndhp[0], pError, sizeof(int),0, 0, 0);
	if (err != DCI_SUCCESS)
	{
		printf("FAILED: DCIDefineArrayOfStruct()\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("FAILED: DCIStmtExecute() select\n");
		ret = DCI_ERROR;
		goto end;
	}

	DCIStmtFetch(pStmt, pError, 100, DCI_FETCH_NEXT, 0);

end:

	err = disconnect(true);
	if(pStmt != NULL)
	{
		err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
		if (err != DCI_SUCCESS)
		{
			printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
		}
	}
	return ret;
}

sword test_begin_end2()
{
	typedef struct TestBeginEnd2Data
	{
		char	sql[LEN];
		char	check_sql[LEN];
		int		check_value;
		int		expect_success;
	} TestBeginEnd2Data;
	
	typedef struct TestBeginEnd2Table
	{
		char	createsql[LEN];
		char	dropsql[LEN];
	} TestBeginEnd2Table;

	sword ret = DCI_SUCCESS;
	int col_val = 0;
	unsigned int i = 0;

	TestBeginEnd2Data testdata[] = {
		{	"BEGIN DELETE T_BEGIN_END2;INSERT INTO T_BEGIN_END2(ID,NAME) VALUES(1,'aBcD');UPDATE T_BEGIN_END2 SET ID = 2 WHERE NAME = 'aBcD'; END; \n ",
			"select id from t_begin_end2 where name='aBcD'",2,DCI_SUCCESS
		},
		/*
		{	"  bEGiN  iNSeRT IntO  T_BEGIN_END2(ID,NAME) VALUES(1,'aBcD');UPDATE T_BEGIN_END2 SET ID = 2 WHERE NAME = 'aBcD') ; eNd ; ",
			"select id from t_begin_end2 where name='aBcD'",2,DCI_SUCCESS
		},
		*/
		{	"BEGIN INSERT INTO T_BEGIN_END2(ID,NAME) VALUES(1,'aBcD');INSERT INTO T_BEGIN_END2(ID,NAME) VALUES(1,'aBcD'); END;",
			"select count(*) from t_begin_end2",0,DCI_ERROR
		},
		{	"BEGIN INSERT INTO T_BEGIN_END2(ID,NAME) VALUES(1,'aBcD');INSERT INTO T_BEGIN_END2(ID,NAME) VALUES(1,'aBcD');INSERT INTO T_BEGIN_END2(ID,NAME) VALUES(2,'HELLO'); END;",
			"select count(*) from t_begin_end2",0,DCI_ERROR
		},
		{	"BEGIN; DELETE T_BEGIN_END2;INSERT INTO T_BEGIN_END2(ID,NAME) VALUES(1,'aBcD');END;",
			"",0,DCI_SUCCESS
		},
		{	"BEGIN DELETE T_BEGIN_END2;INSERT INTO T_BEGIN_END2(ID,NAME) VALUES(1,'aBcD')END;",
			"",0,DCI_ERROR
		},
		{	"BEGIN DELETE T_BEGIN_END2;INSERT INTO T_BEGIN_END2(ID,NAME) VALUES(1,'aBcD');END",
			//"",0,DCI_ERROR
			"",0,DCI_SUCCESS //712 supported
		},
		{	"BEGIN DELETE T_BEGIN_END2 INSERT INTO T_BEGIN_END2(ID,NAME) VALUES(1,'aBcD');END;",
			"",0,DCI_ERROR
		},
		{	"BEGIN DELETE \"BEGIN\";INSERT INTO \"BEGIN\" VALUES(1);END;",
			"",0,DCI_SUCCESS
		},
		{	"BEGIN DELETE \"END\";INSERT INTO \"END\" VALUES(1);END;",
			"",0,DCI_SUCCESS
		},
		{	"BEGIN\n DELETE \"END\";INSERT INTO \"END\" VALUES(1);END;",
			"",0,DCI_SUCCESS
		},
		{	"BEGIN\r DELETE \"END\";INSERT INTO \"END\" VALUES(1);END;",
			"",0,DCI_SUCCESS
		},
		{	"BEGIN CREATE TABLE TEST_BEGIN_END2(ID INT); INSERT INTO TEST_BEGIN_END2 VALUES(12);END;",
			"SELECT COUNT(*) FROM TEST_BEGIN_END2",1,DCI_SUCCESS
		}
	};

	TestBeginEnd2Table testtable[] = {
		{"CREATE TABLE T_BEGIN_END2(ID INT PRIMARY KEY,NAME VARCHAR2(40))","DROP TABLE  IF EXISTS T_BEGIN_END2"},
		{"CREATE TABLE \"BEGIN\" (\"BEGIN\" INT)","DROP TABLE IF EXISTS \"BEGIN\""},
		{"CREATE TABLE \"END\" (\"END\" INT)","DROP TABLE  IF EXISTS  \"END\""},
		{"","BEGIN DROP TABLE IF EXISTS TEST_BEGIN_END2;END;"}
	
	};

	for( i=0; i<sizeof(testtable) / sizeof(testtable[0]); i++ )
	{		
		if(strlen(testtable[i].dropsql) > 0)
		{
			err = test_execute(testtable[i].dropsql);
		}

		if(strlen(testtable[i].createsql) > 0)
		{
			err = test_execute(testtable[i].createsql);
		}
	}
	
	for( i=0; i<sizeof(testdata) / sizeof(testdata[0]); i++ )
	{
		test_execute("delete t_begin_end2");

		ret = test_execute(testdata[i].sql);		
		if(ret != testdata[i].expect_success)
		{			
			printf("failed,sql:%s,expect:%d\n",testdata[i].sql,testdata[i].expect_success);
			ret = DCI_ERROR;
			goto end;
		}
		
		if(strlen(testdata[i].check_sql) > 0)
		{
			col_val = 0;
			ret = get_check_value(testdata[i].check_sql,&col_val);
			if(ret != DCI_SUCCESS)
			{
				printf("faild:get_single_int\n");
				ret = DCI_ERROR;
				goto end;		
			}
			
			if(testdata[i].check_value != col_val)
			{
				printf("failed:checkvalue,sql:%s,check_value:%d,actual value:%d\n",testdata[i].sql,testdata[i].check_value,col_val);
				ret = DCI_ERROR;
				goto end;
			}

		}

	}

	for( i=0; i<sizeof(testtable) / sizeof(testtable[0]); i++ )
	{
		err = test_execute(testtable[i].dropsql);
	}

end:

	return ret;
}

sword test_UseDciDat_0()
{
	sword	ret = DCI_SUCCESS;
	char	sql[512] = "";
	char	table[] = "test_UseDciDat_0";
	DCIDescribe	*pDescribe;
	DCIParam	*param = NULL, *param_list = NULL;
	DCIParam	*routine1 = NULL, *routine2 = NULL;

	ub2		col_type1 = 0, col_type2 = 0;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	snprintf(sql, sizeof(sql), "drop table if exists test_UseDciDat_0");
	err = execute(sql);

	snprintf(sql, sizeof(sql), "create table test_UseDciDat_0(c1 timestamp(0), c2 timestamp)");
	err = execute(sql);
	if (err != DCI_SUCCESS)
	{
		printf("create table test_UseDciDat_0(c1 timestamp(0), c2 timestamp);failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((void *)pEnv, (void **)&pDescribe, DCI_HTYPE_DESCRIBE, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIDescribeAny(pSvcCtx, pError, (void *)table, (ub4)strlen((char *)table), DCI_OTYPE_NAME, DCI_DEFAULT, DCI_PTYPE_TABLE, pDescribe);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)pDescribe, DCI_HTYPE_DESCRIBE, (void *)&param, (ub4 *)0, (ub4)DCI_ATTR_PARAM, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)param, (ub4)DCI_DTYPE_PARAM, (void *)&param_list, (ub4 *)0, (ub4) DCI_ATTR_LIST_COLUMNS, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIParamGet((void *)param_list, DCI_DTYPE_PARAM, pError, (void **)&routine1, (ub4)1);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)routine1, DCI_DTYPE_PARAM, (void *)&col_type1, (ub4 *)NULL, (ub4)DCI_ATTR_DATA_TYPE, pError);
	if (err != DCI_SUCCESS || col_type1 != SQLT_DAT)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIParamGet((void *)param_list, DCI_DTYPE_PARAM, pError, (void **)&routine2, (ub4)2);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)routine2, DCI_DTYPE_PARAM, (void *)&col_type2, (ub4 *)NULL, (ub4)DCI_ATTR_DATA_TYPE, pError);
	if (err != DCI_SUCCESS || col_type2 != SQLT_DAT)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((void *)pDescribe, (ub4)DCI_HTYPE_DESCRIBE);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	err = test_execute("drop table test_UseDciDat_0");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

sword test_UseDciDat_1()
{
	sword	ret = DCI_SUCCESS;
	char	sql[512] = "";
	char	table[] = "test_UseDciDat_1";
	DCIDescribe	*pDescribe;
	DCIParam	*param = NULL, *param_list = NULL;
	DCIParam	*routine1 = NULL, *routine2 = NULL;

	ub2		col_type1 = 0, col_type2 = 0;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	snprintf(sql, sizeof(sql), "drop table test_UseDciDat_1");
	err = execute(sql);

	snprintf(sql, sizeof(sql), "create table test_UseDciDat_1(c1 timestamp(0), c2 timestamp)");
	err = execute(sql);
	if (err != DCI_SUCCESS)
	{
		printf("create table test_UseDciDat_1(c1 timestamp(0), c2 timestamp);failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((void *)pEnv, (void **)&pDescribe, DCI_HTYPE_DESCRIBE, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIDescribeAny(pSvcCtx, pError, (void *)table, (ub4)strlen((char *)table), DCI_OTYPE_NAME, DCI_DEFAULT, DCI_PTYPE_TABLE, pDescribe);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)pDescribe, DCI_HTYPE_DESCRIBE, (void *)&param, (ub4 *)0, (ub4)DCI_ATTR_PARAM, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)param, (ub4)DCI_DTYPE_PARAM, (void *)&param_list, (ub4 *)0, (ub4) DCI_ATTR_LIST_COLUMNS, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIParamGet((void *)param_list, DCI_DTYPE_PARAM, pError, (void **)&routine1, (ub4)1);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)routine1, DCI_DTYPE_PARAM, (void *)&col_type1, (ub4 *)NULL, (ub4)DCI_ATTR_DATA_TYPE, pError);
	if (err != DCI_SUCCESS || col_type1 != SQLT_DAT)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIParamGet((void *)param_list, DCI_DTYPE_PARAM, pError, (void **)&routine2, (ub4)2);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)routine2, DCI_DTYPE_PARAM, (void *)&col_type2, (ub4 *)NULL, (ub4)DCI_ATTR_DATA_TYPE, pError);
	if (err != DCI_SUCCESS || col_type2 != SQLT_TIMESTAMP)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((void *)pDescribe, (ub4)DCI_HTYPE_DESCRIBE);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	err = test_execute("drop table test_UseDciDat_1");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

sword test_UseDciDat_2()
{
	sword	ret = DCI_SUCCESS;
	char	sql[512] = "";
	char	table[] = "test_UseDciDat_2";
	DCIDescribe	*pDescribe;
	DCIParam	*param = NULL, *param_list = NULL;
	DCIParam	*routine1 = NULL, *routine2 = NULL;

	ub2		col_type1 = 0, col_type2 = 0;
	
	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	snprintf(sql, sizeof(sql), "drop table test_UseDciDat_2");
	err = execute(sql);

	snprintf(sql, sizeof(sql), "create table test_UseDciDat_2(c1 timestamp(0), c2 timestamp)");
	err = execute(sql);
	if (err != DCI_SUCCESS)
	{
		printf("create table test_UseDciDat_2(c1 timestamp(0), c2 timestamp);failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((void *)pEnv, (void **)&pDescribe, DCI_HTYPE_DESCRIBE, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIDescribeAny(pSvcCtx, pError, (void *)table, (ub4)strlen((char *)table), DCI_OTYPE_NAME, DCI_DEFAULT, DCI_PTYPE_TABLE, pDescribe);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)pDescribe, DCI_HTYPE_DESCRIBE, (void *)&param, (ub4 *)0, (ub4)DCI_ATTR_PARAM, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)param, (ub4)DCI_DTYPE_PARAM, (void *)&param_list, (ub4 *)0, (ub4) DCI_ATTR_LIST_COLUMNS, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIParamGet((void *)param_list, DCI_DTYPE_PARAM, pError, (void **)&routine1, (ub4)1);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)routine1, DCI_DTYPE_PARAM, (void *)&col_type1, (ub4 *)NULL, (ub4)DCI_ATTR_DATA_TYPE, pError);
	if (err != DCI_SUCCESS || col_type1 != SQLT_TIMESTAMP)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIParamGet((void *)param_list, DCI_DTYPE_PARAM, pError, (void **)&routine2, (ub4)2);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((void *)routine2, DCI_DTYPE_PARAM, (void *)&col_type2, (ub4 *)NULL, (ub4)DCI_ATTR_DATA_TYPE, pError);
	if (err != DCI_SUCCESS || col_type2 != SQLT_TIMESTAMP)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((void *)pDescribe, (ub4)DCI_HTYPE_DESCRIBE);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	err = test_execute("drop table test_UseDciDat_2");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

sword test_bug14060()
{
	sword	ret = DCI_SUCCESS;
	char	sql[512] = "";
	DCIStmt	*pStmt = NULL;
	DCIDefine	*pDefine = NULL;
	int		out[2] = {0};

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	snprintf(sql, sizeof(sql), "drop trigger if exists insert_changes ON test_bug14060A");
	err = execute(sql);
	snprintf(sql, sizeof(sql), "drop table if exists test_bug14060A");
	err = execute(sql);
	snprintf(sql, sizeof(sql), "drop table if exists test_bug14060B");
	err = execute(sql);

	snprintf(sql, sizeof(sql), "CREATE TABLE test_bug14060A(col INT);");
	err = execute(sql);
	snprintf(sql, sizeof(sql), "CREATE TABLE test_bug14060B(col INT);");
	err = execute(sql);

	snprintf(sql, sizeof(sql), "INSERT INTO test_bug14060A VALUES(10);");
	err = execute(sql);
	snprintf(sql, sizeof(sql), "INSERT INTO test_bug14060A VALUES(20);");
	err = execute(sql);

	snprintf(sql, sizeof(sql), "CREATE TRIGGER insert_changes BEFORE UPDATE ON test_bug14060A "
		"FOR EACH ROW "
		"AS "
		"DECLARE "
		"valuediff 	INT;"
		"BEGIN "
		"valuediff := new.col - old.col;"
		"INSERT INTO test_bug14060B VALUES(valuediff);"
		"END;");
	err = execute(sql);

	snprintf(sql, sizeof(sql), "UPDATE test_bug14060A SET col = 100;");
	err = execute(sql);

	snprintf(sql, sizeof(sql), "SELECT * FROM test_bug14060B;");
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

	err = DCIDefineByPos(pStmt, &pDefine, pError, 1, (void *)out, (sb4)sizeof(int), (ub2)SQLT_INT, (void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 2, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS && out[0] == 90 && out[1] == 80)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %s\n", msg);

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
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	err = test_execute("drop trigger insert_changes ON test_bug14060A");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	err = test_execute("drop table test_bug14060A");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	err = test_execute("drop table test_bug14060B");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

sword test_bug15341()
{
	err = test_execute("drop table if exists  test_bug15341");
	err = test_execute("create table test_bug15341(cdate1 date, cdate2 date, ctime1 time, ctime2 time, ctime3 time with time zone, ctime4 time with time zone, ctimestamp1 timestamp, ctimestamp2 timestamp, ctimestamp3 timestamp with time zone, ctimestamp4 timestamp with time zone)");
	if (err != DCI_SUCCESS)
	{
		printf("test_execute failed\n");
		return DCI_ERROR;	
	}

	err = test_execute("insert into  test_bug15341 values('2011-02-02 00:00:00', '2011-02-03 00:00:00', '04:05:06.789', '05:05:06.789', '04:05:06.789-4', '05:05:06.789-8', '2003-01-08 04:05:06.789', '2004-01-08 05:05:06.789', '2003-01-08 04:05:06.789-8', '2004-01-08 05:05:06.789-8')");
	if (err != DCI_SUCCESS)
	{
		printf("test_execute failed\n");
		return DCI_ERROR;	
	}

	err = test_select("select cdate2-cdate1, ctime2-ctime1, ctime4-ctime3, ctimestamp2-ctimestamp1, ctimestamp4-ctimestamp3 from test_bug15341");
	if (err != DCI_SUCCESS)
	{
		printf("test_select failed\n");
		return DCI_ERROR;	
	}

	return DCI_SUCCESS;
}

sword test_BIT()
{
	sword	ret = DCI_SUCCESS;
	char	sql[512] = "";
	DCIStmt	*pStmt = NULL;
	DCIDefine	*pDefine = NULL;
	char		out[256] = {0};

	int counter = 1;
	DCIParam *mypard = (DCIParam *) 0;
	DciText	*col_name = NULL;
	ub2	dtype = 0;
	ub2 col_width = 0;
	int col_name_len = 0;
	sb4	parm_status = 0;
	err = connect(true);

	DCIBind *bindp = NULL;
	char *valuep = "0110";
	int col_num = 0;

	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	snprintf(sql, sizeof(sql), "DROP TABLE  IF EXISTS TEST_BIT;");
	err = execute(sql);

	snprintf(sql, sizeof(sql), "CREATE TABLE TEST_BIT(FLAG BIT(4), col BIT VARYING (20));");
	err = execute(sql);

	snprintf(sql, sizeof(sql), "INSERT INTO TEST_BIT VALUES(B'0110', B'0101000010001');");
	err = execute(sql);

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}	

	snprintf(sql, sizeof(sql), "SELECT * FROM TEST_BIT WHERE FLAG = :1;");

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

	err = DCIBindByPos(pStmt, &bindp, pError, 1, valuep, strlen(valuep), SQLT_AFC, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %s\n", msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS )
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %s\n", msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet(pStmt, DCI_HTYPE_STMT, &col_num, 0, DCI_ATTR_PARAM_COUNT, pError);

	/* Request a parameter descriptor for position 1 in the select-list */

	parm_status = DCIParamGet((dvoid *)pStmt, DCI_HTYPE_STMT, pError, (dvoid **)&mypard, (ub4) counter);

	if (parm_status == DCI_SUCCESS)
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
	}

	err = DCIDefineByPos(pStmt, &pDefine, pError, 2, (void *)out, (sb4)sizeof(out), (ub2)SQLT_CHR, (void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtFetch(pStmt, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT);
	if (err != DCI_SUCCESS && out[0] != '0' && out[1] != '1')
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %s\n", msg);

		ret = DCI_ERROR;
	}

free_resource:
	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}
	err = DCIDescriptorFree((void *)mypard, (const ub4)DCI_DTYPE_PARAM); 
	if (err != DCI_SUCCESS) 
	{		
		printf("DCIDescriptorFree mypard DCI_HTYPE_STMT failed\n");
	}

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	err = test_execute("DROP TABLE TEST_BIT;");
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

sword test_FloatBindByInt()
{
	DciText *sql = (DciText*)"SELECT V_FLOAT FROM T_FLOATBINDBYINT";
	sword ret = DCI_SUCCESS;
	DciText insertSql[200];
	DCIStmt *pStmt = NULL;
	int value_id = 0;	

	test_execute("DROP TABLE IF EXISTS T_FLOATBINDBYINT");
	err = test_execute("CREATE TABLE T_FLOATBINDBYINT(V_FLOAT NUMERIC(6,2))");
	if(err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}
	
	snprintf((char*)insertSql, 200, "INSERT INTO T_FLOATBINDBYINT VALUES (123.45)");
	err = test_execute((char*)insertSql);
	if(err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
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

	value_id = 0;
	err = DCIStmtFetch2(pStmt, pError, (ub4)1, (ub2)DCI_FETCH_NEXT, 0, (ub4)DCI_DEFAULT);

	if(err != DCI_SUCCESS && DCI_NO_DATA != err)
	{
		ret = DCI_ERROR;
		goto end;
	}

	if(err == DCI_NO_DATA)
	{
		ret = DCI_ERROR;
		goto end;
	}	

	printf("value_id %d\n", value_id);

end:
	if(pStmt)
		err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
	pStmt = NULL;
	
	err = disconnect(true);
	test_execute("DROP TABLE T_FLOATBINDBYINT");

	return ret;
}

sword 
test_bug21728()
{
	DCIStmt	*pStmt;
	sword	err = DCI_SUCCESS, ret = DCI_SUCCESS;
	char	exec_buf[SQL_LEN];
	ub4 	parmcnt = 0;

	ub4     i;
	DCIParam *colhd = (DCIParam *) 0;

	err = test_execute("DROP TABLE IF EXISTS TEST_BUG21728;");
	err = test_execute("CREATE TABLE TEST_BUG21728(a NUMERIC(10, 2), b VARCHAR(20) NOT NULL);");

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect fail\n");
		return DCI_ERROR;
	}

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (const ub4)DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("DCIHandleAlloc fail\n");
		goto end;
	}

	memset(exec_buf, 0, SQL_LEN);
	sprintf(exec_buf, "SELECT * FROM TEST_BUG21728");
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("DCIStmtPrepare fail\n");
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("DCIStmtExecute fail\n");
		goto free_resource;
	}

	/* Get the number of columns in the select list */
	err = DCIAttrGet((void *)pStmt, DCI_HTYPE_STMT, (void *)&parmcnt, (ub4 *)0, DCI_ATTR_PARAM_COUNT, pError);

	/* go through the column list and retrieve the datatype of each column. We start from pos = 1 */
	printf("TYPE\tNAME\tLENGHT\tPRECISION\tSCALE\tNULLABLE\n");
	for (i = 1; i <= parmcnt; i++)
	{
		DciText *name;
		ub2 field_type = 0;

		/* ub4 field_len, len; */		
		ub2 field_len;
		ub4 len;
		sb1 field_scale = 0;
		sb2 field_precision = 0;
		ub1 isNULL = 0;

		/* get parameter for column i */
		DCIParamGet((void *)pStmt, DCI_HTYPE_STMT, pError, (void **)&colhd, i);

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
		DCIAttrGet((void *)colhd, DCI_DTYPE_PARAM, (void *)&isNULL, (ub4 *)&len, DCI_ATTR_IS_NULL, pError);

		printf("%d\t%s\t%d\t%d\t\t%d\t%d\n", field_type, name, field_len, field_precision, field_scale, isNULL);

		if (i == 1 && isNULL != 1)
			ret = DCI_ERROR;
		else if (i == 2 && isNULL != 0)
			ret = DCI_ERROR;

		DCIDescriptorFree((void *)colhd, (const ub4)DCI_DTYPE_PARAM);
	}
free_resource:
	
	err = DCIHandleFree((void *)pStmt, (ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree fail\n");
		ret = DCI_ERROR;
	}

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
	{
		printf("disconnect fail\n");
		ret = DCI_ERROR;
	}

	return ret;	
}

sword test_select_end_colon_r_n()
{
	/* bug#19886: synchronize from 613. */
	sword ret = DCI_SUCCESS;
	DCIStmt *stmthp = NULL;
	int count = 0;
	DCIDefine *define = NULL;
	DciText *mySelect = (DciText*)"select count(*) from (select 1 from dual) T ; \n \r";
	ub2 data_type = 0;
	ub4 len = 0;
	DCIParam *colhd = NULL;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	ret = DCIHandleAlloc(pEnv, (void**)&stmthp, DCI_HTYPE_STMT, (const size_t)0, (void**)NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	ret = DCIStmtPrepare(stmthp, pError, mySelect, (ub4)strlen((char*)mySelect), DCI_NTV_SYNTAX, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtPrepare DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	ret = DCIStmtExecute(pSvcCtx, stmthp, pError, 0, 0, NULL, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtExecute DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	ret = DCIParamGet((void *)stmthp, DCI_HTYPE_STMT, pError, (void**)&colhd, 1);
	if (err != DCI_SUCCESS)
	{
		printf("DCIParamGet DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	ret = DCIAttrGet((const void*)colhd, DCI_DTYPE_PARAM, (void*)&data_type, (ub4 *)&len, DCI_ATTR_DATA_TYPE, pError);
	if (err != DCI_SUCCESS)
	{
		printf("DCIAttrGet DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	if(!data_type)
	{
		ret = DCI_ERROR;
		goto end;	
	}

	ret = DCIDefineByPos(stmthp, &define, pError, 1, (void*)&count, sizeof(int), SQLT_INT, (void*)NULL, (ub2*)NULL, (ub2*)NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIDefineByPos DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	ret = DCIStmtFetch(stmthp, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIStmtFetch DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	if(1 != count)
	{
		ret = DCI_ERROR;
		goto end;	
	}	

end:
	err = DCIDescriptorFree((void *)colhd, (const ub4)DCI_DTYPE_PARAM); 
	if (err != DCI_SUCCESS) 
	{		
		printf("DCIDescriptorFree colhd DCI_HTYPE_STMT failed\n");
	}
	
	if(stmthp)
		DCIHandleFree(stmthp, DCI_HTYPE_STMT);

	err = disconnect(true);
	return ret;
}

sword 
test_bug22130()
{
	DCIStmt	*pStmt;
	sword	err = DCI_SUCCESS, ret = DCI_SUCCESS;

	// 20
	char sql[] = {"begin\nupdate test_bug22130 set id='2' where id='1';\nupdate test_bug22130 set id='3' where id='2';\nupdate test_bug22130 set id='4' where id='3';\nupdate test_bug22130 set id='5' where id='4';\nupdate test_bug22130 set id='6' where id='5';\nupdate test_bug22130 set id='7' where id='6';\nupdate test_bug22130 set id='7' where id='6';\nupdate test_bug22130 set id='7' where id='6';\nupdate test_bug22130 set id='7' where id='6';\nupdate test_bug22130 set id='7' where id='6';\nupdate test_bug22130 set id='7' where id='6';\nupdate test_bug22130 set id='7' where id='6';\nupdate test_bug22130 set id='7' where id='6';\nupdate test_bug22130 set id='7' where id='6';\nupdate test_bug22130 set id='7' where id='6';\nupdate test_bug22130 set id='7' where id='6';\nupdate test_bug22130 set id='7' where id='6';\nupdate test_bug22130 set id='7' where id='6';\nupdate test_bug22130 set id='7' where id='6';\nupdate test_bug22130 set id='7' where id='6';\nend;\n"};
	// 5
	char sql2[] = {"begin\nupdate test_bug22130 set id='2' where id='1';\nupdate test_bug22130 set id='3' where id='2';\nupdate test_bug22130 set id='4' where id='3';\nupdate test_bug22130 set id='5' where id='4';\nupdate test_bug22130 set id='6' where id='5';\nend;\n"};
	char buffer[1024] = "";

	err = test_execute("DROP TABLE IF EXISTS TEST_BUG22130;");
	err = test_execute("CREATE TABLE TEST_BUG22130(ID TEXT);");

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect fail\n");
		return DCI_ERROR;
	}

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (const ub4)DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("DCIHandleAlloc fail\n");
		goto end;
	}

	strcpy(buffer, sql);
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)buffer, (ub4)strlen(buffer), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("DCIStmtPrepare fail\n");
		goto free_resource;
	}

	/* will not break the while loop. */
	strcpy(buffer, sql2);
	err = DCIStmtPrepare(pStmt, pError, (const DciText *)buffer, (ub4)strlen(buffer), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		printf("DCIStmtPrepare fail\n");
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((void *)pStmt, (ub4)DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
{
		printf("DCIHandleFree fail\n");
		ret = DCI_ERROR;
	}

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
	{
		printf("disconnect fail\n");
		ret = DCI_ERROR;
}

	return ret;
}
sword 
test_bug22490_inner(const char* datatypestr)
{
	DCIStmt *hstmt;
	DciText sql[1024];
	sword	ret = DCI_ERROR;
	DCIDefine   *bndhp[1] ;
	DCIDate		*dt = NULL;
	if(connect(true) == DCI_ERROR)
	{
		printf("Connect failed.\n");
		return DCI_ERROR;
	}

	snprintf((char*)sql, sizeof(sql), "select cast(null as %s);", datatypestr);

	err = DCIHandleAlloc((void *)pEnv, (void **)&hstmt, (const ub4)DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc fail\n");
		disconnect(true);
		return DCI_ERROR;
	}

	if (DCIStmtPrepare(hstmt, pError, (DciText*)sql, (ub4)strlen((char *)sql),
		(ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		report_error(pError);
		goto end;
	}

	dt = (DCIDate *) malloc(8);

	sb2 ind;
	if (DCIDefineByPos(hstmt, &bndhp[0], pError, 1,
		(dvoid *)dt, (sb4)sizeof(DCIDate), (ub2)SQLT_DAT,
		(dvoid *)&ind, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error(pError);
		goto end;
	}

	if (DCIStmtExecute(pSvcCtx, hstmt, pError, (ub4)0, (ub4) 0,
		(CONST DCISnapshot*)0, (DCISnapshot*) 0,
		(ub4) DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtExecute() select\n");
		report_error(pError);
		goto end;
	}
	DCIParam    *colhd;
	if(DCIParamGet(hstmt, DCI_HTYPE_STMT, pError, (void **) &colhd, 1))
	{
		report_error(pError);
		goto end;
	}
	ub2 dsize;
	if(DCIAttrGet(colhd, DCI_DTYPE_PARAM, &dsize, 0, DCI_ATTR_DATA_SIZE, pError))
	{
		report_error(pError);
		goto end;
	}
	sb2 pre;
	if(DCIAttrGet(colhd, DCI_DTYPE_PARAM, &pre, NULL, DCI_ATTR_PRECISION, pError))
	{
		report_error(pError);
		goto end;
	}
	sb1 scale;
	if(DCIAttrGet(colhd, DCI_DTYPE_PARAM, &scale, NULL, DCI_ATTR_SCALE, pError))
	{
		report_error(pError);
		goto end;
	}

	while (true) 
	{
		sword fret = DCIStmtFetch(hstmt, pError, 1, DCI_FETCH_NEXT, 0);
		if (fret == DCI_ERROR || fret == DCI_NO_DATA)
			break;
		printf("data_size: %d \tprecision: %d \tscale: %d\tIsNull: %s\n", dsize, pre, scale, (ind == -1 ? "Yes" : "No"));

		if(ind != -1 || 7 != dsize)
			goto end;
	}
	ret = DCI_SUCCESS;
end:
	err = DCIDescriptorFree((void *)colhd, (const ub4)DCI_DTYPE_PARAM);	
	if (err != DCI_SUCCESS)	
	{		
		ret = DCI_ERROR;	
	}
	
	if(dt)
		free(dt);
	DCIHandleFree(hstmt, DCI_HTYPE_STMT);
	disconnect(true);
	return ret;
}
sword 
test_bug22490()
{
	char * datatypes[] = {
		"timestamp(0)",
		"timestamp(1)",
		"timestamp(2)",
		"timestamp(3)",	
		"timestamp(4)",
		"timestamp(5)",
		"timestamp(6)",
		"timestamp(0) with time zone",
		"timestamp(1) with time zone",
		"timestamp(2) with time zone",
		"timestamp(3) with time zone",	
		"timestamp(4) with time zone",
		"timestamp(5) with time zone",
		"timestamp(6) with time zone"
	};
	size_t i = 0;

	for(i = 0; i < sizeof(datatypes)/sizeof(datatypes[0]); i++)
	{
		if(DCI_ERROR == test_bug22490_inner(datatypes[i]))
			return DCI_ERROR;
	}
	return DCI_SUCCESS;
}

sword
test_bug22502_inner(int argpre, int argsca)
{
	DCIStmt			*hstmt;
	DciText			sql[1024];
	sword			ret = DCI_ERROR;
	DCIDefine		*bndhp[1] ;
	DCINumber		*nm = NULL;
	char			intpart[40];
	char			fracpart[40];
	char			testnum[40];
	DCIParam    *colhd = NULL;
	memset(intpart, '9', sizeof(intpart));
	memset(fracpart, '9', sizeof(fracpart));

	if (argpre > (int)sizeof(intpart) - 1 ||
		argsca > (int)sizeof(fracpart) - 1)
		return DCI_ERROR;
	if(argpre == 0)
	{
		snprintf((char*)sql, sizeof(sql), "select cast(99 as \"NUMERIC\");");
	}
	else
	{
		intpart[argpre - argsca] = fracpart[argsca] = 0;
	
		sprintf(testnum, "%s%s%s", intpart, strlen(fracpart) ? "." : "" , fracpart);
		snprintf((char*)sql, sizeof(sql), "select cast(%s as numeric(%d, %d));", testnum, argpre, argsca);

	}
	if(connect(true) == DCI_ERROR)
	{
		printf("Connect failed.\n");
		return DCI_ERROR;
	}


	err = DCIHandleAlloc((void *)pEnv, (void **)&hstmt, (const ub4)DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc fail\n");
		disconnect(true);
		return DCI_ERROR;
	}

	if (DCIStmtPrepare(hstmt, pError, (DciText*)sql, (ub4)strlen((char *)sql),
		(ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		report_error(pError);
		goto end;
	}

	nm = (DCINumber *) malloc(sizeof(DCINumber));

	sb2 ind;
	if (DCIDefineByPos(hstmt, &bndhp[0], pError, 1,
		(dvoid *)nm, (sb4)sizeof(DCINumber), (ub2)SQLT_NUM,
		(dvoid *)&ind, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error(pError);
		goto end;
	}

	if (DCIStmtExecute(pSvcCtx, hstmt, pError, (ub4)0, (ub4) 0,
		(CONST DCISnapshot*)0, (DCISnapshot*) 0,
		(ub4) DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtExecute() select\n");
		report_error(pError);
		goto end;
	}
	
	if(DCIParamGet(hstmt, DCI_HTYPE_STMT, pError, (void **) &colhd, 1))
	{
		report_error(pError);
		goto end;
	}
	ub2 dsize;
	if(DCIAttrGet(colhd, DCI_DTYPE_PARAM, &dsize, 0, DCI_ATTR_DATA_SIZE, pError))
	{
		report_error(pError);
		goto end;
	}
	sb2 pre;
	if(DCIAttrGet(colhd, DCI_DTYPE_PARAM, &pre, NULL, DCI_ATTR_PRECISION, pError))
	{
		report_error(pError);
		goto end;
	}
	sb1 scale;
	if(DCIAttrGet(colhd, DCI_DTYPE_PARAM, &scale, NULL, DCI_ATTR_SCALE, pError))
	{
		report_error(pError);
		goto end;
	}

	while (true) 
	{
		sword fret = DCIStmtFetch(hstmt, pError, 1, DCI_FETCH_NEXT, 0);
		if (fret == DCI_ERROR || fret == DCI_NO_DATA)
			break;
		printf("sql:%s\ndata_size: %d \tprecision: %d \tscale: %d\tIsNull: %s\n", sql, dsize, pre, scale, (ind == -1 ? "Yes" : "No"));

		if (ind == -1 || 
			sizeof(DCINumber) != dsize ||
			/* argpre == 0 means we test the type "NUMERIC" without precision or scale */
			(0 == argpre ? 38 != pre : pre != argpre) ||
			scale != argsca)
			goto end;

	}
	ret = DCI_SUCCESS;
end:
	err = DCIDescriptorFree((void *)colhd, (const ub4)DCI_DTYPE_PARAM); 
	if (err != DCI_SUCCESS) 
	{		
		printf("DCIDescriptorFree colhd DCI_HTYPE_STMT failed\n");
	}
	if(nm)
		free(nm);
	DCIHandleFree(hstmt, DCI_HTYPE_STMT);
	disconnect(true);
	return ret;
}


sword
test_bug22502()
{
	int i, j;
	for (i = 1; i <= 38; i++)
	{
		for(j = 0; j <= 38; j++)
		{
			/* scale must be less than precision. */
			if(i <= j)
				continue;
			if(DCI_ERROR == test_bug22502_inner(i, j))
			{
				printf("bug#22502 failed: precision = %d , scale = %d \n", i, j);
				return DCI_ERROR;
			}
		}
	}

	if(DCI_ERROR == test_bug22502_inner(0, 0))
	{
		printf("bug#22502 failed: \"NUMERIC\" test failed.\n");
		return DCI_ERROR;
	}

	return DCI_SUCCESS;
}

sword
test_bug22774()
{
	text * sql = (text*)"select col from bug22774 order by col";
	int		buf[1000];
	const	ub4 fetch_rows = 100;
	DCIDefine *def = NULL;
	ub4 i = 0;
	DCIStmt *stmthp = NULL;
	sword ret = DCI_ERROR;
	sb2 	ind[100];

	memset(buf, 0, sizeof(buf));
	memset(ind, -1, sizeof(ind));
	test_execute("drop table if exists bug22774");
	if (test_execute("create table bug22774(col int)"))
	{
		printf("Create table failed: ");
		report_error(pError);
		return DCI_ERROR;
	}

	if (test_execute("begin \n\
					  for i in 1..100 loop \n\
					  insert into bug22774 values(i); \n\
					  end loop; \n\
					  end;"))
	{
		printf("Insert failed : ");
		report_error(pError);
		return DCI_ERROR;
	}

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

	if (DCIStmtPrepare(stmthp, pError, sql, strlen((char*)sql), 0, 0))
	{
		printf("DCIStmtPrepare failed: ");
		report_error(pError);
		goto end;
	}

	if (DCIDefineByPos(stmthp, &def, pError, 1, buf, sizeof(buf)/fetch_rows, (ub2)SQLT_INT, (dvoid*)ind, NULL, NULL, DCI_DEFAULT))
	{
		printf("DCIDefineByPos failed: ");
		report_error(pError);
		goto end;
	}

	if (DCIDefineArrayOfStruct(def, pError, 0, 0, 0, 0))
	{
		printf("DCIDefineArrayOfStruct failed: ");
		report_error(pError);
		goto end;
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, 0, 0, (DCISnapshot*)NULL, (DCISnapshot*)NULL, 0))
	{
		printf("DCIStmtExecute failed: ");
		report_error(pError);
		goto end;
	}

	if (DCIStmtFetch(stmthp, pError, fetch_rows, DCI_FETCH_NEXT, DCI_DEFAULT))
	{
		printf("DCIStmtFetch failed: ");
		report_error(pError);
		goto end;
	}

	for(i = 1; i < sizeof(buf)/sizeof(buf[0]); i++)
	{
		if (buf[i])
		{
			goto end;
		}
	}

	for(i = 1; i < fetch_rows; i++)
	{
		if (ind[i] != -1)
		{
			goto end;
		}
	}

	memset(ind, -1, sizeof(ind));
	if (DCIStmtPrepare(stmthp, pError, sql, strlen((char*)sql), 0, 0))
	{
		printf("DCIStmtPrepare failed: ");
		report_error(pError);
		goto end;
	}

	if (DCIDefineByPos(stmthp, &def, pError, 1, buf, sizeof(int), (ub2)SQLT_INT, (dvoid*)ind, NULL, NULL, DCI_DEFAULT))
	{
		printf("DCIDefineByPos failed: ");
		report_error(pError);
		goto end;
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, 0, 0, (DCISnapshot*)NULL, (DCISnapshot*)NULL, 0))
	{
		printf("DCIStmtExecute failed: ");
		report_error(pError);
		goto end;
	}

	if (DCIStmtFetch(stmthp, pError, fetch_rows, DCI_FETCH_NEXT, DCI_DEFAULT))
	{
		printf("DCIStmtFetch failed: ");
		report_error(pError);
		goto end;
	}

	for(i = 0; i < fetch_rows; i++)
	{
		if (buf[i] != (int)(i + 1))
		{
			goto end;
		}
	}
	
	for(i = 0; i < fetch_rows; i ++)
	{
		if (ind[i] == -1)
			goto end;
	}

	ret = DCI_SUCCESS;
end:
	if (stmthp)
		DCIHandleFree(stmthp, DCI_HTYPE_STMT);
	disconnect(true);
	return ret;
}

sword
test_bug22775()
{
	DCIError *err = NULL;
	connect(true);
	if (DCIHandleAlloc(pEnv, (void**)&err, DCI_HTYPE_ERROR, 0, NULL))
	{
		printf("Alloc error handle failed.\n");
		return DCI_ERROR;
	}
	DCIStmtSetPieceInfo(NULL, DCI_HTYPE_STMT, err, NULL, NULL, 0, NULL, NULL);
	DCIHandleFree(err, DCI_HTYPE_ERROR);
	disconnect(true);
	return DCI_SUCCESS;
}

sword
test_bind_out_dml_returning_into()
{
	char in_1[100] = "qweqwe";
	ub4 out_1=0;
	DCIStmt *stmthp = NULL;
	ub4		i = 0;
	sword	ret = DCI_ERROR;

	DciText *sqldropTable = (DciText *)"drop table if exists room";
	DciText *sqlcreateTable = (DciText *)"create table room (id int,name varchar(20));";
	DciText *sqlInsert = (DciText *)"insert into room values (1,'qweqwe'),(2,'asdasd'),(3,'zxczxc');";
	DciText * sql_dml_out = (DciText*)"update room set id=id+1 where name = :in_1 returning id into :out_1 ;";
	//DciText * sql_dml_out = (DciText*)"begin update room set id=id+1 where name='qweqwe' returning id into :out_1 ; end;";

	struct {
		DciText *phname;
		ub2 dty;
		sb4 value_sz;
		dvoid* value;
	} vars[] = {
		{(DciText*)":in_1",			SQLT_CHR, strlen(in_1),		(dvoid*)in_1},
		{(DciText*)":out_1",			SQLT_INT, sizeof(out_1),		(dvoid*)&out_1}
	};


	DCIBind		*bindhp[sizeof(vars)/sizeof(vars[0])] = {NULL};
	sb2			indp[sizeof(vars)/sizeof(vars[0])];

	memset(indp, 0, sizeof(indp));

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

	//drop room table
	if (DCIStmtPrepare(stmthp, pError, sqldropTable, strlen((char*)sqldropTable), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT) ||
		DCIStmtExecute(pSvcCtx, stmthp, pError, 0, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("create room table failed: ");
		report_error(pError);
		goto end;
	}
	//create room table
	if (DCIStmtPrepare(stmthp, pError, sqlcreateTable, strlen((char*)sqlcreateTable), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT) ||
		DCIStmtExecute(pSvcCtx, stmthp, pError, 0, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("create room table failed: ");
		report_error(pError);
		goto end;
	}
	//insert values
	if (DCIStmtPrepare(stmthp, pError, sqlInsert, strlen((char*)sqlInsert), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT) ||
		DCIStmtExecute(pSvcCtx, stmthp, pError, 0, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("insert values failed: ");
		report_error(pError);
		goto end;
	}

	//get out refcursor from anonymous
	if (DCIStmtPrepare(stmthp, pError, sql_dml_out, strlen((char*)sql_dml_out), 0, 0))
	{
		printf("DCIStmtPrepare failed: ");
		report_error(pError);
		goto end;
	}

	out_1 = 9999;
	for(i = 0; i < sizeof(vars)/sizeof(vars[0]); i++)
	{
		if (DCIBindByName(stmthp, bindhp + i, pError, vars[i].phname,
			strlen((char*)(vars[i].phname)), vars[i].value, vars[i].value_sz,
			vars[i].dty, (dvoid*)&indp[i], NULL, NULL, 0, NULL, DCI_DEFAULT))
		{
			printf("DCIBindByName failed: ");
			report_error(pError);
			goto end;
		}
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4) 0, (DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4) DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed: ");
		report_error(pError);
		goto end;
	}

	if(out_1!=2){
		ret = -1;
		printf("get dml returing into failed !!!");
	}else{
		ret = 0;
		printf("get dml returing into success");
	}

end:
	if (stmthp)
		DCIHandleFree(stmthp, DCI_HTYPE_STMT);

	disconnect(true);
	return ret;
#undef CHECK_RESULTS

}


sword
test_bind_in_anonymous_block2()
{
	ub4 int_;
	DCIStmt *stmt_recursor = NULL;
	DCIStmt *stmthp = NULL;
	ub4		i = 0;
	sword	ret = DCI_ERROR;

	DciText *sqldropTable = (DciText *)"drop table if exists room";
	DciText *sqlcreateTable = (DciText *)"create table room (id int,name varchar(20));";
	DciText *sqlInsert = (DciText *)"insert into room values (1,'qweqwe'),(2,'asdasd'),(3,'zxczxc');";

	DciText *sqlprocedure = (DciText *)"create or replace procedure fa_out_refcursor(a int,c inout refcursor) as "
									"begin "
										"open c FOR SELECT * FROM room where id = a; "
									"end;";
	DciText * sql_out_refcursor = (DciText*)"begin public.fa_out_refcursor(:int_,:stmt_recursor);end;";
	//DciText * sql_out_refcursor = (DciText*)"call public.fa_out_refcursor(:int_,:stmt_recursor);";

	struct {
		DciText *phname;
		ub2 dty;
		sb4 value_sz;
		dvoid* value;
	} vars[] = {
		{(DciText*)":int_",			SQLT_INT, sizeof(int_),		(dvoid*)&int_},
		{(DciText*)":stmt_recursor",		SQLT_RSET, 8,	(dvoid*)&stmt_recursor}
	};


	DCIBind		*bindhp[sizeof(vars)/sizeof(vars[0])] = {NULL};
	sb2			indp[sizeof(vars)/sizeof(vars[0])];

	memset(indp, 0, sizeof(indp));

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

	//drop room table
	if (DCIStmtPrepare(stmthp, pError, sqldropTable, strlen((char*)sqldropTable), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT) ||
		DCIStmtExecute(pSvcCtx, stmthp, pError, 0, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("create room table failed: ");
		report_error(pError);
		goto end;
	}
	//create room table
	if (DCIStmtPrepare(stmthp, pError, sqlcreateTable, strlen((char*)sqlcreateTable), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT) ||
		DCIStmtExecute(pSvcCtx, stmthp, pError, 0, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("create room table failed: ");
		report_error(pError);
		goto end;
	}
	//insert values
	if (DCIStmtPrepare(stmthp, pError, sqlInsert, strlen((char*)sqlInsert), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT) ||
		DCIStmtExecute(pSvcCtx, stmthp, pError, 0, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("insert values failed: ");
		report_error(pError);
		goto end;
	}
	//create procedure
	if (DCIStmtPrepare(stmthp, pError, sqlprocedure, strlen((char*)sqlprocedure), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT) ||
		DCIStmtExecute(pSvcCtx, stmthp, pError, 0, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("create procedure failed: ");
		report_error(pError);
		goto end;
	}

	//get out refcursor from anonymous
	if (DCIStmtPrepare(stmthp, pError, sql_out_refcursor, strlen((char*)sql_out_refcursor), 0, 0))
	{
		printf("DCIStmtPrepare failed: ");
		report_error(pError);
		goto end;
	}

	int_ = 2;

	for(i = 0; i < sizeof(vars)/sizeof(vars[0]); i++)
	{
		if (DCIBindByName(stmthp, bindhp + i, pError, vars[i].phname,
			strlen((char*)(vars[i].phname)), vars[i].value, vars[i].value_sz,
			vars[i].dty, (dvoid*)&indp[i], NULL, NULL, 0, NULL, DCI_DEFAULT))
		{
			printf("DCIBindByName failed: ");
			report_error(pError);
			goto end;
		}
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4) 0, (DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4) DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed: ");
		report_error(pError);
		//goto end;
	}

	//get refcursor
	if (stmt_recursor)
	{
		ub4 cFetchRows = 1;
		err = Fetch(pEnv, stmt_recursor, pError, cFetchRows);
		err = DCIHandleFree((void *) stmt_recursor, DCI_HTYPE_STMT);
	}
	ret = err;

end:
	if (stmthp)
		DCIHandleFree(stmthp, DCI_HTYPE_STMT);

	disconnect(true);
	return ret;
#undef CHECK_RESULTS

}

sword 
test_bind_in_anonymous_block()
{
	DciText * sql = (DciText*)"begin select cast('123汉字拼音' as text) as text_, "
		"cast('123汉字拼音' as varchar(100 byte)) as varchar_, "
		"cast('123汉字拼音' as char(100 byte)) as char_, "
		"cast(1234567890 as number) as number_, "
		"cast(1234567890 as int) as int_, "
		"cast(1234567890 as bigint) as bigint_, "
		"cast(123 as tinyint) as tinyint_, "
		"cast(123 as smallint) as smallint_, "
		"cast(123.456000 as float) as float_, "
		"cast('2000-01-01' as date) as date_, "
		"cast('2000-01-01 12:12:12' as timestamp) as timestamp_ "
		"into "
		":text_, "
		":varchar_, "
		":char_, "
		":number_, "
		":int_, "
		":bigint_, "
		":tinyint_, "
		":smallint_, "
		":float_, "
		":date_, "
		":timestamp_ "
		" from dual; end;";

	char char_[100] = {0}, varchar_[100] = {0}, text_[100] = {0} ;
	ub1 tinyint_;
	ub2 smallint_;
	ub4 int_;
	ub8 bigint_;
	float float_ = 0;
	DCIDateTime	*date_ = NULL;
	DCIDateTime *timestamp_ = NULL;
	DCINumber	number_ = {{0}};

	DCIStmt *stmthp = NULL;
	ub4		i = 0;
	sword	ret = DCI_ERROR;
	DciText *sqlSet = (DciText *)"set compatible_level=oracle";
	const DciText *dateFmt = (DciText*)"YYYY-mm-dd HH24:MI:SS US";

	struct {
		DciText *phname;
		ub2 dty;
		sb4 value_sz;
		dvoid* value;
	} vars[] = {
		{(DciText*)":text_",		SQLT_CHR, sizeof(text_),	(dvoid*)text_},
		{(DciText*)":varchar_",		SQLT_CHR, sizeof(varchar_),	(dvoid*)varchar_},
		{(DciText*)":char_",		SQLT_CHR, sizeof(char_),	(dvoid*)char_},
		{(DciText*)":number_",		SQLT_VNU, sizeof(number_),	(dvoid*)&number_},
		{(DciText*)":int_",			SQLT_INT, sizeof(int_),		(dvoid*)&int_},
		{(DciText*)":bigint_",		SQLT_INT, sizeof(bigint_),	(dvoid*)&bigint_},
		{(DciText*)":tinyint_",		SQLT_INT, sizeof(tinyint_),	(dvoid*)&tinyint_},
		{(DciText*)":smallint_",	SQLT_INT, sizeof(smallint_),(dvoid*)&smallint_},
		{(DciText*)":float_",		SQLT_FLT, sizeof(float_),	(dvoid*)&float_},
		{(DciText*)":date_",		SQLT_DATE, sizeof(date_),	(dvoid*)&date_},
		{(DciText*)":timestamp_",	SQLT_TIMESTAMP, sizeof(timestamp_), (dvoid*)&timestamp_}
	};

	DCIBind		*bindhp[sizeof(vars)/sizeof(vars[0])] = {NULL};
	DCIDefine	*defhp[sizeof(vars)/sizeof(vars[0])] = {NULL};
	sb2			indp[sizeof(vars)/sizeof(vars[0])];

	memset(indp, -1, sizeof(indp));
#define CHECK_RESULTS()\
	do \
	{\
		char szDate[100],  szTimestamp[100];\
		ub4	bufsz;\
		int	 inumber;\
\
		if (DCINumberToInt(pError, &number_, sizeof(inumber), DCI_DEFAULT, &inumber))\
		{\
			printf("DCINumberToInt failed: ");\
			report_error(pError);\
			goto end;\
		}\
\
		if (DCIDateTimeToText(pEnv, pError, date_, dateFmt, strlen((char*)dateFmt), 4, NULL, 0, &(bufsz = sizeof(szDate)), (DciText*)szDate) ||\
			DCIDateTimeToText(pEnv, pError, timestamp_, dateFmt, strlen((char*)dateFmt), 4, NULL, 0, &(bufsz = sizeof(szTimestamp)), (DciText*)szTimestamp))\
		{\
			printf("DCIDateTimeToText failed: ");\
			report_error(pError);\
			goto end;\
		}\
	\
		if (1234567890 != int_ ||\
			1234567890 != bigint_ ||\
			1234567890 != inumber ||\
			123 != smallint_ ||\
			123 != tinyint_ ||\
			((123.45600f - float_) > 0.0001 && (123.45600f - float_) < -0.0001) ||\
			strncmp("123汉字拼音", text_, strlen("123汉字拼音")) ||\
			strncmp("123汉字拼音", varchar_, strlen("123汉字拼音")) ||\
			strncmp("123汉字拼音", char_, strlen("123汉字拼音")) ||\
			strcmp("2000-01-01 00:00:00 0000004", szDate) ||\
			strcmp("2000-01-01 12:12:12 0000004", szTimestamp)\
			)\
		{\
			printf("int: %d\nbigint: %d\ninumber: %d\nsmallint: %d\ntinyint: %d\nfloat: %d\n"\
				"text: %d\nvarchar: %d\nchar: %d\ndate: %d\ntimestamp: %d\n", \
				1234567890 != int_, \
				1234567890 != bigint_, \
				1234567890 != inumber, \
				123 != smallint_, \
				123 != tinyint_, \
				((123.45600f - float_) > 0.0001 && (123.45600f - float_) < -0.0001),\
				strncmp("123汉字拼音", text_, strlen("123汉字拼音")) == 0,\
				strncmp("123汉字拼音", varchar_, strlen("123汉字拼音")) == 0,\
				strncmp("123汉字拼音", char_, strlen("123汉字拼音")) == 0, \
				strcmp("2000-01-01 00:00:00 0000004", szDate) == 0, \
				strcmp("2000-01-01 12:12:12 0000004", szTimestamp) == 0\
				); \
			goto end;\
		}\
	} while(0)

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
/*
	if (DCIStmtPrepare(stmthp, pError, sqlSet, strlen((char*)sqlSet), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT) ||
		DCIStmtExecute(pSvcCtx, stmthp, pError, 0, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("set compatible_level failed: ");
		report_error(pError);
		goto end;
	}
*/
	if (DCIStmtPrepare(stmthp, pError, sql, strlen((char*)sql), 0, 0))
	{
		printf("DCIStmtPrepare failed: ");
		report_error(pError);
		goto end;
	}

	for (i = 0; i < sizeof(vars)/sizeof(vars[0]); i++)
	{
		switch(vars[i].dty)
		{
		case SQLT_TIMESTAMP:
			DCIDescriptorAlloc(pEnv, (dvoid**)vars[i].value, (ub4)DCI_DTYPE_TIMESTAMP, (size_t)0, (dvoid**)0);
			break;
		case SQLT_DATE:
			DCIDescriptorAlloc(pEnv, (dvoid**)vars[i].value, (ub4)DCI_DTYPE_DATE, (size_t)0, (dvoid**)0);
			break;
		default:
			break;
		}
	}


	for(i = 0; i < sizeof(vars)/sizeof(vars[0]); i++)
	{
		if (DCIBindByName(stmthp, bindhp + i, pError, vars[i].phname,
			strlen((char*)(vars[i].phname)), vars[i].value, vars[i].value_sz, 
			vars[i].dty, (dvoid*)&indp[i], NULL, NULL, 0, NULL, DCI_DEFAULT))
		{
			printf("DCIBindByName failed: ");
			report_error(pError);
			goto end;
		}
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4) 0, (DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4) DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed: ");
		report_error(pError);
		goto end;
	}


	CHECK_RESULTS();

	memset(indp, 0, sizeof(indp));
	execute("drop table if exists TestAnonymous");
	execute("create table TestAnonymous(text_ text, varchar_ varchar(100 byte), char_ char(100 byte), number_ number(38, 15), int_ int, bigint_ bigint, tinyint_ tinyint, smallint_ smallint, float_ float, date_ date, timestamp_ timestamp);");

	sql = (DciText*)"begin insert into TestAnonymous(text_, varchar_, char_, number_, int_, bigint_, tinyint_, smallint_, float_, date_, timestamp_) values"
		"(:text_, :varchar_, :char_, :number_, :int_, :bigint_, :tinyint_, :smallint_, :float_, :date_, :timestamp_); end;";

	if (DCIStmtPrepare(stmthp, pError, sql, strlen((char*)sql), 0, 0))
	{
		printf("DCIStmtPrepare failed: ");
		report_error(pError);
		goto end;
	}

	for(i = 0; i < sizeof(vars)/sizeof(vars[0]); i++)
	{
		if (DCIBindByName(stmthp,(bindhp + i), pError, vars[i].phname,
			strlen((char*)(vars[i].phname)), vars[i].value, vars[i].value_sz, 
			vars[i].dty, (dvoid*)&indp[i], NULL, NULL, 0, NULL, DCI_DEFAULT))
		{
			printf("DCIBindByName failed: ");
			report_error(pError);
			goto end;
		}
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4) 0, (DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4) DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed: ");
		report_error(pError);
		goto end;
	}

	CHECK_RESULTS();

	sql = (DciText *)"select text_, varchar_, char_, number_, int_, bigint_, tinyint_, smallint_, float_, date_, timestamp_ from TestAnonymous;";
	if (DCIStmtPrepare(stmthp, pError, sql, strlen((char*)sql), 0, 0))
	{
		printf("DCIStmtPrepare failed: ");
		report_error(pError);
		goto end;
	}

	bigint_ = int_ = smallint_ = tinyint_ = *text_ = *varchar_ = *char_ = 0;
	float_ = 0;

	memset(&number_, 0, sizeof(number_));

	for (i = 0; i < sizeof(vars)/sizeof(vars[0]); i++)
	{
		switch(vars[i].dty)
		{
		case SQLT_TIMESTAMP:
			DCIDescriptorFree(*((DCIDateTime**)vars[i].value), (ub4)DCI_DTYPE_TIMESTAMP);
			DCIDescriptorAlloc(pEnv, (dvoid**)vars[i].value, (ub4)DCI_DTYPE_TIMESTAMP, (size_t)0, (dvoid**)0);
			break;
		case SQLT_DATE:
			DCIDescriptorFree(*((DCIDateTime**)vars[i].value), (ub4)DCI_DTYPE_DATE);
			DCIDescriptorAlloc(pEnv, (dvoid**)vars[i].value, (ub4)DCI_DTYPE_DATE, (size_t)0, (dvoid**)0);
			break;
		default:
			break;
		}
	}

	for(i = 0; i < sizeof(vars)/sizeof(vars[0]); i++)
	{
		if (DCIDefineByPos(stmthp, (defhp + i), pError, i + 1, vars[i].value, vars[i].value_sz, 
			vars[i].dty, indp + i, NULL, NULL, DCI_DEFAULT))
		{
			printf("DCIDefineByPos failed: ");
			report_error(pError);
			goto end;
		}
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)0, (ub4) 0, (DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4) DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed: ");
		report_error(pError);
		goto end;
	}

	if (DCI_ERROR == DCIStmtFetch(stmthp, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT))
	{
		printf("DCIStmtFetch failed: ");
		report_error(pError);
		goto end;
	}

	CHECK_RESULTS();

	memset(indp, -1, sizeof(indp));

	sql = (DciText*)"begin insert into TestAnonymous(text_, varchar_, char_, number_, int_, bigint_, tinyint_, smallint_, float_, date_, timestamp_) values"
		"(:text_, :varchar_, :char_, :number_, :int_, :bigint_, :tinyint_, :smallint_, :float_, :date_, :timestamp_); end;";

	if (DCIStmtPrepare(stmthp, pError, sql, strlen((char*)sql), 0, 0))
	{
		printf("DCIStmtPrepare failed: ");
		report_error(pError);
		goto end;
	}

	for(i = 0; i < sizeof(vars)/sizeof(vars[0]); i++)
	{
		if (DCIBindByName(stmthp,(bindhp + i), pError, vars[i].phname,
			strlen((char*)(vars[i].phname)), vars[i].value, vars[i].value_sz, 
			vars[i].dty, (dvoid*)&indp[i], NULL, NULL, 0, NULL, DCI_DEFAULT))
		{
			printf("DCIBindByName failed: ");
			report_error(pError);
			goto end;
		}
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4) 0, (DCISnapshot *) NULL, (DCISnapshot *) NULL, (ub4) DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed: ");
		report_error(pError);
		goto end;
	}

	for (i = 0; i < sizeof(indp)/sizeof(indp[0]); i++)
	{
		if (indp[i] != -1)
			goto end;
	}
	ret = DCI_SUCCESS;
end:
	if (stmthp)
		DCIHandleFree(stmthp, DCI_HTYPE_STMT);

	for (i = 0; i < sizeof(vars)/sizeof(vars[0]); i++)
	{
		switch(vars[i].dty)
		{
		case SQLT_TIMESTAMP:
			DCIDescriptorFree(*((DCIDateTime**)vars[i].value), (ub4)DCI_DTYPE_TIMESTAMP);
			break;
		case SQLT_DATE:
			DCIDescriptorFree(*((DCIDateTime**)vars[i].value), (ub4)DCI_DTYPE_DATE);
			break;
		default:
			break;
		}
	}
	disconnect(true);
	return ret;
#undef CHECK_RESULTS

}
typedef struct vbi{
	ub2 len;
	ub1 content[2000];
}vbi;

#define MAX_ROW 10
#define MAX_ROW_SZ sizeof(vbi)
sword test_bug23012()
{
	text    *sqlstmt = (text*)"insert into bug23012 values(:1, :2)";
	DCIDefine 	*defp;
	DCIBind		*bindp[2];
	ub4 	i;
	sb2		ind[MAX_ROW];
	vbi		vbiin[MAX_ROW];
	vbi 	vbiout[MAX_ROW];
	ub4		vbisz = sizeof(vbiin);
	int		id[MAX_ROW];
	DCIStmt	*stmthp = NULL;
	sword	ret = DCI_ERROR;
	
	if (connect(true))
		return DCI_ERROR;

	memset (vbiin, 'A',vbisz);
	memset (vbiout, 1,vbisz);
	for(i = 0 ; i < MAX_ROW; i ++)
	{
		vbiin[i].len = (sizeof(vbiin[0].content))/(i+1);
		id[i] = i;
	}

	memset(ind, 0, sizeof(ind));
	execute("drop table if exists bug23012");
	if (execute("create table bug23012(id number, col bytea)"))
	{
		report_error(pError);
		return DCI_ERROR;
	}

	if (DCIHandleAlloc(pEnv, (void**)&stmthp, DCI_HTYPE_STMT, 0, NULL))
	{
		printf("Failed to alloc stmt\n");
		goto end;
	}

	if (DCIStmtPrepare(stmthp, pError, sqlstmt, (ub4)strlen((char *)sqlstmt),
		(ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		report_error(pError);
		goto end;
	}
	
	if (DCIBindByPos(stmthp, &bindp[0], pError, 2, vbiin, MAX_ROW_SZ, SQLT_VBI, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT))
	{
		printf("DCIBindByPos failed:");
		report_error(pError);
		goto end;
	}	
	if (DCIBindByPos(stmthp, &bindp[1], pError, 1, &id, sizeof(id[0]), SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT))
	{
		printf("DCIBindByPos failed:");
		report_error(pError);
		goto end;
	}
/*	if (DCIBindArrayOfStruct(bindp, pError, MAX_ROW_SZ, 0, 0, 0))
	{
		printf("DCIBindArrayOfStruct failed:");
		report_error(pError);
		return DCI_ERROR;
	}
*/
	if (DCIStmtExecute(pSvcCtx, stmthp, pError, MAX_ROW, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed");
		report_error(pError);
		goto end;
	}
	
	if (DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCITransCommit() select\n");
		report_error(pError);
		goto end;
	}
	
	sqlstmt = (text*)"select col from bug23012 order by id asc";
	
	if (DCIStmtPrepare(stmthp, pError, sqlstmt, (ub4)strlen((char *)sqlstmt),
		(ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		report_error(pError);
		goto end;
	}

	if (DCIDefineByPos(stmthp, &defp, pError, 1,
		(dvoid *)vbiout, (sb4)MAX_ROW_SZ, (ub2)SQLT_VBI,
		(dvoid *)ind, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error(pError);
		goto end;
	}
	if (DCIDefineArrayOfStruct(defp, pError, MAX_ROW_SZ, 0, 0, 0))
	{
		printf("FAILED: DCIDefineArrayOfStruct()\n");
		report_error(pError);
		goto end;
	}
	if (DCIStmtExecute(pSvcCtx, stmthp, pError, MAX_ROW, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed");
		report_error(pError);
		goto end;
	}

	for(i = 0; i < MAX_ROW; i++)
	{
		ub4 j = 0;
		if (vbiin[i].len != vbiout[i].len)
		{
			printf("%d len is not the same, in: %d \t out:%d\n", i, vbiin[i].len, vbiout[i].len);
			goto end;
		}
		for (j = 0; j < vbiout[i].len; j++)
		{
			if (vbiout[i].content[j] != 'A')
			{
				printf("%d's content %d is not same, values is %d\n", i, j, vbiout[i].content[j]);
				goto end;
			}
		}
		
		for (; j < MAX_ROW_SZ -2 ; j++)
		{
			if (vbiout[i].content[j] != 1)
			{
				printf("%d's content %d is not zero, values is %d\n", i, j, vbiout[i].content[j]);
				goto end;
			}
		}
	}
	if (DCITransCommit(pSvcCtx,  pError,(ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCITransCommit() select\n");
		report_error(pError);
		goto end;
	}
	/* ---------------------------------------------------------
	   test null values	
	 ------------------------------------------------------------*/
	if (execute("truncate table bug23012") ||
		execute("insert into bug23012 values(1, NULL)"))
	{
		printf("truncate failed.\n");
		report_error(pError);
		goto end;
	}
	
	sqlstmt = (text*)"select col from bug23012 order by id asc";
	
	if (DCIStmtPrepare(stmthp, pError, sqlstmt, (ub4)strlen((char *)sqlstmt),
		(ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		report_error(pError);
		goto end;
	}
	
	memset(vbiout, 100, vbisz);

	if (DCIDefineByPos(stmthp, &defp, pError, 1,
		(dvoid *)vbiout, (sb4)MAX_ROW_SZ, (ub2)SQLT_VBI,
		(dvoid *)ind, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error(pError);
		goto end;
	}
	if (DCIDefineArrayOfStruct(defp, pError, MAX_ROW_SZ, 0, 0, 0))
	{
		printf("FAILED: DCIDefineArrayOfStruct()\n");
		report_error(pError);
		goto end;
	}
	if (DCIStmtExecute(pSvcCtx, stmthp, pError, 1, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed");
		report_error(pError);;
		goto end;
	}

	if (ind[0] != -1)
	{
		printf("NULL SQLT_VBI's ind is not -1\n");
		goto end;
	}
	
	if (vbiout[0].len != 0)
	{
		printf("NULL SQLT_VBI's len is not null\n");
		goto end;
	}

	for(i = 0; i < vbisz - sizeof(ub2); i ++)
	{
		if (vbiout[0].content[i] != 100)
		{
			printf("NULL SQLT_VBI's content has been wrote.\n");
			goto end;
		}
	}
	ret = DCI_SUCCESS;
end:
	if (stmthp)
		DCIHandleFree(stmthp, DCI_HTYPE_STMT);
	disconnect(true);
	return ret;
}

sword test_bug23027()
{
	sword ret = DCI_ERROR;
	DCIStmt *stmthp = NULL;
	char	sql[1024] = "begin :3 := :1; :4 := :2; end;";
	int		vars[4] = {1, 2, 3, 4};
	sb2		ind[4] = {0, 0, 0, 0};
	DCIBind	*bnd[4];
	ub4		i = 0;
	if (connect(true))
		return DCI_ERROR;

	if (DCIHandleAlloc(pEnv, (dvoid**)&stmthp, DCI_HTYPE_STMT, 0, NULL))
	{
		printf("DCIHandleAlloc failed.\n");
		goto err;
	}

	if (DCIStmtPrepare(stmthp, pError, (text*)sql, (ub4)strlen(sql), DCI_NTV_SYNTAX, DCI_DEFAULT))
	{
		printf("DCIStmtPrepare failed:");
		report_error(pError);
		goto err;
	}

	for (i = 0; i < sizeof(vars)/sizeof(vars[0]); i++)
	{
		if (DCIBindByPos(stmthp, &bnd[i], pError, i + 1, &vars[i], sizeof(vars[i]), SQLT_INT, (dvoid*)&ind[i], NULL, NULL, 0, NULL, DCI_DEFAULT))
		{
			printf("DCIBindByPos failed:");
			report_error(pError);
			goto err;
		}
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, 1, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed:");
		report_error(pError);
		goto err;
	}
	if (vars[3] != vars[1] || vars[2] != vars[0])
	{
		printf("Parameter not passed out\n");
		goto err;
	}

	strcpy(sql, "begin select null into :1 from dual; end;");
	if (DCIStmtPrepare(stmthp, pError, (text*)sql, (ub4)strlen(sql), DCI_NTV_SYNTAX, DCI_DEFAULT))
	{
		printf("DCIStmtPrepare failed:");
		report_error(pError);
		goto err;
	}

	if (DCIBindByPos(stmthp, &bnd[0], pError, 1, &vars[0], sizeof(vars[0]), SQLT_INT, (dvoid*)&ind[0], NULL, NULL, 0, NULL, DCI_DEFAULT))
	{
		printf("DCIBindByPos failed:");
		report_error(pError);
		goto err;
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, 1, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed:");
		report_error(pError);
		goto err;
	}
	if (ind[0] != -1)
	{
		printf("Null value failed.\n");
		goto err;
	}
	ret = DCI_SUCCESS;
err:
	if (stmthp)
	{
		DCIHandleFree(stmthp, DCI_HTYPE_STMT);
		stmthp = NULL;
	}

	disconnect(true);

	return ret;
}
/* bug#23066: DCI can't get oids' datatype and datasize. */
sword test_bug23066()
{
	sword ret = DCI_ERROR;
	DCIStmt *stmthp = NULL;
	char	sql[1024] = "select cast(1024 as oid) as col from dual";
	ub2		datatype;
	ub4 	datasize = 0, len = 0;
	DCIParam *colhd = (DCIParam *) 0;   /* column handle */
	
	if (connect(true))
		return DCI_ERROR;
	if (DCIHandleAlloc(pEnv, (dvoid**)&stmthp, DCI_HTYPE_STMT, 0, NULL))
	{
		printf("DCIHandleAlloc failed.\n");
		goto err;
	}

	if (DCIStmtPrepare(stmthp, pError, (text*)sql, (ub4)strlen(sql), DCI_NTV_SYNTAX, DCI_DEFAULT))
	{
		printf("DCIStmtPrepare failed:");
		report_error(pError);
		goto err;
	}
	
	if (DCIStmtExecute(pSvcCtx, stmthp, pError, 1, 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("DCIStmtExecute failed:");
		report_error(pError);
		goto err;
	}
	
	if (DCIParamGet((void *)stmthp, DCI_HTYPE_STMT, pError, (void **)&colhd, 1))
	{
		printf("DCIParamGet Failed:");
		report_error(pError);
		goto err;
	}
	
	if (DCIAttrGet((void *)colhd, DCI_DTYPE_PARAM, (void *)&datatype, (ub4 *)&len, DCI_ATTR_DATA_TYPE, pError))
	{
		printf("DCIAttrGet DCI_ATTR_DATA_TYPE failed:");
		report_error(pError);
		goto err;
	}
	
	if (DCIAttrGet((void *)colhd, DCI_DTYPE_PARAM, (void *)&datasize, (ub4 *)&len, DCI_ATTR_DATA_SIZE, pError))
	{
		printf("DCIAttrGet DCI_ATTR_DATA_SIZE failed:");
		report_error(pError);
		goto err;
	}

	printf("OID's type code is %d, and data size is %d\n", datatype, datasize);
	
	if (datatype != SQLT_NUM || datasize != sizeof(DCINumber))
		goto err;
	
	ret = DCI_SUCCESS;
err:
	err = DCIDescriptorFree((void *)colhd, (const ub4)DCI_DTYPE_PARAM); 
	if (err != DCI_SUCCESS) 
	{		
		printf("DCIDescriptorFree colhd DCI_HTYPE_STMT failed\n");
	}
	if (stmthp)
	{
		DCIHandleFree(stmthp, DCI_HTYPE_STMT);
		stmthp = NULL;
	}

	disconnect(true);

	return ret;	
}

sword test_bug23073()
{
	DCIInterval *intval[3] = {NULL};
	DCIStmt *stmthp = NULL;
	DCIBind *bnd = NULL;
	char *intvstr[3] = {" 0 0: 2: 3", "-999999999 23:59:59.999999", "999999999 23:59:59.999999"};
	char *sql = NULL;
	sword ret = DCI_ERROR;

	DCIDefine *def = NULL;
	char	buf[3][100];
	ub4 i = 0;

	test_execute("drop table if exists test_bug23073");
	//test_execute("create table test_bug23073(id int identity, col interval day(9) to second(6))");
	test_execute("create table test_bug23073(id serial, col interval day to second)");
	if (connect(true))
		return DCI_ERROR;
	if (DCIHandleAlloc(pEnv, (dvoid**)&stmthp, DCI_HTYPE_STMT, 0, NULL))
	{
		printf("Failed to alloc stmt\n");
		goto err;
	}

	for (i = 0; i < sizeof(intval)/sizeof(intval[0]); i ++)
	{
		if (DCIDescriptorAlloc(pEnv, (dvoid**)&intval[i], DCI_DTYPE_INTERVAL_DS, 0, NULL))
		{
			printf("Failed to alloc interval\n");
			goto err;
		}
	}

	for (i = 0; i < sizeof(intval)/sizeof(intval[0]); i ++)
	{
		if (DCIIntervalFromText(pEnv, pError, (text*)intvstr[i], strlen(intvstr[i]), intval[i]))
		{
			printf("Failed to convert text into interval\n");
			report_error(pError);
			goto err;
		}
	}

	sql = "insert into test_bug23073(col) values(:1) ";

	if (DCIStmtPrepare(stmthp, pError, (text*)sql, strlen(sql), DCI_NTV_SYNTAX, DCI_DEFAULT))
	{
		printf("Failed to prepare:");
		report_error(pError);
		goto err;
	}

	if (DCIBindByPos(stmthp, &bnd, pError, 1, &intval, sizeof(intval[0]), SQLT_INTERVAL_DS, NULL, NULL, NULL, 0, NULL, 0))
	{
		printf("Failed to bind:");
		report_error(pError);
		goto err;
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, sizeof(intval)/sizeof(intval[0]), 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("Failed to execute:");
		report_error(pError);
		goto err;
	}

	sql = "select col from test_bug23073 order by id asc";

	if (DCIStmtPrepare(stmthp, pError, (text*)sql, strlen(sql), DCI_NTV_SYNTAX, DCI_DEFAULT))
	{
		printf("Failed to prepare:");
		report_error(pError);
		goto err;
	}

	if (DCIDefineByPos(stmthp, &def, pError, 1, &buf, sizeof(buf[0]), SQLT_STR, NULL, NULL, NULL, 0))
	{
		printf("Failed to define by pos:");
		report_error(pError);
		goto err;
	}


	if (DCIStmtExecute(pSvcCtx, stmthp, pError, sizeof(intval)/sizeof(intval[0]), 0, NULL, NULL, DCI_DEFAULT))
	{
		printf("Failed to execute:");
		report_error(pError);
		goto err;
	}

	if //(strcmp("+999999999 23:59:59.999999", buf[2]) ||
		//strcmp("+000000000 00:02:03.000000", buf[0]) ||
		(strcmp("999999999 23:59:59.999999", buf[2]) ||
		strcmp("0:02:03.000000", buf[0]) ||
		strcmp("-999999999 23:59:59.999999", buf[1]))
		goto err;

	ret =  DCI_SUCCESS;
err:
	if (stmthp)
		DCIHandleFree(stmthp, DCI_HTYPE_STMT);
	for (i = 0; i < sizeof(intval)/sizeof(intval[0]); i ++)
	{
		if (intval[i])
			DCIDescriptorFree(intval[i], DCI_DTYPE_INTERVAL_DS);
	}
	disconnect(true);
	return ret;
}

sword 
test_bug23191()
{
	DCIDuration		dur;
	sword			ret = DCI_ERROR;

	if (connect(true))
		return ret;

	if (DCIDurationBegin(pEnv, pError, pSvcCtx, DCI_DURATION_SESSION, &dur))
	{
		printf("ERROR: Failed to begin duration.\n");
		disconnect(true);
		goto free_resource;
	}

	disconnect(true);

	DCIDurationEnd(pEnv, pError, pSvcCtx, dur);

	ret = DCI_SUCCESS;

free_resource:
	return ret;
}

sword test_bug24038()
{
    text    *sqlstmt = (text*)"insert into test_bug24038 values(:bind1, :bind2, :bind3, :bind4);";
    DCIBind *pBind = NULL;
    DCIStmt *stmthp = NULL;
    int     ins_buf[MAX_COLUMN_NUM] = {14, 15, 16, 17};
    ub4     size = 4;
    ub4     startloc = 1;
    sb4     found = 1;
    text    *bvnp[10] = {NULL};
    ub1     bvnl[10];
    text    *invp[10] = {NULL};
    ub1     inpl[10];
    ub1     dupl[10];
    DCIBind *hndl;
    sword ret = DCI_ERROR;
    unsigned int     i;
    DciText bind_name[][6] = {"bind1", "bind2", "bind3", "bind4"};

    if (connect(true))
        return DCI_ERROR;

    if (DCIHandleAlloc(pEnv, (dvoid**)&stmthp, DCI_HTYPE_STMT, 0, NULL))
    {
        printf("Failed to alloc stmt\n");
        goto err;
    }

    execute("drop table if exists test_bug24038");
    execute("create table test_bug24038(a int, b int, c int, d int)");

    if (DCIStmtPrepare(stmthp, pError, sqlstmt, (ub4)strlen((char *)sqlstmt),
            (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
    {
            printf("FAILED: DCIStmtPrepare() select\n");
            report_error(pError);
            return DCI_ERROR;
    }

    if (DCIBindByPos(stmthp, &pBind, pError, 1, &ins_buf[0], sizeof(int), SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT))
    {
            printf("DCIBindByPos failed:");
            report_error(pError);
            return DCI_ERROR;
    }

    if (DCIBindByPos(stmthp, &pBind, pError, 2, &ins_buf[1], sizeof(int), SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT))
    {
            printf("DCIBindByPos failed:");
                report_error(pError);
                return DCI_ERROR;
    }

    if (DCIBindByPos(stmthp, &pBind, pError, 3, &ins_buf[2], sizeof(int), SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT))
    {
            printf("DCIBindByPos failed:");
            report_error(pError);
            return DCI_ERROR;
    }

    if (DCIBindByPos(stmthp, &pBind, pError, 4, &ins_buf[3], sizeof(int), SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT))
    {
            printf("DCIBindByPos failed:");
            report_error(pError);
            return DCI_ERROR;
    }

    ret = DCIStmtGetBindInfo(stmthp, pError,size,startloc,&found,bvnp,bvnl,invp,inpl,dupl,&hndl);
    if (ret == DCI_ERROR)
    {
            printf("FAILED: DCIStmtGetBindInfo\n");
            report_error(pError);
            return DCI_ERROR;
    }

    if (DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4)0,
            (CONST DCISnapshot*)0, (DCISnapshot*)0,
            (ub4)DCI_DEFAULT))
    {
            printf("FAILED: DCIStmtExecute() select\n");
            report_error(pError);
            return DCI_ERROR;
    }

    for (i = 0; i < size; ++i)
    {
    	if(strcmp((char *)bvnp[i+startloc-1], (char *)bind_name[i+startloc-1]))
    		goto err;
    }

    ret = DCI_SUCCESS;

err:
    if (stmthp)
        DCIHandleFree(stmthp, DCI_HTYPE_STMT);
	
    disconnect(true);
    return ret;
}

sword test_bug24040()
{
	DCIInterval *intval_ds[4] = {NULL};
	DCIInterval *intval_ym[4] = {NULL};
	char *intvstr_ds[2] = {"-2015 23:40:20.666666", "2016 13:24:22.999999"};
	char *intvstr_ym[2] = {"-1920-8", "2019-1"};
	DciText	buf_ds[4][100];
	DciText	buf_ym[4][100];	
	size_t resultlen;
	sword ret = DCI_ERROR;
	sword connected;
	ub4 i = 0;
	sb4 year=2016,month=8,day=5056,hour=22,minute=6,second=11,fsecond=2323;

	connected =  connect(true);

	for (i = 0; i < sizeof(intval_ds)/sizeof(intval_ds[0]); i ++)
	{
		if (DCIDescriptorAlloc(pEnv, (dvoid**)&intval_ds[i], DCI_DTYPE_INTERVAL_DS, 0, NULL))
		{
			printf("Failed to alloc interval\n");
			goto err;
		}
	}

	for (i = 0; i < sizeof(intval_ds)/sizeof(intval_ym[0]); i ++)
	{
		if (DCIDescriptorAlloc(pEnv, (dvoid**)&intval_ym[i], DCI_DTYPE_INTERVAL_YM, 0, NULL))
		{
			printf("Failed to alloc interval\n");
			goto err;
		}
	}	

	for (i = 0; i < 2; i ++)
	{
		if (DCIIntervalFromText(pEnv, pError, (text*)intvstr_ds[i], strlen(intvstr_ds[i]), intval_ds[i]))
		{
			printf("Failed to convert text into interval\n");
			report_error(pError);
			goto err;
		}
	}

	for (i = 0; i < 2; i ++)
	{
		if (DCIIntervalFromText(pEnv, pError, (text*)intvstr_ym[i], strlen(intvstr_ym[i]), intval_ym[i]))
		{
			printf("Failed to convert text into interval\n");
			report_error(pError);
			goto err;
		}
	}

	if(DCIIntervalAdd(pEnv,pError,intval_ds[0],intval_ds[1],intval_ds[2]))
	{
		printf("Failed DCIIntervalAdd\n");
		report_error(pError);
		goto err;
	}

	if(DCIIntervalAssign(pEnv,pError,intval_ds[0],intval_ds[3]))
	{
		printf("Failed DCIIntervalAssign\n");
		report_error(pError);
		goto err;
	}

	if(DCIIntervalAdd(pEnv,pError,intval_ym[0],intval_ym[1],intval_ym[2]))
	{
		printf("Failed DCIIntervalAdd\n");
		report_error(pError);
		goto err;
	}

	if(DCIIntervalAssign(pEnv,pError,intval_ym[0],intval_ym[3]))
	{
		printf("Failed DCIIntervalAssign\n");
		report_error(pError);
		goto err;
	}

	if(DCIIntervalSetDaySecond(pEnv,pError,day,hour,minute,second,fsecond,intval_ds[0]))
	{
		printf("Failed DCIIntervalSetDaySecond\n");
		report_error(pError);
		goto err;
	}

	if(DCIIntervalSetYearMonth(pEnv,pError,year,month,intval_ym[0]))
	{
		printf("Failed DCIIntervalSetYearMonth\n");
		report_error(pError);
		goto err;
	}

	for (i = 0; i < sizeof(intval_ds)/sizeof(intval_ds[0]); i ++)
	{
		if(DCIIntervalToText(pEnv, pError, intval_ds[i], (ub1) 9, (ub1)9,
                     buf_ds[i],(ub4)MAXDATELEN,(size_t *)&resultlen))
		{
			printf("Failed DCIIntervalToText\n");
			report_error(pError);
			goto err;
		}
	}

	for (i = 0; i < sizeof(intval_ym)/sizeof(intval_ym[0]); i ++)
	{
		if(DCIIntervalToText(pEnv, pError, intval_ym[i], (ub1) 9, (ub1)1,
                     buf_ym[i],(ub4)MAXDATELEN,(size_t *)&resultlen))
		{
			printf("Failed DCIIntervalToText\n");
			report_error(pError);
			goto err;
		}
	}

	if (strcmp("-000002015 23:40:20.666666000", (char *)buf_ds[3]) ||
		strcmp("+000000000 13:44:02.333333000", (char *)buf_ds[2]) ||
		strcmp("+000005056 22:06:11.232300000", (char *)buf_ds[0]))
		goto err;		
	if (strcmp("-000001920-08", (char *)buf_ym[3]) ||
		strcmp("+000000098-05", (char *)buf_ym[2]) ||
		strcmp("+000002016-08", (char *)buf_ym[0]))
		goto err;

	ret =  DCI_SUCCESS;
err:
	for (i = 0; i < sizeof(intval_ds)/sizeof(intval_ds[0]); i ++)
	{
		if (intval_ds[i])
			DCIDescriptorFree(intval_ds[i], DCI_DTYPE_INTERVAL_DS);
	}
	for (i = 0; i < sizeof(intval_ym)/sizeof(intval_ym[0]); i ++)
	{
		if (intval_ym[i])
			DCIDescriptorFree(intval_ym[i], DCI_DTYPE_INTERVAL_YM);
	}
	if (!connected)
		disconnect(true);
	return ret;
}

sword test_bug24041()
{
	DCIDate *date[3] = {NULL};
	DCIDateTime *datetime[2] = {NULL};
	ub4 len = 30;
	ub4 len1 = 30;
	char datetime_data[] = "2014-06-11 13:28:32 08:00";
	char datetime_out[sizeof(datetime_data)];
	char datetime_out1[sizeof(datetime_data)];
	sword ret = DCI_ERROR;
	sword connected;
	ub4 i = 0;
	sb2	year = 2016, year1;
	ub1	month = 5, day = 11, hour = 15, min = 40, sec = 24;
	ub1 month1, day1, hour1, min1, sec1;

	connected =  connect(true);

	for (i = 0; i < sizeof(date)/sizeof(date[0]); i ++)
	{
		date[i] = (DCIDate *)malloc (sizeof(DCIDate));
		memset(date[i], 0, sizeof(DCIDate));
	}

	for (i = 0; i < sizeof(datetime)/sizeof(datetime[0]); i ++)
	{
		if (DCIDescriptorAlloc(pEnv, (dvoid**)&datetime[i], DCI_DTYPE_TIMESTAMP_TZ, 0, NULL))
		{
			printf("Failed to alloc datetime\n");
			goto err;
		}
	}

	DCIDateSetDate(date[0], year, month, day);

	DCIDateSetTime(date[0], hour, min, sec);

	if(DCIDateAssign(pError, date[0], date[1]))
	{
		printf("Failed DCIDateAssign\n");
		goto err;
	}

	DCIDateGetDate(date[1], &year1, &month1, &day1);

	DCIDateGetTime(date[1], &hour1, &min1, &sec1);

	if ( year != year1  ||
		month != month1 ||
		 day  != day1   ||
		 hour != hour1  ||
		 min  != min1 	 ||
		 sec  != sec1)
		goto err;

	if(DCIDateSysDate(pError, date[2]))
	{
		printf("Failed DCIDateSysDate\n");
		goto err;
	}

	if(DCIDateTimeSysTimeStamp(pEnv,pError,datetime[0]))
	{
		printf("Failed DCIDateTimeSysTimeStamp\n");
		report_error(pError);
		goto err;
	}

	if (DCIDateTimeAssign(pEnv,pError,datetime[0],datetime[1]))
	{
		printf("Failed DCIDateTimeAssign\n");
		report_error(pError);
		goto err;
	}

	if (DCIDateTimeToText(pEnv, pError, datetime[0], 0, 0, 0, NULL, 0, &len,(DciText *)datetime_out))
	{	
		printf("Failed DCIDateToText\n");
		report_error(pError);
		goto err;
	}

	if (DCIDateTimeToText(pEnv, pError, datetime[1], 0, 0, 0, NULL, 0, &len1,(DciText *)datetime_out1))
	{	
		printf("Failed DCIDateToText\n");
		report_error(pError);
		goto err;
	}

	if(strcmp(datetime_out,datetime_out1))
		goto err;

	ret =  DCI_SUCCESS;

err:
	for (i = 0; i < sizeof(date)/sizeof(date[0]); i ++)
	{
		if (date[i])
		{
			free(date[i]);
			date[i] = NULL;
		}
	}

	for (i = 0; i < sizeof(datetime)/sizeof(datetime[0]); i ++)
	{
		if (datetime[i])
			DCIDescriptorFree(datetime[i], DCI_DTYPE_TIMESTAMP_TZ);
	}
	if (!connected)
		disconnect(true);
	return ret;
}



