#include "DciTest.h"

char	*table_name = "T_NUMBER";
char	sql[256] = "";

typedef struct
{
	int		precision;
	int		scale;
	char	insert_str[50];
	char	fetch_str[50];

	DCINumber	num;
}TestCase;

TestCase testcase[] =
{
	{1, 0, 	"0", "0", {"\x01\x80"}},

	{5, 0, 	"99999", "99999", {"\x04\xc3\x0a\x64\x64"}},
	{5, 0, 	"-99999", "-99999", {"\x05\x3c\x5c\x02\x02\x66"}},
	{5, 0, 	"12", "12", {"\x02\xc1\x0d"}},
	{5, 0, 	"-12", "-12", {"\x03\x3e\x59\x66"}},
	{5, 0, 	"123", "123",  {"\x03\xc2\x02\x18"}},
	{5, 0, 	"-123", "-123", {"\x04\x3d\x64\x4e\x66"}},

	{5, 0, 	"1000", "1000", {"\x02\xc2\x0b"}},
	{5, 0, 	"-1000", "-1000", {"\x03\x3d\x5b\x66"}},
	{5, 0, 	"10000", "10000", {"\x02\xc3\x02"}},
	{5, 0, 	"-10000", "-10000", {"\x03\x3c\x64\x66"}},

	{15, 5, "123.456", "123.45600", {"\x05\xc2\x02\x18\x2e\x3d"}},
	{15, 5, "-123.456", "-123.45600", {"\x06\x3d\x64\x4e\x38\x29\x66"}},
	{15, 5, "1234567890.12345", "1234567890.12345", {"\x09\xc5\x0d\x23\x39\x4f\x5b\x0d\x23\x33"}},
	{15, 5, "-1234567890.12345", "-1234567890.12345", {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}},

	{15, 5, "123450000", "123450000.00000", {"\x04\xc5\x02\x18\x2e"}},
	{15, 5, "-123450000", "-123450000.00000", {"\x05\x3a\x64\x4e\x38\x66"}},
	{15, 5, "12345000", "12345000.00000", {"\x04\xc4\x0d\x23\x33"}},
	{15, 5, "-12345000", "-12345000.00000", {"\x05\x3b\x59\x43\x33\x66"}},

	{15, 15, "0.123456", ".123456000000000", {"\x04\xc0\x0d\x23\x39"}},
	{15, 15, "-0.123456", "-.123456000000000", {"\x05\x3f\x59\x43\x2d\x66"}},
	{15, 15, "0.12345", ".123450000000000", {"\x04\xc0\x0d\x23\x33"}},
	{15, 15, "-0.12345", "-.123450000000000", {"\x05\x3f\x59\x43\x33\x66"}},

	{15, 15, "0.000123456", ".000123456000000", {"\x05\xbf\x02\x18\x2e\x3d"}},
	{15, 15, "-0.000123456", "-.000123456000000", {"\x06\x40\x64\x4e\x38\x29\x66"}},
	{15, 15, "0.0000123456", ".000012345600000", {"\x04\xbe\x0d\x23\x39"}},
	{15, 15, "-0.0000123456", "-.000012345600000", {"\x05\x41\x59\x43\x2d\x66"}},

	{38, 0, "99999999999999999999999999999999999999", "99999999999999999999999999999999999999", {"\x14\xd3\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64"}},
	{38, 0, "-99999999999999999999999999999999999999", "-99999999999999999999999999999999999999", {"\x15\x2c\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02"}}, //差一个0x66
	{38, 38, "0.99999999999999999999999999999999999999", ".99999999999999999999999999999999999999", {"\x14\xc0\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64"}},
	{38, 38, "-0.99999999999999999999999999999999999999", "-.99999999999999999999999999999999999999", {"\x15\x3f\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02"}} /*差一个0x66 */
};

sword
TestNumberFetch()
{
	int		i, j, k, failed = 0;
	sword	retcode = DCI_SUCCESS;
	char	errbuf[256];
	sb4		ret;
	ub4		len = 0;

	DCIStmt		*stmthp = NULL;
	DCIDefine	*pDCIDefine;

	DCINumber	out_num;
	char		out_str[50];
	DciText		fmt[50];
	char		*tmp_dst = NULL;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	//结果集使用 SQLT_VNU 绑定
	//使用字符串的形式插入数据，以 DCINumber形式取出
	//比较取出的 DCINumber 和 转换后的字符串
	for (i = 0; i < (int)(sizeof(testcase) / sizeof(testcase[0])); i++)
	{
		if (testcase[i].num.DCINumberPart[0] == DCI_NUMBER_SIZE - 1)
			testcase[i].num.DCINumberPart[21] = '\x66';

		retcode = DCIHandleAlloc((void *)pEnv, (void **)&stmthp, (const ub4)DCI_HTYPE_STMT, 0, NULL);
		if (retcode != DCI_SUCCESS)
		{
			retcode = DCI_ERROR;
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n", errbuf);
			goto end;
		}

		//drop table
		sprintf(sql, "drop table %s", table_name);
		retcode = DCIStmtPrepare(stmthp, pError,(DciText *)sql, (ub4)strlen(sql),DCI_NTV_SYNTAX, DCI_DEFAULT);
		retcode = DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4)0, (CONST DCISnapshot*) 0, (DCISnapshot*) 0, (ub4) DCI_DEFAULT) ;

		//create table
		sprintf(sql, "create table %s(c numeric(%d, %d))", table_name, testcase[i].precision, testcase[i].scale);
		retcode = DCIStmtPrepare(stmthp, pError,(DciText *)sql, (ub4)strlen(sql),DCI_NTV_SYNTAX, DCI_DEFAULT);
		retcode = DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4)0, (CONST DCISnapshot*) 0, (DCISnapshot*) 0, (ub4) DCI_DEFAULT) ;
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		//insert data
		sprintf(sql, "insert into %s values('%s')", table_name, testcase[i].insert_str);
		retcode = DCIStmtPrepare(stmthp, pError,(DciText *)sql, (ub4)strlen(sql),DCI_NTV_SYNTAX, DCI_DEFAULT);
		retcode = DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4)0, (CONST DCISnapshot*) 0, (DCISnapshot*) 0, (ub4) DCI_DEFAULT) ;
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		//select
		sprintf(sql, "select * from %s", table_name);
		retcode = DCIStmtPrepare(stmthp, pError,(DciText *)sql, (ub4)strlen(sql),DCI_NTV_SYNTAX, DCI_DEFAULT);
		retcode = DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)0, (ub4)0, (CONST DCISnapshot*) 0, (DCISnapshot*) 0, (ub4) DCI_DEFAULT) ;
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		retcode = DCIDefineByPos(stmthp, &pDCIDefine, pError, 1, &out_num, sizeof(DCINumber), SQLT_VNU, 0, 0, 0, DCI_DEFAULT);
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}
		memset(&out_num, 0, DCI_NUMBER_SIZE);
		retcode = DCIStmtFetch(stmthp, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT);
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		//期望值与实际值的比较
		//DCINumber
		if (out_num.DCINumberPart[0] != testcase[i].num.DCINumberPart[0])
			failed++;
		else
		{
			retcode = memcmp((void *)&out_num, (void *)&(testcase[i].num), out_num.DCINumberPart[0] + 1);
			if (retcode != 0)
				failed++;
		}

		//text
		len = 50;
		k = 0;
		fmt[k++] = '9';
		for (j = 1; j < testcase[i].precision - testcase[i].scale; j++)
			fmt[k++] = '9';
		if (testcase[i].scale > 0)
			fmt[k++] = '.';
		for (j = 0; j < testcase[i].scale; j++)
			fmt[k++] = '9';
		fmt[k] = '\0';

		memset(out_str, 0, len);
		retcode = DCINumberToText(pError, &out_num, (const DciText *)fmt, (ub4)strlen((char *)fmt), NULL, 0, &len, (DciText *)out_str);
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		tmp_dst = out_str;
		while (*tmp_dst == ' ')
			tmp_dst++;

		//期望值与实际值的比较
		//字符串
		if (strcmp(tmp_dst, testcase[i].fetch_str) != 0)
			failed++;

		retcode = DCITransRollback(pSvcCtx, pError, (ub4)DCI_DEFAULT);
		if (retcode != DCI_SUCCESS)
			retcode = DCI_ERROR;

		retcode = DCIHandleFree((void *)stmthp, (const ub4)DCI_HTYPE_STMT);
	}

end:
	err = disconnect(true);

	if (failed == 0)
		return DCI_SUCCESS;
	else
	{
		printf("failed %d\n", failed);
		return DCI_ERROR;
	}
}

sword
TestNumberInsert()
{
	int		i, failed = 0;
	sword	retcode = DCI_SUCCESS;
	char	errbuf[256];
	sb4		ret;

	DCIStmt		*stmthp = NULL;
	DCIBind		*pDCIBind = NULL;
	DCIDefine	*pDCIDefine = NULL;

	char		out_str[50];

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	//insert SQLT_VNU
	//使用绑定参数的形式插入数据，以字符串形式取出
	//比较取出的字符串 和 转换后得到的 DCINumber
	for (i = 0; i < (int)(sizeof(testcase) / sizeof(testcase[0])); i++)
	{
		retcode = DCIHandleAlloc((void *)pEnv, (void **)&stmthp, (const ub4)DCI_HTYPE_STMT, 0, NULL);
		if (retcode != DCI_SUCCESS)
		{
			retcode = DCI_ERROR;
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
			goto end;
		}

		//drop table
		sprintf(sql, "drop table %s", table_name);
		retcode = DCIStmtPrepare(stmthp, pError,(DciText *)sql, (ub4)strlen(sql),DCI_NTV_SYNTAX, DCI_DEFAULT);
		retcode = DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4)0, (CONST DCISnapshot*) 0, (DCISnapshot*) 0, (ub4) DCI_DEFAULT) ;

		//create table
		sprintf(sql, "create table %s(c number(%d, %d))", table_name, testcase[i].precision, testcase[i].scale);
		retcode = DCIStmtPrepare(stmthp, pError,(DciText *)sql, (ub4)strlen(sql),DCI_NTV_SYNTAX, DCI_DEFAULT);
		retcode = DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4)0, (CONST DCISnapshot*) 0, (DCISnapshot*) 0, (ub4) DCI_DEFAULT) ;
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		//insert data
		sprintf(sql, "insert into %s values(:0)", table_name);
		retcode = DCIStmtPrepare(stmthp, pError,(DciText *)sql, (ub4)strlen(sql),DCI_NTV_SYNTAX, DCI_DEFAULT);

		retcode = DCIBindByPos(stmthp, &pDCIBind, pError, 1, &(testcase[i].num), sizeof(DCINumber), SQLT_VNU, (void *)0, (ub2 *)0, 
			(ub2 *)0, (ub4)0,(ub4 *)0, (ub4)DCI_DEFAULT);
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		retcode = DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4)0, (CONST DCISnapshot*) 0, (DCISnapshot*) 0, (ub4) DCI_DEFAULT) ;
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		//select
		sprintf(sql, "select * from %s", table_name);
		retcode = DCIStmtPrepare(stmthp, pError,(DciText *)sql, (ub4)strlen(sql),DCI_NTV_SYNTAX, DCI_DEFAULT);
		retcode = DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)0, (ub4)0, (CONST DCISnapshot*) 0, (DCISnapshot*) 0, (ub4) DCI_DEFAULT) ;
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		memset(out_str, 0, 50);
		retcode = DCIDefineByPos(stmthp, &pDCIDefine, pError,1, &out_str, 50 - 2, SQLT_VCS, 0, 0, 0, DCI_DEFAULT);
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}
		retcode = DCIStmtFetch(stmthp, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT);
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		//期望值与实际值的比较
		if (strncmp(out_str + 2, testcase[i].fetch_str, *(unsigned short *)out_str) != 0)
		{
			if (*(out_str + 2) == '0' && *(out_str + 3) == '.')
			{
				if (strncmp(out_str + 3, testcase[i].fetch_str, *(unsigned short *)out_str - 1) != 0)
					failed++;
			}
			else if (*(out_str + 2) == '-' && *(out_str + 3) == '0' && *(out_str + 4) == '.')
			{
				if (*(testcase[i].fetch_str) != '-')
					failed++;
				if (strncmp(out_str + 4, testcase[i].fetch_str + 1, *(unsigned short *)out_str - 2) != 0)
					failed++;
			}
			else
				failed++;
		}

		retcode = DCITransRollback(pSvcCtx, pError, (ub4)DCI_DEFAULT);
		if (retcode != DCI_SUCCESS)
			retcode = DCI_ERROR;

		retcode = DCIHandleFree((void *)stmthp, (const ub4)DCI_HTYPE_STMT);
	}

