#include "DciTest.h"
#include "testUTF16.h"

extern char	User_UTF16[NAME_LEN * 2];
extern char	Pwd_UTF16[NAME_LEN * 2];
extern char	DbName_UTF16[NAME_LEN * 2];

sword
connect(bool useLogon)
{
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

	err = DCIInitialize((ub4) DCI_DEFAULT, (dvoid *)0, (dvoid * (*)(dvoid *, size_t)) 0, (dvoid * (*)(dvoid *, dvoid *, size_t))0, (void (*)(dvoid *, dvoid *)) 0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIInitialize failed\n");
		return err;
	} 

	err = DCIEnvInit((DCIEnv **) &pEnv, DCI_DEFAULT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIEnvInit failed\n");
		return err;
	} 

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pError, DCI_HTYPE_ERROR, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc failed\n");
		return err;
	}

#ifdef WIN32
	QueryPerformanceCounter(&start);
#else
	gettimeofday(&start_t, NULL);
#endif

	if (useLogon)
	{
		err = DCILogon (pEnv, pError, &pSvcCtx,
			(const DciText *)User, (ub4)strlen(User),
			(const DciText *)Pwd, (ub4)strlen(Pwd),
			(const DciText *)DbName, (ub4)strlen(DbName));
	}
	else
	{
		err = DCIHandleAlloc(pEnv, (dvoid **) &pServer, DCI_HTYPE_SERVER, (size_t) 0, NULL);
		err = DCIHandleAlloc(pEnv, (void **) &pSvcCtx, DCI_HTYPE_SVCCTX, 0, NULL);

		err = DCIServerAttach(pServer, pError, (DciText *) DbName, (sb4)strlen(DbName), DCI_DEFAULT);
		err = DCIAttrSet(pSvcCtx, DCI_HTYPE_SVCCTX, pServer, 0, DCI_ATTR_SERVER, pError);

		err = DCIHandleAlloc(pEnv, (dvoid **) &pSession, (ub4) DCI_HTYPE_SESSION, (size_t) 0, NULL);
		err = DCIAttrSet(pSession, DCI_HTYPE_SESSION, (void *) User, (ub4)strlen(User), DCI_ATTR_USERNAME, pError);
		err = DCIAttrSet(pSession, DCI_HTYPE_SESSION, (void *) Pwd, (ub4)strlen(Pwd), DCI_ATTR_PASSWORD, pError);
		err = DCISessionBegin(pSvcCtx, pError, pSession, DCI_CRED_RDBMS, DCI_DEFAULT);
	}


#ifdef WIN32
	QueryPerformanceCounter(&end);
	cost = (double)(end.QuadPart - start.QuadPart)/frq.QuadPart;
	//printf("connect: %f\n", cost);
#else
	gettimeofday(&stop_t, NULL);
	if (stop_t.tv_usec < start_t.tv_usec)
	{
		stop_t.tv_sec--;
		stop_t.tv_usec += 1000000;
	}
	cost_usec = (long) ((stop_t.tv_sec - start_t.tv_sec) * 1000000 + (stop_t.tv_usec - start_t.tv_usec));

	printf("connect: %ld us\n", cost_usec);
#endif
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCILogon failed: %d %s\n", errcode, msg);

		return err;
	}

	if (useLogon == false)
	{
		/* Set Session Into ServerContext */
		err = DCIAttrSet(pSvcCtx, DCI_HTYPE_SVCCTX, pSession, 0, DCI_ATTR_SESSION, pError);
	}

	return err;
}

sword
disconnect(bool useLogon)
{
	if (pSvcCtx)
	{
		if (useLogon)
		{
			err = DCILogoff(pSvcCtx, pError);
			if (err != DCI_SUCCESS)
			{
				printf("DCILogoff failed\n");
			}
		}
		else
		{
			err = DCISessionEnd(pSvcCtx, pError, pSession, DCI_DEFAULT);
			err = DCIServerDetach(pServer, pError, DCI_DEFAULT);

			err = DCIHandleFree(pSession, DCI_HTYPE_SESSION);
			pSession = NULL;
		}

		/* Free ServContext */
		err = DCIHandleFree((dvoid *) pSvcCtx, (ub4) DCI_HTYPE_SVCCTX); 
		if (err != DCI_SUCCESS)
		{
			printf("DCIHandleFree DCI_HTYPE_SVCCTX failed\n");
		}

		pSvcCtx = NULL;

		if (useLogon == false)
		{
			err = DCIHandleFree(pServer, DCI_HTYPE_SERVER);
			pServer = NULL;
		}
	}

	/* Free Error */
	if (pError)
	{
		err = DCIHandleFree((dvoid *) pError, (ub4) DCI_HTYPE_ERROR); 
		if (err != DCI_SUCCESS)
		{
			printf("DCIHandleFree DCI_HTYPE_ERROR failed\n");
		}
		pError = NULL;
	}

	/* Free Env Handle */
	if (pEnv)
	{
		err = DCIHandleFree((dvoid *) pEnv, (ub4) DCI_HTYPE_ENV);
		if (err != DCI_SUCCESS)
		{
			printf("DCIHandleFree DCI_HTYPE_ENV failed\n");
		}
		pEnv = NULL;
	}

	if (err != DCI_SUCCESS)
	{	
		report_error(pError);
		printf("disconnect failed\n");
	}
	return err;
}

