#include "DciTest.h"
#include "package.h"

void test_package()
{
#if 1 /* Change 0 to 1 if add "UsePackage=1" into sys_service.conf */
	printf("Package used\n");

	// create package
	err = test_execute(
		"CREATE OR REPLACE PACKAGE PUBLIC.TRIGGER_PKG AS "
		"PROCEDURE PRO_PKG(p1 smallint, p2 smallint, p3 int, p4 bigint, p5 numeric, p6 numeric(20,0), p7 real, p8 float8, p9 timestamp, p10 char(14 byte), p11 char(14 char));"
		"PROCEDURE PRO_PKG2(in p1 smallint, out p2 refcursor);"
		"PROCEDURE PRO_PKG3(in p1 smallint, out p2 refcursor, out p3 refcursor);"
		"END;");
	check(err, "test_execute: CREATE PACKAGE TRIGGER_PKG");
	err = test_execute(
		"CREATE OR REPLACE PACKAGE BODY PUBLIC.TRIGGER_PKG AS "
		"PROCEDURE PRO_PKG(p1 smallint, p2 smallint, p3 int, p4 bigint, p5 numeric, p6 numeric(20,0), p7 real, p8 float8, p9 timestamp, p10 char(14 byte), p11 char(14 char)) as "
		"begin INSERT INTO T_INT_CHAR VALUES(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11); end;"
		"PROCEDURE PRO_PKG2(in p1 smallint, out p2 refcursor) as "
		"begin open p2 for select * from T_INT_CHAR where c1 = p1; end;"
		"PROCEDURE PRO_PKG3(in p1 smallint, out p2 refcursor, out p3 refcursor) as "
		"begin open p2 for select * from T_INT_CHAR where c1 = p1; open p3 for select * from T_INT_CHAR where c1 = p1; end;"
		"END;");
	check(err, "test_execute: CREATE PACKAGE BODY TRIGGER_PKG AND PROCEDURE PRO_PKG, PRO_PKG2");

	print_bar();

	test_select("select object_name from user_objects where object_name = 'TRIGGER_PKG' and (object_type = 'PACKAGE' or object_type = 'PACKAGE BODY') and status<>'VALID'");

	err = connect(true);

	// call pkg.proc
	err = test_proc("trigger_pkg", "pro_pkg", 2);
	check(err, "test_proc: TRIGGER_PKG.PRO_PKG");

	err = disconnect(true);

	print_bar();

	err = connect(true);

	// call pkg.proc
	err = test_proc("TRIGGER_PKG", "PRO_PKG2", 1);
	check(err, "test_proc: TRIGGER_PKG.PRO_PKG2");

	err = test_proc("TRIGGER_PKG", "PRO_PKG2", 1);
	check(err, "test_proc: TRIGGER_PKG.PRO_PKG2");

	err = disconnect(true);

	print_bar();

	err = connect(true);

	// call pkg.proc
	err = test_proc("TRIGGER_PKG", "PRO_PKG3", 1);
	check(err, "test_proc: TRIGGER_PKG.PRO_PKG3");

	err = test_proc("TRIGGER_PKG", "PRO_PKG3", 1);
	check(err, "test_proc: TRIGGER_PKG.PRO_PKG3");

	err = disconnect(true);

#else

	printf("Package not used\n");

	// create procedure
	err = test_execute("CREATE OR REPLACE PROCEDURE pro(p1 smallint, p2 smallint, p3 int, p4 bigint, p5 numeric, p6 numeric(20,0), p7 real, p8 float8, p9 timestamp(0), p10 char(14 byte), p11 char(14 char)) as "
		"begin INSERT INTO t VALUES(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11); end;");
	check(err, "test_execute: create procedure PRO");

	// call procedure
	err = test_proc("PUBLIC", "pro", 2);
	check(err, "test_proc: PRO");

	print_bar();

	// create procedure
	err = test_execute("CREATE OR REPLACE PROCEDURE pro2(p1 smallint, p2 refcursor) as "
		"begin open p2 for select * from t where c1 = p1; end;");
	check(err, "test_execute: create procedure PRO2");

	// call procedure
	err = test_proc("PUBLIC", "PRO2", 1);
	check(err, "test_proc: PRO2");

#endif
}


