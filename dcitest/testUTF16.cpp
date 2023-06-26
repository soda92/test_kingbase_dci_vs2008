#include "DciTest.h"
#include "testUTF16.h"

#define NUM_ROWS	3
#define SQLSTR_LEN	256

int		test_utf16_run = true;
char	User_UTF16[NAME_LEN * 2] = "";
char	Pwd_UTF16[NAME_LEN * 2] = "";
char	DbName_UTF16[NAME_LEN * 2] = "";

char sql_drop[SQLSTR_LEN] = "DROP TABLE IF EXISTS T_UTF16";
char sql_drop_UTF16[SQLSTR_LEN * 2] = "";

char sql_create[3][SQLSTR_LEN] =
	{	"CREATE TABLE T_UTF16(c1 int, c2 char(10))",
		"CREATE TABLE T_UTF16(c1 int, c2 varchar(10))",
		"CREATE TABLE T_UTF16(c1 int, c2 text)",
	};
char sql_create_UTF16[NUM_ROWS][SQLSTR_LEN * 2];

char sql_insert[SQLSTR_LEN] = "INSERT INTO T_UTF16 VALUES(:1, :2)";
char sql_insert_UTF16[SQLSTR_LEN * 2] = "";

char sql_select[SQLSTR_LEN] = "select * from T_UTF16";
char sql_select_UTF16[SQLSTR_LEN * 2] = "";

char bind_name1[10] = ":1";
char bind_name1_UTF16[20] = "";
char bind_name2[10] = ":2";
char bind_name2_UTF16[20] = "";

ub2 charset_id = DCI_UTF16ID;

int bind_int[NUM_ROWS] = {123, 456, 789};

char bind_char_LE[NUM_ROWS][11 * 2] = {"\x0a\x4e\x31\x00\x00\x00", //上1
								"\x5f\x6c\xcf\x82\x32\x00\x00\x00", //江苏2
								"\x41\x00\x42\x00\x43\x00\x44\x00\x45\x00\x00\x4e\x8c\x4e\x09\x4e\xdb\x56\x94\x4e\x00", //ABCDE一二三四五		
								};

char bind_char[NUM_ROWS][11 * 2];

typedef struct result 
{
	int res1;
	char res2[21];
}result;

result res[NUM_ROWS];
result res_expect[NUM_ROWS];

/*
* transform ascii string to utf16
* if success, return 0
* if failed, return -1
*/
int
ascii_to_utf16(char *src, char *dst)
{
	int i = 0;

	if (!src || !dst)
		return 0;

	for (i = 0; i <= (int)strlen(src); i++)
	{
		if (src[i] >= 0x20 || src[i] == '\0')
		{
			//if (bigendian) 
			if(BYTE_ORDER == BIG_ENDIAN)
			{
				dst[i * 2] = '\0';
				dst[i * 2 + 1] = src[i];
			}
			else
			{
				dst[i * 2] = src[i];
				dst[i * 2 + 1] = '\0';
			}
		}
		else
			return -1;
	}

	return 0;
}

/*
* transform UTF16 little endian string to utf16
* if success, return 0
* if failed, return -1
*/
int
utf16_to_utf16(char *src_LE, char *dst, int len_bytes)
{
	if (!src_LE || !dst)
		return 0;

	/*
	bug#17888:for BigEndian
	*/
	//if (bigendian) 
	if(BYTE_ORDER == BIG_ENDIAN)
	{
		int i = 0;
		/*
		bug#17888:for BigEndian
		*/
		/*
		for (i = 0; i < len_bytes; i += 2)
		*/
		for (i = 0; i < len_bytes/2; i ++)
		{
			dst[i * 2] = src_LE[i * 2 + 1];
			dst[i * 2 + 1] = src_LE[i * 2];
		}
	}
	else
	{
		memcpy(dst, src_LE, len_bytes);
	}

	return 0;
}

sword
test_UTF16_logon()
{
	sword ret = DCI_SUCCESS;

	//test connect by DCILogon
	err = connect_UTF16(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
	}
	else
		err = disconnect_UTF16(true);

	//test connect by DCISessionBegin etc.
	err = connect_UTF16(false);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
	}

	err = disconnect_UTF16(false);

	return ret;
}