end:
	err = disconnect(true);

	if (failed == 0)
		return DCI_SUCCESS;
	else
	{
		printf("failed %d\n", failed);
		return DCI_ERROR;
	}
}

#define NUM_ROWS	3

typedef struct 
{
	char	insert_str[50];
	char	fetch_str[50];

	DCINumber	num;
}TestCase_Batch;

TestCase_Batch testcase_batch[] = 
{
	{"123.456", "123.45600", {"\x05\xc2\x02\x18\x2e\x3d"}},
	{"-123.456", "-123.45600", {"\x06\x3d\x64\x4e\x38\x29\x66"}},
	{"1234567890.12345", "1234567890.12345", {"\x09\xc5\x0d\x23\x39\x4f\x5b\x0d\x23\x33"}},

	{"-1234567890.12345", "-1234567890.12345", {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}},
	{"123450000", "123450000.00000", {"\x04\xc5\x02\x18\x2e"}},
	{"-123450000", "-123450000.00000", {"\x05\x3a\x64\x4e\x38\x66"}},

	{"12345000", "12345000.00000", {"\x04\xc4\x0d\x23\x33"}},
	{"-12345000", "-12345000.00000", {"\x05\x3b\x59\x43\x33\x66"}},
};

sword
TestNumberBatchFetch()
{
	int		i, failed = 0;
	sword	retcode = DCI_SUCCESS;
	char	errbuf[256];
	sb4		ret;

	DCIStmt		*stmthp = NULL;
	DCIDefine	*pDCIDefine;

	DCINumber	out_num[NUM_ROWS];

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	//结果集使用 SQLT_VNU 批量绑定，以 DCINumber形式取出
	//比较取出的 DCINumber 和 转换后的字符串
	retcode = DCIHandleAlloc((void *)pEnv, (void **)&stmthp, (const ub4)DCI_HTYPE_STMT, 0, NULL);
	if (retcode != DCI_SUCCESS)
	{
		retcode = DCI_ERROR;
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n", errbuf);
		goto end;
	}

	//drop table
	sprintf(sql, "drop table %s", table_name);
	retcode = DCIStmtPrepare(stmthp, pError,(DciText *)sql, (ub4)strlen(sql),DCI_NTV_SYNTAX, DCI_DEFAULT);
	retcode = DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4)0, (CONST DCISnapshot*) 0, (DCISnapshot*) 0, (ub4) DCI_DEFAULT) ;

	//create table
	sprintf(sql, "create table %s(c numeric(15, 5))", table_name);
	retcode = DCIStmtPrepare(stmthp, pError,(DciText *)sql, (ub4)strlen(sql),DCI_NTV_SYNTAX, DCI_DEFAULT);
	retcode = DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4)0, (CONST DCISnapshot*) 0, (DCISnapshot*) 0, (ub4) DCI_DEFAULT) ;
	if (retcode != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}
	
	for (i = 0; i < (int)(sizeof(testcase_batch) / sizeof(testcase_batch[0])); i++)
	{
		//insert data
		sprintf(sql, "insert into %s values('%s')", table_name, testcase_batch[i].insert_str);
		retcode = DCIStmtPrepare(stmthp, pError,(DciText *)sql, (ub4)strlen(sql),DCI_NTV_SYNTAX, DCI_DEFAULT);
		retcode = DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4)0, (CONST DCISnapshot*) 0, (DCISnapshot*) 0, (ub4) DCI_DEFAULT) ;
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}
	}

	//select
	sprintf(sql, "select * from %s", table_name);
	retcode = DCIStmtPrepare(stmthp, pError,(DciText *)sql, (ub4)strlen(sql),DCI_NTV_SYNTAX, DCI_DEFAULT);
	retcode = DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)0, (ub4)0, (CONST DCISnapshot*) 0, (DCISnapshot*) 0, (ub4) DCI_DEFAULT) ;
	if (retcode != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCIDefineByPos(stmthp, &pDCIDefine, pError, 1, &out_num, sizeof(DCINumber), SQLT_VNU, 0, 0, 0, DCI_DEFAULT);
	if (retcode != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCIDefineArrayOfStruct(pDCIDefine,pError, sizeof(DCINumber), 0, 0, 0);
	if (retcode != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}


	for (i = 0; i < (int)(sizeof(testcase_batch) / sizeof(testcase_batch[0])); i += NUM_ROWS)
	{
		ub4 rows = 0, j = 0;

		memset(out_num, 0, DCI_NUMBER_SIZE * NUM_ROWS);
		retcode = DCIStmtFetch(stmthp, pError, NUM_ROWS, DCI_FETCH_NEXT, DCI_DEFAULT);
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		err = DCIAttrGet(stmthp, DCI_HTYPE_STMT, &rows, 0, DCI_ATTR_ROW_COUNT, pError);

		//期望值与实际值的比较
		//DCINumber
		for (j = 0; j < rows - i; j++)
		{
			if (out_num[j].DCINumberPart[0] != testcase_batch[i + j].num.DCINumberPart[0])
				failed++;
			else
			{
				retcode = memcmp((void *)&(out_num[j]), (void *)&(testcase_batch[i + j].num), out_num[j].DCINumberPart[0] + 1);
				if (retcode != 0)
					failed++;
			}
		}
	}

	retcode = DCIHandleFree((void *)stmthp, (const ub4)DCI_HTYPE_STMT);

end:
	err = disconnect(true);

	if (failed == 0)
		return DCI_SUCCESS;
	else
	{
		printf("failed %d\n", failed);
		return DCI_ERROR;
	}
}

sword
TestNumberBatchInsert()
{
	int		i, failed = 0;
	sword	retcode = DCI_SUCCESS;
	char	errbuf[256];
	sb4		ret;

	DCIStmt		*stmthp = NULL;
	DCIBind		*pDCIBind = NULL;
	DCIDefine	*pDCIDefine = NULL;

	DCINumber	in_num[NUM_ROWS];

	char		out_str[50];

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	//insert SQLT_VNU
	//使用批量绑定参数的形式插入数据，以字符串形式取出

	retcode = DCIHandleAlloc((void *)pEnv, (void **)&stmthp, (const ub4)DCI_HTYPE_STMT, 0, NULL);
	if (retcode != DCI_SUCCESS)
	{
		retcode = DCI_ERROR;
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
		goto end;
	}

	//drop table
	sprintf(sql, "drop table %s", table_name);
	retcode = DCIStmtPrepare(stmthp, pError,(DciText *)sql, (ub4)strlen(sql),DCI_NTV_SYNTAX, DCI_DEFAULT);
	retcode = DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4)0, (CONST DCISnapshot*) 0, (DCISnapshot*) 0, (ub4) DCI_DEFAULT) ;

	//create table
	sprintf(sql, "create table %s(c number(15, 5))", table_name);
	retcode = DCIStmtPrepare(stmthp, pError,(DciText *)sql, (ub4)strlen(sql),DCI_NTV_SYNTAX, DCI_DEFAULT);
	retcode = DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)1, (ub4)0, (CONST DCISnapshot*) 0, (DCISnapshot*) 0, (ub4) DCI_DEFAULT) ;
	if (retcode != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	//insert data
	sprintf(sql, "insert into %s values(:0)", table_name);
	retcode = DCIStmtPrepare(stmthp, pError,(DciText *)sql, (ub4)strlen(sql),DCI_NTV_SYNTAX, DCI_DEFAULT);

	retcode = DCIBindByPos(stmthp, &pDCIBind, pError, 1, in_num, sizeof(DCINumber), SQLT_VNU, (void *)0, (ub2 *)0, 
		(ub2 *)0, (ub4)0,(ub4 *)0, (ub4)DCI_DEFAULT);

	if (retcode != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCIBindArrayOfStruct(pDCIBind, pError, sizeof(DCINumber), 0, 0, 0);

	if (retcode != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	for (i = 0; i < (int)(sizeof(testcase_batch) / sizeof(testcase_batch[0])); i += NUM_ROWS)
	{
		ub4 j = 0;

		for (j = 0; (j + i < sizeof(testcase_batch) / sizeof(testcase_batch[0])) && j < NUM_ROWS; j++)
			memcpy(&in_num[j], &(testcase_batch[i + j].num), DCI_NUMBER_SIZE);
		
		retcode = DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)j, (ub4)0, (CONST DCISnapshot*) 0, (DCISnapshot*) 0, (ub4) DCI_DEFAULT) ;
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}
	}

	//select
	sprintf(sql, "select * from %s", table_name);
	retcode = DCIStmtPrepare(stmthp, pError,(DciText *)sql, (ub4)strlen(sql),DCI_NTV_SYNTAX, DCI_DEFAULT);
	retcode = DCIStmtExecute(pSvcCtx, stmthp, pError, (ub4)0, (ub4)0, (CONST DCISnapshot*) 0, (DCISnapshot*) 0, (ub4) DCI_DEFAULT) ;
	if (retcode != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}
	retcode = DCIDefineByPos(stmthp, &pDCIDefine, pError,1, &out_str, 50 - 2, SQLT_VCS, 0, 0, 0, DCI_DEFAULT);
	if (retcode != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	for (i = 0; i < (int)(sizeof(testcase_batch) / sizeof(testcase_batch[0])); i++)
	{
		memset(out_str, 0, 50);
		retcode = DCIStmtFetch(stmthp, pError, 1, DCI_FETCH_NEXT, DCI_DEFAULT);
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		//期望值与实际值的比较
		if (strncmp(out_str + 2, testcase_batch[i].fetch_str, *(unsigned short *)out_str) != 0)
			failed++;
	}

	retcode = DCITransRollback(pSvcCtx, pError, (ub4)DCI_DEFAULT);
	if (retcode != DCI_SUCCESS)
		retcode = DCI_ERROR;

	retcode = DCIHandleFree((void *)stmthp, (const ub4)DCI_HTYPE_STMT);

end:
	err = disconnect(true);

	if (failed == 0)
		return DCI_SUCCESS;
	else
	{
		printf("failed %d\n", failed);
		return DCI_ERROR;
	}
}