sword
test_proc(char *pck_name, char *proc_name, ub4 cExecCount)
{
	sword 		ret = DCI_SUCCESS;
	DCIStmt     *stmhp1 = NULL;
	DCIStmt     *stmhp2 = NULL;
	DCIBind     *bindhp[32];

	char        plscall[2048];

	DCIDescribe *deschp = NULL; 
	DCIParam    *package = NULL; 
	DCIParam    *subprogs = NULL; 

	ub2         num_subs = 0; 
	ub2         num_args = 0; 
	ub1         type; 
	ub2			data_type[32];
	ub2			data_size[32];
	ub1			data_precision[32]; 
	sb1			data_scale[32]; 
	DciText     *name;
	ub4         namelen;
	char        name_str[64];
	bool        find_proc = FALSE;
	char        temp_str[32];
	ub4			i, j;

	err = DCIHandleAlloc((DCIEnv *)pEnv,(dvoid **)&deschp, (ub4)DCI_HTYPE_DESCRIBE,(size_t)0, (dvoid **)0); 

	if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
	{
		printf("Failed to DCIHandleAlloc at %s:%d\n", __FILE__, __LINE__);
		report_error(pError);
		return DCI_ERROR;
	}
	err = DCIDescribeAny((DCISvcCtx *)pSvcCtx, (DCIError *)pError, (dvoid *)pck_name, (ub4)strlen(pck_name), (ub1)DCI_OTYPE_NAME,(ub1)DCI_DEFAULT, 
		(ub1)DCI_PTYPE_PKG,(DCIDescribe *)deschp); 
	if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
	{
		printf("Failed to DCIDescribeAny at %s:%d\n", __FILE__, __LINE__);
		report_error(pError);
		return DCI_ERROR;
	}

	err = DCIAttrGet((CONST dvoid *)deschp,(ub4)DCI_HTYPE_DESCRIBE, (dvoid *)&package,(ub4 *)0, (ub4)DCI_ATTR_PARAM,(DCIError *)pError); 
	if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
	{
		printf("Failed to DCIAttrGet DCI_HTYPE_DESCRIBE DCI_ATTR_PARAM at %s:%d\n", __FILE__, __LINE__);
		report_error(pError);
		return DCI_ERROR;
	}

	err = DCIAttrGet((CONST dvoid *)package,(ub4)DCI_DTYPE_PARAM, (dvoid *)&subprogs,(ub4 *)0, (ub4)DCI_ATTR_LIST_SUBPROGRAMS,(DCIError *)pError); 
	if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
	{
		printf("Failed to DCIAttrGet DCI_DTYPE_PARAM DCI_ATTR_LIST_SUBPROGRAMS at %s:%d\n", __FILE__, __LINE__);
		report_error(pError);
		return DCI_ERROR;
	}

	err = DCIAttrGet((CONST dvoid *)subprogs,(ub4)DCI_DTYPE_PARAM, (dvoid *)&num_subs,(ub4 *)0, (ub4)DCI_ATTR_NUM_PARAMS,(DCIError *)pError); 
	if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
	{
		printf("Failed to DCIAttrGet DCI_DTYPE_PARAM DCI_ATTR_NUM_PARAMS at %s:%d\n", __FILE__, __LINE__);
		report_error(pError);
		return DCI_ERROR;
	}

	for (i = 0; i < num_subs; i++) 
	{ 
		DCIParam    *routine = (DCIParam*)0; 
		DCIParam    *param_list = (DCIParam*)0; 
		DCIParam    *arg = (DCIParam*)0;

		//Get the routine parameter. 
		err = DCIParamGet((CONST dvoid *)subprogs,(ub4)DCI_DTYPE_PARAM, (DCIError *) pError, (dvoid **)&routine, (ub4)i); 
		if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
		{
			printf("Failed to DCIParamGet DCI_DTYPE_PARAM at %s:%d\n", __FILE__, __LINE__);
			report_error(pError);
			return DCI_ERROR;
		}

		err = DCIAttrGet((CONST dvoid *)routine, (ub4)DCI_DTYPE_PARAM, (dvoid *) &type,(ub4 *)0, (ub4)DCI_ATTR_PTYPE,(DCIError *) pError); 
		if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
		{
			printf("Failed to DCIParamGet DCI_DTYPE_PARAM DCI_ATTR_PTYPE at %s:%d\n", __FILE__, __LINE__);
			report_error(pError);
			return DCI_ERROR;
		}

		if (type == DCI_PTYPE_PROC)
		{
			//Get the routine type and name... 
			err = DCIAttrGet((CONST dvoid *)routine,(ub4)DCI_DTYPE_PARAM, (dvoid *)&name,(ub4 *)&namelen, (ub4)DCI_ATTR_NAME,(DCIError *) pError);
			if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
			{
				printf("Failed to DCIAttrGet DCI_DTYPE_PARAM DCI_ATTR_NAME at %s:%d\n", __FILE__, __LINE__);
				report_error(pError);
				return DCI_ERROR;
			}

			strncpy(name_str, (char *)name, (size_t)namelen);
			name_str[namelen] = '\0';            

			if (stricmp((const char *)name_str, proc_name) == 0) //find procedure in package
			{
				find_proc = TRUE;

				// ...and its list of parameters. 
				err = DCIAttrGet((CONST dvoid *)routine,(ub4)DCI_DTYPE_PARAM, (dvoid *)&param_list,(ub4 *)0, (ub4)DCI_ATTR_LIST_ARGUMENTS,(DCIError *) pError); 
				if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
				{
					printf("Failed to DCIAttrGet DCI_DTYPE_PARAM DCI_ATTR_LIST_ARGUMENTS at %s:%d\n", __FILE__, __LINE__);
					report_error(pError);
					return DCI_ERROR;
				}

				err = DCIAttrGet((CONST dvoid *)param_list,(ub4)DCI_DTYPE_PARAM, (dvoid *)&num_args,(ub4 *)0, (ub4)DCI_ATTR_NUM_PARAMS,(DCIError *) pError); 
				if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
				{
					printf("Failed to DCIAttrGet DCI_DTYPE_PARAM DCI_ATTR_NUM_PARAMS at %s:%d\n", __FILE__, __LINE__);
					report_error(pError);
					return DCI_ERROR;
				}

				for (j = 0; j < num_args; j++)
				{ 
					//Get parameter details. 
					name[0] = '\0';
					data_type[j] = 0;
					data_size[j] = 0;
					data_precision[j] = 0;
					data_scale[j] = 0;

					err = DCIParamGet((CONST dvoid *)param_list,(ub4)DCI_DTYPE_PARAM, (DCIError *) pError,(dvoid **)&arg,(ub4)j+1); 
					if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
					{
						printf("Failed to DCIParamGet DCI_DTYPE_PARAM arg at %s:%d\n", __FILE__, __LINE__);
						report_error(pError);
						return DCI_ERROR;
					}

					err = DCIAttrGet((CONST dvoid *)arg,(ub4)DCI_DTYPE_PARAM, (dvoid *)&name,(ub4 *)0, (ub4)DCI_ATTR_NAME,(DCIError *) pError); 
					if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
					{
						printf("Failed to DCIParamGet DCI_DTYPE_PARAM DCI_ATTR_NAME at %s:%d\n", __FILE__, __LINE__);
						report_error(pError);
						return DCI_ERROR;
					}

					err = DCIAttrGet((CONST dvoid *)arg,(ub4)DCI_DTYPE_PARAM, (dvoid *)&data_type[j],(ub4 *)0, (ub4)DCI_ATTR_DATA_TYPE, (DCIError *) pError);
					if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
					{
						printf("Failed to DCIParamGet DCI_DTYPE_PARAM DCI_ATTR_DATA_TYPE at %s:%d\n", __FILE__, __LINE__);
						report_error(pError);
						return DCI_ERROR;
					}

					err = DCIAttrGet((CONST dvoid *)arg,(ub4)DCI_DTYPE_PARAM, (dvoid *)&data_size[j],(ub4 *)0, (ub4)DCI_ATTR_DATA_SIZE, (DCIError *) pError);
					if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
					{
						printf("Failed to DCIParamGet DCI_DTYPE_PARAM DCI_ATTR_DATA_SIZE at %s:%d\n", __FILE__, __LINE__);
						report_error(pError);
						return DCI_ERROR;
					}

					err = DCIAttrGet((CONST dvoid *)arg,(ub4)DCI_DTYPE_PARAM, (dvoid *)&data_precision[j],(ub4 *)0, (ub4)DCI_ATTR_PRECISION, (DCIError *) pError);
					if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
					{
						printf("Failed to DCIParamGet DCI_DTYPE_PARAM DCI_ATTR_PRECISION at %s:%d\n", __FILE__, __LINE__);
						report_error(pError);
						return DCI_ERROR;
					}

					err = DCIAttrGet((CONST dvoid *)arg,(ub4)DCI_DTYPE_PARAM, (dvoid *)&data_scale[j],(ub4 *)0, (ub4)DCI_ATTR_SCALE, (DCIError *) pError);
					if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
					{
						printf("Failed to DCIParamGet DCI_DTYPE_PARAM DCI_ATTR_SCALE at %s:%d\n", __FILE__, __LINE__);
						report_error(pError);
						return DCI_ERROR;
					}

					printf("%d, %s, %d, %d, %d\n", data_type[j], name, data_size[j], data_precision[j], data_scale[j]);
				} // end for loop through arguments  

			} // end find procedure in package
		}// end type == DCI_PTYPE_PROC

		if ( find_proc )
			break;
	}//end for loop through subroutines

	err = DCIHandleFree((dvoid *)deschp,(ub4)DCI_HTYPE_DESCRIBE);    

	strcpy(plscall, "begin ");
	strcat(plscall, pck_name);
	strcat(plscall, ".");
	strcat(plscall, proc_name);

	if (num_args == 0)
	{
		strcat(plscall, "(); end;");
	}
	else
	{
		strcat(plscall, "(");
		for (i = 0; i < num_args; i++) 
		{
			sprintf(temp_str,":%d,",i);
			strcat(plscall,temp_str);
		}   
		plscall[strlen(plscall)-1]=0;
		strcat(plscall, "); end;");
	}

	printf("%s\n", plscall);

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &stmhp1, DCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0);
	if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
	{
		printf("Failed to DCIHandleAlloc DCI_HTYPE_STMT at %s:%d\n", __FILE__, __LINE__);
		return DCI_ERROR;
	}

	err = DCIStmtPrepare (stmhp1, pError,(const DciText*) plscall, (ub4) strlen((char*)plscall), (ub4) DCI_NTV_SYNTAX,   (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
	{
		printf("Failed to DCIStmtPrepare \n%s\t at %s:%d\n", plscall, __FILE__, __LINE__);
		report_error(pError);
		return DCI_ERROR;
	}

	KDB_INT64	*int64_value[32];
	int		*int_value[32];
	short	*short_value[32];
	unsigned char *uchar_value[32];
	float	*float_value[32];
	double	*double_value[32];
	char	*str_value[32];
	DCIDate	*date_value[32];

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
	}

	for (i = 0; i < num_args; i++) 
	{
		char        temp_str[32];

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
					int64_value[i][j] = 1234567890987654321LL;
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
					int64_value[i][j] = 1234567890987654321LL;
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
				memcpy(str_value[i] + j * data_precision[i], "03070081610010", data_precision[i]);
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
			for (j= 0; j < cExecCount; j++)
			{
				memcpy(str_value[i] + j * data_precision[i], "03070081610010", data_precision[i]);
			}

			err = DCIBindByName(stmhp1, &bindhp[i], pError, (text *)temp_str, 2, str_value[i], data_precision[i], SQLT_CHR, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
			err = DCIBindArrayOfStruct(bindhp[i],pError,data_precision[i], 0,	0, 0);
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
				char	*pcur;
				int		temp_int;

				pcur = str_value[i];
				sscanf(pcur, "%d", &temp_int);
				date_value[i][j].DCIDateYYYY =(sb2)temp_int;
				pcur = strchr(pcur, '-');
				if (pcur == NULL)
				{
					printf("参数格式错误\n");
				}
				pcur += 1;

				sscanf(pcur, "%d", &temp_int);
				date_value[i][j].DCIDateMM = (ub1)temp_int;
				pcur=strchr(pcur, '-');
				if (pcur == NULL)
				{
					printf("参数格式错误\n");
				}
				pcur+=1;

				sscanf(pcur, "%d" ,&temp_int);
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

		default: 
			printf("unknown data type(%d)\n", data_type[i]);
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
	}

	if (stmhp2)
	{
		ub4 cFetchRows = 4;
		
		err = Fetch(pEnv, stmhp2, pError, cFetchRows);
		err = DCIHandleFree((void *) stmhp2, DCI_HTYPE_STMT);
	}

	err = DCIHandleFree((void *) stmhp1, DCI_HTYPE_STMT);

	return ret;
}

sword 
test_trigger()
{
	char *sql = "CREATE TRIGGER trigger1 BEFORE UPDATE OR INSERT ON T_TRIGGER \
						FOR EACH ROW \
						AS \
						BEGIN \
						IF new.col > 100 AND new.col < 1000 THEN \
						new.col := 50; \
					END IF; \
					IF new.col > 1000 THEN \
						RAISE EXCEPTION 'failed'; \
					END IF; \
					END;";

	test_execute("DROP TABLE IF EXISTS T_TRIGGER");
	test_execute("DROP TRIGGER IF EXISTS trigger1");

	test_execute("CREATE TABLE T_TRIGGER(COL INT)");
	test_execute("INSERT INTO T_TRIGGER VALUES(10);");
	test_execute("INSERT INTO T_TRIGGER VALUES(20);");

	return test_execute(sql);
}


#ifdef WIN32
DWORD WINAPI work_bind_cursor_thread(LPVOID pM)
{
	DCIBind  *pBind[3];
	DCIStmt  *m_stmhp1 = NULL;
	DCIStmt  *m_stmhp2 = NULL;
	DCIStmt	*pStmt = NULL;
	int		int_value = 1;
	
	DciText *sqlTest = (DciText *)"call pro_multpthread(:index, :cursor1, :cursor2)";
	if(pM == NULL)
	{
		return 0;
	}
	
	DCISvcCtx *pSvcCtx = (DCISvcCtx*)pM;
	
	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		report_error(pError);
		return 0;
	}
	
	if (DCIStmtPrepare(pStmt, pError, sqlTest, strlen((char*)sqlTest), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("Test if kingbase support datetime PREPARE failed.\n");
		report_error(pError);
		goto free_resource;
	}

	err = DCIBindByName(pStmt, &pBind[0], pError, (DciText *)":index", strlen(":index"), (void*) &int_value, sizeof(int), SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	err = DCIBindArrayOfStruct(pBind[0], pError, sizeof(int), 0, 0, 0);
	
	err = DCIBindByName(pStmt, &pBind[1], pError, (DciText *)":cursor1", strlen(":cursor1"), &m_stmhp1, 4, SQLT_RSET, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	err = DCIBindArrayOfStruct(pBind[1], pError, sizeof(DCIStmt *), 0, 0, 0);

	err = DCIBindByName(pStmt, &pBind[2], pError, (DciText *)":cursor2", strlen(":cursor2"), &m_stmhp2, 4, SQLT_RSET, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	err = DCIBindArrayOfStruct(pBind[2], pError, sizeof(DCIStmt *), 0, 0, 0);
	

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;
		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);
		goto free_resource;
	}

	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
		goto free_resource;
	}
	
	if (m_stmhp1)
	{
		ub4 cFetchRows = 1;
		err = Fetch(pEnv, m_stmhp1, pError, cFetchRows);
		err = DCIHandleFree((void *) m_stmhp1, DCI_HTYPE_STMT);
	}

	if (m_stmhp2)
	{
		ub4 cFetchRows = 1;
		err = Fetch(pEnv, m_stmhp2, pError, cFetchRows);
		err = DCIHandleFree((void *) m_stmhp2, DCI_HTYPE_STMT);
	}

free_resource:
	err = DCIHandleFree((void *) pStmt, DCI_HTYPE_STMT);
	return 0;
	
}

DWORD WINAPI work_commit_thread(LPVOID pM) 
{
	int count = 0;
	if(pM == NULL)
	{
		return 0;
	}
	DCISvcCtx *pSvcCtx = (DCISvcCtx*)pM;

	while(1)
	{
		err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			printf("DCITransCommit failed\n");
		}
	
		Sleep(500);
		if(++count == 10)
			break;
	}
	
	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
	}
	return 0;
}


