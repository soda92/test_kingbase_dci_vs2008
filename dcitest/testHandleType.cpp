#include "DciTest.h"

sword test_Handle_Type()
{
	if (test_HandleFree() != DCI_SUCCESS)
	{
		printf("test_Handle_Type: test_DCIHandleFree() failed!\n");
		return DCI_ERROR;
	}

/*
	if (test_DescriptorFree() != DCI_SUCCESS)
	{
		printf("test_Handle_Type: test_DCIDescriptorFree() failed!\n");
		return DCI_ERROR;
	}

*/
	if (test_ErrorGet() != DCI_SUCCESS)
	{
		printf("test_Handle_Type: test_DCIErrorGet() failed!\n");
		return DCI_ERROR;
	}

	if (test_AttrSet() != DCI_SUCCESS)
	{
		printf("test_Handle_Type: test_DCIAttrSet() failed!\n");
		return DCI_ERROR;
	}

	if (test_AttrGet_ParamGet() != DCI_SUCCESS)
	{
		printf("test_Handle_Type: test_DCIAttrGet_DCIParamGet() failed!\n");
		return DCI_ERROR;
	}

/*
	if (test_DateTimeConstruct() != DCI_SUCCESS)
	{
		printf("test_Handle_Type: test_DCIDateTimeConstruct() failed!\n");
		return DCI_ERROR;
	}
*/
	return DCI_SUCCESS;
}

sword test_HandleFree()
{
	sword ret = DCI_SUCCESS;
	ub4 HandleType[] = 
	{
		DCI_HTYPE_ERROR,
		DCI_HTYPE_SVCCTX,
		DCI_HTYPE_SERVER,
		DCI_HTYPE_TRANS,
		DCI_HTYPE_DESCRIBE,
		DCI_HTYPE_DIRPATH_CTX,
		//DCI_HTYPE_DIRPATH_STREAM,//this handle need DCI_HTYPE_DIRPATH_CTX, not env.
	};

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

	for (unsigned int i = 0;i < sizeof(HandleType) / sizeof(HandleType[0]);i++)
	{
		dvoid *pHdl = NULL;
		err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pHdl, HandleType[i], 0, NULL);
		if (err != DCI_SUCCESS)
		{
			printf("DCIHandleAlloc failed\n");
			return err;
		}

		for (unsigned int j = 0;j < sizeof(HandleType) / sizeof(HandleType[0]);j++)
		{
			if (HandleType[j] != HandleType[i])
			{
				err = DCIHandleFree(pHdl, HandleType[j]);
				if (err != DCI_INVALID_HANDLE)
				{
					return DCI_ERROR;
				}
			}
			else
			{
				err = DCIHandleFree(pHdl, HandleType[j]);
				if (err != DCI_SUCCESS)
				{
					return DCI_ERROR;
				}

				err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pHdl, HandleType[i], 0, NULL);
				if (err != DCI_SUCCESS)
				{
					printf("DCIHandleAlloc failed\n");
					return err;
				}
			}
		}

		err = DCIHandleFree(pHdl, HandleType[i]);
		if (err != DCI_SUCCESS)
		{
			return DCI_ERROR;
		}
	}

	err = DCIHandleAlloc(pEnv, (void **) &pSvcCtx, DCI_HTYPE_SVCCTX, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc failed\n");
		return err;
	}

	err = DCIHandleAlloc(pEnv, (dvoid **) &pSession, (ub4) DCI_HTYPE_SESSION, (size_t) 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc failed\n");
		return err;
	}

	for (unsigned int i = 0;i < sizeof(HandleType) / sizeof(HandleType[0]);i++)
	{
		err = DCIHandleFree(pSession, HandleType[i]);
		if (err != DCI_INVALID_HANDLE)
		{
			return DCI_ERROR;
		}
	}

	err = DCIHandleFree(pSession, DCI_HTYPE_SESSION);
	if (err != DCI_SUCCESS)
	{
		return DCI_ERROR;
	}

	for (unsigned int i = 0;i < sizeof(HandleType) / sizeof(HandleType[0]);i++)
	{
		if (HandleType[i] != DCI_HTYPE_SVCCTX)
		{
			err = DCIHandleFree(pSvcCtx, HandleType[i]);
			if (err != DCI_INVALID_HANDLE)
			{
				return DCI_ERROR;
			}
		}
	}

	err = DCIHandleFree(pSvcCtx, DCI_HTYPE_SVCCTX);
	if (err != DCI_SUCCESS)
	{
		return DCI_ERROR;
	}

	for (unsigned int i = 0;i < sizeof(HandleType) / sizeof(HandleType[0]);i++)
	{
		err = DCIHandleFree(pEnv, HandleType[i]);
		if (err != DCI_INVALID_HANDLE)
		{
			return DCI_ERROR;
		}
	}

	err = DCIHandleFree(pEnv, DCI_HTYPE_ENV);
	if (err != DCI_SUCCESS)
	{
		return DCI_ERROR;
	}

	return ret;
}