sword
test_UTF16_statement_chinese()
{
	sword err = 0;

	char sql_drop_chinese[SQLSTR_LEN] = "drop table if exists 测试中文表";
	char sql_create_chinese[SQLSTR_LEN] = "create table 测试中文表(第一列 int)";

	char sql_drop_chinese_UTF16_LE[SQLSTR_LEN * 2] = "\x64\x00\x72\x00\x6f\x00\x70\x00\x20\x00\x74\x00\x61\x00\x62\x00\x6c\x00\x65\x00\x20\x00\x4b\x6d\xd5\x8b\x2d\x4e\x87\x65\x68\x88\x00\x00";
	char sql_create_chinese_UTF16_LE[SQLSTR_LEN * 2] = "\x63\x00\x72\x00\x65\x00\x61\x00\x74\x00\x65\x00\x20\x00\x74\x00\x61\x00\x62\x00\x6c\x00\x65\x00\x20\x00\x4b\x6d\xd5\x8b\x2d\x4e\x87\x65\x68\x88\x28\x00\x2c\x7b\x00\x4e\x17\x52\x20\x00\x69\x00\x6e\x00\x74\x00\x29\x00\x00\x00";

	char sql_drop_chinese_UTF16[SQLSTR_LEN * 2] = "";
	char sql_create_chinese_UTF16[SQLSTR_LEN * 2] = "";

	utf16_to_utf16(sql_drop_chinese_UTF16_LE, sql_drop_chinese_UTF16, SQLSTR_LEN * 2);
	utf16_to_utf16(sql_create_chinese_UTF16_LE, sql_create_chinese_UTF16, SQLSTR_LEN * 2);

	err = test_execute((char *)sql_drop_chinese);
	err = test_UTF16_execute((char *)sql_drop_chinese_UTF16);

	err = test_execute((char *)sql_create_chinese);
	if (err != DCI_SUCCESS)
	{
		printf("create table failed\n");
		return err;
	}

	err = test_UTF16_execute((char *)sql_create_chinese_UTF16);
	if (err != DCI_ERROR)
	{
		printf("create table should fail, but success\n");
		return err;
	}

	err = test_UTF16_execute((char *)sql_drop_chinese_UTF16);
	if (err != DCI_SUCCESS)
	{
		printf("drop table failed\n");
		return err;
	}

	err = test_execute((char *)sql_drop_chinese);
	if (err != DCI_ERROR)
	{
		printf("drop table should fail, but success\n");
		return err;
	}

	return DCI_SUCCESS;
}