#else
void *work_bind_cursor_thread(void *arg)
{
	DCIBind  *pBind[3];
	DCIStmt  *m_stmhp1 = NULL;
	DCIStmt  *m_stmhp2 = NULL;
	DCIStmt	*pStmt = NULL;
	int		int_value = 1;
	
	DciText *sqlTest = (DciText *)"call pro_multpthread(:index, :cursor1, :cursor2)";
	if(arg == NULL)
	{
		pthread_exit(NULL);
	}
	
	DCISvcCtx *pSvcCtx = (DCISvcCtx*)arg;
	
	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		report_error(pError);
		pthread_exit(NULL);
	}
	
	if (DCIStmtPrepare(pStmt, pError, sqlTest, strlen((char*)sqlTest), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("Test if kingbase support datetime PREPARE failed.\n");
		report_error(pError);
		goto free_resource;
	}

	err = DCIBindByName(pStmt, &pBind[0], pError, (DciText *)":index", strlen(":index"), (void*) &int_value, sizeof(int), SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	err = DCIBindArrayOfStruct(pBind[0], pError, sizeof(int), 0, 0, 0);
	
	err = DCIBindByName(pStmt, &pBind[1], pError, (DciText *)":cursor1", strlen(":cursor1"), &m_stmhp1, 4, SQLT_RSET, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	err = DCIBindArrayOfStruct(pBind[1], pError, sizeof(DCIStmt *), 0, 0, 0);

	err = DCIBindByName(pStmt, &pBind[2], pError, (DciText *)":cursor2", strlen(":cursor2"), &m_stmhp2, 4, SQLT_RSET, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	err = DCIBindArrayOfStruct(pBind[2], pError, sizeof(DCIStmt *), 0, 0, 0);
	

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;
		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);
		goto free_resource;
	}

	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
		goto free_resource;
	}
	
	if (m_stmhp1)
	{
		ub4 cFetchRows = 1;
		err = Fetch(pEnv, m_stmhp1, pError, cFetchRows);
		err = DCIHandleFree((void *) m_stmhp1, DCI_HTYPE_STMT);
	}

	if (m_stmhp2)
	{
		ub4 cFetchRows = 1;
		err = Fetch(pEnv, m_stmhp2, pError, cFetchRows);
		err = DCIHandleFree((void *) m_stmhp2, DCI_HTYPE_STMT);
	}

free_resource:
	err = DCIHandleFree((void *) pStmt, DCI_HTYPE_STMT);
	pthread_exit(NULL);
	
}