sword
test_logon()
{
	sword ret = DCI_SUCCESS;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
	}

	err = disconnect(true);

	return ret;
}

sword
test_sessionbegin()
{
	sword ret = DCI_SUCCESS;

	err = connect(false);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
	}

	err = disconnect(false);
	return ret;
}


sword 
connect2(bool useLogon)
{
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

	err = DCIEnvCreate((DCIEnv **) &pEnv, DCI_DEFAULT, 0, 0, 0, 0, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIEnvCreate failed\n");
		return err;
	} 

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pError, DCI_HTYPE_ERROR, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc failed\n");
		return err;
	}

#ifdef WIN32
	QueryPerformanceCounter(&start);
#else
	gettimeofday(&start_t, NULL);
#endif

	if (useLogon)
	{
		err = DCILogon (pEnv, pError, &pSvcCtx,
			(const DciText *)User, NAME_LEN,
			(const DciText *)Pwd, NAME_LEN,
			(const DciText *)DbName, NAME_LEN);
	}
	else
	{
		err = DCIHandleAlloc(pEnv, (dvoid **) &pServer, DCI_HTYPE_SERVER, (size_t) 0, NULL);
		err = DCIHandleAlloc(pEnv, (void **) &pSvcCtx, DCI_HTYPE_SVCCTX, 0, NULL);

		err = DCIServerAttach(pServer, pError, (DciText *)DbName, NAME_LEN, DCI_DEFAULT);
		err = DCIAttrSet(pSvcCtx, DCI_HTYPE_SVCCTX, pServer, 0, DCI_ATTR_SERVER, pError);

		err = DCIHandleAlloc(pEnv, (dvoid **) &pSession, (ub4) DCI_HTYPE_SESSION, (size_t) 0, NULL);
		err = DCIAttrSet(pSession, DCI_HTYPE_SESSION, (void *)User, NAME_LEN, DCI_ATTR_USERNAME, pError);
		err = DCIAttrSet(pSession, DCI_HTYPE_SESSION, (void *)Pwd, NAME_LEN, DCI_ATTR_PASSWORD, pError);
		err = DCISessionBegin(pSvcCtx, pError, pSession, DCI_CRED_RDBMS, DCI_DEFAULT);
	}

#ifdef WIN32
	QueryPerformanceCounter(&end);
	cost = (double)(end.QuadPart - start.QuadPart)/frq.QuadPart;
	//printf("connect: %f\n", cost);
#else
	gettimeofday(&stop_t, NULL);
	if (stop_t.tv_usec < start_t.tv_usec)
	{
		stop_t.tv_sec--;
		stop_t.tv_usec += 1000000;
	}
	cost_usec = (long) ((stop_t.tv_sec - start_t.tv_sec) * 1000000 + (stop_t.tv_usec - start_t.tv_usec));

	printf("connect: %ld us\n", cost_usec);
#endif
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCILogon failed: %d %s\n", errcode, msg);

		return err;
	}

	if (useLogon == false)
	{
		/* Set Session Into ServerContext */
		err = DCIAttrSet(pSvcCtx, DCI_HTYPE_SVCCTX, pSession, 0, DCI_ATTR_SESSION, pError);
	}

	return err;
}

sword
test_logon2()
{
	sword ret = DCI_SUCCESS;

	err = connect2(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
	}

	err = disconnect(true);

	return ret;
}

sword
test_sessionbegin2()
{
	sword ret = DCI_SUCCESS;

	err = connect2(false);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
	}

	err = disconnect(false);
	return ret;
}


sword
connect_UTF16(bool useLogon)
{
	char exec_buf[50] = "set char_default_type to 'char'";
	char exec_buf_utf16[50 * 2];
	DCIStmt 	*pStmt = NULL;
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
	err = DCIEnvCreate((DCIEnv **) &pEnv, DCI_DEFAULT | DCI_UTF16, 0,   
		0, 0, 0, (size_t) 0, (void  **)0);
	if (err != DCI_SUCCESS)
	{
		printf("DCIEnvCreate failed\n");
		return err;
	} 

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pError, DCI_HTYPE_ERROR, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc failed\n");
		return err;
	}