sword
test_UTF16_execute(char *sql)
{
	sword ret = DCI_SUCCESS;
	DCIStmt		*pStmt = NULL;

	err = connect_UTF16(true);
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

	err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, SQLSTR_LEN * 2, (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
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
	err = disconnect_UTF16(true);
	return ret;
}

sword
test_UTF16_insert(ub2 type)
{
	sword	ret = DCI_SUCCESS;
	DCIStmt	*stmhp = NULL;
	DCIBind	*bindhp = NULL;
	
	DciText sqlstate[6] = "", msg[256] = "";
	sb4		errcode = 0;

	err = connect_UTF16(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &stmhp, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(stmhp, pError, (const DciText*)sql_insert_UTF16, SQLSTR_LEN * 2, (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);
	
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByName(stmhp, &bindhp, pError, (const DciText *)bind_name1_UTF16, 4, &(bind_int[0]), sizeof(int), SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByName(stmhp, &bindhp, pError, (const DciText *)bind_name2_UTF16, 4, bind_char, 8, type, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}
	err = DCIAttrSet(bindhp, DCI_HTYPE_BIND, &charset_id, 2, DCI_ATTR_CHARSET_ID, pError);


	err = DCIStmtExecute(pSvcCtx, stmhp, pError, 1, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);
		
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((dvoid *) stmhp, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

end:
	err = disconnect_UTF16(true);
	return ret;
}

sword test_UTF16_fetch(ub2 type, int rows)
{
	sword		ret = DCI_SUCCESS;
	DCIStmt		*stmhp = NULL;
	DCIDefine	*definehp = NULL;

	DciText		sqlstate[6] = "", msg[256] = "";
	sb4			errcode = 0;

	int			i = 0, j = 0;

	err = connect_UTF16(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &stmhp, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(stmhp, pError, (const DciText*)sql_select_UTF16, SQLSTR_LEN * 2, (ub4) DCI_NTV_SYNTAX, (ub4) DCI_UTF16);
	if (err != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(stmhp, &definehp, pError, 1, &(res->res1), sizeof(int), (ub2)SQLT_INT, (void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(stmhp, &definehp, pError, 2, (void *)(res[0].res2), 22, (ub2)type, (void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	err = DCIAttrSet(definehp, DCI_HTYPE_DEFINE, &charset_id, 2, DCI_ATTR_CHARSET_ID, pError);


	err = DCIStmtExecute(pSvcCtx, stmhp, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %s\n", msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	for (i = 0; i < rows; i++)
	{
		memset(res[i].res2, 0, 22);
		err = DCIStmtFetch(stmhp, pError, 1, (ub2)DCI_FETCH_NEXT, (ub4)DCI_DEFAULT);

		if (res[0].res1 != res_expect[i].res1)
			ret = DCI_ERROR;

		for (j = 0; j < 22; j++)
		{
			if (res[0].res2[j] != res_expect[i].res2[j])
				ret = DCI_ERROR;
		}
	}

	err = DCIStmtFetch(stmhp, pError, 1, (ub2)DCI_FETCH_NEXT, (ub4)DCI_DEFAULT);
	if (err != DCI_NO_DATA)
	{
		ret = DCI_ERROR;
	}

free_resource:
	err = DCIHandleFree((dvoid *) stmhp, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

end:
	err = disconnect_UTF16(true);
	return ret;
}

sword
test_UTF16_insert_batch(ub2 type)
{
	sword	ret = DCI_SUCCESS;
	DCIStmt	*stmhp = NULL;
	DCIBind	*bindhp = NULL;

	DciText sqlstate[6] = "", msg[256] = "";
	sb4		errcode = 0;

	err = connect_UTF16(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &stmhp, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(stmhp, pError, (const DciText*)sql_insert_UTF16, SQLSTR_LEN * 2, (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindByName(stmhp, &bindhp, pError, (const DciText *)bind_name1_UTF16, 4, bind_int, sizeof(int), SQLT_INT, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindArrayOfStruct(bindhp, pError, sizeof(bind_int[0]), 0, 0, 0);
	if (err != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}


	err = DCIBindByName(stmhp, &bindhp, pError, (const DciText *)bind_name2_UTF16, 4, bind_char, 22, type, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIBindArrayOfStruct(bindhp, pError, sizeof(bind_char[0]), 0, 0, 0);
	if (err != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIAttrSet(bindhp, DCI_HTYPE_BIND, &charset_id, 2, DCI_ATTR_CHARSET_ID, pError);


	err = DCIStmtExecute(pSvcCtx, stmhp, pError, NUM_ROWS, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %d %s\n", errcode, msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	err = DCIHandleFree((dvoid *) stmhp, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

end:
	err = disconnect_UTF16(true);
	return ret;
}


sword
test_UTF16_fetch_batch(ub2 type)
{
	sword		ret = DCI_SUCCESS;
	DCIStmt		*stmhp = NULL;
	DCIDefine	*definehp = NULL;

	DciText		sqlstate[6] = "", msg[256] = "";
	sb4			errcode = 0;

	int			i = 0, j = 0;

	err = connect_UTF16(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		ret = DCI_ERROR;
	}

	err = DCIHandleAlloc((dvoid *) pEnv, (dvoid **) &stmhp, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIStmtPrepare(stmhp, pError, (const DciText*)sql_select_UTF16, SQLSTR_LEN * 2, (ub4) DCI_NTV_SYNTAX, (ub4) DCI_UTF16);
	if (err != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtPrepare failed: %d %s\n", errcode, msg);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(stmhp, &definehp, pError, 1, (void *)&(res[0].res1), sizeof(int), (ub2)SQLT_INT, (void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	err = DCIDefineArrayOfStruct(definehp, pError, sizeof(res[0]), 0, 0, 0);

	err = DCIDefineByPos(stmhp, &definehp, pError, 2, (void *)(res[0].res2), 22, (ub2)type, (void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	err = DCIAttrSet(definehp, DCI_HTYPE_DEFINE, &charset_id, 2, DCI_ATTR_CHARSET_ID, pError);
	err = DCIDefineArrayOfStruct(definehp, pError, sizeof(res[0]), 0, 0, 0);

	err = DCIStmtExecute(pSvcCtx, stmhp, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIStmtExecute failed: %s\n", msg);

		ret = DCI_ERROR;
		goto free_resource;
	}

	for (i = 0; i < NUM_ROWS; i++)
	{
		res[i].res1 = 0;
		memset(res[i].res2, 0, 22);
	}

	err = DCIStmtFetch(stmhp, pError, NUM_ROWS, (ub2)DCI_FETCH_NEXT, (ub4)DCI_DEFAULT);

	for (i = 0; i < NUM_ROWS; i++)
	{
		if (res[i].res1 != res_expect[i].res1)
			ret = DCI_ERROR;

		for (j = 0; j < 22; j++)
		{
			if (res[i].res2[j] != res_expect[i].res2[j])
				ret = DCI_ERROR;
		}
	}

	err = DCIStmtFetch(stmhp, pError, 1, (ub2)DCI_FETCH_NEXT, (ub4)DCI_DEFAULT);
	if (err != DCI_NO_DATA)
	{
		ret = DCI_ERROR;
	}

free_resource:
	err = DCIHandleFree((dvoid *) stmhp, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

end:
	err = disconnect_UTF16(true);
	return ret;
}

sword
test_UTF16()
{
	int i = 0, j = 0, k = 0;

	ub2		datatype[] = 
	{
		SQLT_AFC,
		SQLT_CHR,
		SQLT_STR,
		//SQLT_VCS
	};

	//get UTF16 string
	ascii_to_utf16(sql_drop, sql_drop_UTF16);
	
	for ( i = 0; i < (int)(sizeof(sql_create) / sizeof(sql_create[0])); i++)
		ascii_to_utf16(sql_create[i], sql_create_UTF16[i]);

	ascii_to_utf16(sql_insert, sql_insert_UTF16);
	ascii_to_utf16(sql_select, sql_select_UTF16);

	ascii_to_utf16(bind_name1, bind_name1_UTF16);
	ascii_to_utf16(bind_name2, bind_name2_UTF16);
	
	utf16_to_utf16((char *)bind_char_LE, (char *)bind_char, NUM_ROWS * (11 * 2));

	//test connect
	err = test_UTF16_logon();
	if (err != DCI_SUCCESS)
		return err;

	//test statement with chinese
	err = test_UTF16_statement_chinese();
	if (err != DCI_SUCCESS)
		return err;

	// test data with chinese
	for (k = 0; k < (int)(sizeof(sql_create_UTF16) / sizeof(sql_create_UTF16[0])); k++)
	{	
		for (i = 0; i < NUM_ROWS; i++)
		{
			res_expect[i].res1 = bind_int[i];
			memcpy(res_expect[i].res2, bind_char[i], 22);

			if (k == 0) // char(10), add blanks
			{
				for (j = 0; j < 20; j += 2)
				{
					if (res_expect[i].res2[j] == '\0' && res_expect[i].res2[j + 1] == '\0')
					{
						//if (bigendian) 
						if(BYTE_ORDER == BIG_ENDIAN)
							res_expect[i].res2[j + 1] = ' ';
						else
							res_expect[i].res2[j] = ' '; 
					}
				}
			}
		}

		printf("\ntest %s\n", sql_create[k]);

		//test insert 1 row and fetch 1 row
		for (i = 0; i < (int)(sizeof(datatype) / sizeof(datatype[0])); i++)
		{
			printf("test DCI datatype %d\n", datatype[i]);

			err = test_UTF16_execute((char *)sql_drop_UTF16);
			err = test_UTF16_execute((char *)sql_create_UTF16[k]);
			if (err != DCI_SUCCESS)
				return err;

			err = test_UTF16_insert(datatype[i]);
			if (err != DCI_SUCCESS)
			{
				printf("test_UTF16_insert 1 row failed\n");
				return err;
			}
			else
				printf("test_UTF16_insert 1 row success\n");

			err = test_UTF16_fetch(datatype[i], 1);
			if (err != DCI_SUCCESS)
			{
				printf("test_UTF16_fetch 1 row failed\n");
				return err;
			}
			else
				printf("test_UTF16_fetch 1 row success\n");

			//test batch insert and batch fetch
			err = test_UTF16_execute((char *)sql_drop_UTF16);
			if (err != DCI_SUCCESS)
				return err;

			err = test_UTF16_execute((char *)sql_create_UTF16[k]);
			if (err != DCI_SUCCESS)
				return err;

			err = test_UTF16_insert_batch(datatype[i]);
			if (err != DCI_SUCCESS)
			{
				printf("test_UTF16_insert_batch failed\n");
				return err;
			}
			else
				printf("test_UTF16_insert_batch success\n");

			err = test_UTF16_fetch_batch(datatype[i]);
			if (err != DCI_SUCCESS)
			{
				printf("test_UTF16_fetch_batch failed\n");
				return err;
			}
			else
				printf("test_UTF16_fetch_batch success\n");

			//test fetch 1 row by 1 row
			err = test_UTF16_fetch(datatype[i], NUM_ROWS);
			if (err != DCI_SUCCESS)
			{
				printf("test_UTF16_fetch 1 row by 1 row failed\n");
				return err;
			}
			else
				printf("test_UTF16_fetch 1 row by 1 row success\n");
		}
	}

	err = test_UTF16_execute((char *)sql_drop_UTF16);

	if (test_UTF16_AttrGet_NAME() != DCI_SUCCESS)
		return DCI_ERROR;

	return DCI_SUCCESS;
}

// inputbytelen must include the ended "\x00\x00"
ub4	ucs2bytelen(const char *ucs2str, ub4 inputbytelen)
{
	ub4 len = 0;
	if (ucs2str == NULL)
		return 0;

	while (len < inputbytelen)
	{
		while (ucs2str[len] != '\0')
			len++;

		if (ucs2str[len + 1] == '\0' && len%2 == 0)
			break;
		else
			len++;
	}

	return len;
}

sword
test_UTF16_AttrGet_NAME()
{
	sword	ret = DCI_SUCCESS;
	char	**colNameLst = NULL;
	ub4		*colNameLenLst = NULL;
	ub2		numcols = 0;

	DCIDescribe *pDsc = NULL;
	DCIParam	*colLstDesc = NULL,
				*colDesc = NULL;

	char tblname_chinese[SQLSTR_LEN] = "测试中文表";
	char clnnameLst_chinese[][SQLSTR_LEN] = 
										{
											"第一列",
											"第二列2",
											"第三3"
										};
	
	// "drop table 测试中文表"
	char sql_drop_chinese_UTF16_LE[SQLSTR_LEN * 2] = "\x64\x00\x72\x00\x6f\x00\x70\x00\x20\x00\x74\x00\x61\x00\x62\x00\x6c\x00\x65\x00\x20\x00\x4b\x6d\xd5\x8b\x2d\x4e\x87\x65\x68\x88\x00\x00";
	// "create table 测试中文表(第一列 int, 第二列2 char, 第三3 int)";
	char sql_create_chinese_UTF16_LE[SQLSTR_LEN * 2] = "\x63\x00\x72\x00\x65\x00\x61\x00\x74\x00\x65\x00\x20\x00\x74\x00\x61\x00\x62\x00\x6c\x00\x65\x00\x20\x00\x4b\x6d\xd5\x8b\x2d\x4e\x87\x65\x68\x88\x28\x00\x2c\x7b\x00\x4e\x17\x52\x20\x00\x69\x00\x6e\x00\x74\x00\x2c\x00\x20\x00\x2c\x7b\x8c\x4e\x17\x52\x32\x00\x20\x00\x63\x00\x68\x00\x61\x00\x72\x00\x2c\x00\x20\x00\x2c\x7b\x09\x4e\x33\x00\x20\x00\x69\x00\x6e\x00\x74\x00\x29\x00\x00\x00";
	char tblname_chinese_UTF16_LE[SQLSTR_LEN * 2] = "\x4b\x6d\xd5\x8b\x2d\x4e\x87\x65\x68\x88\x00\x00";
	char clnnameLst_chinese_UTF16_LE[][SQLSTR_LEN * 2] = 
	{
		"\x2c\x7b\x00\x4e\x17\x52\x00\x00",
		"\x2c\x7b\x8c\x4e\x17\x52\x32\x00\x00\x00",
		"\x2c\x7b\x09\x4e\x33\x00\x00\x00",
	};

	char sql_drop_chinese_UTF16[SQLSTR_LEN * 2] = "";
	char sql_create_chinese_UTF16[SQLSTR_LEN * 2] = "";
	char tblname_chinese_UTF16[SQLSTR_LEN * 2] = "";
	char clnnameLst_chinese_UTF16[3][SQLSTR_LEN * 2];
	memset(clnnameLst_chinese_UTF16, 0, SQLSTR_LEN * 2 * 3);

	utf16_to_utf16(sql_drop_chinese_UTF16_LE, sql_drop_chinese_UTF16, SQLSTR_LEN * 2);
	utf16_to_utf16(sql_create_chinese_UTF16_LE, sql_create_chinese_UTF16, SQLSTR_LEN * 2);
	utf16_to_utf16(tblname_chinese_UTF16_LE, tblname_chinese_UTF16, SQLSTR_LEN * 2);

	for (ub4 i = 0;i < sizeof(clnnameLst_chinese) / sizeof(clnnameLst_chinese[0]);i++)
	{
		utf16_to_utf16(clnnameLst_chinese_UTF16_LE[i], clnnameLst_chinese_UTF16[i], SQLSTR_LEN * 2);
	}

	test_UTF16_execute(sql_drop_chinese_UTF16);
	if (test_UTF16_execute(sql_create_chinese_UTF16))
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("test_UTF16_execute failed: %d %s\n", errcode, msg);

		return err;
	}

	if (connect_UTF16(true))
		return DCI_ERROR;

	if (DCIHandleAlloc((void *)pEnv, (void **)&pDsc, (ub4)DCI_HTYPE_DESCRIBE, (size_t)0, (void **)0))
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	/* get the describe handle for the table */
	if (DCIDescribeAny(pSvcCtx, pError, (void *)tblname_chinese, strlen(tblname_chinese), DCI_OTYPE_NAME, 0, DCI_PTYPE_TABLE, pDsc))
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	/* get the parameter handle */
	if (DCIAttrGet((void *)pDsc, DCI_HTYPE_DESCRIBE, (void *)&colLstDesc, (ub4 *)0, DCI_ATTR_PARAM, pError))
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	numcols = 0;
	if (DCIAttrGet((void *)colLstDesc, DCI_DTYPE_PARAM, (void *)&numcols, (ub4 *)0, DCI_ATTR_NUM_COLS, pError))
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	colNameLst = new char*[numcols];
	if (colNameLst == NULL)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	memset(colNameLst, 0, sizeof(char *) * numcols);

	colNameLenLst = new ub4[numcols];
	if (colNameLenLst == NULL)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}
	memset(colNameLenLst, 0, sizeof(ub4) * numcols);

	for (int i = 0;i < numcols;i++)
	{
		colNameLst[i] = new char[(NAME_LEN + 1) * 2];
		if (colNameLst[i] == NULL)
		{
			ret = DCI_ERROR;
			goto free_resource;
		}
		memset(colNameLst[i], 0, (NAME_LEN + 1) * 2);
	}

	for (int i = 1; i <= numcols;i++)
	{
		if (DCIParamGet((CONST dvoid *)colLstDesc, (ub4)DCI_DTYPE_PARAM, pError, (dvoid **)&colDesc, i))
		{
			ret = DCI_ERROR;
			goto free_resource;
		}

		if (DCIAttrGet((void *)colDesc, DCI_DTYPE_PARAM, colNameLst[i-1], &colNameLenLst[i-1], DCI_ATTR_NAME, pError))
		{
			ret = DCI_ERROR;
			goto free_resource;
		}
	}

	for (int i = 0;i < numcols;i++)
	{
		if (memcmp(colNameLst[i], clnnameLst_chinese_UTF16[i], ucs2bytelen((const char *)colNameLst[i], (NAME_LEN + 1) * 2)))
		{
			ret = DCI_ERROR;
			goto free_resource;
		}
	}

free_resource:
	if (disconnect_UTF16(true))
		ret = DCI_ERROR;

	for (int i = 0;i < numcols;i++)
	{
		if (colNameLst[i])
		{
			delete[] colNameLst[i];
			colNameLst[i] = NULL;
		}
	}

	if(colNameLenLst)
	{
		delete[] colNameLenLst;
		colNameLenLst = NULL;
	}
	if (colNameLst)
	{
		delete[] colNameLst;
		colNameLst = NULL;
	}

	
	DCIHandleFree((void *)pDsc, (ub4)DCI_HTYPE_DESCRIBE);
	return ret;
}