void *work_commit_thread(void *arg)
{
	int count = 0;
	if(arg == NULL)
	{
		pthread_exit(NULL);
	}
	DCISvcCtx *pSvcCtx = (DCISvcCtx*)arg;

	while(1)
	{
		err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			printf("DCITransCommit failed\n");
		}
	
		sleep(0.5);
		if(++count == 10)
			break;
	}
	
	err = DCITransCommit(pSvcCtx, pError,(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("DCITransCommit failed\n");
	}
	pthread_exit(NULL);
}


#endif



sword 
test_out_refcursor_holdable()
{
	int ret = DCI_SUCCESS;

	err = test_execute("CREATE OR REPLACE PROCEDURE pro_multpthread(in p1 smallint, out refcur1 refcursor, out refcur2 refcursor) as "
	"begin open refcur1 for select cast('2015-05-25 10:33:28' as timestamp) from dual limit p1;open refcur2 for select cast('2015-05-25 10:33:28' as timestamp) from dual limit p1;end;");

	check(err, "test_proc_outrefcursor: create procedure pro_multpthread");
	err = connect(true);

#ifdef WIN32
	HANDLE handle1=CreateThread(NULL,0,work_bind_cursor_thread,NULL,0,NULL); 
	HANDLE handle2=CreateThread(NULL,0,work_commit_thread,NULL,0,NULL); 
	WaitForSingleObject(handle1, INFINITE);  
	WaitForSingleObject(handle2, INFINITE);  
#else
	int i = 0;
	pthread_t threadid[2];
	
	if((ret = pthread_create(&threadid[0], NULL, work_bind_cursor_thread,(void*)pSvcCtx)) != 0)
	{  
		printf("can't create thread:%d  %s\n",i, strerror(ret));
	}

	if((ret = pthread_create(&threadid[1], NULL, work_commit_thread,(void*)pSvcCtx)) != 0)
	{  
		printf("can't create thread:%d  %s\n",i, strerror(ret));
	}

	for (i = 0; i < 2; i++)
	{
		pthread_join(threadid[i], NULL);
	}
#endif
	err = disconnect(true);

	err = test_execute("DROP PROCEDURE pro_multpthread(in p1 smallint, out refcur1 refcursor, out refcur2 refcursor)");
	check(err, "test_proc_outrefcursor: drop procedure pro_multpthread");

	
	return ret;
	

}