#ifdef WIN32
	QueryPerformanceCounter(&start);
#else
	gettimeofday(&start_t, NULL);
#endif

	if (useLogon)
	{
		err = DCILogon (pEnv, pError, &pSvcCtx,
			(const DciText *)User_UTF16, NAME_LEN,
			(const DciText *)Pwd_UTF16, NAME_LEN,
			(const DciText *)DbName_UTF16, NAME_LEN);
	}
	else
	{
		err = DCIHandleAlloc(pEnv, (dvoid **) &pServer, DCI_HTYPE_SERVER, (size_t) 0, NULL);
		err = DCIHandleAlloc(pEnv, (void **) &pSvcCtx, DCI_HTYPE_SVCCTX, 0, NULL);

		err = DCIServerAttach(pServer, pError, (DciText *)DbName_UTF16, NAME_LEN, DCI_DEFAULT);
		err = DCIAttrSet(pSvcCtx, DCI_HTYPE_SVCCTX, pServer, 0, DCI_ATTR_SERVER, pError);

		err = DCIHandleAlloc(pEnv, (dvoid **) &pSession, (ub4) DCI_HTYPE_SESSION, (size_t) 0, NULL);
		err = DCIAttrSet(pSession, DCI_HTYPE_SESSION, (void *)User_UTF16, NAME_LEN, DCI_ATTR_USERNAME, pError);
		err = DCIAttrSet(pSession, DCI_HTYPE_SESSION, (void *)Pwd_UTF16, NAME_LEN, DCI_ATTR_PASSWORD, pError);
		err = DCISessionBegin(pSvcCtx, pError, pSession, DCI_CRED_RDBMS, DCI_DEFAULT);
	}


#ifdef WIN32
	QueryPerformanceCounter(&end);
	cost = (double)(end.QuadPart - start.QuadPart)/frq.QuadPart;
	//printf("connect: %f\n", cost);
#else
	gettimeofday(&stop_t, NULL);
	if (stop_t.tv_usec < start_t.tv_usec)
	{
		stop_t.tv_sec--;
		stop_t.tv_usec += 1000000;
	}
	cost_usec = (long) ((stop_t.tv_sec - start_t.tv_sec) * 1000000 + (stop_t.tv_usec - start_t.tv_usec));

	printf("connect: %ld us\n", cost_usec);
#endif
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCILogon failed: %d %s\n", errcode, msg);

		return err;
	}

	if (useLogon == false)
	{
		/* Set Session Into ServerContext */
		err = DCIAttrSet(pSvcCtx, DCI_HTYPE_SVCCTX, pSession, 0, DCI_ATTR_SESSION, pError);
	}

	{
		err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
		if (err != DCI_SUCCESS)
		{
			printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
			return DCI_ERROR;
		}

		ascii_to_utf16(exec_buf, exec_buf_utf16);
		err = DCIStmtPrepare(pStmt, pError, (const DciText*) exec_buf_utf16, (ub4) strlen(exec_buf_utf16), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			DciText sqlstate[6] = "", msg[256] = "";
			sb4 	errcode = 0;

			DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
			printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);
			return DCI_ERROR;
		}

		err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			DciText sqlstate[6] = "", msg[256] = "";
			sb4 	errcode = 0;

			DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
			printf("--> DCIStmtExecute failed: %d %s\n", errcode, msg);
			return DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			printf("DCITransCommit failed\n");
			return DCI_ERROR;
		}


		err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
		if (err != DCI_SUCCESS)
		{
			printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
			return DCI_ERROR;
		}
	}
	return err;
}

sword
disconnect_UTF16(bool useLogon)
{
	if (pSvcCtx)
	{
		if (useLogon)
		{
			err = DCILogoff(pSvcCtx, pError);
			if (err != DCI_SUCCESS)
			{
				printf("DCILogoff failed\n");
			}
		}
		else
		{
			err = DCISessionEnd(pSvcCtx, pError, pSession, DCI_DEFAULT);
			err = DCIServerDetach(pServer, pError, DCI_DEFAULT);

			err = DCIHandleFree(pSession, DCI_HTYPE_SESSION);
		}

		/* Free ServContext */
		err = DCIHandleFree((dvoid *) pSvcCtx, (ub4) DCI_HTYPE_SVCCTX); 
		if (err != DCI_SUCCESS)
		{
			printf("DCIHandleFree DCI_HTYPE_SVCCTX failed\n");
		}

		if (useLogon == false)
			err = DCIHandleFree(pServer, DCI_HTYPE_SERVER);
	}

	/* Free Error */
	if (pError)
	{
		err = DCIHandleFree((dvoid *) pError, (ub4) DCI_HTYPE_ERROR); 
		if (err != DCI_SUCCESS)
		{
			printf("DCIHandleFree DCI_HTYPE_ERROR failed\n");
		}
	}

	/* Free Env Handle */
	if (pEnv)
	{
		err = DCIHandleFree((dvoid *) pEnv, (ub4) DCI_HTYPE_ENV);
		if (err != DCI_SUCCESS)
		{
			printf("DCIHandleFree DCI_HTYPE_ENV failed\n");
		}
	}

	if (err != DCI_SUCCESS)
	{	
		report_error(pError);
		printf("disconnect failed\n");
	}
	return err;
}