typedef struct  
{
	char	str[50];
	int		isNegative;
	ub2		minBytes_sb;
	ub2		minBytes_ub;

	DCINumber num;

	sb1 n_sb1;
	ub1 n_ub1;

	sb2 n_sb2;
	ub2 n_ub2;

	sb4 n_sb4;
	ub4 n_ub4;

	sb8 n_sb8;
	ub8 n_ub8;
}TestCase_Int;

TestCase_Int testcase_int[] =
{
	// = 0
	{"0", 0, 1, 1, {"\x01\x80"}, 0, 0, 0, 0, 0, 0, 0, 0},

	// 1 bytes
	{"12", 0, 1, 1, {"\x02\xc1\x0d"}, 12, 12, 12, 12, 12, 12, 12, 12},
	{"-12", 1, 1, 0, {"\x03\x3e\x59\x66"}, -12, 0, -12, 0, -12, 0, -12, 0},

	// 1 bytes ～ 2 bytes
	{"128", 0, 2, 1, {"\x03\xc2\x02\x1d"}, 127, 128, 128, 128, 128, 128, 128, 128},
	{"-128", 1, 1, 0, {"\x04\x3d\x64\x49\x66"}, -128, 0, -128, 0, -128, 0, -128, 0},

	{"256", 0, 2, 2, {"\x03\xc2\x03\x39"}, 127, 255, 256, 256, 256, 256, 256, 256},
	{"-256", 1, 2, 0, {"\x04\x3d\x63\x2d\x66"}, -128, 0, -256, 0, -256, 0, -256, 0},

	// 2 bytes
	{"10000", 0, 2, 2, {"\x02\xc3\x02"}, 127, 255, 10000, 10000, 10000, 10000, 10000, 10000},
	{"-10000", 1, 2, 0, {"\x03\x3c\x64\x66"}, -128, 0, -10000, 0, -10000, 0, -10000, 0},

	// 2 bytes ~ 4 bytes
	{"32768", 0, 4, 2, {"\x04\xc3\x04\x1c\x45"}, 127, 255, 32767, 32768, 32768, 32768, 32768, 32768},
	{"-32768", 1, 2, 0, {"\x05\x3c\x62\x4a\x21\x66"}, -128, 0, -32768, 0, -32768, 0, -32768, 0},

	{"65536", 0, 4, 4, {"\x04\xc3\x07\x38\x25"}, 127, 255, 32767, 65535, 65536, 65536, 65536, 65536},
	{"-65536", 1, 4, 0, {"\x05\x3c\x5f\x2e\x41\x66"}, -128, 0, -32768, 0, -65536, 0, -65536, 0},

	// 4 bytes
	{"123450000", 0, 4, 4, {"\x04\xc5\x02\x18\x2e"}, 127, 255, 32767, 65535, 123450000, 123450000, 123450000, 123450000},
	{"-123450000", 1, 4, 0, {"\x05\x3a\x64\x4e\x38\x66"}, -128, 0, -32768, 0, -123450000, 0, -123450000, 0},

	// 4 bytes ～ 8 bytes
	{"2147483648", 0, 8, 4, {"\x06\xc5\x16\x30\x31\x25\x31"}, 127, 255, 32767, 65535, 2147483647, 2147483648UL, 2147483648UL, 2147483648UL},
	{"-2147483648", 1, 4, 0, {"\x07\x3a\x50\x36\x35\x41\x35\x66"}, -128, 0, -32768, 0, -2147483647-1, 0, -2147483647-1, 0},

	{"4294967296", 0, 8, 8, {"\x06\xc5\x2b\x5f\x61\x49\x61"}, 127, 255, 32767, 65535, 2147483647, 4294967295UL, 4294967296ULL, 4294967296ULL},
	{"-4294967296", 1, 8, 0, {"\x07\x3a\x3b\x07\x05\x1d\x05\x66"}, -128, 0, -32768, 0, -2147483647-1, 0, -4294967296LL, 0},

	// 8 bytes
	{"123456789012345", 0, 8, 8, {"\x09\xc8\x02\x18\x2e\x44\x5a\x02\x18\x2e"}, 127, 255, 32767, 65535, 2147483647, 4294967295UL, 123456789012345ULL, 123456789012345ULL},
	{"-123456789012345", 1, 8, 0, {"\x0a\x37\x64\x4e\x38\x22\x0c\x64\x4e\x38\x66"}, -128, 0, -32768, 0, -2147483647-1, 0, -123456789012345LL, 0},

	{"9223372036854775808", 0, 16, 8, {"\x0b\xca\x0a\x17\x22\x49\x04\x45\x37\x4e\x3b\x09"}, 127, 255, 32767, 65535, 2147483647, 4294967295UL, 9223372036854775807ULL, 9223372036854775808ULL},
	{"-9223372036854775808", 1, 8, 0, {"\x0c\x35\x5c\x4f\x44\x1d\x62\x21\x2f\x18\x2b\x5d\x66"}, -128, 0, -32768, 0, -2147483647-1, 0, -9223372036854775807LL-1, 0},

	{"18446744073709551616", 0, 16, 16, {"\x0b\xca\x13\x2d\x44\x2d\x08\x26\x0a\x38\x11\x11"}, 127, 255, 32767, 65535, 2147483647, 4294967295UL, 9223372036854775807ULL, 18446744073709551615ULL},
	{"-18446744073709551616", 1, 16, 0, {"\x0c\x35\x53\x39\x22\x39\x5e\x40\x5c\x2e\x55\x55\x66"}, -128, 0, -32768, 0, -2147483647-1, 0, -9223372036854775807LL-1, 0},

	// > 8 bytes
	{"99999999999999999999999999999999999999", 0, 16, 16, {"\x14\xd3\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64"}, 127, 255, 32767, 65535, 2147483647, 4294967295UL, 9223372036854775807ULL, 18446744073709551615ULL},
	//{"-99999999999999999999999999999999999999", 1, 16, 0, {"\x15\x2c\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02"}, -128, 0, -32768, 0, -2147483647-1, 0, -9223372036854775807LL-1, 0}, //差一个0x66

	// float
	{"1234567890.12345", 0, 4, 4, {"\x09\xc5\x0d\x23\x39\x4f\x5b\x0d\x23\x33"}, 127, 255, 32767, 65535, 1234567890, 1234567890, 1234567890, 1234567890},
	{"-1234567890.12345", 1, 4, 4, {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}, -128, 0, -32768, 0, -1234567890, 0, -1234567890, 0},

};

sword
TestDciNumberFromInt()
{
	int		i, failed = 0;
	sword	retcode = DCI_SUCCESS;
	char	errbuf[256];
	sb4		ret;

	DCINumber	out_num, tmp_num;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	for (i = 0; i < (int)(sizeof(testcase_int) / sizeof(testcase_int[0]) - 2); i++) //不测最后两个浮点数的情况
	{
		//sb1
		if (testcase_int[i].minBytes_sb <= sizeof(sb1))
		{
			memset(&out_num, 0, DCI_NUMBER_SIZE);
			retcode = DCINumberFromInt(pError, (CONST dvoid *)&testcase_int[i].n_sb1, sizeof(sb1), DCI_NUMBER_SIGNED, &out_num);
			if (retcode != DCI_SUCCESS)
			{
				failed++;
				DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
				printf("errhp =%s\n",errbuf);
			}
			if (out_num.DCINumberPart[0] != testcase_int[i].num.DCINumberPart[0])
				failed++;
			else if (memcmp(&out_num, &(testcase_int[i].num), out_num.DCINumberPart[0] + 1) != 0)
				failed++;
		}

		//ub1
		if (testcase_int[i].minBytes_ub <= sizeof(ub1) && testcase_int[i].isNegative == 0)
		{
			memset(&out_num, 0, DCI_NUMBER_SIZE);
			retcode = DCINumberFromInt(pError, (CONST dvoid *)&testcase_int[i].n_ub1, sizeof(ub1), DCI_NUMBER_UNSIGNED, &out_num);
			if (retcode != DCI_SUCCESS)
			{
				failed++;
				DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
				printf("errhp =%s\n",errbuf);
			}
			if (out_num.DCINumberPart[0] != testcase_int[i].num.DCINumberPart[0])
				failed++;
			else if (memcmp(&out_num, &(testcase_int[i].num), out_num.DCINumberPart[0] + 1) != 0)
				failed++;
		}

		//sb2
		if (testcase_int[i].minBytes_sb <= sizeof(sb2))
		{
			memset(&out_num, 0, DCI_NUMBER_SIZE);
			retcode = DCINumberFromInt(pError, (CONST dvoid *)&testcase_int[i].n_sb2, sizeof(sb2), DCI_NUMBER_SIGNED, &out_num);
			if (retcode != DCI_SUCCESS)
			{
				failed++;
				DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
				printf("errhp =%s\n",errbuf);
			}
			if (out_num.DCINumberPart[0] != testcase_int[i].num.DCINumberPart[0])
				failed++;
			else if (memcmp(&out_num, &(testcase_int[i].num), out_num.DCINumberPart[0] + 1) != 0)
				failed++;
		}

		//ub2
		if (testcase_int[i].minBytes_ub <= sizeof(ub2) && testcase_int[i].isNegative == 0)
		{
			memset(&out_num, 0, DCI_NUMBER_SIZE);
			retcode = DCINumberFromInt(pError, (CONST dvoid *)&testcase_int[i].n_ub2, sizeof(ub2), DCI_NUMBER_UNSIGNED, &out_num);
			if (retcode != DCI_SUCCESS)
			{
				failed++;
				DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
				printf("errhp =%s\n",errbuf);
			}
			if (out_num.DCINumberPart[0] != testcase_int[i].num.DCINumberPart[0])
				failed++;
			else if (memcmp(&out_num, &(testcase_int[i].num), out_num.DCINumberPart[0] + 1) != 0)
				failed++;
		}

		//sb4
		if (testcase_int[i].minBytes_sb <= sizeof(sb4))
		{
			memset(&out_num, 0, DCI_NUMBER_SIZE);
			retcode = DCINumberFromInt(pError, (CONST dvoid *)&testcase_int[i].n_sb4, sizeof(sb4), DCI_NUMBER_SIGNED, &out_num);
			if (retcode != DCI_SUCCESS)
			{
				failed++;
				DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
				printf("errhp =%s\n",errbuf);
			}
			if (out_num.DCINumberPart[0] != testcase_int[i].num.DCINumberPart[0])
				failed++;
			else if (memcmp(&out_num, &(testcase_int[i].num), out_num.DCINumberPart[0] + 1) != 0)
				failed++;
		}

		//ub4
		if (testcase_int[i].minBytes_ub <= sizeof(ub4) && testcase_int[i].isNegative == 0)
		{
			memset(&out_num, 0, DCI_NUMBER_SIZE);
			retcode = DCINumberFromInt(pError, (CONST dvoid *)&testcase_int[i].n_ub4, sizeof(ub4), DCI_NUMBER_UNSIGNED, &out_num);
			if (retcode != DCI_SUCCESS)
			{
				failed++;
				DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
				printf("errhp =%s\n",errbuf);
			}
			if (out_num.DCINumberPart[0] != testcase_int[i].num.DCINumberPart[0])
				failed++;
			else if (memcmp(&out_num, &(testcase_int[i].num), out_num.DCINumberPart[0] + 1) != 0)
				failed++;
		}

		//sb8
		if (testcase_int[i].minBytes_sb <= sizeof(sb8))
		{
			memset(&out_num, 0, DCI_NUMBER_SIZE);
			retcode = DCINumberFromInt(pError, (CONST dvoid *)&testcase_int[i].n_sb8, sizeof(sb8), DCI_NUMBER_SIGNED, &out_num);
			if (retcode != DCI_SUCCESS)
			{
				failed++;
				DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
				printf("errhp =%s\n",errbuf);
			}
			if (out_num.DCINumberPart[0] != testcase_int[i].num.DCINumberPart[0])
				failed++;
			else if (memcmp(&out_num, &(testcase_int[i].num), out_num.DCINumberPart[0] + 1) != 0)
				failed++;
		}

		//ub8
		if (testcase_int[i].minBytes_ub <= sizeof(ub8) && testcase_int[i].isNegative == 0)
		{
			memset(&out_num, 0, DCI_NUMBER_SIZE);
			retcode = DCINumberFromInt(pError, (CONST dvoid *)&testcase_int[i].n_ub8, sizeof(ub8), DCI_NUMBER_UNSIGNED, &out_num);
			if (retcode != DCI_SUCCESS)
			{
				failed++;
				DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
				printf("errhp =%s\n",errbuf);
			}
			if (out_num.DCINumberPart[0] != testcase_int[i].num.DCINumberPart[0])
				failed++;
			else if (memcmp(&out_num, &(testcase_int[i].num), out_num.DCINumberPart[0] + 1) != 0)
				failed++;
		}
	}

	//test invalid parameter
	retcode = DCINumberFromInt(NULL, (CONST dvoid *)&testcase_int[0].n_ub8, sizeof(ub8), DCI_NUMBER_UNSIGNED, &tmp_num);
	if (retcode != DCI_INVALID_HANDLE)
		failed++;

	retcode = DCINumberFromInt(pError, NULL, sizeof(ub8), DCI_NUMBER_UNSIGNED, &tmp_num);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCINumberFromInt(pError, (CONST dvoid *)&testcase_int[0].n_ub8, 0, DCI_NUMBER_UNSIGNED, &tmp_num);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCINumberFromInt(pError, (CONST dvoid *)&testcase_int[0].num, 3, DCI_NUMBER_UNSIGNED, &tmp_num);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCINumberFromInt(pError, (CONST dvoid *)&testcase_int[0].num, sizeof(ub8), 1, &tmp_num);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCINumberFromInt(pError, (CONST dvoid *)&testcase_int[0].num, sizeof(ub8), DCI_NUMBER_UNSIGNED, NULL);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	err = disconnect(true);

	if (failed == 0)
		return DCI_SUCCESS;
	else
	{
		printf("failed %d\n", failed);
		return DCI_ERROR;
	}
}