sword test_DescriptorFree()
{
	sword ret = DCI_SUCCESS;
	ub4 HandleType[] = 
	{
		DCI_DTYPE_LOB,
		DCI_DTYPE_PARAM,
		DCI_DTYPE_DATE,
		DCI_DTYPE_TIME,
		DCI_DTYPE_TIME_TZ,
		DCI_DTYPE_TIMESTAMP,
		DCI_DTYPE_TIMESTAMP_LTZ,
		DCI_DTYPE_TIMESTAMP_TZ,
		DCI_DTYPE_INTERVAL_DS,
		DCI_DTYPE_INTERVAL_YM,
	};

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

	for (unsigned int i = 0;i < sizeof(HandleType) / sizeof(HandleType[0]);i++)
	{
		dvoid *pHdl = NULL;
		err = DCIDescriptorAlloc((dvoid *) pEnv, (dvoid **) &pHdl, HandleType[i], 0, NULL);
		if (err != DCI_SUCCESS)
		{
			printf("DCIDescriptorAlloc failed\n");
			return err;
		}

		for (unsigned int j = 0;j < sizeof(HandleType) / sizeof(HandleType[0]);j++)
		{
			if (HandleType[j] != HandleType[i])
			{
				err = DCIDescriptorFree(pHdl, HandleType[j]);
				if (err != DCI_INVALID_HANDLE)
				{
					ret = DCI_ERROR;
				}
			}
			else
			{
				err = DCIDescriptorFree(pHdl, HandleType[j]);
				if (err != DCI_SUCCESS)
				{
					printf("DCIDescriptorAlloc1 failed\n");
					ret = DCI_ERROR;
				}

				err = DCIDescriptorAlloc((dvoid *) pEnv, (dvoid **) &pHdl, HandleType[i], 0, NULL);
				if (err != DCI_SUCCESS)
				{
					printf("DCIDescriptorAlloc2 failed\n");
					return err;
				}
			}
		}

		err = DCIDescriptorFree(pHdl, HandleType[i]);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
		}
	}

	for (unsigned int i = 0;i < sizeof(HandleType) / sizeof(HandleType[0]);i++)
	{
		err = DCIDescriptorFree(pEnv, HandleType[i]);
		if (err != DCI_INVALID_HANDLE)
		{
			ret = DCI_ERROR;
		}
	}

	if (pEnv)
	{
		err = DCIHandleFree((dvoid *) pEnv, (ub4) DCI_HTYPE_ENV);
		if (err != DCI_SUCCESS)
		{
			printf("DCIHandleFree DCI_HTYPE_ENV failed\n");
			ret = DCI_ERROR;
		}
	}

	return ret;
}

sword test_ErrorGet()
{
	sword ret = DCI_SUCCESS;
	DCIStmt *pStmt = NULL;
	char exec_buf[] = "drop table test_DCIErrorGet";
	DciText sqlstate[6] = "", msg[256] = "";
	sb4		errcode = 0;

	err = connect(true);
	if (err != DCI_SUCCESS)
		return err;

	/* test  DCIErrorGet(pEnv, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);*/
	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (ub4)DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);

	err = DCIErrorGet(pEnv, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
	if (err != DCI_INVALID_HANDLE)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIHandleFree(pStmt, DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
	}

	/* test DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ENV);*/
	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (ub4)DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);

	err = DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ENV);
	if (err != DCI_INVALID_HANDLE)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIHandleFree(pStmt, DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
	}

	/* test DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);*/
	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, (ub4)DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);

	err = DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
	if (err != DCI_SUCCESS && err != DCI_NO_DATA)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIHandleFree(pStmt, DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
	}
	goto end;