#ifdef	WIN32
#define HTHREAD                   HANDLE
#define THREAD_ERROR			  NULL 
#define THREAD_FUNC_RET			  unsigned
typedef THREAD_FUNC_RET(__stdcall *THREAD_FUNC)(void *);
#else /* LINUX */
#define HTHREAD					  pthread_t
#define THREAD_ERROR			  -1
#define THREAD_FUNC_RET			  void*
#define __stdcall                     
#define Sleep(ms)				  usleep(ms*1000)
typedef THREAD_FUNC_RET(__stdcall *THREAD_FUNC)(void *);

// for linux
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif

typedef struct SvcPack
{
	DCISvcCtx* svc;
	DCIStmt* stmt;
	sword ret;
}SvcPack;

bool isCloseFinish = true;
bool isExecuteFinish = true;

HTHREAD createThreadAndRun(THREAD_FUNC func, void *arg)
{
	HTHREAD handle;
#ifdef WIN32
	handle = (HTHREAD)_beginthreadex(NULL, 0, func, arg, 0, NULL);
#else
	pthread_create(&handle, NULL, func, arg);
	pthread_detach(handle);
#endif // WIN32
	return handle;
}

THREAD_FUNC_RET __stdcall closeSvcSocket(void *arg)
{
	sword retcode;
	SvcPack* pack = (SvcPack*)arg;
	isCloseFinish = false;
	
	Sleep(200);
	retcode = DCICloseSvcSocket(pack->svc);
	if (retcode != DCI_SUCCESS)
	{
		printf("CloseSvcSocket failed\n");
	}
	pack->ret = retcode;

	isCloseFinish = true;
	return 0;
}

THREAD_FUNC_RET __stdcall executeSQL(void *arg)
{
	sword retcode;
	SvcPack* pack = (SvcPack*)arg;
	isExecuteFinish = false;

	retcode = DCIStmtExecute(pack->svc, pack->stmt, pError, 1, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (retcode != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);
	}
	pack->ret = retcode;

	isExecuteFinish = true;
	return 0;
}



sword
TestCloseSvcSocket()
{
	ub4		i, j, failed = 0;
	DCIStmt	*pStmt = NULL;
	HTHREAD handle;
	SvcPack closePack, executePack;
	sword ret = DCI_SUCCESS;
	char *sqls[]={
		"create temp table TestCloseSvcSocket(id int);",
		"select 1;",
		"insert into TestCloseSvcSocket(id) values(100)",
		"select SYS_SLEEP(1);",
		"select SYS_SLEEP(50)"
	};


	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		failed++;
		printf("connect failed\n");
		return DCI_ERROR;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	closePack.svc = pSvcCtx;
	executePack.svc = pSvcCtx; 
	executePack.stmt = pStmt;

	for (i = 0; i < sizeof(sqls) / sizeof(sqls[0]); i++)
	{
		closePack.ret = DCI_SUCCESS;
		executePack.ret = DCI_SUCCESS;
		char *sql=sqls[i];

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

		isCloseFinish = false;
		isExecuteFinish = false;
		createThreadAndRun(executeSQL, &executePack);
		createThreadAndRun(closeSvcSocket, &closePack);

		while (!isCloseFinish || !isExecuteFinish)
		{
			Sleep(1000);
		}

		if(executePack.ret == DCI_ERROR)
		{
			DciText sqlstate[6] = "", msg[256] = "";
			sb4		errcode = 0;

			DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
			printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);
			if (errcode != 65002)
			{
				failed++;
			}
			else
			{
				if (closePack.ret != DCI_SUCCESS)
				{
					failed++;
				}

			}

		}
	}
	
free_resource:
	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

end:
	err = disconnect(true);

	if (failed == 0)
		return ret;
	else
	{
		printf("failed = %d\n", failed);
		return DCI_ERROR;
	}
	return DCI_SUCCESS;
}