sword
TestDciNumberToInt()
{
	int		i, failed = 0;
	sword	retcode = DCI_SUCCESS;
	char	errbuf[256];
	sb4		ret;

	sb1 out_sb1;
	ub1 out_ub1;

	sb2 out_sb2;
	ub2 out_ub2;

	sb4 out_sb4;
	ub4 out_ub4;

	sb8 out_sb8;
	ub8 out_ub8;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	for (i = 0; i < (int)(sizeof(testcase_int) / sizeof(testcase_int[0])); i++)
	{
		out_sb1 = 0;
		out_ub1 = 0;
		out_sb2 = 0;
		out_ub2 = 0;
		out_sb4 = 0;
		out_ub4 = 0;
		out_sb8 = 0;
		out_ub8 = 0;

		//sb1
		retcode = DCINumberToInt(pError, (CONST DCINumber *)&testcase_int[i].num, sizeof(sb1), DCI_NUMBER_SIGNED, &out_sb1);
		if (testcase_int[i].minBytes_sb > sizeof(sb1))
		{
			if (retcode != DCI_ERROR)
				failed++;
		}
		else
		{
			if (retcode != DCI_SUCCESS)
				failed++;
		}
		if (retcode != DCI_SUCCESS)
		{
			errbuf[0] = '\0';
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		//ub1
		retcode = DCINumberToInt(pError, (CONST DCINumber *)&testcase_int[i].num, sizeof(ub1), DCI_NUMBER_UNSIGNED, &out_ub1);
		if (testcase_int[i].isNegative == 1)
		{
			if (retcode != DCI_ERROR)
				failed++;
		}
		else if (testcase_int[i].minBytes_ub > sizeof(ub1))
		{
			if (retcode != DCI_ERROR)
				failed++;
		}
		else if (retcode != DCI_SUCCESS)
		{
			failed++;
		}
		if (retcode != DCI_SUCCESS)
		{
			errbuf[0] = '\0';
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		//sb2
		retcode = DCINumberToInt(pError, (CONST DCINumber *)&testcase_int[i].num, sizeof(sb2), DCI_NUMBER_SIGNED, &out_sb2);
		if (testcase_int[i].minBytes_sb > sizeof(sb2))
		{
			if (retcode != DCI_ERROR)
				failed++;
		}
		else
		{
			if (retcode != DCI_SUCCESS)
				failed++;
		}
		if (retcode != DCI_SUCCESS)
		{
			errbuf[0] = '\0';
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		//ub2
		retcode = DCINumberToInt(pError, (CONST DCINumber *)&testcase_int[i].num, sizeof(ub2), DCI_NUMBER_UNSIGNED, &out_ub2);
		if (testcase_int[i].isNegative == 1)
		{
			if (retcode != DCI_ERROR)
				failed++;
		}
		else if (testcase_int[i].minBytes_ub > sizeof(ub2))
		{
			if (retcode != DCI_ERROR)
				failed++;
		}
		else if (retcode != DCI_SUCCESS)
		{
			failed++;
		}
		if (retcode != DCI_SUCCESS)
		{
			errbuf[0] = '\0';
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		//sb4
		retcode = DCINumberToInt(pError, (CONST DCINumber *)&testcase_int[i].num, sizeof(sb4), DCI_NUMBER_SIGNED, &out_sb4);
		if (testcase_int[i].minBytes_sb > sizeof(sb4))
		{
			if (retcode != DCI_ERROR)
				failed++;
		}
		else
		{
			if (retcode != DCI_SUCCESS)
				failed++;
		}
		if (retcode != DCI_SUCCESS)
		{
			errbuf[0] = '\0';
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		//ub4
		retcode = DCINumberToInt(pError, (CONST DCINumber *)&testcase_int[i].num, sizeof(ub4), DCI_NUMBER_UNSIGNED, &out_ub4);
		if (testcase_int[i].isNegative == 1)
		{
			if (retcode != DCI_ERROR)
				failed++;
		}
		else if (testcase_int[i].minBytes_ub > sizeof(ub4))
		{
			if (retcode != DCI_ERROR)
				failed++;
		}
		else if (retcode != DCI_SUCCESS)
		{
			failed++;
		}
		if (retcode != DCI_SUCCESS)
		{
			errbuf[0] = '\0';
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		//sb8
		retcode = DCINumberToInt(pError, (CONST DCINumber *)&testcase_int[i].num, sizeof(sb8), DCI_NUMBER_SIGNED, &out_sb8);
		if (testcase_int[i].minBytes_sb > sizeof(sb8))
		{
			if (retcode != DCI_ERROR)
				failed++;
		}
		else
		{
			if (retcode != DCI_SUCCESS)
				failed++;
		}
		if (retcode != DCI_SUCCESS)
		{
			errbuf[0] = '\0';
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		//ub8
		retcode = DCINumberToInt(pError, (CONST DCINumber *)&testcase_int[i].num, sizeof(ub8), DCI_NUMBER_UNSIGNED, &out_ub8);
		if (testcase_int[i].isNegative == 1)
		{
			if (retcode != DCI_ERROR)
				failed++;
		}
		else if (testcase_int[i].minBytes_ub > sizeof(ub8))
		{
			if (retcode != DCI_ERROR)
				failed++;
		}
		else if (retcode != DCI_SUCCESS)
		{
			failed++;
		}
		if (retcode != DCI_SUCCESS)
		{
			errbuf[0] = '\0';
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}

		if (out_sb1 != testcase_int[i].n_sb1)
			failed++;
		if (out_ub1 != testcase_int[i].n_ub1)
			failed++;		
		if (out_sb2 != testcase_int[i].n_sb2)
			failed++;		
		if (out_ub2 != testcase_int[i].n_ub2)
			failed++;		
		if (out_sb4 != testcase_int[i].n_sb4)
			failed++;		
		if (out_ub4 != testcase_int[i].n_ub4)
			failed++;		
		if (out_sb8 != testcase_int[i].n_sb8)
			failed++;		
		if (out_ub8 != testcase_int[i].n_ub8)
			failed++;
	}

	//test invalid parameter
	retcode = DCINumberToInt(NULL, (CONST DCINumber *)&testcase_int[0].num, sizeof(ub8), DCI_NUMBER_UNSIGNED, &out_ub8);
	if (retcode != DCI_INVALID_HANDLE)
		failed++;

	retcode = DCINumberToInt(pError, NULL, sizeof(ub8), DCI_NUMBER_UNSIGNED, &out_ub8);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCINumberToInt(pError, (CONST DCINumber *)&testcase_int[0].num, 0, DCI_NUMBER_UNSIGNED, &out_ub8);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCINumberToInt(pError, (CONST DCINumber *)&testcase_int[0].num, 3, DCI_NUMBER_UNSIGNED, &out_ub8);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCINumberToInt(pError, (CONST DCINumber *)&testcase_int[0].num, sizeof(ub8), 1, &out_ub8);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCINumberToInt(pError, (CONST DCINumber *)&testcase_int[0].num, sizeof(ub8), DCI_NUMBER_UNSIGNED, NULL);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	err = disconnect(true);

	if (failed == 0)
		return DCI_SUCCESS;
	else
	{
		printf("failed %d\n", failed);
		return DCI_ERROR;
	}
}

typedef struct  
{
	char	str[50];
	int		minBytes;

	DCINumber num;

	float	n_float4;
	double	n_float8;

	DCINumber out_float4;
	DCINumber out_float8;
}TestCase_Real;

TestCase_Real testcase_real[] =
{
	{"0", 4, {"\x01\x80"}, 0, 0, {"\x01\x80"}, {"\x01\x80"}},

	{"0.000123456", 4, {"\x05\xbf\x02\x18\x2e\x3d"}, 0.000123456f, 0.000123456, {"\x05\xbf\x02\x18\x2e\x3d"}, {"\x05\xbf\x02\x18\x2e\x3d"}},
	{"-0.000123456", 4, {"\x06\x40\x64\x4e\x38\x29\x66"}, -0.000123456f, -0.000123456, {"\x06\x40\x64\x4e\x38\x29\x66"}, {"\x06\x40\x64\x4e\x38\x29\x66"}},

	{"0.0000123456", 4, {"\x04\xbe\x0d\x23\x39"}, 0.0000123456f, 0.0000123456, {"\x04\xbe\x0d\x23\x39"}, {"\x04\xbe\x0d\x23\x39"}},
	{"-0.0000123456", 4, {"\x05\x41\x59\x43\x2d\x66"}, -0.0000123456f, -0.0000123456, {"\x05\x41\x59\x43\x2d\x66"}, {"\x05\x41\x59\x43\x2d\x66"}},

	{"123450000", 4, {"\x04\xc5\x02\x18\x2e"}, 123450000, 123450000, {"\x04\xc5\x02\x18\x2e"}, {"\x04\xc5\x02\x18\x2e"}},
	{"-123450000", 4, {"\x05\x3a\x64\x4e\x38\x66"}, -123450000, -123450000, {"\x05\x3a\x64\x4e\x38\x66"}, {"\x05\x3a\x64\x4e\x38\x66"}},

#ifdef WIN32
	{"1234567890.12345", 4, {"\x09\xc5\x0d\x23\x39\x4f\x5b\x0d\x23\x33"}, 1.2345679e9f, 1234567890.12345L, {"\x05\xc5\x0d\x23\x39\x51"}, {"\x09\xc5\x0d\x23\x39\x4f\x5b\x0d\x23\x33"}},
	{"-1234567890.12345", 4, {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}, -1.2345679e9f, -1234567890.12345L, {"\x06\x3a\x59\x43\x2d\x15\x66"}, {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}},

	{"123456789012345", 4, {"\x09\xc8\x02\x18\x2e\x44\x5a\x02\x18\x2e"}, 1.2345679e14f, 123456789012345L, {"\x05\xc8\x02\x18\x2e\x45"}, {"\x09\xc8\x02\x18\x2e\x44\x5a\x02\x18\x2e"}},
	{"-123456789012345", 4, {"\x0a\x37\x64\x4e\x38\x22\x0c\x64\x4e\x38\x66"}, -1.2345679e14f, -123456789012345.0L, {"\x06\x37\x64\x4e\x38\x21\x66"}, {"\x0a\x37\x64\x4e\x38\x22\x0c\x64\x4e\x38\x66"}},
#else
	{"1234567890.12345", 4, {"\x09\xc5\x0d\x23\x39\x4f\x5b\x0d\x23\x33"}, 1.234568e9f, 1234567890.12345L, {"\x05\xc5\x0d\x23\x39\x51"}, {"\x09\xc5\x0d\x23\x39\x4f\x5b\x0d\x23\x33"}},
	{"-1234567890.12345", 4, {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}, -1.234568e9f, -1234567890.12345L, {"\x06\x3a\x59\x43\x2d\x15\x66"}, {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}},

	{"123456789012345", 4, {"\x09\xc8\x02\x18\x2e\x44\x5a\x02\x18\x2e"}, 1.234568e14f, 123456789012345ULL, {"\x05\xc8\x02\x18\x2e\x45"}, {"\x09\xc8\x02\x18\x2e\x44\x5a\x02\x18\x2e"}},
	{"-123456789012345", 4, {"\x0a\x37\x64\x4e\x38\x22\x0c\x64\x4e\x38\x66"}, -1.234568e14f, -123456789012345.0L, {"\x06\x37\x64\x4e\x38\x21\x66"}, {"\x0a\x37\x64\x4e\x38\x22\x0c\x64\x4e\x38\x66"}},
#endif

	{"99999999999999999999999999999999999999", 4, {"\x14\xd3\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64"}, 1e38f, 1e38L, {"\x02\xd4\x02"}, {"\x02\xd4\x02"}},
	{"-99999999999999999999999999999999999999", 4, {"\x15\x2c\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02"}, -1e38f, -1e38L, {"\x03\x2b\x64\x66"}, {"\x03\x2b\x64\x66"}}, // num 差一个0x66

	{"59999999999999999999999999999999999999", 4, {"\x14\xd3\x3c\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64\x64"}, 6e37f, 6e37L, {"\x02\xd3\x3d"}, {"\x02\xd3\x3d"}},
	{"-59999999999999999999999999999999999999", 4, {"\x15\x2c\x2a\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02"}, -6e37f, -6e37L, {"\x03\x2c\x29\x66"}, {"\x03\x2c\x29\x66"}}, // num 差一个0x66

	//+/-1.0e46
	//{"10000000000000000000000000000000000000000000000", 8, {"\x02\xEF\x02"}, 3.4028235e38f, 9.9999999999999999e45f, {"\x00"}, {"\x00"}},
	//{"-10000000000000000000000000000000000000000000000", 8, {"\x03\x10\x64\x66"}, -3.4028235e38f, -9.9999999999999999e45f, {"\x00"}, {"\x00"}},

};

sword
TestDciNumberFromReal()
{
	int		i, failed = 0;
	sword	retcode = DCI_SUCCESS;
	char	errbuf[256];
	sb4		ret;

	DCINumber	out_num;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	for (i = 0; i < (int)(sizeof(testcase_real) / sizeof(testcase_real[0])); i++)
	{
		//float4
		memset(&out_num, 0, DCI_NUMBER_SIZE);
		retcode = DCINumberFromReal(pError, (CONST dvoid *)&testcase_real[i].n_float4, sizeof(float), &out_num);
		if (retcode != DCI_SUCCESS)
		{
			failed++;
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}
		if (out_num.DCINumberPart[0] != testcase_real[i].out_float4.DCINumberPart[0])
			failed++;
		else if (memcmp(&out_num, &(testcase_real[i].out_float4), out_num.DCINumberPart[0] + 1) != 0)
			failed++;

		//float8
		memset(&out_num, 0, DCI_NUMBER_SIZE);
		retcode = DCINumberFromReal(pError, (CONST dvoid *)&testcase_real[i].n_float8, sizeof(double), &out_num);
		if (retcode != DCI_SUCCESS)
		{
			failed++;
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}
		if (out_num.DCINumberPart[0] != testcase_real[i].out_float8.DCINumberPart[0])
			failed++;
		else if (memcmp(&out_num, &(testcase_real[i].out_float8), out_num.DCINumberPart[0] + 1) != 0)
			failed++;
	}

	//test invalid parameter
	retcode = DCINumberFromReal(NULL, (CONST dvoid *)&testcase_real[0].n_float4, sizeof(float), &out_num);
	if (retcode != DCI_INVALID_HANDLE)
		failed++;

	retcode = DCINumberFromReal(pError, NULL, sizeof(float), &out_num);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCINumberFromReal(pError, (CONST dvoid *)&testcase_real[0].n_float4, 0, &out_num);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCINumberFromReal(pError, (CONST dvoid *)&testcase_real[0].n_float4, 3, &out_num); // 执行成功？？？
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCINumberFromReal(pError, (CONST dvoid *)&testcase_real[0].n_float4, sizeof(float), NULL);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	err = disconnect(true);

	if (failed == 0)
		return DCI_SUCCESS;
	else
	{
		printf("failed %d\n", failed);
		return DCI_ERROR;
	}

}

sword
TestDciNumberToReal()
{
	int		i, failed = 0;
	sword	retcode = DCI_SUCCESS;
	char	errbuf[256];
	sb4		ret;

	float		out_float4;
	double		out_float8;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	for (i = 0; i < (int)(sizeof(testcase_real) / sizeof(testcase_real[0])); i++)
	{
		out_float4 = 0;
		out_float8 = 0;

		//float4
		retcode = DCINumberToReal(pError, (CONST DCINumber *)&testcase_real[i].num, sizeof(float), &out_float4);
		//if (testcase_real[i].minBytes > sizeof(float))
		//{
		//	if (retcode != DCI_ERROR)
		//		failed++;
		//}
		//else
		{
			if (retcode != DCI_SUCCESS)
				failed++;
		}
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}
		if ((out_float4 - testcase_real[i].n_float4) / out_float4 > 0.01 
			|| (out_float4 - testcase_real[i].n_float4) / out_float4 < -0.01 )
			failed++;

		//float8
		retcode = DCINumberToReal(pError, (CONST DCINumber *)&testcase_real[i].num, sizeof(double), &out_float8);
		//if (testcase_real[i].minBytes > sizeof(double))
		//{
		//	if (retcode != DCI_ERROR)
		//		failed++;
		//}
		//else
		{
			if (retcode != DCI_SUCCESS)
				failed++;
		}
		if (retcode != DCI_SUCCESS)
		{
			DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("errhp =%s\n",errbuf);
		}
		if ((out_float8 - testcase_real[i].n_float8) / out_float8 > 0.01
			|| (out_float8 - testcase_real[i].n_float8) / out_float8 < -0.01 )
			failed++;
	}

	//test invalid parameter
	retcode = DCINumberToReal(NULL, (CONST DCINumber *)&testcase_real[0].n_float4, sizeof(float), &out_float4);
	if (retcode != DCI_INVALID_HANDLE)
		failed++;

	retcode = DCINumberToReal(pError, NULL, sizeof(float), &out_float4);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCINumberToReal(pError, (CONST DCINumber *)&testcase_real[0].n_float4, 0, &out_float4);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCINumberToReal(pError, (CONST DCINumber *)&testcase_real[0].n_float4, 3, &out_float4); //??? 执行成功
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCINumberToReal(pError, (CONST DCINumber *)&testcase_real[0].n_float4, sizeof(float), NULL);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	err = disconnect(true);

	if (failed == 0)
		return DCI_SUCCESS;
	else
	{
		printf("failed %d\n", failed);
		return DCI_ERROR;
	}
}

sword
TestDciNumberFromText()
{
	int		i, failed = 0;
	sword	retcode = DCI_SUCCESS;
	char	errbuf[256];
	sb4		ret;

	DCIError	*pDCIError = NULL;

	DCINumber out_num;

	typedef struct  
	{
		char	str[50];

		DCINumber num;
		char	format[50];

		sword	ret;
	}TestCase_FromText;

	TestCase_FromText testcase_fromtext[] =
	{
		// test 0/9/./+/-
		{"0", {"\x01\x80"}, "9", DCI_SUCCESS},
		{"0", {"\x01\x80"}, "0", DCI_SUCCESS},
		{"0", {"\x01\x80"}, "99", DCI_SUCCESS},
		{"0", {"\x01\x80"}, "99.99", DCI_SUCCESS},

		{"0", {"\x01\x80"}, "+9",DCI_ERROR}, //ORA-22061: invalid format text [+9]
		{"0", {"\x01\x80"}, "-9", DCI_ERROR}, //ORA-22061: invalid format text [-9]
		{"0", {"\x01\x80"}, "+99", DCI_ERROR}, //ORA-22061: invalid format text [+99]
		{"0", {"\x01\x80"}, "-99", DCI_ERROR}, //ORA-22061: invalid format text [-99]
		{"0", {"\x01\x80"}, "+99.99", DCI_ERROR}, //ORA-22061: invalid format text [+99.99]
		{"0", {"\x01\x80"}, "-99.99", DCI_ERROR}, //ORA-22061: invalid format text [-99.99]
		{"0", {"\x01\x80"}, "00.00", DCI_ERROR}, //ORA-22062: invalid input string [0]
		{"0", {"\x01\x80"}, "09.99", DCI_ERROR}, //ORA-22062: invalid input string [0]
		{"0", {"\x01\x80"}, "99.00", DCI_SUCCESS},
		{"0", {"\x01\x80"}, " 9", DCI_ERROR}, //ORA-22061: invalid format text [ 9]

		{"1234567890", {"\x06\xc5\x0d\x23\x39\x4f\x5b"}, "99999", DCI_ERROR}, //ORA-22062: invalid input string [1234567890]
		{"1234567890", {"\x06\xc5\x0d\x23\x39\x4f\x5b"}, "9999999999", DCI_SUCCESS},
		{"1234567890", {"\x06\xc5\x0d\x23\x39\x4f\x5b"}, "999999999999", DCI_SUCCESS},
		{"1234567890", {"\x06\xc5\x0d\x23\x39\x4f\x5b"}, "99999999999.99", DCI_SUCCESS},

		{"-1234567890", {"\x07\x3a\x59\x43\x2d\x17\x0b\x66"}, "99999", DCI_ERROR}, //ORA-22062: invalid input string [1234567890]
		{"-1234567890", {"\x07\x3a\x59\x43\x2d\x17\x0b\x66"}, "9999999999", DCI_SUCCESS},
		{"-1234567890", {"\x07\x3a\x59\x43\x2d\x17\x0b\x66"}, "999999999999", DCI_SUCCESS},
		{"-1234567890", {"\x07\x3a\x59\x43\x2d\x17\x0b\x66"}, "99999999999.99", DCI_SUCCESS},

		{"1234567890.12345", {"\x09\xc5\x0d\x23\x39\x4f\x5b\x0d\x23\x33"}, "9999", DCI_ERROR}, //ORA-22062: invalid input string [1234567890.12345]
		{"1234567890.12345", {"\x09\xc5\x0d\x23\x39\x4f\x5b\x0d\x23\x33"}, "9999999999.999", DCI_ERROR},//ORA-22062: invalid input string [1234567890.12345]
		{"1234567890.12345", {"\x09\xc5\x0d\x23\x39\x4f\x5b\x0d\x23\x33"}, "9999999999.99999", DCI_SUCCESS},
		{"1234567890.12345", {"\x09\xc5\x0d\x23\x39\x4f\x5b\x0d\x23\x33"}, "9999999999999.9999999", DCI_SUCCESS},

		{"-1234567890.12345", {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}, "9999", DCI_ERROR},  //ORA-22062: invalid input string [-1234567890.12345]
		{"-1234567890.12345", {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}, "9999999999.999", DCI_ERROR}, //ORA-22062: invalid input string [-1234567890.12345]
		{"-1234567890.12345", {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}, "9999999999.99999", DCI_SUCCESS},
		{"-1234567890.12345", {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}, "9999999999999.9999999", DCI_SUCCESS},
		{"-1234567890.12345", {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}, "9999999999999.00000", DCI_SUCCESS},
		{"-1234567890.12345", {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}, "0000000000.00000", DCI_SUCCESS},
		{"-1234567890.12345", {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}, "00000000000.00000", DCI_ERROR}, //ORA-22062: invalid input string [-1234567890.12345]

		{"0.000123456", {"\x05\xbf\x02\x18\x2e\x3d"}, "9999", DCI_ERROR}, //ORA-22062: invalid input string [0.000123456]
		{"0.000123456", {"\x05\xbf\x02\x18\x2e\x3d"}, "9999.999", DCI_ERROR}, //ORA-22062: invalid input string [0.000123456]
		{"0.000123456", {"\x05\xbf\x02\x18\x2e\x3d"}, "9999.999999999", DCI_SUCCESS},
		{"0.000123456", {"\x05\xbf\x02\x18\x2e\x3d"}, "9999.999999999999", DCI_SUCCESS},

		{"0.000123456", {"\x05\xbf\x02\x18\x2e\x3d"}, ".9999999", DCI_ERROR}, //ORA-22062: invalid input string [0.000123456]
		{".000123456", {"\x05\xbf\x02\x18\x2e\x3d"}, ".0099999", DCI_ERROR},  //ORA-22062: invalid input string [.000123456]
		{"0.000123456", {"\x05\xbf\x02\x18\x2e\x3d"}, ".009999999", DCI_ERROR},  //ORA-22062: invalid input string [0.000123456]
		{".000123456", {"\x05\xbf\x02\x18\x2e\x3d"}, ".00999999999", DCI_SUCCESS},

		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, "9999", DCI_ERROR}, //ORA-22062: invalid input string [-0.000123456]
		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, "9999.999",  DCI_ERROR}, //ORA-22062: invalid input string [-0.000123456]
		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, "9999.999999999", DCI_SUCCESS},
		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, "9999.999999999999", DCI_SUCCESS},
		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, "0000.000000000000", DCI_ERROR}, //ORA-22062: invalid input string [-0.000123456]
		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, "0099.999999999999", DCI_ERROR}, //ORA-22062: invalid input string [-0.000123456]

		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, ".9999999", DCI_ERROR}, //ORA-22062: invalid input string [-0.000123456]
		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, ".0000999", DCI_ERROR}, //ORA-22062: invalid input string [-0.000123456]
		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, ".0099999", DCI_ERROR}, //ORA-22062: invalid input string [-0.000123456]
		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, ".00999999999", DCI_ERROR}, //ORA-22062: invalid input string [-0.000123456]
		{"-.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, ".00999999999", DCI_SUCCESS},
	};

	retcode = DCIInitialize(DCI_THREADED, NULL, NULL, NULL, NULL);
	retcode = DCIEnvInit(&pEnv, DCI_DEFAULT, 0, NULL);
	retcode = DCIHandleAlloc(pEnv, (void **)&pDCIError, DCI_HTYPE_ERROR, 0, NULL);

	for (i = 0; i < (int)(sizeof(testcase_fromtext) / sizeof(testcase_fromtext[0])); i++)
	{
		memset(&out_num, 0, DCI_NUMBER_SIZE);

		retcode = DCINumberFromText(pDCIError, (const DciText *)testcase_fromtext[i].str, (ub4)strlen(testcase_fromtext[i].str), (const DciText *)testcase_fromtext[i].format, (ub4)strlen(testcase_fromtext[i].format), NULL, 0, &out_num);

		if (retcode != testcase_fromtext[i].ret)
		{
			failed++;
		}
		if (retcode != DCI_SUCCESS)
		{
			errbuf[0] = '\0';
			DCIErrorGet(pDCIError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("i = %d, errhp =%s\n",i, errbuf);
		}

		if (testcase_fromtext[i].ret == DCI_SUCCESS)
		{
			if (out_num.DCINumberPart[0] != testcase_fromtext[i].num.DCINumberPart[0])
				failed++;
			else if (memcmp(&out_num, &(testcase_fromtext[i].num), out_num.DCINumberPart[0] + 1) != 0)
				failed++;
		}
	}

	//test invalid parameter
	retcode = DCINumberFromText(NULL, (const DciText *)testcase_fromtext[0].str, (ub4)strlen(testcase_fromtext[0].str), (const DciText *)testcase_fromtext[0].format, (ub4)strlen(testcase_fromtext[0].format), NULL, 0, &out_num);
	if (retcode != DCI_INVALID_HANDLE)
		failed++;

	retcode = DCINumberFromText(pDCIError, NULL, (ub4)strlen(testcase_fromtext[0].str), (const DciText *)testcase_fromtext[0].format, (ub4)strlen(testcase_fromtext[0].format), NULL, 0, &out_num);
	//ORA-21560: argument 2 is null, invalid, or out of range
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pDCIError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCINumberFromText(pDCIError, (const DciText *)testcase_fromtext[0].str, 0, (const DciText *)testcase_fromtext[0].format, (ub4)strlen(testcase_fromtext[0].format), NULL, 0, &out_num);
	//ORA-21560: argument 3 is null, invalid, or out of range
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pDCIError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	//此用例在 oracle 上异常
	retcode = DCINumberFromText(pDCIError, (const DciText *)testcase_fromtext[0].str, (ub4)strlen(testcase_fromtext[0].str), NULL, (ub4)strlen(testcase_fromtext[0].format), NULL, 0, &out_num);
	if (retcode != DCI_SUCCESS)
	{
		failed++;
		errbuf[0] = '\0';
		DCIErrorGet(pDCIError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}
	else
	{
		;
	}

	retcode = DCINumberFromText(pDCIError, (const DciText *)testcase_fromtext[0].str, (ub4)strlen(testcase_fromtext[0].str), (const DciText *)testcase_fromtext[0].format, 0, NULL, 0, &out_num);
	//ORA-22062: invalid input string [0]
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pDCIError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCINumberFromText(pDCIError, (const DciText *)testcase_fromtext[0].str, (ub4)strlen(testcase_fromtext[0].str), (const DciText *)testcase_fromtext[0].format, (ub4)strlen(testcase_fromtext[0].format), NULL, 0, NULL);
	//ORA-21560: argument 8 is null, invalid, or out of range
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pDCIError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	//使用 nls_params
	memset(&out_num, 0, DCI_NUMBER_SIZE);
	retcode = DCINumberFromText(pDCIError, (const DciText *)testcase_fromtext[0].str, (ub4)strlen(testcase_fromtext[0].str), NULL, 0, (const DciText *)testcase_fromtext[0].format, (ub4)strlen(testcase_fromtext[0].format), &out_num);
	if (retcode != testcase_fromtext[0].ret)
	{
		failed++;
	}
	if (retcode != DCI_SUCCESS)
	{
		errbuf[0] = '\0';
		DCIErrorGet(pDCIError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("i = %d, errhp =%s\n",i, errbuf);
	}
	if (testcase_fromtext[0].ret == DCI_SUCCESS)
	{
		if (out_num.DCINumberPart[0] != testcase_fromtext[0].num.DCINumberPart[0])
			failed++;
		else if (memcmp(&out_num, &(testcase_fromtext[0].num), out_num.DCINumberPart[0] + 1) != 0)
			failed++;
	}

	if (pDCIError)
	{
		err = DCIHandleFree((dvoid *) pDCIError, (ub4) DCI_HTYPE_ERROR); 
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

	if (failed == 0)
		return DCI_SUCCESS;
	else
	{
		printf("failed = %d\n", failed);
		return DCI_ERROR;
	}
}

sword
TestDciNumberToText()
{
	int		i, failed = 0;
	sword	retcode = DCI_SUCCESS;
	char	errbuf[256];
	sb4		ret;

	DCIError	*pDCIError = NULL;

	ub4		ind;
	DciText out_format_str[50];

	typedef struct  
	{
		char	str[50];
		DCINumber num;

		char	format[50];
		char	format_str[50];

		sword	ret;
		ub4		len;
	}TestCase_ToText;

	TestCase_ToText testcase_totext[] =
	{
		// test 0/9/./+/-
		{"0", {"\x01\x80"}, "9", " 0", DCI_SUCCESS, 2},
		{"0", {"\x01\x80"}, "+9", "", DCI_ERROR, 0}, //ORA-22061: invalid format text [+9]
		{"0", {"\x01\x80"}, "-9", "", DCI_ERROR, 0}, //ORA-22061: invalid format text [-9]
		{"0", {"\x01\x80"}, "99", "  0", DCI_SUCCESS, 3},
		{"0", {"\x01\x80"}, "+99", "  0", DCI_ERROR, 0}, //ORA-22061: invalid format text [+99]
		{"0", {"\x01\x80"}, "-99", "  0", DCI_ERROR, 0}, //ORA-22061: invalid format text [-99]
		{"0", {"\x01\x80"}, "99.99", "   .00", DCI_SUCCESS, 6},
		{"0", {"\x01\x80"}, "+99.99", "   .00", DCI_ERROR, 0}, //ORA-22061: invalid format text [+99.99]
		{"0", {"\x01\x80"}, "-99.99", "", DCI_ERROR, 0}, //ORA-22061: invalid format text [-99.99]
		{"0", {"\x01\x80"}, "00.00", " 00.00", DCI_SUCCESS, 6},

		{"1234567890", {"\x06\xc5\x0d\x23\x39\x4f\x5b"}, "99999", "", DCI_ERROR, 0}, //ORA-22065: number to text translation for the given format causes overflow
		{"1234567890", {"\x06\xc5\x0d\x23\x39\x4f\x5b"}, "9999999999", " 1234567890", DCI_SUCCESS, 11},
		{"1234567890", {"\x06\xc5\x0d\x23\x39\x4f\x5b"}, "999999999999", "   1234567890", DCI_SUCCESS, 13},
		{"1234567890", {"\x06\xc5\x0d\x23\x39\x4f\x5b"}, "99999999999.99", "  1234567890.00", DCI_SUCCESS, 15},
		{"1234567890", {"\x06\xc5\x0d\x23\x39\x4f\x5b"}, "00000000000.00", " 01234567890.00", DCI_SUCCESS, 15},
		{"1234567890", {"\x06\xc5\x0d\x23\x39\x4f\x5b"}, "09999999999.99", " 01234567890.00", DCI_SUCCESS, 15},

		{"-1234567890", {"\x07\x3a\x59\x43\x2d\x17\x0b\x66"}, "99999", "", DCI_ERROR, 0}, //ORA-22065: number to text translation for the given format causes overflow
		{"-1234567890", {"\x07\x3a\x59\x43\x2d\x17\x0b\x66"}, "9999999999", "-1234567890", DCI_SUCCESS, 11},
		{"-1234567890", {"\x07\x3a\x59\x43\x2d\x17\x0b\x66"}, "999999999999", "  -1234567890", DCI_SUCCESS, 13},
		{"-1234567890", {"\x07\x3a\x59\x43\x2d\x17\x0b\x66"}, "99999999999.99", " -1234567890.00", DCI_SUCCESS, 15},
		{"-1234567890", {"\x07\x3a\x59\x43\x2d\x17\x0b\x66"}, "00000000000.00", "-01234567890.00", DCI_SUCCESS, 15},
		{"-1234567890", {"\x07\x3a\x59\x43\x2d\x17\x0b\x66"}, "09999999999.99", "-01234567890.00", DCI_SUCCESS, 15},

		{"1234567890.12345", {"\x09\xc5\x0d\x23\x39\x4f\x5b\x0d\x23\x33"}, "9999", "", DCI_ERROR, 0}, //ORA-22065: number to text translation for the given format causes overflow
		{"1234567890.12345", {"\x09\xc5\x0d\x23\x39\x4f\x5b\x0d\x23\x33"}, "9999999999.999", " 1234567890.123", DCI_SUCCESS, 15},
		{"1234567890.12345", {"\x09\xc5\x0d\x23\x39\x4f\x5b\x0d\x23\x33"}, "9999999999.99999", " 1234567890.12345", DCI_SUCCESS, 17},
		{"1234567890.12345", {"\x09\xc5\x0d\x23\x39\x4f\x5b\x0d\x23\x33"}, "9999999999999.9999999", "    1234567890.1234500", DCI_SUCCESS, 22},

		{"-1234567890.12345", {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}, "9999", "", DCI_ERROR, 0}, //ORA-22065: number to text translation for the given format causes overflow},
		{"-1234567890.12345", {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}, "9999999999.999", "-1234567890.123", DCI_SUCCESS, 15},
		{"-1234567890.12345", {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}, "9999999999.99999", "-1234567890.12345", DCI_SUCCESS, 17},
		{"-1234567890.12345", {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}, "9999999999999.9999999", "   -1234567890.1234500", DCI_SUCCESS, 22},

		{"-1234567890.12345", {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}, "    9999999999999.9999999", "", DCI_ERROR, 0}, //ORA-22061: invalid format text [    9999999999999.9999999]
		{"-1234567890.12345", {"\x0a\x3a\x59\x43\x2d\x17\x0b\x59\x43\x33\x66"}, "999 999 999 999.9999999", "", DCI_ERROR, 0}, //ORA-22061: invalid format text [999 999 999 999.9999999]

		{"0.000123456", {"\x05\xbf\x02\x18\x2e\x3d"}, "9999", "    0", DCI_SUCCESS, 5},
		{"0.000123456", {"\x05\xbf\x02\x18\x2e\x3d"}, "9999.999", "     .000", DCI_SUCCESS, 9},
		{"0.000123456", {"\x05\xbf\x02\x18\x2e\x3d"}, "9999.999999999", "     .000123456", DCI_SUCCESS, 15},
		{"0.000123456", {"\x05\xbf\x02\x18\x2e\x3d"}, "9999.999999999999", "     .000123456000", DCI_SUCCESS, 18},

		{"0.000123456", {"\x05\xbf\x02\x18\x2e\x3d"}, ".9999999", " .0001235", DCI_SUCCESS, 9},
		{"0.000123456", {"\x05\xbf\x02\x18\x2e\x3d"}, ".0000999", " .0001235", DCI_SUCCESS, 9},
		{"0.000123456", {"\x05\xbf\x02\x18\x2e\x3d"}, ".0099999", " .0001235", DCI_SUCCESS, 9},
		{"0.000123456", {"\x05\xbf\x02\x18\x2e\x3d"}, ".009999999", " .000123456", DCI_SUCCESS, 11},
		{"0.000123456", {"\x05\xbf\x02\x18\x2e\x3d"}, ".00999999999", " .00012345600", DCI_SUCCESS, 13},

		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, "9999", "   -0", DCI_SUCCESS, 5},
		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, "9999.999", "    -.000", DCI_SUCCESS, 9},
		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, "9999.999999999", "    -.000123456", DCI_SUCCESS, 15},
		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, "9999.999999999999", "    -.000123456000", DCI_SUCCESS, 18},
		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, "0000.000000000000", "-0000.000123456000", DCI_SUCCESS, 18},
		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, "0099.999999999999", "-0000.000123456000", DCI_SUCCESS, 18},

		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, ".9999999", "-.0001235", DCI_SUCCESS, 9},
		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, ".0000999", "-.0001235", DCI_SUCCESS, 9},
		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, ".0099999", "-.0001235", DCI_SUCCESS, 9},
		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, ".009999999", "-.000123456", DCI_SUCCESS, 11},
		{"-0.000123456", {"\x06\x40\x64\x4e\x38\x29\x66"}, ".00999999999", "-.00012345600", DCI_SUCCESS, 13},

		//进位
		{"99.99999", {"\x05\xc1\x64\x64\x64\x5b"}, "9999.9999", "  100.0000", DCI_SUCCESS, 10},
		{"99.99999", {"\x05\xc1\x64\x64\x64\x5b"}, "0999.9999", " 0100.0000", DCI_SUCCESS, 10},
		{"99.99999", {"\x05\xc1\x64\x64\x64\x5b"}, "999.9999", " 100.0000", DCI_SUCCESS, 9},
		{"99.99999", {"\x05\xc1\x64\x64\x64\x5b"}, "99.9999", "", DCI_ERROR, 0}, //ORA-22065: number to text translation for the given format causes overflow

		{"-99.99999", {"\x06\x3e\x02\x02\x02\x0b\x66"}, "9999.9999", " -100.0000", DCI_SUCCESS, 10},
		{"-99.99999", {"\x06\x3e\x02\x02\x02\x0b\x66"}, "0999.9999", "-0100.0000", DCI_SUCCESS, 10},
		{"-99.99999", {"\x06\x3e\x02\x02\x02\x0b\x66"}, "99.9999", "", DCI_ERROR, 0}, //ORA-22065: number to text translation for the given format causes overflow
		{"-99.99999", {"\x06\x3e\x02\x02\x02\x0b\x66"}, "999.9999", "-100.0000", DCI_SUCCESS, 9}
	};

	retcode = DCIInitialize(DCI_THREADED, NULL, NULL, NULL, NULL);
	retcode = DCIEnvInit(&pEnv, DCI_DEFAULT, 0, NULL);
	retcode = DCIHandleAlloc(pEnv, (void **)&pDCIError, DCI_HTYPE_ERROR, 0, NULL);

	for (i = 0; i < (int)(sizeof(testcase_totext) / sizeof(testcase_totext[0])); i++)
	{
		memset(out_format_str, 0, 50);
		ind = 50;

		retcode = DCINumberToText(pDCIError, (CONST DCINumber *)&testcase_totext[i].num, (const DciText *)testcase_totext[i].format, (ub4)strlen(testcase_totext[i].format), NULL, 0, &ind, out_format_str);

		if (retcode != testcase_totext[i].ret)
		{
			failed++;
		}
		if (retcode != DCI_SUCCESS)
		{
			errbuf[0] = '\0';
			DCIErrorGet(pDCIError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
			printf("i = %d, errhp =%s\n",i, errbuf);
		}

		if (testcase_totext[i].ret == DCI_SUCCESS)
		{
			if (strcmp(testcase_totext[i].format_str, (const char *)out_format_str) != 0)
				failed++;
			if (ind != testcase_totext[i].len)
				failed++;
		}
	}

	//test invalid parameter
	retcode = DCINumberToText(NULL, (CONST DCINumber *)&testcase_totext[0].num, (const DciText *)testcase_totext[0].format, (ub4)strlen(testcase_totext[0].format), NULL, 0, &ind, out_format_str);
	if (retcode != DCI_INVALID_HANDLE)
		failed++;

	retcode = DCINumberToText(pDCIError, NULL, (const DciText *)testcase_totext[0].format, (ub4)strlen(testcase_totext[0].format), NULL, 0, &ind, out_format_str);
	//invalid format text [no format]
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pDCIError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	//此用例在 oracle 上异常
	retcode = DCINumberToText(pDCIError, (CONST DCINumber *)&testcase_totext[0].num, NULL, (ub4)strlen(testcase_totext[0].format), NULL, 0, &ind, out_format_str);
	if (retcode != DCI_SUCCESS)
	{
		failed++;
		errbuf[0] = '\0';
		DCIErrorGet(pDCIError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	memset(out_format_str, 0, 50);
	retcode = DCINumberToText(pDCIError, (CONST DCINumber *)&testcase_totext[0].num, (const DciText *)testcase_totext[0].format, 0, NULL, 0, &ind, out_format_str);
	if (retcode != DCI_SUCCESS)
	{
		failed++;
		errbuf[0] = '\0';
		DCIErrorGet(pDCIError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}
	else
	{
		if (strcmp((const char *)out_format_str, " ") != 0)
			failed++;
		if (ind != 1)
			failed++;
	}

	//此用例在 oracle 上异常
	retcode = DCINumberToText(pDCIError, (CONST DCINumber *)&testcase_totext[0].num, (const DciText *)testcase_totext[0].format, (ub4)strlen(testcase_totext[0].format), NULL, 0, NULL, out_format_str);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pDCIError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	retcode = DCINumberToText(pDCIError, (CONST DCINumber *)&testcase_totext[0].num, (const DciText *)testcase_totext[0].format, (ub4)strlen(testcase_totext[0].format), NULL, 0, &ind, NULL);
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pDCIError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	ind = 0;
	retcode = DCINumberToText(pDCIError, (CONST DCINumber *)&testcase_totext[0].num, (const DciText *)testcase_totext[0].format, (ub4)strlen(testcase_totext[0].format), NULL, 0, &ind, out_format_str);
	//ORA-22059: buffer size [1] is too small - [2] is needed
	if (retcode != DCI_ERROR)
		failed++;
	else
	{
		errbuf[0] = '\0';
		DCIErrorGet(pDCIError, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
		printf("errhp =%s\n",errbuf);
	}

	if (pDCIError)
	{
		err = DCIHandleFree((dvoid *) pDCIError, (ub4) DCI_HTYPE_ERROR); 
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

	if (failed == 0)
		return DCI_SUCCESS;
	else
	{
		printf("failed = %d\n", failed);
		return DCI_ERROR;
	}
}


sword 
TestDciNumber()
{
	int		i, j, failed = 0;
	sword	retcode = DCI_SUCCESS;
	char	errbuf[256];
	sb4		ret;
	const double DEVIATION = 0.0001;
	
	DCINumber result;
	DCIError *pNumErr = NULL;

	typedef struct
	{
		int val;
		DCINumber num;
	} INumber;

	typedef struct
	{
		double val;
		DCINumber num;
	} DNumber;
	
	INumber inum[]={
		{0},{1},{2},{12},{10007},{-211},{45031},{-20000}
	};
	int iLen = sizeof(inum) / sizeof(inum[0]);

	DNumber dnum[]={
		{0},{1},{-1.000},{-1250.2},{2.33335},{-1234.0005},{-1.2501},{10000.0007}		
	};
	int dLen = sizeof(dnum)/sizeof(dnum[0]);


	retcode = DCIInitialize(DCI_THREADED, NULL, NULL, NULL, NULL);
	retcode = DCIEnvInit(&pEnv, DCI_DEFAULT, 0, NULL);
	retcode = DCIHandleAlloc(pEnv, (void **)&pNumErr, DCI_HTYPE_ERROR, 0, NULL);

	for (i = 0; i<iLen;i++)
	{
		retcode = DCINumberFromInt(pNumErr, (CONST dvoid *)&inum[i].val, sizeof(int), DCI_NUMBER_SIGNED, &inum[i].num);
		if (retcode != DCI_SUCCESS)
			failed++;
	}

	for (i = 0; i<dLen;i++)
	{
		retcode = DCINumberFromReal(pNumErr, (CONST dvoid *)&dnum[i].val, sizeof(double),  &dnum[i].num);
		if (retcode != DCI_SUCCESS)
			failed++;
	}

	//add
	{
		int iOut;
		double dOut;
		for (i = 0; i < iLen; i++)
		{
			for (j = 0; j < iLen; j++)
			{
				retcode = DCINumberAdd(pNumErr, &inum[i].num, &inum[j].num, &result);
				if (retcode != DCI_SUCCESS)
				{
					failed++;
					errbuf[0] = '\0';
					DCIErrorGet(pNumErr, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
					printf("errhp =%s\n",errbuf);
				}

				retcode = DCINumberToInt(pNumErr, &result, sizeof(int), DCI_NUMBER_SIGNED, &iOut);
				if (retcode != DCI_SUCCESS)
					failed++;

				if(inum[i].val + inum[j].val != iOut)
				{
					failed++;
					printf("err: %d + %d != %d \n", inum[i].val ,inum[j].val, iOut);
				}
			}
		}

		for (i = 0; i < dLen; i++)
		{
			for (j = 0; j < dLen; j++)
			{
				retcode = DCINumberAdd(pNumErr, &dnum[i].num, &dnum[j].num, &result);
				if (retcode != DCI_SUCCESS)
				{
					failed++;
					errbuf[0] = '\0';
					DCIErrorGet(pNumErr, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
					printf("errhp =%s\n",errbuf);
				}

				retcode = DCINumberToReal(pNumErr, &result, sizeof(double), &dOut);
				if (retcode != DCI_SUCCESS)
					failed++;
				
				if (fabs(dnum[i].val + dnum[j].val - dOut) > DEVIATION)
				{
					failed++;
					printf("err: %lf + %lf != %lf \n", dnum[i].val ,dnum[j].val, dOut);
				}
			}
		}
	}

	//Mul
	{
		int iOut;
		double dOut;
		for (i = 0; i < iLen; i++)
		{
			for (j = 0; j < iLen; j++)
			{
				retcode = DCINumberMul(pNumErr, &inum[i].num, &inum[j].num, &result);
				if (retcode != DCI_SUCCESS)
				{
					failed++;
					errbuf[0] = '\0';
					DCIErrorGet(pNumErr, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
					printf("errhp =%s\n",errbuf);
				}

				retcode = DCINumberToInt(pNumErr, &result, sizeof(int), DCI_NUMBER_SIGNED, &iOut);
				if (retcode != DCI_SUCCESS)
					failed++;

				if(inum[i].val * inum[j].val != iOut)
				{
					failed++;
					printf("err: %d * %d != %d \n", inum[i].val ,inum[j].val, iOut);
				}
			}
		}

		for (i = 0; i < dLen; i++)
		{
			for (j = 0; j < dLen; j++)
			{
				retcode = DCINumberMul(pNumErr, &dnum[i].num, &dnum[j].num, &result);
				if (retcode != DCI_SUCCESS)
				{
					failed++;
					errbuf[0] = '\0';
					DCIErrorGet(pNumErr, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
					printf("errhp =%s\n",errbuf);
				}

				retcode = DCINumberToReal(pNumErr, &result, sizeof(double), &dOut);
				if (retcode != DCI_SUCCESS)
					failed++;

				if (fabs(dnum[i].val * dnum[j].val - dOut) > DEVIATION)
				{
					failed++;
					printf("err: %lf * %lf != %lf \n", dnum[i].val ,dnum[j].val, dOut);
				}
			}
		}
	}

	//div
	{
		int iOut;
		double dOut;
		for (i = 0; i < iLen; i++)
		{
			for (j = 0; j < iLen; j++)
			{
				retcode = DCINumberDiv(pNumErr, &inum[i].num, &inum[j].num, &result);
				if (inum[j].val == 0)
				{
					if (retcode == DCI_SUCCESS)
						failed++;

					errbuf[0] = '\0';
					DCIErrorGet(pNumErr, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
					printf("errhp =%s\n",errbuf);
					continue;
				}
				
				if (retcode != DCI_SUCCESS)
				{
					failed++;
					errbuf[0] = '\0';
					DCIErrorGet(pNumErr, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
					printf("errhp =%s\n",errbuf);
					continue;
				}

				retcode = DCINumberToInt(pNumErr, &result, sizeof(int), DCI_NUMBER_SIGNED, &iOut);
				if (retcode != DCI_SUCCESS)
					failed++;

				if(inum[i].val / inum[j].val != iOut)
				{
					failed++;
					printf("err: %d / %d != %d \n", inum[i].val ,inum[j].val, iOut);
				}
			}
		}

		for (i = 0; i < dLen; i++)
		{
			for (j = 0; j < dLen; j++)
			{
				retcode = DCINumberDiv(pNumErr, &dnum[i].num, &dnum[j].num, &result);
				if (dnum[j].val == 0)
				{
					if (retcode == DCI_SUCCESS)
						failed++;

					errbuf[0] = '\0';
					DCIErrorGet(pNumErr, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
					printf("errhp =%s\n",errbuf);
					continue;
				}

				if (retcode != DCI_SUCCESS)
				{
					failed++;
					errbuf[0] = '\0';
					DCIErrorGet(pNumErr, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
					printf("errhp =%s\n",errbuf);
					continue;
				}

				retcode = DCINumberToReal(pNumErr, &result, sizeof(double), &dOut);
				if (retcode != DCI_SUCCESS)
					failed++;

				if (fabs(dnum[i].val / dnum[j].val - dOut) > DEVIATION)
				{
					failed++;
					printf("err: %lf / %lf != %lf \n", dnum[i].val ,dnum[j].val, dOut);
				}
			}
		}
	}

	//Mod
	{
		int iOut;
		for (i = 0; i < iLen; i++)
		{
			for (j = 0; j < iLen; j++)
			{
				retcode = DCINumberMod(pNumErr, &inum[i].num, &inum[j].num, &result);
				if (inum[j].val == 0)
				{
					if (retcode == DCI_SUCCESS)
						failed++;

					errbuf[0] = '\0';
					DCIErrorGet(pNumErr, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
					printf("errhp =%s\n",errbuf);
					continue;
				}

				if (retcode != DCI_SUCCESS)
				{
					failed++;
					errbuf[0] = '\0';
					DCIErrorGet(pNumErr, 1, NULL, &ret, (DciText*)errbuf, sizeof(errbuf), DCI_HTYPE_ERROR);
					printf("errhp =%s\n",errbuf);
					continue;
				}

				retcode = DCINumberToInt(pNumErr, &result, sizeof(int), DCI_NUMBER_SIGNED, &iOut);
				if (retcode != DCI_SUCCESS)
					failed++;

				if(inum[i].val % inum[j].val != iOut)
				{
					failed++;
					printf("err: %d Mod %d != %d \n", inum[i].val ,inum[j].val, iOut);
				}
			}
		}
	}

	//assign
	{
		int iOut;
		double dOut;
		for (i = 0; i < iLen; i++)
		{
			retcode = DCINumberAssign(pNumErr, &inum[i].num, &result);
			if (retcode != DCI_SUCCESS)
			{
				failed++;
				continue;
			}
			retcode = DCINumberToInt(pNumErr, &result, sizeof(int), DCI_NUMBER_SIGNED, &iOut);
			if (retcode != DCI_SUCCESS)
				failed++;

			if(inum[i].val != iOut)
			{
				failed++;
				printf("err: Assign  %d != %d \n", inum[i].val, iOut);
			}
		}

		for (i = 0; i < dLen; i++)
		{
			retcode = DCINumberAssign(pNumErr, &dnum[i].num, &result);
			if (retcode != DCI_SUCCESS)
			{
				failed++;
				continue;
			}

			retcode = DCINumberToReal(pNumErr, &result, sizeof(double), &dOut);
			if (retcode != DCI_SUCCESS)
				failed++;

			if (fabs(dnum[i].val - dOut) > DEVIATION)
			{
				failed++;
				printf("err: Assign  %lf != %lf \n", dnum[i].val, dOut);
			}
		}
	}

	//isInt
	{
		boolean iOut;
		boolean dOut;
		for (i = 0; i < iLen; i++)
		{
			retcode = DCINumberIsInt(pNumErr, &inum[i].num, &iOut);
			if (!iOut)
			{
				failed++;
				printf("err: %d is not int \n", inum[i].val);
			}
		}

		for (i = 0; i < dLen; i++)
		{
			retcode = DCINumberIsInt(pNumErr, &dnum[i].num, &dOut);
			double iVal = (int)dnum[i].val;
			if (dOut != (fabs(iVal - dnum[i].val) < DEVIATION))
			{
				failed++;
				if (dOut)
					printf("err: %lf is not int \n", dnum[i].val);
				else
					printf("err: %lf is int \n", dnum[i].val);
			}
		}
	}

	if (pNumErr)
	{
		err = DCIHandleFree((dvoid *) pNumErr, (ub4) DCI_HTYPE_ERROR); 
		if (err != DCI_SUCCESS)
		{
			printf("DCIHandleFree DCI_HTYPE_ERROR failed\n");
			failed++;
		}
	}

	/* Free Env Handle */
	if (pEnv)
	{
		err = DCIHandleFree((dvoid *) pEnv, (ub4) DCI_HTYPE_ENV);
		if (err != DCI_SUCCESS)
		{
			printf("DCIHandleFree DCI_HTYPE_ENV failed\n");
			failed++;
		}
	}

	if (failed == 0)
		return DCI_SUCCESS;
	else
	{
		printf("failed = %d\n", failed);
		return DCI_ERROR;
	}
}