free_resource:
	err = DCIHandleFree(pStmt, DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
	}

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		return err;
	else
		return ret;
}

sword test_AttrSet()
{
	sword			ret = DCI_SUCCESS;
	DCIDirPathCtx	*dpctx = NULL;
	DCIParam		*param = NULL;
	DCIStmt			*stmt = NULL;
	char	username[] = "abc";
	int		rows = 5;
	int		bufsize = 10;
	char	exec_buf[] = "select * from test_DCIAttrSet";

	err = test_execute("drop table if exists test_DCIAttrSet");
	err = test_execute("create table test_DCIAttrSet(c int)");
	if (err != DCI_SUCCESS)
		return err;

	err = connect(true);
	if (err != DCI_SUCCESS)
		return err;

	err = DCIHandleAlloc((void *)pEnv, (void **)&dpctx, DCI_HTYPE_DIRPATH_CTX, (size_t)0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc((void *)pEnv, (void **)&stmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrSet(pSvcCtx, DCI_HTYPE_SESSION, (void *) pServer, (ub4) 0, DCI_ATTR_SERVER, pError);	
	if (err != DCI_INVALID_HANDLE)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrSet(pSession, DCI_HTYPE_SVCCTX, (void *) username, (ub4) sizeof(username), DCI_ATTR_USERNAME, pError);	
	if (err != DCI_INVALID_HANDLE)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrSet(stmt, DCI_HTYPE_SVCCTX, (void *) &rows, (ub4) sizeof(rows), DCI_ATTR_PREFETCH_ROWS, pError);	
	if (err != DCI_INVALID_HANDLE)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrSet(stmt, DCI_HTYPE_STMT, (void *) &rows, (ub4) sizeof(rows), DCI_ATTR_PREFETCH_ROWS, pError);	
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrSet(dpctx, DCI_HTYPE_SVCCTX, (void *) &bufsize, (ub4) sizeof(bufsize), DCI_ATTR_BUF_SIZE, pError);	
	if (err != DCI_INVALID_HANDLE)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtPrepare(stmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	
	// bug 31171
	err = DCIParamGet((dvoid *)stmt, DCI_HTYPE_STMT, pError, (dvoid **)&param, (ub4) 1);
	if (err != DCI_ERROR)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, stmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIParamGet((dvoid *)stmt, DCI_HTYPE_STMT, pError, (dvoid **)&param, (ub4) 1);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrSet(param, DCI_HTYPE_SVCCTX, (void *) username, (ub4) sizeof(username), DCI_ATTR_NAME, pError);
	if (err != DCI_INVALID_HANDLE)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
/*	
	err = DCIDescriptorFree(param, DCI_DTYPE_PARAM);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
	}
	param = NULL;
*/	
	err = DCIHandleFree(dpctx, DCI_HTYPE_DIRPATH_CTX);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
	}
	if(stmt != NULL)
	{
		err = DCIHandleFree(stmt, DCI_HTYPE_STMT);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
		}
	}
end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;
	else
		return ret;
}

sword test_AttrGet_ParamGet()
{
	sword ret = DCI_SUCCESS;
	DCIStmt *pStmt = NULL;
	char exec_buf[] = "select 1";
	DCIParam *mypard = (DCIParam *) 0;
	ub4 counter = 1;
	ub4 col_width =0;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		return DCI_ERROR;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(pStmt, pError, (const DciText *)exec_buf, (ub4)strlen(exec_buf), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		sb4 errcode;
		DciText sqlstate[6];
		DciText errmsg[255];

		err = DCIErrorGet((void *)pError, 1, sqlstate, &errcode, errmsg, (ub4)255, (ub4)DCI_HTYPE_ERROR);
		if (errcode != 942 || strcmp((char *)sqlstate, "42P01") != 0)
			ret = DCI_ERROR;

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (const DCISnapshot *)NULL, (DCISnapshot *)NULL, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIParamGet((dvoid *)pStmt, DCI_DTYPE_PARAM, pError, (dvoid **)&mypard, (ub4) counter);
	if (err != DCI_INVALID_HANDLE)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIParamGet((dvoid *)pStmt, DCI_HTYPE_STMT, pError, (dvoid **)&mypard, (ub4) counter);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((dvoid*) mypard, (ub4) DCI_HTYPE_STMT,
		(dvoid*) &col_width,(ub4 *) 0, (ub4) DCI_ATTR_DATA_SIZE,
		(DCIError *) pError );
	if (err != DCI_INVALID_HANDLE)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrGet((dvoid*) mypard, (ub4) DCI_DTYPE_PARAM,
		(dvoid*) &col_width,(ub4 *) 0, (ub4) DCI_ATTR_DATA_SIZE,
		(DCIError *) pError );
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree(pStmt, DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
	}
/*
	err = DCIDescriptorFree((void *)mypard, (const ub4)DCI_DTYPE_PARAM);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
	}
*/
end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;
	else
		return ret;
}

sword test_DateTimeConstruct()
{
	sword ret = DCI_SUCCESS;
	DCIDateTime *dt_value = NULL;

	err = connect(false);
	if (err != DCI_SUCCESS)
	{
		return DCI_ERROR;
	}

	err = DCIDescriptorAlloc(pEnv,(dvoid **)&dt_value, (ub4) DCI_DTYPE_DATE, (size_t) 0, (dvoid **) 0);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIDateTimeConstruct(pEnv, pError, (DCIDateTime *)dt_value, 2010, 10, 17, 0, 0, 0, 0, NULL, 0);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	
	err = DCIDateTimeConstruct(pSession, pError, (DCIDateTime *)dt_value, 2011, 4, 21, 0, 0, 0, 0, NULL, 0);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDateTimeConstruct(pSvcCtx, pError, (DCIDateTime *)dt_value, 2011, 4, 21, 0, 0, 0, 0, NULL, 0);
	if (err != DCI_INVALID_HANDLE)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDateTimeConstruct(pServer, pError, (DCIDateTime *)dt_value, 2011, 4, 21, 0, 0, 0, 0, NULL, 0);
	if (err != DCI_INVALID_HANDLE)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
free_resource:
	err = DCIDescriptorFree(dt_value, DCI_DTYPE_DATE);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

end:
	err = disconnect(false);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;
	else
		return ret;
}
sword test_DateTimeFromText()
{
	sword ret = DCI_SUCCESS;
	DCIDateTime *dt_value = NULL;
	char	errbuf[256];

	err = connect(false);
	if (err != DCI_SUCCESS)
	{
		return DCI_ERROR;
	}

	err = DCIDescriptorAlloc(pEnv,(dvoid **)&dt_value, (ub4) DCI_DTYPE_DATE, (size_t) 0, (dvoid **) 0);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIDateTimeFromText(NULL,NULL,(text*)"2012-12-12",strlen("2012-12-12"),(text*)"yyyy-mm-dd",strlen("yyyy-mm-dd"),NULL,0,dt_value);
	if (err != DCI_INVALID_HANDLE)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	err = DCIDateTimeFromText(pEnv,pError,(text*)"2012-12-12",strlen("2012-12-12"),(text*)"yyyy-mm-dd",strlen("yyyy-mm-dd"),NULL,0,dt_value);
	if (err != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
		ret = DCI_ERROR;
		goto free_resource;
	}
	err = DCIDateTimeFromText(pEnv,pError,(text*)"2012-12-12-21-34-53.123000",strlen("2012-12-12-21-34-53.123000"),(text*)"YYYY-mm-dd-HH24-MI-SS.US2",strlen("YYYY-mm-dd-HH24-MI-SS.US2"),NULL,0,dt_value);
	if (err != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
		ret = DCI_ERROR;
		goto free_resource;
	}
	err = DCIDateTimeFromText(pEnv,pError,NULL,0,(text*)"yyyy-mm-dd",strlen("yyyy-mm-dd"),NULL,0,dt_value);
	if (err != DCI_ERROR)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDateTimeFromText(pEnv,pError,(text*)"2012-12-12",strlen("2012-12-12"),(text*)"yyyy-mm-dd",strlen("yyyy-mm-dd"),NULL, 0, dt_value);
	if (err != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIDescriptorFree(dt_value, DCI_DTYPE_DATE);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

end:
	err = disconnect(false);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;
	else
		return ret;
}
sword test_DateTimeToText()
{
	sword ret = DCI_SUCCESS;
	DCIDateTime *dt_value;
	DciText bufp[256] = "";
	int bufsz = 256;
	DciText bufp2[2] = "";
	int bufsz2 = 2;
	sb4 sb4ErrorCode; 
	char sErrorMsg[512] = {0};



	err = connect(false);
	if (err != DCI_SUCCESS)
	{
		return DCI_ERROR;
	}

	err = DCIDescriptorAlloc(pEnv,(dvoid **)&dt_value, (ub4) DCI_DTYPE_DATE, (size_t) 0, (dvoid **) 0);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIDateTimeConstruct(pEnv, pError, dt_value,2012,7,29,16,53,36,123456, NULL, 0);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	
	err = DCIDateTimeToText(NULL, NULL, dt_value, (text*)"YYYY-mm-dd-HH24-MI-SS-US", 
		strlen("YYYY-mm-dd-HH24-MI-SS-US"), 4, NULL, 0, (ub4*)&bufsz, bufp);
	if (err != DCI_INVALID_HANDLE)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	bufsz = 256;
	err = DCIDateTimeToText(pSvcCtx, pError, dt_value, (text*)"YYYY-mm-dd-HH24-MI-SS-US", 
		strlen("YYYY-mm-dd-HH24-MI-SS-US"), 4, NULL, 0, (ub4*)&bufsz, bufp);
	if (err != DCI_INVALID_HANDLE)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	bufsz = 256;
	err = DCIDateTimeToText(pEnv, pError, dt_value, (text*)"US-YYYY-mm-dd-HH24-MI-SS-", 
		strlen("US-YYYY-mm-dd-HH24-MI-SS-"), 4, NULL, 0, (ub4*)&bufsz, bufp);
	if (err != DCI_SUCCESS && 0 != strcmp("0001234-2012-07-29-16-53-36-", (const char *)bufp) )
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	bufsz = 256;
	err = DCIDateTimeToText(pEnv, pError, dt_value, (text*)"YYYY-mm-dd-HH24-MI-SS-US8", 
		strlen("YYYY-mm-dd-HH24-MI-SS-US8"), 4, NULL, 0, (ub4*)&bufsz, bufp);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	//multiple US string
	bufsz = 256;
	err = DCIDateTimeToText(pEnv, pError, dt_value, (text*)"YYYY-mm-dd-HH24-MI-SS-US--US6", 
		strlen("YYYY-mm-dd-HH24-MI-SS-US--US6"), 4, NULL, 0, (ub4*)&bufsz, bufp);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	bufsz = 256;
	err = DCIDateTimeToText(pEnv, pError, dt_value, NULL, 0, 0, NULL, 0, (ub4*)&bufsz, bufp);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	err = DCIDateTimeToText(pEnv, pError, dt_value, NULL, 0, 0, NULL, 0, (ub4*)&bufsz2, bufp2);
	/* if buffer size has been changed*/
	if (err != DCI_ERROR)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	if(DCI_SUCCESS == (err = DCIErrorGet(pError, 1, NULL, &sb4ErrorCode, (DciText*)sErrorMsg, sizeof(sErrorMsg), DCI_HTYPE_ERROR)))
	{
		printf("the error message: %s , the error code: %d\n", sErrorMsg, sb4ErrorCode);
		if (1877 != sb4ErrorCode )
		{
			ret = DCI_ERROR;
			goto free_resource;
		}
	}

free_resource:
	err = DCIDescriptorFree(dt_value, DCI_DTYPE_DATE);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

end:
	err = disconnect(false);
	if (err != DCI_SUCCESS)
		return DCI_ERROR;
	else
		return ret;
}
