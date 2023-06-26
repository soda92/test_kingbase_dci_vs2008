#include "DciTest.h"

#define	BLOB_MAXLENGTH 2147483647 //2G - 1
#define	CLOB_MAXLENGTH 1073741823 //1G - 1

char	*table_name1 = "T_LOB_1";
char	*table_name2 = "T_LOB_2";

DCIStmt	*pStmt = NULL;

DCILobLocator	*dst_locp = NULL;
DCILobLocator	*src_locp = NULL;

sword
drop_table(char *table_name)
{
	char	drop_table[256] = "";
	DCIStmt	*pStmt = NULL;
	sword	ret = DCI_SUCCESS;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		return DCI_ERROR;
	}

	sprintf((char *)drop_table, "drop table if exists %s", table_name);

	err = DCIStmtPrepare(pStmt, pError, (const OraText *)drop_table, 
		(ub4)strlen(drop_table), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);

	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, 
		(const DCISnapshot *)0, (DCISnapshot *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	if (pStmt)
	{
		err = DCIHandleFree((dvoid *)pStmt, DCI_HTYPE_STMT);
		if (err != DCI_SUCCESS)
			ret = DCI_ERROR;
		pStmt = NULL;
	}

	return ret;
}

sword
create_table_clob(char *table_name)
{
	char	create_table[256] = "";
	DCIStmt	*pStmt = NULL;
	sword	ret = DCI_SUCCESS;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		return DCI_ERROR;
	}

	sprintf((char *)create_table, "create table %s(C CLOB)", table_name);
//	sprintf((char *)create_table, "create table %s(C TEXT)", table_name);


	err = DCIStmtPrepare(pStmt, pError, (const OraText *)create_table, 
		(ub4)strlen(create_table), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, 
		(const DCISnapshot *)0, (DCISnapshot *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	if (pStmt)
	{
		err = DCIHandleFree((dvoid *)pStmt, DCI_HTYPE_STMT);
		if (err != DCI_SUCCESS)
			ret = DCI_ERROR;
		pStmt = NULL;
	}

	return ret;
}

sword
create_table_blob(char *table_name)
{
	char	create_table[256] = "";
	DCIStmt	*pStmt = NULL;
	sword	ret = DCI_SUCCESS;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		return DCI_ERROR;
	}

	sprintf((char *)create_table, "create table %s(C BLOB)", table_name);
	//sprintf((char *)create_table, "create table %s(C BYTEA)", table_name);

	err = DCIStmtPrepare(pStmt, pError, (const OraText *)create_table, 
		(ub4)strlen(create_table), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, 
		(const DCISnapshot *)0, (DCISnapshot *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	if (pStmt)
	{
		err = DCIHandleFree((dvoid *)pStmt, DCI_HTYPE_STMT);
		pStmt = NULL;
		if (err != DCI_SUCCESS)
			ret = DCI_ERROR;
	}

	return ret;
}

sword
insert_table_clob(char *table_name, char *buf)
{
	char	insert_table[256] = "";
	DCIStmt	*pStmt = NULL;
	sword	ret = DCI_SUCCESS;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		return DCI_ERROR;
	}

	if (buf != NULL)
		sprintf((char *)insert_table, "insert into %s values('%s')", table_name, buf);
	else
		sprintf((char *)insert_table, "insert into %s values(NULL)", table_name);

	err = DCIStmtPrepare(pStmt, pError, (const OraText *)insert_table, 
		(ub4)strlen(insert_table), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, 
		(const DCISnapshot *)0, (DCISnapshot *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	if (pStmt)
	{
		err = DCIHandleFree((dvoid *)pStmt, DCI_HTYPE_STMT);
		pStmt = NULL;
		if (err != DCI_SUCCESS)
			ret = DCI_ERROR;
	}

	return ret;
}

sword
insert_table_blob(char *table_name, char *buf, int buflen)
{
	DCIBind	*pBind = NULL;
	char	insert_table[256] = "";
	DCIStmt	*pStmt = NULL;
	sb2		indp = 0;
	sword	ret = DCI_SUCCESS;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		return DCI_ERROR;
	}

	sprintf((char *)insert_table, "insert into %s values(:1)", table_name);

	err = DCIStmtPrepare(pStmt, pError, (const OraText *)insert_table, 
		(ub4)strlen(insert_table), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (buf == NULL)
		indp = DCI_IND_NULL;

	err = DCIBindByPos(pStmt, &pBind, pError, (ub4)1, (void *)buf, 
		(sb4)buflen, (ub2)SQLT_BIN, indp != 0? &indp : (void *)0, (ub2 *)0, 
		(ub2 *)0, (ub4)0,(ub4 *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, 
		(const DCISnapshot *)0, (DCISnapshot *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	if (pStmt)
	{
		err = DCIHandleFree((dvoid *)pStmt, DCI_HTYPE_STMT);
		pStmt = NULL;
		if (err != DCI_SUCCESS)
			ret = DCI_ERROR;
	}

	return ret;
}

DCILobLocator *
select_table_clob(char *table_name, DCIDefine * pDefine, DCILobLocator *clob_loc)
{
	char	seclect_table[256];
	DCIStmt	*pStmt = NULL;
	sword	ret = DCI_SUCCESS;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		return NULL;
	}

	sprintf((char *)seclect_table, "select * from %s for update", table_name);

	err = DCIDescriptorAlloc((void *)pEnv, (void **)&clob_loc, DCI_DTYPE_LOB, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtPrepare(pStmt, pError, (const OraText *)seclect_table, 
		(ub4)strlen(seclect_table), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt, &pDefine, pError, 
		(ub4)1, 
		(void *)&clob_loc, 
		sizeof(DCILobLocator *), 
		(ub2)SQLT_CLOB, 
		(void *)0, 
		(ub2 *)0, 
		(ub2 *)0, 
		(ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, 
		(const DCISnapshot *)0, (DCISnapshot *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS && err != DCI_NO_DATA)
	{
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	if (pStmt)
	{
		err = DCIHandleFree((dvoid *)pStmt, DCI_HTYPE_STMT);
		pStmt = NULL;
		if (err != DCI_SUCCESS)
			ret = DCI_ERROR;
	}

	if (ret == DCI_SUCCESS)
		return clob_loc;
	else
		return NULL;
}

DCILobLocator *
select_table_blob(char *table_name, DCIDefine * pDefine, DCILobLocator *blob_loc)
{
	char	seclect_table[256];
	DCIStmt	*pStmt = NULL;
	sword	ret = DCI_SUCCESS;

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		printf("DCIHandleAlloc pStmt failed\n");
		return NULL;
	}

	sprintf((char *)seclect_table, "select * from %s", table_name);

	err = DCIDescriptorAlloc((void *)pEnv, (void **)&blob_loc, DCI_DTYPE_LOB, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtPrepare(pStmt, pError, (const OraText *)seclect_table, 
		(ub4)strlen(seclect_table), (ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIDefineByPos(pStmt, &pDefine, pError, 
		(ub4)1, 
		(void *)&blob_loc, 
		sizeof(DCILobLocator *), 
		(ub2)SQLT_BLOB, 
		(void *)0, 
		(ub2 *)0, 
		(ub2 *)0, 
		(ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, 
		(const DCISnapshot *)0, (DCISnapshot *)0, (ub4)DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		ret = DCI_ERROR;
		goto free_resource;
	}

free_resource:
	if (pStmt)
	{
		err = DCIHandleFree((dvoid *)pStmt, DCI_HTYPE_STMT);
		pStmt = NULL;
		if (err != DCI_SUCCESS)
			ret = DCI_ERROR;
	}

	if (ret == DCI_SUCCESS)
		return blob_loc;
	else
		return NULL;
}

//分配一个大字段存贮描述结构指针
static sword
alloc_lob_desc(DCIEnv *envhp, DCILobLocator **lobsrc)
{
	if (DCIDescriptorAlloc(envhp, (dvoid **) lobsrc,
		(ub4) DCI_DTYPE_LOB, (size_t) 0,
		(dvoid **) 0) != DCI_SUCCESS )
	{
		printf("DCIDescriptorAlloc failed\n");
		return DCI_ERROR;
	}

	return DCI_SUCCESS;
}

static sword
write_blob_loc(DCISvcCtx *svchp, DCIError *errhp, DCILobLocator *lobp)
{
	char	buf[1024];
	ub4		writesize = 0;
	ub4		amtp;
	sb4		err;

	memset(buf, 'a', 1024);

	amtp = 1024;

	err = DCILobWrite(svchp, 
		errhp, 
		lobp,
		&amtp, 
		(ub4) 1, 
		(void *) buf, 
		(ub4) 1024, 
		(ub1) 0, 
		(void *) 0, 
		0, 
		(ub2) 0, 
		(ub1) SQLCS_IMPLICIT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		return err;
	}
	else
		writesize += amtp;

	return DCI_SUCCESS;
}

static sword
write_clob_loc(DCISvcCtx *svchp, DCIError *errhp, DCILobLocator *lobp)
{
	char	buf[1024];
	ub4		writesize = 0;
	ub4		amtp;
	sb4		err;

	memset(buf, 'a', 1024);

	amtp = 1024;

	err = DCILobWrite(svchp, 
		errhp, 
		lobp, 
		&amtp, 
		(ub4) 1,
		(void *) buf, 
		(ub4) 1024, 
		(ub1) 0, 
		(void *) 0, 
		0, 
		(ub2) 0, 
		(ub1) SQLCS_IMPLICIT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		return err;
	}
	else
		writesize += amtp;

	return DCI_SUCCESS;
}

static sword
prepare_lob_operation(DCISvcCtx* pSvcCtx, DCIEnv* pEnv, DCIStmt* pStmt, 
					  DCIError* pError, DCIDefine* dfnhp[], DCILobLocator* lobp[])
{
	char	*sql_creat = "CREATE TABLE TEST_LOB(C1 BLOB, C2 CLOB)";
//	char	*sql_creat = "CREATE TABLE TEST_LOB(C1 BYTEA, C2 TEXT)";
	char	*sql_drop = "DROP TABLE IF EXISTS TEST_LOB";
	char	*sql_select = "SELECT C1, C2 FROM TEST_LOB FOR UPDATE";
	char	*sql_insert = "INSERT INTO TEST_LOB VALUES('1', '1')";

	err = DCIStmtPrepare(pStmt, pError, (unsigned char *)sql_drop, (ub4)strlen((char *)sql_drop),
		(ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4) 1, (ub4) 0,
		(CONST DCISnapshot*) 0, (DCISnapshot*) 0,
		(ub4) DCI_DEFAULT);

	err = DCIStmtPrepare(pStmt, pError, (unsigned char *)sql_creat, (ub4)strlen((char *)sql_creat),
		(ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
	{
		report_error(pError);
		return disconnect(true);
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4) 1, (ub4) 0,
		(CONST DCISnapshot*) 0, (DCISnapshot*) 0,
		(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS && err != DCI_SUCCESS_WITH_INFO)
	{
		report_error(pError);
		return disconnect(true);
	}

	//先给大字段插入一个NULL
	err = DCIStmtPrepare(pStmt, pError, (unsigned char *)sql_insert, 
		(ub4)strlen((char *)sql_insert),(ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		printf("FAILED: DCIStmtPrepare() select\n");
		return disconnect(true);
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4) 1, (ub4) 0,
		(CONST DCISnapshot*) 0, (DCISnapshot*) 0,
		(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		report_error(pError);
		printf("FAILED: DCIStmtExecute() insert\n");
		return disconnect(true);
	}

	err = DCIStmtPrepare(pStmt, pError, (OraText *)sql_select, 
		(ub4)strlen((char *)sql_select),(ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		report_error(pError);
		return DCI_ERROR;
	}

	dfnhp[0] = (DCIDefine *) 0;
	dfnhp[1] = (DCIDefine *) 0;

	err = DCIDefineByPos(pStmt, &dfnhp[0], pError, (ub4) 1,
		(dvoid *) &lobp[0], (sb4) 4, (ub2) SQLT_BLOB,
		(dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error(pError);
		return DCI_ERROR;
	}

	err = DCIDefineByPos(pStmt, &dfnhp[1], pError, (ub4) 2,
		(dvoid *) &lobp[1], (sb4) 4, (ub2) SQLT_CLOB,
		(dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error(pError);
		return DCI_ERROR;
	}

	err = DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4) 1, (ub4) 0,
		(CONST DCISnapshot*) 0, (DCISnapshot*) 0,
		(ub4) DCI_DEFAULT);
	if (err != DCI_SUCCESS)
	{
		printf("FAILED: DCIStmtExecute() select\n");
		report_error(pError);
		return DCI_ERROR;
	}

	return DCI_SUCCESS;
}

//	创建一个表，表结构为：CREATE TABLE TEST_LOB(C1 CLOB, C2 BLOB)，并插入一条记录
static sword
create_table(DCIEnv* pEnv, DCIStmt* pStmt, DCIDefine* dfnhp[2], DCILobLocator* lobp[2])
{
	sword	ret = DCI_SUCCESS;

	prepare_lob_operation(pSvcCtx, pEnv, pStmt, pError, dfnhp, lobp);

	ret = write_blob_loc(pSvcCtx, pError, lobp[0]);
	if (ret != DCI_SUCCESS)
		return ret;

	ret = write_clob_loc(pSvcCtx, pError, lobp[1]);
	if (ret != DCI_SUCCESS)
		return ret;

	return ret;
}

sword
TestLobCopy_clob()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;
	typedef struct test_clob_hnd
	{
		DCISvcCtx	*pSvcCtx; 
		DCIError	*pError;
		int			dst_flag;
		int			src_flag;
		ub4			amount;
		ub4			dst_offset;
		ub4			src_offset;
		sword		ret_val;
	}test_clob_hnd;

	test_clob_hnd clob_array[]=
	{
		{NULL, pError, 1, 1, BUFLEN, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, NULL, 1, 1, BUFLEN, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 0, 1, BUFLEN, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, 0, BUFLEN, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, 1, BUFLEN, 1, 1, DCI_SUCCESS},
	};

	typedef struct test_clob_suite
	{
		char	*dst_in;
		char	*src_in;
		ub4		amount;
		ub4		dst_offset;
		ub4		src_offset;
		char	expect_ret[BUFLEN * 2];
		sword	ret_val;
	}test_clob_suite;

	test_clob_suite test_clob[] = 
	{
		/* bug#31170 */
		//参数 amount 大于 src_locp 表示的 LOB对象从 src_offset 之后的剩余长度
		{"北京人大金仓", "研发中心工程部", 30, 1, 1, "研发中心工程部", DCI_SUCCESS},
		//参数 amount 小于src_locp 表示的 LOB 对象从 src_offset 之后剩余的长度
		{"北京人大金仓", "研发中心工程部", 8, 1, 1, "研发中心金仓", DCI_SUCCESS},
		//参数 amount 等于src_locp 表示的 LOB 对象从 src_offset 之后剩余的长度
		{"北京人大金仓", "研发中心工程部", 14, 1, 1, "研发中心工程部", DCI_SUCCESS},
		//参数 amount 为 0
		{"北京人大金仓", "研发中心工程部", 0, 1, 1, "北京人大金仓", DCI_SUCCESS},

		//参数 dst_offset 大于 dst_lcop 表示的 LOB 对象长度值
		{"北京人大金仓", "研发中心工程部", 14, 16, 1, "北京人大金仓   研发中心工程部", DCI_SUCCESS},
		//参数 dst_offset 小于 dst_lcop 表示的 LOB 对象长度值
		{"北京人大金仓", "研发中心工程部", 14, 1, 1, "研发中心工程部", DCI_SUCCESS},
		//参数 dst_offset 等于 dst_lcop 表示的 LOB 对象长度值
		{"北京人大金仓", "研发中心工程部", 14, 11, 1, "北京人大金研发中心工程部", DCI_SUCCESS},
		//参数 dst_offset 为 0
		{"北京人大金仓", "研发中心工程部", 14, 0, 1, "北京人大金仓研发中心工程部", DCI_ERROR},

		//参数 src_offset 大于 src_locp 表示的 LOB 对象长度值
		{"北京人大金仓", "研发中心工程部", 14, 1, 20, "北京人大金仓", DCI_ERROR},
		//参数 src_offset 小于 src_locp 表示的 LOB 对象长度值
		{"北京人大金仓", "研发中心工程部", 14, 1, 5, "中心工程部仓", DCI_SUCCESS},
		//参数 src_offset 等于 src_locp 表示的 LOB 对象长度值
		{"北京人大金仓", "研发中心工程部",14, 1, 13, "部京人大金仓", DCI_SUCCESS},
		//参数 src_offset 为 0
		{"北京人大金仓", "研发中心工程部", 14, 1, 0, "北京人大金仓", DCI_ERROR},

		//参数 amount 和参数 dst_offset 之和大于 1G – 1
		//{"北京人大金仓",  "研发中心工程部", CLOB_MAXLENGTH, 1, 1, "研发中心工程部", DCI_SUCCESS},
		//参数 amount 和参数 dst_offset 之和大于 1G – 1
		{"北京人大金仓", "研发中心工程部", CLOB_MAXLENGTH, 1, 1, "北京人大金仓", DCI_ERROR},

		//参数 amount 和参数 dst_offset 之和小于 1G – 1
		{"北京人大金仓", "研发中心工程部", CLOB_MAXLENGTH - 2, 1, 1, "研发中心工程部", DCI_SUCCESS},
		//参数 amount 和参数 dst_offset 之和等于 1G – 1
		{"北京人大金仓", "研发中心工程部", CLOB_MAXLENGTH - 1, 1, 1, "研发中心工程部", DCI_SUCCESS},

		{"aaaa", NULL, 4, 1, 1, "aaaa", DCI_INVALID_HANDLE},
		{NULL, "aaaa", 4, 1, 1, "aaaa", DCI_INVALID_HANDLE},
		{NULL, "", 4, 1, 1, "", DCI_INVALID_HANDLE},
		{"", NULL, 4, 1, 1, "", DCI_INVALID_HANDLE},
		{NULL, NULL, 4, 1, 1, "", DCI_INVALID_HANDLE},
	};

	char	*dst_in, *src_in;
	char	dst_out[BUFLEN * 2];
	ub4		amtp_dst = BUFLEN * 2;
	int		i = 0;

	dst_in = test_clob[0].dst_in;
	src_in = test_clob[0].src_in;

	//test handle
	for (i = 0;i < (int)sizeof(clob_array) / (int)sizeof(clob_array[0]); i++)
	{
		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_clob(table_name2, src_in);

		if (clob_array[i].dst_flag == 1)
			dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);

		if (clob_array[i].src_flag == 1)
			src_locp = select_table_clob(table_name2, pDefine2, src_locp);

		err = DCILobCopy(clob_array[i].pSvcCtx, clob_array[i].pError, dst_locp, 
			src_locp, clob_array[i].amount, clob_array[i].dst_offset, 
			clob_array[i].src_offset);

		if (err != clob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLObCopy_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		src_locp = NULL;
		dst_locp = NULL;

	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		dst_in = test_clob[i].dst_in;
		src_in = test_clob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_clob(table_name2, src_in);

		if (dst_in != NULL)
			dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);

		if (src_in != NULL)
			src_locp = select_table_clob(table_name2, pDefine2, src_locp);

		err = DCILobCopy(pSvcCtx, pError, dst_locp, 
			src_locp, test_clob[i].amount, test_clob[i].dst_offset, 
			test_clob[i].src_offset);

		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLObCopy_clob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		if (dst_locp != NULL && src_locp != NULL && test_clob[i].ret_val == DCI_SUCCESS)
		{
			amtp_dst = sizeof(dst_out);
			memset(dst_out, 0, amtp_dst);

			err = DCILobRead(pSvcCtx, pError, dst_locp, &amtp_dst, (ub4)1, 
				(void *)dst_out, (ub4)sizeof(dst_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				printf("TestLObCopy_clob: FAILED\n");
				ret = DCI_ERROR;
			}

			if (memcmp(dst_out, test_clob[i].expect_ret, amtp_dst) != 0)
			{
				printf("TestLobCopy_clob: FAILED\n");
				ret = DCI_ERROR;
			}
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	} //end of for

	return ret;
}

sword
TestLobCopy_blob()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;
	typedef struct test_blob_hnd
	{
		DCISvcCtx	*pSvcCtx; 
		DCIError	*pError;
		int			dst_flag;
		int			src_flag;
		ub4			amount;
		ub4			src_offset;
		ub4			dst_offset;
		sword		ret_val;
	}test_blob_hnd;

	test_blob_hnd blob_array[]=
	{
		{NULL, pError, 1, 1, BUFLEN, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, NULL, 1, 1, BUFLEN, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 0, 1, BUFLEN, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, 0, BUFLEN, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, 1, BUFLEN, 1, 1, DCI_SUCCESS},
	};

	typedef struct test_blob_suite
	{
		char	*dst_in;
		int		dst_len;
		char	*src_in;
		int		src_len;
		ub4		amount;
		ub4		dst_offset;
		ub4		src_offset;
		char	expect_ret[BUFLEN * 2];
		sword	ret_val;
	}test_blob_suite;

	test_blob_suite test_blob[] = 
	{
		//参数 amount 大于 src_locp 表示的 LOB对象从 src_offset 之后的剩余长度
		{"123456", 6, "abcdef", 6, 10, 1, 1, "abcdef", DCI_SUCCESS},
		//参数 amount 小于src_locp 表示的 LOB 对象从 src_offset 之后剩余的长度,
		{"123456", 6, "abcdef", 6, 4, 1, 1, "abcd56", DCI_SUCCESS},
		//参数 amount 等于src_locp 表示的 LOB 对象从 src_offset 之后剩余的长度
		{"123456", 6, "abcdef", 6, 6, 1, 1, "abcdef", DCI_SUCCESS},
		//参数 amount 为 0
		{"123456", 6, "abcdef", 6, 0, 1, 1, "123456", DCI_SUCCESS},

		//参数 dst_offset 大于 dst_lcop 表示的 LOB 对象长度值
		{"123456", 6, "abcdef", 6, 6, 10, 1, "123456\0\0\0abcdef", DCI_SUCCESS},
		//参数 dst_offset 小于 dst_lcop 表示的 LOB 对象长度值
		{"123456", 6, "abcdef", 6, 6, 4, 1, "123abcdef", DCI_SUCCESS},
		//参数 dst_offset 等于 dst_lcop 表示的 LOB 对象长度值
		{"123456", 6, "abcdef", 6, 6, 6, 1, "12345abcdef", DCI_SUCCESS},
		//参数 dst_offset 为 0
		{"123456", 6, "abcdef", 6, 6, 0, 1, "123456", DCI_ERROR},

		//参数 src_offset 大于 src_locp 表示的 LOB 对象长度值
		{"123456", 6, "abcdef", 6, 6, 1, 10, "123456", DCI_ERROR},
		//参数 src_offset 小于 src_locp 表示的 LOB 对象长度值
		{"123456", 6, "abcdef", 6, 6, 1, 4, "def456", DCI_SUCCESS},
		//参数 src_offset 等于 src_locp 表示的 LOB 对象长度值
		{"123456", 6, "abcdef", 6, 6, 1, 6, "f23456", DCI_SUCCESS},
		//参数 src_offset 为 0
		{"123456", 6, "abcdef", 6, 6, 1, 0, "123456", DCI_ERROR},

		//参数 amount 和参数 dst_offset 之和大于 2G – 1
		{"123456", 6, "abcdef", 6, BLOB_MAXLENGTH, 1, 1, "123456", DCI_ERROR},
#if !defined(WIN32)
		//参数 amount 和参数 dst_offset 之和小于 2G – 1
		{"123456", 6, "abcdef", 6, BLOB_MAXLENGTH - 2, 1, 1, "abcdef", DCI_SUCCESS},
		//参数 amount 和参数 dst_offset 之和等于 2G – 1
		{"123456", 6, "abcdef", 6, BLOB_MAXLENGTH - 1, 1, 1, "abcdef", DCI_SUCCESS},
#endif
		{NULL, 0, "", 0, 4, 1, 1, "", DCI_INVALID_HANDLE},
		{NULL, 0, NULL, 0, 4, 1, 1, "", DCI_INVALID_HANDLE},
	};

	char	*dst_in, *src_in;
	int		src_len, dst_len;
	char	dst_out[BUFLEN * 2];
	ub4		amtp_dst = BUFLEN * 2;
	int		i = 0;

	dst_in = test_blob[0].dst_in; 
	src_in = test_blob[0].src_in;
	dst_len = test_blob[0].dst_len;
	src_len = test_blob[0].src_len;

	//test handle
	for (i = 0; i < (int)sizeof(blob_array) / (int)sizeof(blob_array[0]); i++)
	{
		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, dst_in, dst_len);
		insert_table_blob(table_name2, src_in, src_len);

		if (blob_array[i].dst_flag == 1)
			dst_locp = select_table_blob(table_name1, pDefine1, dst_locp);

		if (blob_array[i].src_flag == 1)
			src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		err = DCILobCopy(blob_array[i].pSvcCtx, blob_array[i].pError, dst_locp, 
			src_locp, blob_array[i].amount, blob_array[i].dst_offset, 
			blob_array[i].src_offset);

		if (err != blob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		src_locp = NULL;
		dst_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		dst_in = test_blob[i].dst_in; 
		src_in = test_blob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, dst_in, test_blob[i].dst_len);
		insert_table_blob(table_name2, src_in, test_blob[i].src_len);

		if (dst_in != NULL)
			dst_locp = select_table_blob(table_name1, pDefine1, dst_locp);

		if (src_in != NULL)
			src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		err = DCILobCopy(pSvcCtx, pError, dst_locp, 
			src_locp, test_blob[i].amount, test_blob[i].dst_offset, 
			test_blob[i].src_offset);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_blob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		if (dst_locp != NULL && src_locp != NULL && test_blob[i].ret_val == DCI_SUCCESS)
		{
			amtp_dst = sizeof(dst_out);
			memset(dst_out, 0, amtp_dst);
			err = DCILobRead(pSvcCtx, pError, dst_locp, &amtp_dst, (ub4)1, 
				(void *)dst_out, (ub4)sizeof(dst_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				printf("TestLobCopy_blob: FAILED\n");
				ret = DCI_ERROR;
			}

			if (memcmp(dst_out, test_blob[i].expect_ret, amtp_dst))
			{
				printf("TestLobCopy_blob: FAILED\n");
				ret = DCI_ERROR;
			}
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	} //end of for

	return ret;
}

sword
TestLobCopy_clob_blob()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;
	typedef struct test_clob_blob_hnd
	{
		DCISvcCtx	*pSvcCtx; 
		DCIError	*pError;
		int			dst_flag;
		int			src_flag;
		ub4			amount;
		ub4			src_offset;
		ub4			dst_offset;
		sword		ret_val;
	}test_clob_blob_hnd;

	test_clob_blob_hnd clob_blob_array[]=
	{
		{NULL, pError, 1, 1, BUFLEN, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, NULL, 1, 1, BUFLEN, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 0, 1, BUFLEN, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, 0, BUFLEN, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, 1, BUFLEN, 1, 1, DCI_ERROR},
	};

	typedef struct test_clob_blob_suite
	{
		char	*dst_in;
		char	*src_in;
		ub4		src_len;
		ub4		amount;
		ub4		dst_offset;
		ub4		src_offset;
		char	expect_ret[BUFLEN * 2];
		sword	ret_val;
	}test_clob_blob_suite;

	test_clob_blob_suite test_clob_blob[] = 
	{
		{"北京人大金仓", "123456", 6, 6, 1, 1, "123456大金仓", DCI_ERROR},
		{"aaa", "bbbbb", 5, 5, 1, 1, "bbbbbb", DCI_ERROR},
		{NULL, "", 0, 4, 1, 1, "", DCI_INVALID_HANDLE},
		{NULL, NULL, 0, 4, 1, 1, "", DCI_INVALID_HANDLE},
	};

	char	*dst_in, *src_in;
	ub4		src_len;
	int		i = 0;

	//test error handle
	dst_in = test_clob_blob[0].dst_in; 
	src_in = test_clob_blob[0].src_in;
	src_len = test_clob_blob[0].src_len;

	//test handle
	for (i = 0;i < (int)sizeof(clob_blob_array) / (int)sizeof(clob_blob_array[0]); i++)
	{
		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_blob(table_name2, src_in, src_len);

		if (clob_blob_array[i].dst_flag == 1)
			dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);

		if (clob_blob_array[i].src_flag == 1)
			src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		err = DCILobCopy(clob_blob_array[i].pSvcCtx, clob_blob_array[i].pError, dst_locp, 
			src_locp, clob_blob_array[i].amount, clob_blob_array[i].dst_offset, 
			clob_blob_array[i].src_offset);
		if (err != clob_blob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_clob_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		src_locp = NULL;
		dst_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_clob_blob) / (int)sizeof(test_clob_blob[0]); i++)
	{
		dst_in = test_clob_blob[i].dst_in; 
		src_in = test_clob_blob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_blob(table_name2, src_in, src_len);

		if (dst_in != NULL)
			dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);

		if (src_in != NULL)
			src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		err = DCILobCopy(pSvcCtx, pError, dst_locp, src_locp, 
			clob_blob_array[i].amount, clob_blob_array[i].dst_offset, 
			clob_blob_array[i].src_offset);

		if (err != test_clob_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_clob_blob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	} //end of for

	return ret;
}

sword
TestLobCopy_blob_clob()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;
	typedef struct test_blob_clob_hnd
	{
		DCISvcCtx	*pSvcCtx; 
		DCIError	*pError;
		int			dst_flag;
		int			src_flag;
		ub4			amount;
		ub4			src_offset;
		ub4			dst_offset;
		sword		ret_val;
	}test_blob_clob_hnd;

	test_blob_clob_hnd blob_clob_array[]=
	{
		{NULL, pError, 1, 1, BUFLEN, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, NULL, 1, 1, BUFLEN, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 0, 1, BUFLEN, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, 0, BUFLEN, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, 1, BUFLEN, 1, 1, DCI_ERROR},
	};

	typedef struct test_blob_clob_suite
	{
		char	*dst_in;
		ub4		dst_len;
		char	*src_in;
		ub4		amount;
		ub4		src_offset;
		ub4		dst_offset;
		char	expect_ret[BUFLEN * 2];
		sword	ret_val;
	}test_blob_clob_suite;

	test_blob_clob_suite test_blob_clob[] = 
	{
		{"123456", 6, "北京人大金仓", 12, 1, 1, "北京人大金仓", DCI_ERROR},
		{"bbbbb", 5, "aaa", 3, 1, 1, "aaabb", DCI_ERROR},
		{NULL, 0, "", 4, 1, 1, "", DCI_INVALID_HANDLE},
		{NULL, 0, NULL, 4, 1, 1, "", DCI_INVALID_HANDLE},
	};

	char	*dst_in, *src_in;
	ub4		dst_len;
	int		i = 0;

	//test error handle
	dst_in = test_blob_clob[0].dst_in; 
	src_in = test_blob_clob[0].src_in;
	dst_len = test_blob_clob[0].dst_len;

	//test handle
	for (i = 0;i < (int)sizeof(blob_clob_array) / (int)sizeof(blob_clob_array[0]); i++)
	{
		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_clob(table_name2);

		insert_table_blob(table_name1, dst_in, dst_len);
		insert_table_clob(table_name2, src_in);

		if (blob_clob_array[i].dst_flag == 1)
			dst_locp = select_table_blob(table_name1, pDefine1, dst_locp);

		if (blob_clob_array[i].src_flag == 1)
			src_locp = select_table_clob(table_name2, pDefine2, src_locp);

		err = DCILobCopy(blob_clob_array[i].pSvcCtx, blob_clob_array[i].pError, dst_locp, 
			src_locp, blob_clob_array[i].amount, blob_clob_array[i].dst_offset, 
			blob_clob_array[i].src_offset);	

		if (err != blob_clob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_blob_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		src_locp = NULL;
		dst_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_blob_clob) / (int)sizeof(test_blob_clob[0]); i++)
	{
		dst_in = test_blob_clob[i].dst_in;
		src_in = test_blob_clob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_clob(table_name2);

		insert_table_blob(table_name1, dst_in, dst_len);
		insert_table_clob(table_name2, src_in);

		if (dst_in != NULL)
			dst_locp = select_table_blob(table_name1, pDefine1, dst_locp);

		if (src_in != NULL)
			src_locp = select_table_clob(table_name2, pDefine2, src_locp);

		err = DCILobCopy(pSvcCtx, pError, dst_locp, 
			src_locp, test_blob_clob[i].amount, test_blob_clob[i].dst_offset, 
			test_blob_clob[i].src_offset);
		if (err != test_blob_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_blob_clob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	} //end of for

	return ret;
}

sword 
TestLobCopy_clob_enablebuffering()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;
	typedef struct test_clob_suite
	{
		char	*src_in;
		char	*dst_in;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_clob_suite;

	test_clob_suite test_clob[] = 
	{
		{"北京人大金仓", "研发中心工程部", "北京人大金仓研发中心工程部", DCI_ERROR},
		{"aaa", "bbbbb", "aaabbbbb", DCI_ERROR},
	};

	char	*dst_in, *src_in;

	int		i = 0;

	//参数 src_locp 启用缓存子系统

	for (i = 0; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		src_in = test_clob[i].src_in;
		dst_in = test_clob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_clob(table_name2, dst_in);

		src_locp = select_table_clob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobCopy_clob enable buffer failed(enable buffer and don't write)\n");
		}

		err = DCILobCopy(pSvcCtx, pError, dst_locp, src_locp, (ub4)strlen(src_in), 1, 1);
		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_clob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 dst_locp 启用缓存子系统
	for (i = 0 ; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		src_in = test_clob[i].src_in;
		dst_in = test_clob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_clob(table_name2, dst_in);

		src_locp = select_table_clob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobCopy_clob enable buffer failed\n");
		}

		err = DCILobCopy(pSvcCtx, pError, dst_locp, src_locp, (ub4)strlen(src_in), 1, 1);
		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_clob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 src_locp 和 dst_locp 启用缓存子系统
	for (i = 0 ; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		src_in = test_clob[i].src_in;
		dst_in = test_clob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_clob(table_name2, dst_in);

		src_locp = select_table_clob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobCopy_clob enable buffer failed\n");
		}

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobCopy_clob enable buffer failed\n");
		}

		err = DCILobCopy(pSvcCtx, pError, dst_locp, src_locp, (ub4)strlen(src_in), 1, 1);
		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_clob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}
		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	}

	return ret;
}

sword 
TestLobCopy_blob_enablebuffering()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_blob_suite
	{
		char	*src_in;
		char	*dst_in;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_blob_suite;

	test_blob_suite test_blob[] = 
	{
		{"123", "456", "123456", DCI_ERROR},
		{"aaa", "bbbbb", "aaabbbbb", DCI_ERROR},
	};

	char	*dst_in, *src_in;

	int		i = 0;

	//参数 src_locp 启用缓存子系统

	for (i = 0; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		src_in = test_blob[i].src_in;
		dst_in = test_blob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, src_in, (int)strlen(src_in));
		insert_table_blob(table_name2, dst_in, (int)strlen(dst_in));

		src_locp = select_table_blob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobCopy_blob enable buffer failed(enable buffer and don't write)\n");
		}

		err = DCILobCopy(pSvcCtx, pError, dst_locp, src_locp, (ub4)strlen(src_in), 1, 1);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 dst_locp 启用缓存子系统
	for (i = 0 ; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		src_in = test_blob[i].src_in;
		dst_in = test_blob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, src_in, (int)strlen(src_in));
		insert_table_blob(table_name2, dst_in, (int)strlen(dst_in));

		src_locp = select_table_blob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobCopy_blob enable buffer failed\n");
		}

		err = DCILobCopy(pSvcCtx, pError, dst_locp, src_locp, (ub4)strlen(src_in), 1, 1);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 src_locp 和 dst_locp 启用缓存子系统
	for (i = 0 ; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		src_in = test_blob[i].src_in;
		dst_in = test_blob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, src_in, (int)strlen(src_in));
		insert_table_blob(table_name2, dst_in, (int)strlen(dst_in));

		src_locp = select_table_blob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobCopy_blob enable buffer failed\n");
		}

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobCopy_blob enable buffer failed\n");
		}

		err = DCILobCopy(pSvcCtx, pError, dst_locp, src_locp, (ub4)strlen(src_in), 1, 1);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}
		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	}

	return ret;
}

sword 
TestLobCopy_clob_blob_enablebuffering()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_clob_blob_suite
	{
		char	*src_in;
		char	*dst_in;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_clob_blob_suite;

	test_clob_blob_suite test_clob_blob[] = 
	{
		{"北京人大金仓", "研发中心工程部", "北京人大金仓研发中心工程部", DCI_ERROR},
		{"aaa", "bbbbb", "aaabbbbb", DCI_ERROR},
	};

	char	*dst_in, *src_in;

	int		i = 0;

	//参数 src_locp 启用缓存子系统

	for (i = 0; i < (int)sizeof(test_clob_blob) / (int)sizeof(test_clob_blob[0]); i++)
	{
		src_in = test_clob_blob[i].src_in;
		dst_in = test_clob_blob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_blob(table_name2, dst_in, (int)strlen(dst_in));

		src_locp = select_table_clob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobCopy_clob_blob enable buffer failed(enable buffer and don't write)\n");
		}

		err = DCILobCopy(pSvcCtx, pError, dst_locp, src_locp, (ub4)strlen(src_in), 1, 1);
		if (err != test_clob_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_clob_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 dst_locp 启用缓存子系统
	for (i = 0 ; i < (int)sizeof(test_clob_blob) / (int)sizeof(test_clob_blob[0]); i++)
	{
		src_in = test_clob_blob[i].src_in;
		dst_in = test_clob_blob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_blob(table_name2, dst_in, (int)strlen(dst_in));

		src_locp = select_table_clob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobCopy_clob_blob enable buffer failed\n");
		}

		err = DCILobCopy(pSvcCtx, pError, dst_locp, src_locp, (ub4)strlen(src_in), 1, 1);
		if (err != test_clob_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_clob_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 src_locp 和 dst_locp 启用缓存子系统
	for (i = 0 ; i < (int)sizeof(test_clob_blob) / (int)sizeof(test_clob_blob[0]); i++)
	{
		src_in = test_clob_blob[i].src_in;
		dst_in = test_clob_blob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_blob(table_name2, dst_in, (int)strlen(dst_in));

		src_locp = select_table_clob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobCopy_clob_blob enable buffer failed\n");
		}

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobCopy_clob_blob enable buffer failed\n");
		}

		err = DCILobCopy(pSvcCtx, pError, dst_locp, src_locp, (ub4)strlen(src_in), 1, 1);
		if (err != test_clob_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_clob_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}
		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	}

	return ret;
}

sword 
TestLobCopy_blob_clob_enablebuffering()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_blob_clob_suite
	{
		char	*src_in;
		char	*dst_in;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_blob_clob_suite;

	test_blob_clob_suite test_blob_clob[] = 
	{
		{"aaa", "bbbbb", "aaabbbbb", DCI_ERROR},
		{"北京人大金仓", "研发中心工程部", "北京人大金仓研发中心工程部", DCI_ERROR},
	};

	char	*dst_in, *src_in;

	int		i = 0;

	//参数 src_locp 启用缓存子系统

	for (i = 0; i < (int)sizeof(test_blob_clob) / (int)sizeof(test_blob_clob[0]); i++)
	{
		src_in = test_blob_clob[i].src_in;
		dst_in = test_blob_clob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_clob(table_name2);

		insert_table_blob(table_name1, src_in, (int)strlen(src_in));
		insert_table_clob(table_name2, dst_in);

		src_locp = select_table_blob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobCopy_blob_clob enable buffer failed(enable buffer and don't write)\n");
		}

		err = DCILobCopy(pSvcCtx, pError, dst_locp, src_locp, (ub4)strlen(src_in), 1, 1);
		if (err != test_blob_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_blob_clob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 dst_locp 启用缓存子系统
	for (i = 0 ; i < (int)sizeof(test_blob_clob) / (int)sizeof(test_blob_clob[0]); i++)
	{
		src_in = test_blob_clob[i].src_in;
		dst_in = test_blob_clob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_clob(table_name2);

		insert_table_blob(table_name1, src_in, (int)strlen(src_in));
		insert_table_clob(table_name2, dst_in);

		src_locp = select_table_blob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobCopy_blob_clob enable buffer failed\n");
		}

		err = DCILobCopy(pSvcCtx, pError, dst_locp, src_locp, (ub4)strlen(src_in), 1, 1);
		if (err != test_blob_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_blob_clob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 src_locp 和 dst_locp 启用缓存子系统
	for (i = 0 ; i < (int)sizeof(test_blob_clob) / (int)sizeof(test_blob_clob[0]); i++)
	{
		src_in = test_blob_clob[i].src_in;
		dst_in = test_blob_clob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_clob(table_name2);

		insert_table_blob(table_name1, src_in, (int)strlen(src_in));
		insert_table_clob(table_name2, dst_in);

		src_locp = select_table_blob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobCopy_blob_clob enable buffer failed\n");
		}

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobCopy_blob_clob enable buffer failed\n");
		}

		err = DCILobCopy(pSvcCtx, pError, dst_locp, src_locp, (ub4)strlen(src_in), 1, 1);
		if (err != test_blob_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobCopy_blob_clob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}
		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	}

	return ret;
}

sword
TestLobCopy()
{
	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

//#if 0
	if (TestLobCopy_clob() == DCI_ERROR)
	{
		printf("TestLobCopy_clob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobCopy_clob SUCCESS\n");
//#endif

	if (TestLobCopy_blob() == DCI_ERROR)
	{
		printf("TestLobCopy_blob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobCopy_blob SUCCESS\n");

#if 0
	if (TestLobCopy_clob_blob() == DCI_ERROR)
	{
		printf("TestLobCopy_clob_blob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobCopy_clob_blob SUCCESS\n");

	if (TestLobCopy_blob_clob() == DCI_ERROR)
	{
		printf("TestLobCopy_blob_clob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobCopy_blob_clob SUCCESS\n");

	if (TestLobCopy_clob_enablebuffering() == DCI_ERROR)
	{
		printf("TestLobCopy_clob_enablebuffering FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobCopy_clob_enablebuffering SUCCESS\n");
#endif

	if (TestLobCopy_blob_enablebuffering() == DCI_ERROR)
	{
		printf("TestLobCopy_blob_enablebuffering FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobCopy_blob_enablebuffering SUCCESS\n");

#if 0
	if (TestLobCopy_clob_blob_enablebuffering() == DCI_ERROR)
	{
		printf("TestLobCopy_clob_blob_enablebuffering FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobCopy_clob_blob_enablebuffering SUCCESS\n");

	if (TestLobCopy_blob_clob_enablebuffering() == DCI_ERROR)
	{
		printf("TestLobCopy_blob_clob_enablebuffering FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobCopy_blob_clob_enablebuffering SUCCESS\n");
#endif

	err = disconnect(true);

	return DCI_SUCCESS;
}

sword 
TestLobAssign_clob()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;
	typedef struct test_clob_hnd
	{
		DCIEnv		*pEnv; 
		DCIError	*pError;
		int			src_flag;
		int			dst_flag;
		sword		ret_val;
	}test_clob_hnd;

	test_clob_hnd clob_array[]=
	{
		{NULL, pError, 1, 1, DCI_INVALID_HANDLE},
		{pEnv, NULL, 1, 1, DCI_INVALID_HANDLE},
		{pEnv, pError, 0, 1, DCI_INVALID_HANDLE},
		{pEnv, pError, 1, 0, DCI_INVALID_HANDLE},
		{pEnv, pError, 1, 1, DCI_SUCCESS},
	};

	typedef struct test_clob_suite
	{
		char	*src_in;
		char	*dst_in;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_clob_suite;

	test_clob_suite test_clob[] = 
	{
		{"北京人大金仓", "研发中心工程部", "北京人大金仓", DCI_SUCCESS},
		{"aaa", "bbbbb", "aaa", DCI_SUCCESS},
		{NULL, "", "", DCI_INVALID_HANDLE},
		{NULL, NULL, "", DCI_INVALID_HANDLE},
	};

	char	*dst_in, *src_in;
	char	dst_out[BUFLEN];
	ub4		amtp_dst = BUFLEN;
	int		i = 0;

	src_in = test_clob[0].src_in;
	dst_in = test_clob[0].dst_in; 

	//test handle
	for (i = 0;i < (int)sizeof(clob_array) / (int)sizeof(clob_array[0]); i++)
	{
		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_clob(table_name2, dst_in);

		if (clob_array[i].src_flag == 1)
			src_locp = select_table_clob(table_name1, pDefine1, src_locp);

		if (clob_array[i].dst_flag == 1)
			dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobAssign(clob_array[i].pEnv, clob_array[i].pError, src_locp, &dst_locp);

		if (err != clob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAssign_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		src_locp = NULL;
		dst_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		src_in = test_clob[i].src_in;
		dst_in = test_clob[i].dst_in; 

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_clob(table_name2, dst_in);

		if (src_in != NULL)
			src_locp = select_table_clob(table_name1, pDefine1, src_locp);

		if (dst_in != NULL)
			dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);
		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAssign_clob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		//DCILobAssign使目的locator指向源locator指向的大对象，
		//此时locator的内容已经改变，不再指向原来的locator
		//不能再从locator之前指向的大对象字段读取内容
		if (dst_locp != NULL && test_clob[i].ret_val == DCI_SUCCESS)
		{
			amtp_dst = sizeof(dst_out);
			memset(dst_out, 0, amtp_dst);
			err = DCILobRead(pSvcCtx, pError, dst_locp, &amtp_dst, (ub4)1, 
				(void *)dst_out, (ub4)sizeof(dst_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				printf("TestLobAssign_clob: FAILED\n");
				ret = DCI_ERROR;
			}

			if (memcmp(dst_out, test_clob[i].expect_ret, amtp_dst))
			{
				printf("TestLobAssign_clob: FAILED\n");
				ret = DCI_ERROR;
			}
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	} //end of for

	return ret;
}

sword
TestLobAssign_blob()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_blob_hnd
	{
		DCIEnv		*pEnv; 
		DCIError	*pError;
		int			src_flag;
		int			dst_flag;
		sword		ret_val;
	}test_blob_hnd;

	test_blob_hnd blob_array[]=
	{
		{NULL, pError, 1, 1, DCI_INVALID_HANDLE},
		{pEnv, NULL, 1, 1, DCI_INVALID_HANDLE},
		{pEnv, pError, 0, 1, DCI_INVALID_HANDLE},
		{pEnv, pError, 1, 0, DCI_INVALID_HANDLE},
		{pEnv, pError, 1, 1, DCI_SUCCESS},
	};

	typedef struct test_blob_suite
	{
		char	*src_in;
		ub4		src_len;
		char	*dst_in;
		ub4		dst_len;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_blob_suite;

	test_blob_suite test_blob[] = 
	{
		{"123456", 6, "abcdef", 6, "123456", DCI_SUCCESS},
		{"aaa", 3, "bbbbb", 5, "aaa", DCI_SUCCESS},
		{NULL, 0, "", 0, "", DCI_INVALID_HANDLE},
		{NULL, 0, NULL, 0, "", DCI_INVALID_HANDLE},
	};

	char	*dst_in, *src_in;
	ub4		dst_len, src_len;
	char	dst_out[BUFLEN];
	ub4		amtp_dst = BUFLEN;
	int		i = 0;

	src_in = test_blob[0].src_in;
	dst_in = test_blob[0].dst_in; 
	src_len = test_blob[0].src_len;
	dst_len = test_blob[0].dst_len;

	//test handle
	for (i = 0;i < (int)sizeof(blob_array) / (int)sizeof(blob_array[0]); i++)
	{
		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, src_in, src_len);
		insert_table_blob(table_name2, dst_in, dst_len);

		if (blob_array[i].src_flag == 1)
			src_locp = select_table_blob(table_name1, pDefine1, src_locp);

		if (blob_array[i].dst_flag == 1)
			dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobAssign(blob_array[i].pEnv, blob_array[i].pError, src_locp, &dst_locp);
		if (err != blob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAssign_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		src_locp = NULL;
		dst_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		src_in = test_blob[i].src_in;
		dst_in = test_blob[i].dst_in; 

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, src_in, test_blob[i].src_len);
		insert_table_blob(table_name2, dst_in, test_blob[i].dst_len);

		if (src_in != NULL)
			src_locp = select_table_blob(table_name1, pDefine1, src_locp);

		if (dst_in != NULL)
			dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAssign_blob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		if (dst_locp != NULL && test_blob[i].ret_val == DCI_SUCCESS)
		{
			amtp_dst = sizeof(dst_out);
			memset(dst_out, 0, amtp_dst);
			err = DCILobRead(pSvcCtx, pError, dst_locp, &amtp_dst, (ub4)1, 
				(void *)dst_out, (ub4)sizeof(dst_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				printf("TestLobAssign_blob: FAILED\n");
				ret = DCI_ERROR;
			}

			if (memcmp(dst_out, test_blob[i].expect_ret, amtp_dst))
			{
				printf("TestLobAssign_blob: FAILED\n");
				ret = DCI_ERROR;
			}
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	} //end of for

	return ret;
}

sword
TestLobAssign_clob_blob()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_clob_blob_hnd
	{
		DCIEnv		*pEnv; 
		DCIError	*pError;
		int			src_flag;
		int			dst_flag;
		sword		ret_val;
	}test_clob_blob_hnd;

	test_clob_blob_hnd clob_blob_array[]=
	{
		{NULL, pError, 1, 1, DCI_INVALID_HANDLE},
		{pEnv, NULL, 1, 1, DCI_INVALID_HANDLE},
		{pEnv, pError, 0, 1, DCI_INVALID_HANDLE},
		{pEnv, pError, 1, 0, DCI_INVALID_HANDLE},
		{pEnv, pError, 1, 1, DCI_SUCCESS},
	};

	typedef struct test_clob_blob_suite
	{
		char	*src_in;
		char	*dst_in;
		ub4		dst_len;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_clob_blob_suite;

	test_clob_blob_suite test_clob_blob[] = 
	{
		{"北京人大金仓", "abcdef", 6, "北京人大金仓", DCI_SUCCESS},
		{"aaa", "bbbbb", 5, "aaa", DCI_SUCCESS},
		{NULL, "", 0, "", DCI_INVALID_HANDLE},
		{NULL, NULL, 0, "", DCI_INVALID_HANDLE},
	};

	char	*dst_in, *src_in;
	ub4		dst_len;
	char	dst_out[BUFLEN];
	ub4		amtp_dst = BUFLEN;
	int		i = 0;

	src_in = test_clob_blob[0].src_in;
	dst_in = test_clob_blob[0].dst_in; 
	dst_len = test_clob_blob[0].dst_len;

	//test handle
	for (i = 0;i < (int)sizeof(clob_blob_array) / (int)sizeof(clob_blob_array[0]); i++)
	{
		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_blob(table_name2, dst_in, dst_len);

		if (clob_blob_array[i].src_flag == 1)
			src_locp = select_table_clob(table_name1, pDefine1, src_locp);

		if (clob_blob_array[i].dst_flag == 1)
			dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobAssign(clob_blob_array[i].pEnv, clob_blob_array[i].pError, src_locp, &dst_locp);
		if (err != clob_blob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAssign_clob_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		src_locp = NULL;
		dst_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_clob_blob) / (int)sizeof(test_clob_blob[0]); i++)
	{
		src_in = test_clob_blob[i].src_in;
		dst_in = test_clob_blob[i].dst_in; 

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_blob(table_name2, dst_in, test_clob_blob[i].dst_len);

		if (src_in != NULL)
			src_locp = select_table_clob(table_name1, pDefine1, src_locp);

		if (dst_in != NULL)
			dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);
		if (err != test_clob_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAssign_clob_blob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		if (dst_locp != NULL && test_clob_blob[i].ret_val == DCI_SUCCESS)
		{
			amtp_dst = sizeof(dst_out);
			memset(dst_out, 0, amtp_dst);
			err = DCILobRead(pSvcCtx, pError, dst_locp, &amtp_dst, (ub4)1, 
				(void *)dst_out, (ub4)sizeof(dst_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				printf("TestLobAssign_clob_blob: FAILED\n");
				ret = DCI_ERROR;
			}

			if (memcmp(dst_out, test_clob_blob[i].expect_ret, amtp_dst))
			{
				printf("TestLobAssign_clob_blob: FAILED\n");
				ret = DCI_ERROR;
			}
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	} //end of for

	return ret;
}

sword
TestLobAssign_blob_clob()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_blob_clob_hnd
	{
		DCIEnv		*pEnv; 
		DCIError	*pError;
		int			src_flag;
		int			dst_flag;
		sword		ret_val;
	}test_blob_clob_hnd;

	test_blob_clob_hnd blob_clob_array[]=
	{
		{NULL, pError, 1, 1, DCI_INVALID_HANDLE},
		{pEnv, NULL, 1, 1, DCI_INVALID_HANDLE},
		{pEnv, pError, 0, 1, DCI_INVALID_HANDLE},
		{pEnv, pError, 1, 0, DCI_INVALID_HANDLE},
		{pEnv, pError, 1, 1, DCI_SUCCESS},
	};

	typedef struct test_blob_clob_suite
	{
		char	*src_in;
		ub4		src_len;
		char	*dst_in;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_blob_clob_suite;

	test_blob_clob_suite test_blob_clob[] = 
	{
		{"abcdef", 6, "北京人大金仓", "abcdef", DCI_SUCCESS},
		{"bbbbb", 5, "aaa", "bbbbb", DCI_SUCCESS},
		{"", 0, NULL, "", DCI_INVALID_HANDLE},
		{NULL, 0, NULL, "", DCI_INVALID_HANDLE},
	};

	char	*dst_in, *src_in;
	ub4		src_len;
	char	dst_out[BUFLEN];
	ub4		amtp_dst = BUFLEN;
	int		i = 0;

	src_in = test_blob_clob[0].src_in;
	dst_in = test_blob_clob[0].dst_in; 
	src_len = test_blob_clob[0].src_len;
	//test handle
	for (i = 0;i < (int)sizeof(blob_clob_array) / (int)sizeof(blob_clob_array[0]); i++)
	{
		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_clob(table_name2);

		insert_table_blob(table_name1, src_in, src_len);
		insert_table_clob(table_name2, dst_in);

		if (blob_clob_array[i].src_flag == 1)
			src_locp = select_table_blob(table_name1, pDefine1, src_locp);

		if (blob_clob_array[i].dst_flag == 1)
			dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobAssign(blob_clob_array[i].pEnv, blob_clob_array[i].pError, src_locp, &dst_locp);
		if (err != blob_clob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAssign_blob_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		src_locp = NULL;
		dst_locp = NULL;

		DCIHandleFree((dvoid *)pStmt, DCI_HTYPE_STMT);
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_blob_clob) / (int)sizeof(test_blob_clob[0]); i++)
	{
		src_in = test_blob_clob[i].src_in;
		dst_in = test_blob_clob[i].dst_in; 

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_clob(table_name2);

		insert_table_blob(table_name1, src_in, test_blob_clob[i].src_len);
		insert_table_clob(table_name2, dst_in);

		if (src_in != NULL)
			src_locp = select_table_blob(table_name1, pDefine1, src_locp);

		if (dst_in != NULL)
			dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);
		if (err != test_blob_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAssign_blob_clob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		if (dst_locp != NULL && test_blob_clob[i].ret_val == DCI_SUCCESS)
		{
			amtp_dst = sizeof(dst_out);
			memset(dst_out, 0, amtp_dst);
			err = DCILobRead(pSvcCtx, pError, dst_locp, &amtp_dst, (ub4)1, 
				(void *)dst_out, (ub4)sizeof(dst_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				printf("TestLobAssign_blob_clob: FAILED\n");
				ret = DCI_ERROR;
			}

			if (memcmp(dst_out, test_blob_clob[i].expect_ret, amtp_dst))
			{
				printf("TestLobAssign_blob_clob: FAILED\n");
				ret = DCI_ERROR;
			}
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	} //end of for

	return ret;
}

sword 
TestLobAssign_clob_enablebuffering()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_clob_suite
	{
		char	*src_in;
		char	*dst_in;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_clob_suite;

	test_clob_suite test_clob[] = 
	{
		{"北京人大金仓", "研发中心工程部", "北京人大金仓", DCI_ERROR},
		{"aaa", "bbbbb", "aaa", DCI_ERROR},
	};

	char	*dst_in, *src_in;
	char	dst_out[BUFLEN];
	ub4		amtp_dst = BUFLEN;

	char	bufp[BUFLEN] = "北京人大金仓";
	ub4		amtp = BUFLEN;
	int		i = 0;

	//参数 src_locp 启用缓存子系统，不进行写操作

	for (i = 0; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		src_in = test_clob[i].src_in;
		dst_in = test_clob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_clob(table_name2, dst_in);

		src_locp = select_table_clob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAssign_clob enable buffer failed(enable buffer and don't write)\n");
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAssign_clob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		if (dst_locp != NULL)
		{
			amtp_dst = sizeof(dst_out);
			memset(dst_out, 0, amtp_dst);
			err = DCILobRead(pSvcCtx, pError, dst_locp, &amtp_dst, (ub4)1, 
				(void *)dst_out, (ub4)sizeof(dst_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				printf("TestLobAssign_clob_enablebuffering: FAILED\n");
				ret = DCI_ERROR;
			}

			if (memcmp(dst_out, test_clob[i].expect_ret, amtp_dst))
			{
				printf("TestLobAssign_clob_enablebuffering: FAILED\n");
				ret = DCI_ERROR;
			}
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 src_locp 启用缓存子系统，并进行写操作
	for (i = 0 ; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		src_in = test_clob[i].src_in;
		dst_in = test_clob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_clob(table_name2, dst_in);

		src_locp = select_table_clob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAssign_clob enable buffer failed\n");
		}

		err = DCILobWrite(pSvcCtx, pError, src_locp, &amtp, (ub4)1, bufp, 
			(int)sizeof(bufp), (ub1)DCI_ONE_PIECE, 
			(void *)0, 0, 0, SQLCS_IMPLICIT);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAssign_clob enable buffer ,lobwrite failed\n");
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);
		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAssign_clob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	}

	return ret;
}

sword 
TestLobAssign_blob_enablebuffering()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_blob_suite
	{
		char	*src_in;
		char	*dst_in;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_blob_suite;

	test_blob_suite test_blob[] = 
	{
		{"123", "456", "123", DCI_ERROR},
		{"aaa", "bbbbb", "aaa", DCI_ERROR},
	};

	char	*dst_in, *src_in;
	char	dst_out[BUFLEN];
	ub4		amtp_dst = BUFLEN;

	char	bufp[BUFLEN] = "123143252";
	ub4		amtp = BUFLEN;
	int		i = 0;

	//参数 src_locp 启用缓存子系统，不进行写操作

	for (i = 0; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		src_in = test_blob[i].src_in;
		dst_in = test_blob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, src_in, (int)strlen(src_in));
		insert_table_blob(table_name2, dst_in, (int)strlen(dst_in));

		src_locp = select_table_blob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAssign_blob enable buffer failed(enable buffer and don't write)\n");
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);	
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAssign_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		if (dst_locp != NULL)
		{
			amtp_dst = sizeof(dst_out);
			memset(dst_out, 0, amtp_dst);
			err = DCILobRead(pSvcCtx, pError, dst_locp, &amtp_dst, (ub4)1, 
				(void *)dst_out, (ub4)sizeof(dst_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				printf("TestLobAssign_blob_enablebuffering: FAILED\n");
				ret = DCI_ERROR;
			}

			if (memcmp(dst_out, test_blob[i].expect_ret, amtp_dst))
			{
				printf("TestLobAssign_blob_enablebuffering: FAILED\n");
				ret = DCI_ERROR;
			}
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 src_locp 启用缓存子系统，并进行写操作
	for (i = 0 ; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		src_in = test_blob[i].src_in;
		dst_in = test_blob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, src_in, (int)strlen(src_in));
		insert_table_blob(table_name2, dst_in, (int)strlen(dst_in));

		src_locp = select_table_blob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAssign_blob enable buffer failed\n");
		}

		err = DCILobWrite(pSvcCtx, pError, src_locp, &amtp, (ub4)1, bufp, 
			(int)sizeof(bufp), (ub1)DCI_ONE_PIECE, 
			(void *)0, 0, 0, SQLCS_IMPLICIT);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAssign_blob enable buffer ,lobwrite failed\n");
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAssign_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	}

	return ret;
}

sword
TestLobAssign_clob_blob_enablebuffering()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_clob_blob_suite
	{
		char	*src_in;
		char	*dst_in;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_clob_blob_suite;

	test_clob_blob_suite test_clob_blob[] = 
	{
		{"研发中心", "工程部", "研发中心", DCI_ERROR},
		{"aaa", "bbbbb", "aaa", DCI_ERROR},
	};

	char	*dst_in, *src_in;
	char	dst_out[BUFLEN];
	ub4		amtp_dst = BUFLEN;

	char	bufp[BUFLEN] = "北京人大金仓";
	ub4		amtp = BUFLEN;
	int		i = 0;

	//参数 src_locp 启用缓存子系统，不进行写操作

	for (i = 0; i < (int)sizeof(test_clob_blob) / (int)sizeof(test_clob_blob[0]); i++)
	{
		src_in = test_clob_blob[i].src_in;
		dst_in = test_clob_blob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_blob(table_name2, dst_in, (int)strlen(dst_in));

		src_locp = select_table_clob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAssign_clob_blob enable buffer failed(enable buffer and don't write)\n");
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAssign_clob_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		if (dst_locp != NULL)
		{
			amtp_dst = sizeof(dst_out);
			memset(dst_out, 0, amtp_dst);
			err = DCILobRead(pSvcCtx, pError, dst_locp, &amtp_dst, (ub4)1, 
				(void *)dst_out, (ub4)sizeof(dst_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				printf("TestLobAssign_clob_blob_enablebuffering: FAILED\n");
				ret = DCI_ERROR;
			}

			if (memcmp(dst_out, test_clob_blob[i].expect_ret, amtp_dst))
			{
				printf("TestLobAssign_clob_blob_enablebuffering: FAILED\n");
				ret = DCI_ERROR;
			}
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 src_locp 启用缓存子系统，并进行写操作
	for (i = 0 ; i < (int)sizeof(test_clob_blob) / (int)sizeof(test_clob_blob[0]); i++)
	{
		src_in = test_clob_blob[i].src_in;
		dst_in = test_clob_blob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_blob(table_name2, dst_in, (int)strlen(dst_in));

		src_locp = select_table_clob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAssign_clob_blob enable buffer failed\n");
		}

		err = DCILobWrite(pSvcCtx, pError, src_locp, &amtp, (ub4)1, bufp, 
			(int)sizeof(bufp), (ub1)DCI_ONE_PIECE, 
			(void *)0, 0, 0, SQLCS_IMPLICIT);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAssign_clob_blob enable buffer ,lobwrite failed\n");
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);	
		if (err != test_clob_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAssign_clob_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	}

	return ret;
}

sword
TestLobAssign_blob_clob_enablebuffering()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_blob_clob_suite
	{
		char	*src_in;
		char	*dst_in;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_blob_clob_suite;

	test_blob_clob_suite test_blob_clob[] = 
	{
		{"123", "456", "123", DCI_ERROR},
		{"研发中心", "工程部", "研发中心", DCI_ERROR},
	};

	char	*dst_in, *src_in;
	char	dst_out[BUFLEN];
	ub4		amtp_dst = BUFLEN;

	char	bufp[BUFLEN] = "北京人大金仓";
	ub4		amtp = BUFLEN;
	int		i = 0;

	//参数 src_locp 启用缓存子系统，不进行写操作

	for (i = 0; i < (int)sizeof(test_blob_clob) / (int)sizeof(test_blob_clob[0]); i++)
	{
		src_in = test_blob_clob[i].src_in;
		dst_in = test_blob_clob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_clob(table_name2);

		insert_table_blob(table_name1, src_in, (int)strlen(src_in));
		insert_table_clob(table_name2, dst_in);

		src_locp = select_table_blob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAssign_blob_clob enable buffer failed(enable buffer and don't write)\n");
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAssign_blob_clob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		if (dst_locp != NULL)
		{
			amtp_dst = sizeof(dst_out);
			memset(dst_out, 0, amtp_dst);
			err = DCILobRead(pSvcCtx, pError, dst_locp, &amtp_dst, (ub4)1, 
				(void *)dst_out, (ub4)sizeof(dst_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				printf("TestLobAssign_blob_clob_enablebuffering: FAILED\n");
				ret = DCI_ERROR;
			}

			if (memcmp(dst_out, test_blob_clob[i].expect_ret, amtp_dst))
			{
				printf("TestLobAssign_blob_clob_enablebuffering: FAILED\n");
				ret = DCI_ERROR;
			}
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 src_locp 启用缓存子系统，并进行写操作
	for (i = 0 ; i < (int)sizeof(test_blob_clob) / (int)sizeof(test_blob_clob[0]); i++)
	{
		src_in = test_blob_clob[i].src_in;
		dst_in = test_blob_clob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_clob(table_name2);

		insert_table_blob(table_name1, src_in, (int)strlen(src_in));
		insert_table_clob(table_name2, dst_in);

		src_locp = select_table_blob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAssign_blob_clob enable buffer failed\n");
		}

		err = DCILobWrite(pSvcCtx, pError, src_locp, &amtp, (ub4)1, bufp, 
			(int)sizeof(bufp), (ub1)DCI_ONE_PIECE, 
			(void *)0, 0, 0, SQLCS_IMPLICIT);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAssign_blob_clob enable buffer ,lobwrite failed\n");
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);	
		if (err != test_blob_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAssign_blob_clob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	}

	return ret;
}

sword
TestLobAssign()
{
	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	if (TestLobAssign_clob() == DCI_ERROR)
	{
		printf("TestLobAssign_clob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobAssign_clob SUCCESS\n");

	if (TestLobAssign_blob() == DCI_ERROR)
	{
		printf("TestLobAssign_blob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobAssign_blob SUCCESS\n");

	if (TestLobAssign_clob_blob() == DCI_ERROR)
	{
		printf("TestLobAssign_clob_blob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobAssign_clob_blob SUCCESS\n");

	if (TestLobAssign_blob_clob() == DCI_ERROR)
	{
		printf("TestLobAssign_blob_clob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobAssign_blob_clob SUCCESS\n");

	if (TestLobAssign_clob_enablebuffering() == DCI_ERROR)
	{
		printf("TestLobAssign_clob_enablebuffering FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobAssign_clob_enablebuffering SUCCESS\n");

	if (TestLobAssign_blob_enablebuffering() == DCI_ERROR)
	{
		printf("TestLobAssign_blob_enablebuffering FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobAssign_blob_enablebuffering SUCCESS\n");

	if (TestLobAssign_clob_blob_enablebuffering() == DCI_ERROR)
	{
		printf("TestLobAssign_clob_blob_enablebuffering FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobAssign_clob_blob_enablebuffering SUCCESS\n");

	if (TestLobAssign_blob_clob_enablebuffering() == DCI_ERROR)
	{
		printf("TestLobAssign_blob_clob_enablebuffering FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobAssign_blob_clob_enablebuffering SUCCESS\n");

	err = disconnect(true);

	return DCI_SUCCESS;
}

sword 
TestLobAppend_clob()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_clob_hnd
	{
		DCISvcCtx	*pSvcCtx; 
		DCIError	*pError;
		int			dst_flag;
		int			src_flag;
		sword		ret_val;
	}test_clob_hnd;

	test_clob_hnd clob_array[]=
	{
		{NULL, pError, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, NULL, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 0, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, 0, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, 1, DCI_SUCCESS},
	};

	typedef struct test_clob_suite
	{
		char	*dst_in;
		char	*src_in;
		char	expect_ret[BUFLEN * 2];
		sword	ret_val;
	}test_clob_suite;

	test_clob_suite test_clob[] = 
	{
		{"北京人大金仓", "研发中心工程部", "北京人大金仓研发中心工程部" ,DCI_SUCCESS},
		{"aaa", "bbbbb", "aaabbbbb", DCI_SUCCESS},
		{NULL, "", "", DCI_INVALID_HANDLE},
		{NULL, NULL, "", DCI_INVALID_HANDLE},
	};

	char	*dst_in, *src_in;
	char	dst_out[BUFLEN * 2];
	ub4		amtp_dst = BUFLEN * 2;
	int		i = 0;

	//test error handle
	dst_in = test_clob[0].dst_in; 
	src_in = test_clob[0].src_in;

	//test handle
	for (i = 0;i < (int)sizeof(clob_array) / (int)sizeof(clob_array[0]); i++)
	{
		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_clob(table_name2, src_in);

		if (clob_array[i].dst_flag == 1)
			dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);

		if (clob_array[i].src_flag == 1)
			src_locp = select_table_clob(table_name2, pDefine2, src_locp);

		err = DCILobAppend(clob_array[i].pSvcCtx, clob_array[i].pError, dst_locp, src_locp);
		if (err != clob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		src_locp = NULL;
		dst_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		dst_in = test_clob[i].dst_in; 
		src_in = test_clob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_clob(table_name2, src_in);

		if (dst_in != NULL)
			dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);

		if (src_in != NULL)
			src_locp = select_table_clob(table_name2, pDefine2, src_locp);

		err = DCILobAppend(pSvcCtx, pError, dst_locp, src_locp);
		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_clob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		if (dst_locp != NULL && test_clob[i].ret_val == DCI_SUCCESS)
		{
			amtp_dst = sizeof(dst_out);
			memset(dst_out, 0, amtp_dst);

			err = DCILobRead(pSvcCtx, pError, dst_locp, &amtp_dst, (ub4)1, 
				(void *)dst_out, (ub4)sizeof(dst_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				printf("TestLobAppend_clob: FAILED\n");
				ret = DCI_ERROR;
			}

			if (memcmp(dst_out, test_clob[i].expect_ret, amtp_dst))
			{
				printf("TestLobAssign_clob: FAILED\n");
				ret = DCI_ERROR;
			}
		}
		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	} //end of for

	return ret;
}

sword
TestLobAppend_blob()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_blob_hnd
	{
		DCISvcCtx	*pSvcCtx; 
		DCIError	*pError;
		int			dst_flag;
		int			src_flag;
		sword		ret_val;
	}test_blob_hnd;

	test_blob_hnd blob_array[]=
	{
		{NULL, pError, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, NULL, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 0, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, 0, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, 1, DCI_SUCCESS},
	};

	typedef struct test_blob_suite
	{
		char	*dst_in;
		int		dst_len;
		char	*src_in;
		int		src_len;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_blob_suite;

	test_blob_suite test_blob[] = 
	{
		{"aaa", 3, "bbbbb", 5, "aaabbbbb", DCI_SUCCESS},
		{"123456", 6, "789", 3, "123456789", DCI_SUCCESS},
		{NULL, 0, "", 0, "", DCI_INVALID_HANDLE},
		{NULL, 0, NULL, 0, "", DCI_INVALID_HANDLE},
	};

	char	*dst_in, *src_in;
	int		src_len, dst_len;
	char	dst_out[BUFLEN * 2];
	ub4		amtp_dst = BUFLEN * 2;
	int		i = 0;

	dst_in = test_blob[0].dst_in; 
	src_in = test_blob[0].src_in;
	dst_len = test_blob[0].dst_len;
	src_len = test_blob[0].src_len;

	//test handle
	for (i = 0;i < (int)sizeof(blob_array) / (int)sizeof(blob_array[0]); i++)
	{
		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, dst_in, dst_len);
		insert_table_blob(table_name2, src_in, dst_len);

		if (blob_array[i].dst_flag == 1)
			dst_locp = select_table_blob(table_name1, pDefine1, dst_locp);

		if (blob_array[i].src_flag == 1)
			src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		err = DCILobAppend(blob_array[i].pSvcCtx, blob_array[i].pError, dst_locp, src_locp);
		if (err != blob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		src_locp = NULL;
		dst_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		dst_in = test_blob[i].dst_in;
		src_in = test_blob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, dst_in, test_blob[i].dst_len);
		insert_table_blob(table_name2, src_in, test_blob[i].src_len);

		if (dst_in != NULL)
			dst_locp = select_table_blob(table_name1, pDefine1, dst_locp);

		if (src_in != NULL)
			src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		err = DCILobAppend(pSvcCtx, pError, dst_locp, src_locp);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_blob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		if (dst_locp != NULL && test_blob[i].ret_val == DCI_SUCCESS)
		{
			amtp_dst = sizeof(dst_out);
			memset(dst_out, 0, amtp_dst);

			err = DCILobRead(pSvcCtx, pError, dst_locp, &amtp_dst, (ub4)1, 
				(void *)dst_out, (ub4)sizeof(dst_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				printf("TestLobAppend_blob: FAILED\n");
				ret = DCI_ERROR;
			}

			if (memcmp(dst_out, test_blob[i].expect_ret, amtp_dst))
			{
				printf("TestLobAssign_blob: FAILED\n");
				ret = DCI_ERROR;
			}
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	} //end of for

	return ret;
}


sword 
TestLobAppend_clob_blob()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_clob_blob_hnd
	{
		DCISvcCtx	*pSvcCtx; 
		DCIError	*pError;
		int			dst_flag;
		int			src_flag;
		sword		ret_val;
	}test_clob_blob_hnd;

	test_clob_blob_hnd clob_blob_array[]=
	{
		{NULL, pError, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, NULL, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 0, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, 0, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, 1, DCI_ERROR},
	};

	typedef struct test_clob_blob_suite
	{
		char	*dst_in;
		char	*src_in;
		ub4		src_len;
		char	expect_ret[BUFLEN * 2];
		sword	ret_val;
	}test_clob_blob_suite;

	test_clob_blob_suite test_clob_blob[] = 
	{
		{"北京人大金仓", "123456", 6, "北京人大金仓123456", DCI_ERROR},
		{"aaa", "bbbbb", 5, "aaabbbbb", DCI_ERROR},
		{NULL, "", 0 , "", DCI_INVALID_HANDLE},
		{NULL, NULL, 0, "", DCI_INVALID_HANDLE},
	};

	char	*dst_in, *src_in;
	ub4		src_len;
	int		i = 0;

	//test error handle
	dst_in = test_clob_blob[0].dst_in;
	src_in = test_clob_blob[0].src_in;
	src_len = test_clob_blob[0].src_len;

	//test handle
	for (i = 0;i < (int)sizeof(clob_blob_array) / (int)sizeof(clob_blob_array[0]); i++)
	{
		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_blob(table_name2, src_in, src_len);

		if (clob_blob_array[i].dst_flag == 1)
			dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);

		if (clob_blob_array[i].src_flag == 1)
			src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		err = DCILobAppend(clob_blob_array[i].pSvcCtx, clob_blob_array[i].pError, dst_locp, src_locp);
		if (err != clob_blob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_clob_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		src_locp = NULL;
		dst_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_clob_blob) / (int)sizeof(test_clob_blob[0]); i++)
	{
		dst_in = test_clob_blob[i].dst_in; 
		src_in = test_clob_blob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_blob(table_name2, src_in, src_len);

		if (dst_in != NULL)
			dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);

		if (src_in != NULL)
			src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		err = DCILobAppend(pSvcCtx, pError, dst_locp, src_locp);
		if (err != test_clob_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_clob_blob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	} //end of for

	return ret;
}

sword 
TestLobAppend_blob_clob()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_blob_clob_hnd
	{
		DCISvcCtx	*pSvcCtx;
		DCIError	*pError;
		int			dst_flag;
		int			src_flag;
		sword		ret_val;
	}test_blob_clob_hnd;

	test_blob_clob_hnd blob_clob_array[]=
	{
		{NULL, pError, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, NULL, 1, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 0, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, 0, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, 1, DCI_ERROR},
	};

	typedef struct test_blob_clob_suite
	{
		char	*dst_in;
		ub4		dst_len;
		char	*src_in;
		char	expect_ret[BUFLEN * 2];
		sword	ret_val;
	}test_blob_clob_suite;

	test_blob_clob_suite test_blob_clob[] = 
	{
		{"123456", 6, "北京人大金仓", "123456北京人大金仓", DCI_ERROR},
		{"bbbbb", 5, "aaa", "bbbbbaaa", DCI_ERROR},
		{"", 3, NULL, "", DCI_INVALID_HANDLE},
		{NULL, 0, NULL, "", DCI_INVALID_HANDLE},
	};

	char	*dst_in, *src_in;
	ub4		dst_len;
	int		i = 0;

	//test error handle
	dst_in = test_blob_clob[0].dst_in; 
	src_in = test_blob_clob[0].src_in;
	dst_len = test_blob_clob[0].dst_len;

	//test handle
	for (i = 0;i < (int)sizeof(blob_clob_array) / (int)sizeof(blob_clob_array[0]); i++)
	{
		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_clob(table_name2);

		insert_table_blob(table_name1, dst_in, dst_len);
		insert_table_clob(table_name2, src_in);

		if (blob_clob_array[i].dst_flag == 1)
			dst_locp = select_table_blob(table_name1, pDefine1, dst_locp);

		if (blob_clob_array[i].src_flag == 1)
			src_locp = select_table_clob(table_name2, pDefine2, src_locp);

		err = DCILobAppend(blob_clob_array[i].pSvcCtx, blob_clob_array[i].pError, dst_locp, src_locp);
		if (err != blob_clob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_blob_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		src_locp = NULL;
		dst_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_blob_clob) / (int)sizeof(test_blob_clob[0]); i++)
	{
		dst_in = test_blob_clob[i].dst_in; 
		src_in = test_blob_clob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_clob(table_name2);

		insert_table_blob(table_name1, dst_in, dst_len);
		insert_table_clob(table_name2, src_in);

		if (dst_in != NULL)
			dst_locp = select_table_blob(table_name1, pDefine1, dst_locp);

		if (src_in != NULL)
			src_locp = select_table_clob(table_name2, pDefine2, src_locp);

		err = DCILobAppend(pSvcCtx, pError, dst_locp, src_locp);
		if (err != test_blob_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_blob_clob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	} //end of for

	return ret;
}

sword 
TestLobAppend_clob_enablebuffering()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_clob_suite
	{
		char	*src_in;
		char	*dst_in;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_clob_suite;

	test_clob_suite test_clob[] = 
	{
		{"北京人大金仓", "研发中心工程部", "北京人大金仓研发中心工程部", DCI_ERROR},
		{"aaa", "bbbbb", "aaabbbbb", DCI_ERROR},
	};

	char	*dst_in, *src_in;
	int		i = 0;

	//参数 src_locp 启用缓存子系统

	for (i = 0; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		src_in = test_clob[i].src_in;
		dst_in = test_clob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_clob(table_name2, dst_in);

		src_locp = select_table_clob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAppend_clob enable buffer failed(enable buffer and don't write)\n");
		}

		err = DCILobAppend(pSvcCtx, pError, dst_locp, src_locp);
		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_clob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 dst_locp 启用缓存子系统
	for (i = 0 ; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		src_in = test_clob[i].src_in;
		dst_in = test_clob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_clob(table_name2, dst_in);

		src_locp = select_table_clob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAppend_clob enable buffer failed\n");
		}

		err = DCILobAppend(pSvcCtx, pError, dst_locp, dst_locp);
		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_clob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 src_locp 和 dst_locp 启用缓存子系统
	for (i = 0 ; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		src_in = test_clob[i].src_in;
		dst_in = test_clob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_clob(table_name2, dst_in);

		src_locp = select_table_clob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAppend_clob enable buffer failed\n");
		}

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAppend_clob enable buffer failed\n");
		}

		err = DCILobAppend(pSvcCtx, pError, dst_locp, dst_locp);
		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_clob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}
		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	}

	return ret;
}

sword 
TestLobAppend_blob_enablebuffering()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_blob_suite
	{
		char	*src_in;
		char	*dst_in;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_blob_suite;

	test_blob_suite test_blob[] = 
	{
		{"123", "456", "123456", DCI_ERROR},
		{"aaa", "bbbbb", "aaabbbbb", DCI_ERROR},
	};

	char	*dst_in, *src_in;
	int		i = 0;

	//参数 src_locp 启用缓存子系统

	for (i = 0; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		src_in = test_blob[i].src_in;
		dst_in = test_blob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, src_in, (int)strlen(src_in));
		insert_table_blob(table_name2, dst_in, (int)strlen(dst_in));

		src_locp = select_table_blob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAppend_blob enable buffer failed(enable buffer and don't write)\n");
		}

		err = DCILobAppend(pSvcCtx, pError, dst_locp, src_locp);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 dst_locp 启用缓存子系统
	for (i = 0 ; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		src_in = test_blob[i].src_in;
		dst_in = test_blob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, src_in, (int)strlen(src_in));
		insert_table_blob(table_name2, dst_in, (int)strlen(dst_in));

		src_locp = select_table_blob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAppend_blob enable buffer failed\n");
		}

		err = DCILobAppend(pSvcCtx, pError, dst_locp, dst_locp);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 src_locp 和 dst_locp 启用缓存子系统
	for (i = 0 ; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		src_in = test_blob[i].src_in;
		dst_in = test_blob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, src_in, (int)strlen(src_in));
		insert_table_blob(table_name2, dst_in, (int)strlen(dst_in));

		src_locp = select_table_blob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAppend_blob enable buffer failed\n");
		}

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAppend_blob enable buffer failed\n");
		}

		err = DCILobAppend(pSvcCtx, pError, dst_locp, dst_locp);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}
		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	}

	return ret;
}

sword 
TestLobAppend_clob_blob_enablebuffering()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_clob_blob_suite
	{
		char	*src_in;
		char	*dst_in;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_clob_blob_suite;

	test_clob_blob_suite test_clob_blob[] = 
	{
		{"北京人大金仓", "研发中心工程部", "北京人大金仓研发中心工程部", DCI_ERROR},
		{"aaa", "bbbbb", "aaabbbbb", DCI_ERROR},
	};

	char	*dst_in, *src_in;
	int		i = 0;

	//参数 src_locp 启用缓存子系统

	for (i = 0; i < (int)sizeof(test_clob_blob) / (int)sizeof(test_clob_blob[0]); i++)
	{
		src_in = test_clob_blob[i].src_in;
		dst_in = test_clob_blob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_blob(table_name2, dst_in, (int)strlen(dst_in));

		src_locp = select_table_clob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAppend_clob_blob enable buffer failed(enable buffer and don't write)\n");
		}

		err = DCILobAppend(pSvcCtx, pError, dst_locp, src_locp);
		if (err != test_clob_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_clob_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 dst_locp 启用缓存子系统
	for (i = 0 ; i < (int)sizeof(test_clob_blob) / (int)sizeof(test_clob_blob[0]); i++)
	{
		src_in = test_clob_blob[i].src_in;
		dst_in = test_clob_blob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_blob(table_name2, dst_in, (int)strlen(dst_in));

		src_locp = select_table_clob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAppend_clob_blob enable buffer failed\n");
		}

		err = DCILobAppend(pSvcCtx, pError, dst_locp, dst_locp);
		if (err != test_clob_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_clob_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 src_locp 和 dst_locp 启用缓存子系统
	for (i = 0 ; i < (int)sizeof(test_clob_blob) / (int)sizeof(test_clob_blob[0]); i++)
	{
		src_in = test_clob_blob[i].src_in;
		dst_in = test_clob_blob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, src_in);
		insert_table_blob(table_name2, dst_in, (int)strlen(dst_in));

		src_locp = select_table_clob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_blob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAppend_clob_blob enable buffer failed\n");
		}

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAppend_clob_blob enable buffer failed\n");
		}

		err = DCILobAppend(pSvcCtx, pError, dst_locp, dst_locp);
		if (err != test_clob_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_clob_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}
		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	}

	return ret;
}

sword 
TestLobAppend_blob_clob_enablebuffering()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	sword		ret = DCI_SUCCESS;

	typedef struct test_blob_clob_suite
	{
		char	*src_in;
		char	*dst_in;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_blob_clob_suite;

	test_blob_clob_suite test_blob_clob[] = 
	{
		{"aaa", "bbbbb", "aaabbbbb", DCI_ERROR},
		{"北京人大金仓", "研发中心工程部", "北京人大金仓研发中心工程部", DCI_ERROR},
	};

	char	*dst_in, *src_in;
	int		i = 0;

	//参数 src_locp 启用缓存子系统

	for (i = 0; i < (int)sizeof(test_blob_clob) / (int)sizeof(test_blob_clob[0]); i++)
	{
		src_in = test_blob_clob[i].src_in;
		dst_in = test_blob_clob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_clob(table_name2);

		insert_table_blob(table_name1, src_in, (int)strlen(src_in));
		insert_table_clob(table_name2, dst_in);

		src_locp = select_table_blob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAppend_blob_clob enable buffer failed(enable buffer and don't write)\n");
		}

		err = DCILobAppend(pSvcCtx, pError, dst_locp, src_locp);
		if (err != test_blob_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_blob_clob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 dst_locp 启用缓存子系统
	for (i = 0 ; i < (int)sizeof(test_blob_clob) / (int)sizeof(test_blob_clob[0]); i++)
	{
		src_in = test_blob_clob[i].src_in;
		dst_in = test_blob_clob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_clob(table_name2);

		insert_table_blob(table_name1, src_in, (int)strlen(src_in));
		insert_table_clob(table_name2, dst_in);

		src_locp = select_table_blob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAppend_blob_clob enable buffer failed\n");
		}

		err = DCILobAppend(pSvcCtx, pError, dst_locp, dst_locp);
		if (err != test_blob_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_blob_clob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	}

	//参数 src_locp 和 dst_locp 启用缓存子系统
	for (i = 0 ; i < (int)sizeof(test_blob_clob) / (int)sizeof(test_blob_clob[0]); i++)
	{
		src_in = test_blob_clob[i].src_in;
		dst_in = test_blob_clob[i].dst_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_clob(table_name2);

		insert_table_blob(table_name1, src_in, (int)strlen(src_in));
		insert_table_clob(table_name2, dst_in);

		src_locp = select_table_blob(table_name1, pDefine1, src_locp);
		dst_locp = select_table_clob(table_name2, pDefine2, dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAppend_blob_clob enable buffer failed\n");
		}

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("TestLobAppend_blob_clob enable buffer failed\n");
		}

		err = DCILobAppend(pSvcCtx, pError, dst_locp, dst_locp);
		if (err != test_blob_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobAppend_blob_clob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}
		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	}

	return ret;
}

sword
TestLobAppend()
{
	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	if (TestLobAppend_clob() == DCI_ERROR)
	{
		printf("TestLobAppend_clob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobAppend_clob SUCCESS\n");

	if (TestLobAppend_blob() == DCI_ERROR)
	{
		printf("TestLobAppend_blob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobAppend_blob SUCCESS\n");

	if (TestLobAppend_clob_blob() == DCI_ERROR)
	{
		printf("TestLobAppend_clob_blob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobAppend_clob_blob SUCCESS\n");

	if (TestLobAppend_blob_clob() == DCI_ERROR)
	{
		printf("TestLobAppend_blob_clob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobAppend_blob_clob SUCCESS\n");

	if (TestLobAppend_clob_enablebuffering() == DCI_ERROR)
	{
		printf("TestLobAppend_clob_enablebuffering FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobAppend_clob_enablebuffering SUCCESS\n");

	if (TestLobAppend_blob_enablebuffering() == DCI_ERROR)
	{
		printf("TestLobAppend_blob_enablebuffering FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobAppend_blob_enablebuffering SUCCESS\n");

	if (TestLobAppend_clob_blob_enablebuffering() == DCI_ERROR)
	{
		printf("TestLobAppend_clob_blob_enablebuffering FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobAppend_clob_blob_enablebuffering SUCCESS\n");

	if (TestLobAppend_blob_clob_enablebuffering() == DCI_ERROR)
	{
		printf("TestLobAppend_blob_clob_enablebuffering FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobAppend_blob_clob_enablebuffering SUCCESS\n");

	err = disconnect(true);

	return DCI_SUCCESS;
}

sword 
TestLobTrim_clob()
{
	DCILobLocator	*clob_locp = NULL;
	DCIDefine		*pDefine = NULL;
	sword			ret = DCI_SUCCESS;

	typedef struct test_clob_hnd
	{
		DCISvcCtx	*pSvcCtx; 
		DCIError	*pError;
		sword		clob_flag;
		sword		ret_val;
	}test_clob_hnd;

	test_clob_hnd clob_array[]=
	{
		{NULL, pError, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, NULL, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 0, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, DCI_SUCCESS},
	};

	typedef struct test_clob_suite
	{
		char	*str_in;
		ub4		new_len;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_clob_suite;

	test_clob_suite test_clob[] = 
	{
		//参数 newlen 等于 locp 表示的 LOB 对象的长度
		{"北京人大金仓信息技术股份有限公司", 32, "北京人大金仓信息技术股份有限公司", DCI_SUCCESS},
		//参数 newlen 大于 locp 表示的 LOB 对象的长度
		{"北京人大金仓信息技术股份有限公司", 80, "北京人大金仓信息技术股份有限公司", DCI_ERROR},
		//参数 newlen 小于 locp 表示的 LOB 对象的长度
		{"北京人大金仓信息技术股份有限公司", 20, "北京人大金仓信息技术", DCI_SUCCESS},

		{"aaaaaa", 6, "aaaaaa", DCI_SUCCESS},
		{"aaaaaa", 8, "aaaaaa", DCI_ERROR},
		{"aaaaaa", 4, "aaaa", DCI_SUCCESS},

		{"北京人大金仓信息aaaaaa技术股份有限公司", 38, "北京人大金仓信息aaaaaa技术股份有限公司", DCI_SUCCESS},
		{"北京人大金仓信息aaaaaa技术股份有限公司", 80, "北京人大金仓信息aaaaaa技术股份有限公司", DCI_ERROR},
		{"北京人大金仓信息aaaaaa技术股份有限公司", 34, "北京人大金仓信息aaaaaa技术股份有限", DCI_SUCCESS},

		{"123456", 6, "123456", DCI_SUCCESS},
		{"123456", 8, "123456", DCI_ERROR},
		{"123456", 4, "12345", DCI_SUCCESS},

		{"", 0, "", DCI_SUCCESS},
	};

	char	str_out[BUFLEN];
	ub4		amtp = BUFLEN;
	char	*str_in;
	int		new_len;

	str_in = test_clob[0].str_in; 
	new_len = test_clob[0].new_len;

	//test handle
	int		i = 0;
	for (i = 0;i < (int)sizeof(clob_array) / (int)sizeof(clob_array[0]); i++)
	{
		drop_table(table_name1);
		create_table_clob(table_name1);
		insert_table_clob(table_name1, str_in);

		if (clob_array[i].clob_flag == 1)
			clob_locp = select_table_clob(table_name1, pDefine, clob_locp);

		err = DCILobTrim(clob_array[i].pSvcCtx, clob_array[i].pError, clob_locp, new_len);
		if (err != clob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobTrim_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(clob_locp, DCI_DTYPE_LOB);
		clob_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		str_in = test_clob[i].str_in;

		drop_table(table_name1);
		create_table_clob(table_name1);
		insert_table_clob(table_name1, str_in);

		if (str_in != NULL)
			clob_locp = select_table_clob(table_name1, pDefine, clob_locp);

		err = DCILobTrim(pSvcCtx, pError, clob_locp, test_clob[i].new_len);
		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobTrim_clob: FAILED\n");
			DCIDescriptorFree(clob_locp, DCI_DTYPE_LOB);
			ret = DCI_ERROR;
			continue;
		}

		if (clob_locp != NULL && test_clob[i].ret_val == DCI_SUCCESS)
		{
			amtp = sizeof(str_out);
			memset(str_out, 0, amtp);

			err = DCILobRead(pSvcCtx, pError, clob_locp, &amtp, (ub4)1, 
				(void *)str_out, (ub4)sizeof(str_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				printf("TestLobTrim_clob: FAILED\n");
				ret = DCI_ERROR;
			}

			if (memcmp(str_out, test_clob[i].expect_ret, amtp))
			{
				printf("TestLobAssign_blob: FAILED\n");
				ret = DCI_ERROR;
			}
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(clob_locp, DCI_DTYPE_LOB);
		clob_locp = NULL;
	} //end of for

	return ret;
}

sword
TestLobTrim_blob()
{
	DCILobLocator	*blob_locp = NULL;
	DCIDefine		*pDefine = NULL;
	sword			ret = DCI_SUCCESS;

	typedef struct test_blob_hnd
	{
		DCISvcCtx	*pSvcCtx; 
		DCIError	*pError;
		sword		blob_flag;
		sword		ret_val;
	}test_blob_hnd;

	typedef struct test_blob_suite
	{
		char	*str_in;
		ub4		new_len;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_blob_suite;

	test_blob_suite test_blob[] = 
	{
		{"abcdef", 6, "abcdef", DCI_SUCCESS},	//参数 newlen 等于 locp 表示的 LOB 对象的长度
		{"abcdef", 8, "abcdef", DCI_ERROR},		//参数 newlen 大于 locp 表示的 LOB 对象的长度
		{"abcdef", 4, "abcd", DCI_SUCCESS},		//参数 newlen 小于 locp 表示的 LOB 对象的长度

		{"123456", 6, "123456", DCI_SUCCESS},
		{"123456", 8, "123456", DCI_ERROR},
		{"123456", 4, "1234", DCI_SUCCESS},

		{"123abcdef456", 12, "123abcdef456", DCI_SUCCESS},
		{"123abcdef456", 20, "123abcdef456", DCI_ERROR},
		{"123abcdef456", 10, "123abcdef4", DCI_SUCCESS},

		{"\x30\x31\x32\x41\x42\x43", 6, "\x30\x31\x32\x41\x42\x43", DCI_SUCCESS},
		{"\x30\x31\x32\x41\x42\x43", 8, "\x30\x31\x32\x41\x42\x43", DCI_ERROR},
		{"\x30\x31\x32\x41\x42\x43", 4, "\x30\x31\x32\x41", DCI_SUCCESS},

		{"\60\61\62\101\102\103", 6, "\60\61\62\101\102\103", DCI_SUCCESS},
		{"\60\61\62\101\102\103", 8, "\60\61\62\101\102\103", DCI_ERROR},
		{"\60\61\62\101\102\103", 4, "\60\61\62\101", DCI_SUCCESS},

		{"\x30\x31\x32\101\102\103", 6, "\x30\x31\x32\101\102\103", DCI_SUCCESS},
		{"\x30\x31\x32\101\102\103", 8, "\x30\x31\x32\101\102\103", DCI_ERROR},
		{"\x30\x31\x32\101\102\103", 4, "\x30\x31\x32\101", DCI_SUCCESS},

		{"", 0, "", DCI_SUCCESS},
	};

	char	str_out[BUFLEN];
	ub4		amtp = BUFLEN;
	char	*str_in;
	int		len;
	int		new_len;

	test_blob_hnd blob_array[]=
	{
		{NULL, pError, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, NULL, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 0, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, DCI_SUCCESS},
	};

	str_in = test_blob[0].str_in; 
	new_len = test_blob[0].new_len;
	len = (int)strlen(test_blob[0].str_in);

	//test handle
	int	i = 0;
	for (i = 0;i < (int)sizeof(blob_array) / (int)sizeof(blob_array[0]); i++)
	{
		drop_table(table_name1);
		create_table_blob(table_name1);
		insert_table_blob(table_name1, str_in, len);

		if (blob_array[i].blob_flag == 1)
			blob_locp = select_table_blob(table_name1, pDefine, blob_locp);

		err = DCILobTrim(blob_array[i].pSvcCtx, blob_array[i].pError, blob_locp, new_len);
		if (err != blob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobTrim_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(blob_locp, DCI_DTYPE_LOB);
		blob_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		str_in = test_blob[i].str_in;

		drop_table(table_name1);
		create_table_blob(table_name1);
		insert_table_blob(table_name1, str_in, (int)strlen(test_blob[i].str_in));

		if (str_in != NULL)
			blob_locp = select_table_blob(table_name1, pDefine, blob_locp);

		err = DCILobTrim(pSvcCtx, pError, blob_locp, test_blob[i].new_len);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobTrim_blob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		if (blob_locp != NULL && test_blob[i].ret_val == DCI_SUCCESS)
		{
			amtp = sizeof(str_out);
			memset(str_out, 0, amtp);

			err = DCILobRead(pSvcCtx, pError, blob_locp, &amtp, (ub4)1, 
				(void *)str_out, (ub4)sizeof(str_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				printf("TestLobTrim_blob: FAILED\n");
				ret = DCI_ERROR;
			}

			if (memcmp(str_out, test_blob[i].expect_ret, amtp))
			{
				printf("TestLobAssign_blob: FAILED\n");
				ret = DCI_ERROR;
			}
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(blob_locp, DCI_DTYPE_LOB);
		blob_locp = NULL;
	} //end of for

	return ret;
}

sword 
TestLobTrim()
{
	sword ret = DCI_SUCCESS;
	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

#if 0
	if (TestLobTrim_clob() == DCI_ERROR)
	{
		printf("TestLobTrim_clob FAILED\n");
		ret = DCI_ERROR;
	}
	else
		printf("TestLobTrim_clob SUCCESS\n");
#endif

	if (TestLobTrim_blob() == DCI_ERROR)
	{
		printf("TestLobTrim_blob FAILED\n");
		ret = DCI_ERROR;
	}
	else
		printf("TestLobTrim_blob SUCCESS\n");

	err = disconnect(true);

	return ret;
}

sword 
TestLobErase_clob()
{
	DCILobLocator	*clob_locp = NULL;
	DCIDefine		*pDefine = NULL;
	sword			ret = DCI_SUCCESS;

	typedef struct test_clob_hnd
	{
		DCISvcCtx	*pSvcCtx; 
		DCIError	*pError;
		sword		clob_flag;
		sword		ret_val;
	}test_clob_hnd;

	test_clob_hnd clob_array[]=
	{
		{NULL, pError, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, NULL, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 0, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, DCI_SUCCESS},
	};

	typedef struct test_clob_suite
	{
		char	*str_in;
		ub4		amount;
		ub4		offset;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_clob_suite;

	test_clob_suite test_clob[] = 
	{
		{"aaaaaa", 2, 2, "a  aaa", DCI_SUCCESS},		//参数 amount 小于 locp 表示的 LOB 对象的长度值
		//参数 offset 小于 locp 表示的 LOB 对象的长度值

		{"aaaaaa", 6, 2, "a     ", DCI_SUCCESS},		//参数 amount 等于 locp 表示的 LOB 对象的长度值
		{"aaaaaa", 8, 2, "a     ", DCI_SUCCESS},		//参数 amount 大于 locp 表示的 LOB 对象的长度值
		{"aaaaaa", 0, 2, "aaaaaa", DCI_SUCCESS},		//参数 amount 为 0

		{"aaaaaa", 2, 6, "aaaaa ", DCI_SUCCESS},		//参数 offset 等于 locp 表示的 LOB 对象的长度值
		{"aaaaaa", 2, 8, "aaaaaa", DCI_SUCCESS},		//参数 offset 大于 locp 表示的 LOB 对象的长度值
		{"aaaaaa", 2, 0, "aaaaaa", DCI_ERROR},			//参数 offset 为 0

		{"研发中心工程", 4,  3, "研    心工程", DCI_SUCCESS},
		{"研发中心工程", 12, 3, "研          ", DCI_SUCCESS},
		{"研发中心工程", 16,  3,"研          ", DCI_SUCCESS},
		{"研发中心工程", 0,  3, "研发中心工程", DCI_SUCCESS},
		{"研发中心工程", 2,  11,"研发中心工  ", DCI_SUCCESS},
		{"研发中心工程", 2,  15,"研发中心工程", DCI_SUCCESS},
		{"研发中心工程", 2,  0, "研发中心工程", DCI_ERROR},

		{"研发中aaaaaa心工程", 8,  9, "研发中aa        程", DCI_SUCCESS},
		{"研发中aaaaaa心工程", 20, 9, "研发中aa          ", DCI_SUCCESS},
		{"研发中aaaaaa心工程", 12, 9, "研发中aa          ", DCI_SUCCESS},
		{"研发中aaaaaa心工程", 0,  9, "研发中aaaaaa心工程", DCI_SUCCESS},
		{"研发中aaaaaa心工程", 6, 20, "研发中aaaaaa心工程", DCI_SUCCESS},
		{"研发中aaaaaa心工程", 6, 17, "研发中aaaaaa心工  ", DCI_SUCCESS},
		{"研发中aaaaaa心工程", 6, 0,  "研发中aaaaaa心工程", DCI_ERROR},

		{"", 0, 2, "", DCI_ERROR},
	};

	char	str_out[BUFLEN];
	ub4		amtp = BUFLEN;
	char	*str_in;
	ub4		amount;
	ub4		offset;

	str_in = test_clob[0].str_in; 
	amount = test_clob[0].amount;
	offset = test_clob[0].offset;

	//test handle
	int		i = 0;
	for (i = 0;i < (int)sizeof(clob_array) / (int)sizeof(clob_array[0]); i++)
	{
		drop_table(table_name1);
		create_table_clob(table_name1);
		insert_table_clob(table_name1, str_in);

		if (clob_array[i].clob_flag == 1)
			clob_locp = select_table_clob(table_name1, pDefine, clob_locp);

		err = DCILobErase(clob_array[i].pSvcCtx, clob_array[i].pError, clob_locp, &amount, offset);
		if (err != clob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobErase_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(clob_locp, DCI_DTYPE_LOB);
		clob_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		str_in = test_clob[i].str_in;

		drop_table(table_name1);
		create_table_clob(table_name1);
		insert_table_clob(table_name1, str_in);

		if (str_in != NULL)
			clob_locp = select_table_clob(table_name1, pDefine, clob_locp);

		err = DCILobErase(pSvcCtx, pError, clob_locp, &test_clob[i].amount, test_clob[i].offset);
		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobErase_clob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		if (clob_locp != NULL && test_clob[i].ret_val == DCI_SUCCESS)
		{
			amtp = sizeof(str_out);
			memset(str_out, 0, amtp);

			err = DCILobRead(pSvcCtx, pError, clob_locp, &amtp, (ub4)1, 
				(void *)str_out, (ub4)sizeof(str_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				printf("TestLobErase_clob: FAILED\n");
				ret = DCI_ERROR;
			}

			if (memcmp(str_out, test_clob[i].expect_ret, amtp))
			{
				ret = DCI_ERROR;
			}
		}
		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(clob_locp, DCI_DTYPE_LOB);
		clob_locp = NULL;
	} //end of for

	return ret;
}

sword 
TestLobErase_blob()
{
	DCILobLocator	*blob_locp = NULL;
	DCIDefine		*pDefine = NULL;
	sword			ret = DCI_SUCCESS;

	typedef struct test_blob_hnd
	{
		DCISvcCtx	*pSvcCtx; 
		DCIError	*pError;
		sword		blob_flag;
		sword		ret_val;
	}test_blob_hnd;

	test_blob_hnd blob_array[]=
	{
		{NULL, pError, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, NULL, 1, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 0, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, 1, DCI_SUCCESS},
	};

	typedef struct test_blob_suite
	{
		char	*str_in;
		ub4		str_len;
		ub4		amount;
		ub4		offset;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_blob_suite;

	test_blob_suite test_blob[] = 
	{
		{"aaaaaa", 6, 2, 2, "a\0\0aaa", DCI_SUCCESS},		//参数 amount 小于 locp 表示的 LOB 对象的长度值
		//参数 offset 小于 locp 表示的 LOB 对象的长度值

		{"aaaaaa", 6, 6, 2, "a\0\0\0\0\0", DCI_SUCCESS},	//参数 amount 等于 locp 表示的 LOB 对象的长度值
		{"aaaaaa", 6, 8, 2, "a\0\0\0\0\0", DCI_SUCCESS},	//参数 amount 大于 locp 表示的 LOB 对象的长度值
		{"aaaaaa", 6, 0, 2, "aaaaaa", DCI_SUCCESS},			//参数 amount 为 0

		{"aaaaaa", 6, 2, 6, "aaaaa\0", DCI_SUCCESS},		//参数 offset 等于 locp 表示的 LOB 对象的长度值
		{"aaaaaa", 6, 2, 8, "aaaaaa", DCI_SUCCESS},			//参数 offset 大于 locp 表示的 LOB 对象的长度值
		{"aaaaaa", 6, 2, 0, "aaaaaa", DCI_ERROR},			//参数 offset 为 0

		{"\x01\x02\x03\x04\x05\x06", 6, 2, 2, "\x01\x00\x00\x04\x05\x06", DCI_SUCCESS},
		{"\x01\x02\x03\x04\x05\x06", 6, 6, 2, "\x01\0\0\0\0\0", DCI_SUCCESS},
		{"\x01\x02\x03\x04\x05\x06", 6, 8, 2, "\x01\0\0\0\0\0", DCI_SUCCESS},
		{"\x01\x02\x03\x04\x05\x06", 6, 0, 2, "\x01\x02\x03\x04\x05\x06", DCI_SUCCESS},
		{"\x01\x02\x03\x04\x05\x06", 6, 2, 6, "\x01\x02\x03\x04\x05\0", DCI_SUCCESS},
		{"\x01\x02\x03\x04\x05\x06", 6, 2, 8, "\x01\x02\x03\x04\x05\x06", DCI_SUCCESS},
		{"\x01\x02\x03\x04\x05\x06", 6, 2, 0, "\x01\x02\x03\x04\x05\x06", DCI_ERROR},

		{"", 0, 0, 2, "", DCI_ERROR},
	};

	char	str_out[BUFLEN];
	ub4		amtp = BUFLEN;
	char	*str_in;
	int		len;
	ub4		amount;
	ub4		offset;

	str_in = test_blob[0].str_in;
	len = (int)sizeof(test_blob[0].str_in);
	amount = test_blob[0].amount;
	offset = test_blob[0].offset;

	//test handle
	int		i = 0;
	for (i = 0; i < (int)sizeof(blob_array) / (int)sizeof(blob_array[0]); i++)
	{
		drop_table(table_name1);
		create_table_blob(table_name1);
		insert_table_blob(table_name1, str_in, len);

		if (blob_array[i].blob_flag == 1)
			blob_locp = select_table_blob(table_name1, pDefine, blob_locp);

		err = DCILobErase(blob_array[i].pSvcCtx, blob_array[i].pError, blob_locp, &amount, offset);
		if (err != blob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobErase_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(blob_locp, DCI_DTYPE_LOB);
		blob_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		str_in = test_blob[i].str_in;

		drop_table(table_name1);
		create_table_blob(table_name1);
		insert_table_blob(table_name1, str_in, test_blob[i].str_len);

		if (str_in != NULL)
			blob_locp = select_table_blob(table_name1, pDefine, blob_locp);

		err = DCILobErase(pSvcCtx, pError, blob_locp, &test_blob[i].amount, test_blob[i].offset);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobErase_blob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		if (blob_locp != NULL && test_blob[i].ret_val == DCI_SUCCESS)
		{
			amtp = sizeof(str_out);
			memset(str_out, 0, amtp);

			err = DCILobRead(pSvcCtx, pError, blob_locp, &amtp, (ub4)1, 
				(void *)str_out, (ub4)sizeof(str_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				printf("TestLobErase_blob: FAILED\n");
				ret = DCI_ERROR;
			}

			if (memcmp(str_out, test_blob[i].expect_ret, amtp))
			{
				ret = DCI_ERROR;
			}
		}
		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(blob_locp, DCI_DTYPE_LOB);
		blob_locp = NULL;
	} //end of for

	return ret;
}

sword 
TestLobErase_clob_enablebuffering()
{
	DCILobLocator	*clob_locp = NULL;
	DCIDefine		*pDefine = NULL;
	sword			ret = DCI_SUCCESS;

	typedef struct test_clob_suite
	{
		char	*str_in;
		ub4		amount;
		ub4		offset;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_clob_suite;

	test_clob_suite test_clob[] = 
	{
		{"aaaaaa", 2, 2, "a  aaa", DCI_ERROR},		//参数 amount 小于 locp 表示的 LOB 对象的长度值
		//参数 offset 小于 locp 表示的 LOB 对象的长度值

		{"aaaaaa", 6, 2, "a     ", DCI_ERROR},		//参数 amount 等于 locp 表示的 LOB 对象的长度值
		{"aaaaaa", 8, 2, "a     ", DCI_ERROR},		//参数 amount 大于 locp 表示的 LOB 对象的长度值
		{"aaaaaa", 0, 2, "aaaaaa", DCI_ERROR},		//参数 amount 为 0

		{"aaaaaa", 2, 6, "aaaaa ", DCI_ERROR},		//参数 offset 等于 locp 表示的 LOB 对象的长度值
		{"aaaaaa", 2, 8, "aaaaaa", DCI_ERROR},		//参数 offset 大于 locp 表示的 LOB 对象的长度值
		{"aaaaaa", 2, 0, "aaaaaa", DCI_ERROR},		//参数 offset 为 0

		{"研发中心工程", 4, 3, "研    心工程", DCI_ERROR},
		{"研发中心工程",12, 3, "研          ", DCI_ERROR},
		{"研发中心工程",16, 3, "研          ", DCI_ERROR},
		{"研发中心工程", 0, 3, "研发中心工程", DCI_ERROR},
		{"研发中心工程", 4,11, "研发中心工  ", DCI_ERROR},
		{"研发中心工程", 4,15, "研发中心工程", DCI_ERROR},
		{"研发中心工程", 4, 0, "研发中心工程", DCI_ERROR},
	};

	char	*str_in;
	int		i = 0;

	//参数 locp 启用缓存子系统
	for (i = 0; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		str_in = test_clob[i].str_in;

		drop_table(table_name1);
		create_table_clob(table_name1);
		insert_table_clob(table_name1, str_in);

		clob_locp = select_table_clob(table_name1, pDefine, clob_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, clob_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobErase enable buffering failed\n");
		}
		err = DCILobErase(pSvcCtx, pError, clob_locp, &test_clob[i].amount, test_clob[i].offset);
		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobErase_clob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(clob_locp, DCI_DTYPE_LOB);
		clob_locp = NULL;
	} //end of for

	return ret;
}

sword 
TestLobErase_blob_enablebuffering()
{
	DCILobLocator	*blob_locp = NULL;
	DCIDefine		*pDefine = NULL;
	sword			ret = DCI_SUCCESS;

	typedef struct test_blob_suite
	{
		char	*str_in;
		ub4		amount;
		ub4		offset;
		char	expect_ret[BUFLEN];
		sword	ret_val;
	}test_blob_suite;

	test_blob_suite test_blob[] = 
	{
		{"aaaaaa", 2, 2, "a  aaa", DCI_ERROR},		//参数 amount 小于 locp 表示的 LOB 对象的长度值
		//参数 offset 小于 locp 表示的 LOB 对象的长度值

		{"aaaaaa", 6, 2, "a     ", DCI_ERROR},		//参数 amount 等于 locp 表示的 LOB 对象的长度值
		{"aaaaaa", 8, 2, "a     ", DCI_ERROR},		//参数 amount 大于 locp 表示的 LOB 对象的长度值
		{"aaaaaa", 0, 2, "aaaaaa", DCI_ERROR},		//参数 amount 为 0

		{"aaaaaa", 2, 6, "aaaaa ", DCI_ERROR},		//参数 offset 等于 locp 表示的 LOB 对象的长度值
		{"aaaaaa", 2, 8, "aaaaaa", DCI_ERROR},		//参数 offset 大于 locp 表示的 LOB 对象的长度值
		{"aaaaaa", 2, 0, "aaaaaa", DCI_ERROR},		//参数 offset 为 0

	};

	char	*str_in;
	int		i = 0;

	//参数 locp 启用缓存子系统
	for (i = 0; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		str_in = test_blob[i].str_in;

		drop_table(table_name1);
		create_table_blob(table_name1);
		insert_table_blob(table_name1, str_in, (int)strlen(str_in));

		blob_locp = select_table_blob(table_name1, pDefine, blob_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, blob_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobErase enable buffering failed\n");
		}
		err = DCILobErase(pSvcCtx, pError, blob_locp, &test_blob[i].amount, test_blob[i].offset);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobErase_blob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(blob_locp, DCI_DTYPE_LOB);
		blob_locp = NULL;
	} //end of for

	return ret;
}

sword 
TestLobErase()
{
	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}
#if 0
	if (TestLobErase_clob() == DCI_ERROR)
	{
		printf("TestLobErase_clob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobErase_clob SUCCESS\n");
#endif

	if (TestLobErase_blob() == DCI_ERROR)
	{
		printf("TestLobErase_blob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobErase_blob SUCCESS\n");

	if (TestLobErase_clob_enablebuffering() == DCI_ERROR)
	{
		printf("TestLobErase_clob_enablebuffering FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobErase_clob_enablebuffering SUCCESS\n");

	if (TestLobErase_blob_enablebuffering() == DCI_ERROR)
	{
		printf("TestLobErase_blob_enablebuffering FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobErase_blob_enablebuffering SUCCESS\n");

	err = disconnect(true);

	return DCI_SUCCESS;
}

sword 
TestLobEnableBuffering_clob()
{
	DCILobLocator	*clob_locp = NULL;
	DCIDefine		*pDefine = NULL;
	sword			ret = DCI_SUCCESS;

	typedef struct test_clob_hnd
	{
		DCISvcCtx	*pSvcCtx; 
		DCIError	*pError;
		sword		ret_val;
	}test_clob_hnd;

	test_clob_hnd clob_array[]=
	{
		{NULL, pError, DCI_SUCCESS},
		{pSvcCtx, NULL, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, DCI_SUCCESS},
	};

	//针对DCILobLocator *locp 设计一下几种测试用例：
	//1、locp为空
	//2、locp不为空，申请locp之后并没有把它与lob关联
	//3、locp不为空，并且使用DCILobDefine使它与某个lob关联,但lob为空（仅仅数据类型为internal lob的类型）
	//4、locp不为空，并且使用DCILobDefine使它与某个lob关联,lob不为空
	//调用DCILobEnableBuffering之后，以上测试用例的结果如下（下述结果针对Oracle, DCI 控制了程序崩溃的情况）：
	//1、程序崩溃
	//2、DCILobEnableBuffering返回-2(DCI_INVALID_HANDLE)
	//3、程序崩溃
	//4、DCILobEnablebuffering返回DCI_SUCCESS
	typedef struct test_clob_suite
	{
		char	*str_in;
		sword	ret_val;
	}test_clob_suite;

	test_clob_suite test_clob[] = 
	{
		{"北京人大金仓信息技术股份有限公司", DCI_SUCCESS},
	};

	char	*str_in;
	int		len;

	str_in = test_clob[0].str_in; 
	len = (int)strlen(test_clob[0].str_in);

	//test handle
	int		i = 0;
	for (i = 0;i < (int)sizeof(clob_array) / (int)sizeof(clob_array[0]); i++)
	{
		drop_table(table_name1);
		create_table_clob(table_name1);
		insert_table_clob(table_name1, str_in);

		clob_locp = select_table_clob(table_name1, pDefine, clob_locp);

		err = DCILobEnableBuffering(clob_array[i].pSvcCtx, clob_array[i].pError, clob_locp);
		if (err != clob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobEnableBuffering_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(clob_locp, DCI_DTYPE_LOB);
		clob_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		str_in = test_clob[i].str_in;

		drop_table(table_name1);
		create_table_clob(table_name1);
		insert_table_clob(table_name1, str_in);

		clob_locp = select_table_clob(table_name1, pDefine, clob_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, clob_locp);
		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobEnableBuffering_clob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCILobDisableBuffering(pSvcCtx, pError, clob_locp);
		err = DCILobEnableBuffering(pSvcCtx, pError, clob_locp);

		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobEnableBuffering_clob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(clob_locp, DCI_DTYPE_LOB);
		clob_locp = NULL;
	} //end of for

	return ret;
}

sword
TestLobEnableBuffering_blob()
{
	DCILobLocator	*blob_locp = NULL;
	DCIDefine		*pDefine = NULL;
	sword			ret = DCI_SUCCESS;

	typedef struct test_blob_hnd
	{
		DCISvcCtx	*pSvcCtx; 
		DCIError	*pError;
		sword		ret_val;
	}test_blob_hnd;

	test_blob_hnd blob_array[]=
	{
		{NULL, pError, DCI_SUCCESS},
		{pSvcCtx, NULL, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, DCI_SUCCESS},
	};

	typedef struct test_blob_suite
	{
		char	*str_in;
		sword	ret_val;
	}test_blob_suite;

	test_blob_suite test_blob[] = 
	{
		{"aaaaaa", DCI_SUCCESS},
	};

	char	*str_in;

	str_in = test_blob[0].str_in; 

	//test handle
	int		i = 0;
	for (i = 0;i < (int)sizeof(blob_array) / (int)sizeof(blob_array[0]); i++)
	{
		drop_table(table_name1);
		create_table_blob(table_name1);
		insert_table_blob(table_name1, str_in, (int)strlen(test_blob[0].str_in));

		blob_locp = select_table_blob(table_name1, pDefine, blob_locp);

		err = DCILobEnableBuffering(blob_array[i].pSvcCtx, blob_array[i].pError, blob_locp);
		if (err != blob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobEnableBuffering_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(blob_locp, DCI_DTYPE_LOB);
		blob_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		str_in = test_blob[i].str_in;

		drop_table(table_name1);
		create_table_blob(table_name1);
		insert_table_blob(table_name1, str_in, (int)strlen(test_blob[i].str_in));

		blob_locp = select_table_blob(table_name1, pDefine, blob_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, blob_locp);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobEnableBuffering_blob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}
		err = DCILobDisableBuffering(pSvcCtx, pError, blob_locp);	
		err = DCILobEnableBuffering(pSvcCtx, pError, blob_locp);	

		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobEnableBuffering_blob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(blob_locp, DCI_DTYPE_LOB);
		blob_locp = NULL;
	} //end of for

	return ret;
}

sword
TestLobEnableBuffering()
{
	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	if (TestLobEnableBuffering_clob() == DCI_ERROR)
	{
		printf("TestLobEnableBuffering_clob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobEnableBuffering_clob SUCCESS\n");

	if (TestLobEnableBuffering_blob() == DCI_ERROR)
	{
		printf("TestLobEnableBuffering_blob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobEnableBuffering_blob SUCCESS\n");

	err = disconnect(true);

	return DCI_SUCCESS;
}

sword
TestLobDisableBuffering_clob()
{
	DCILobLocator	*clob_locp = NULL;
	DCIDefine		*pDefine = NULL;
	sword			ret = DCI_SUCCESS;

	typedef struct test_clob_hnd
	{
		DCISvcCtx	*pSvcCtx; 
		DCIError	*pError;
		sword		ret_val;
	}test_clob_hnd;

	test_clob_hnd clob_array[]=
	{
		{NULL, pError, DCI_INVALID_HANDLE},
		{pSvcCtx, NULL, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, DCI_SUCCESS},
	};
	typedef struct test_clob_suite
	{
		char	*str_in;
		sword	ret_val;
	}test_clob_suite;

	test_clob_suite test_clob[] = 
	{
		{"北京人大金仓信息技术股份有限公司", DCI_SUCCESS},
	};

	char	*str_in;

	str_in = test_clob[0].str_in;

	//test handle
	int		i = 0;
	for (i = 0;i < (int)sizeof(clob_array) / (int)sizeof(clob_array[0]); i++)
	{
		drop_table(table_name1);
		create_table_clob(table_name1);
		insert_table_clob(table_name1, str_in);

		clob_locp = select_table_clob(table_name1, pDefine, clob_locp);

		err = DCILobDisableBuffering(clob_array[i].pSvcCtx, clob_array[i].pError, clob_locp);
		if (err != clob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobDisableBuffering_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCILobEnableBuffering(clob_array[i].pSvcCtx, clob_array[i].pError, clob_locp);
		err = DCILobDisableBuffering(clob_array[i].pSvcCtx, clob_array[i].pError, clob_locp);
		if (err != clob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobDisableBuffering_clob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(clob_locp, DCI_DTYPE_LOB);
		clob_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		str_in = test_clob[i].str_in;

		drop_table(table_name1);
		create_table_clob(table_name1);
		insert_table_clob(table_name1, str_in);

		clob_locp = select_table_clob(table_name1, pDefine, clob_locp);

		err = DCILobDisableBuffering(pSvcCtx, pError, clob_locp);
		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobDisableBuffering_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCILobEnableBuffering(pSvcCtx, pError, clob_locp);
		err = DCILobDisableBuffering(pSvcCtx, pError, clob_locp);
		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobDisableBuffering_clob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(clob_locp, DCI_DTYPE_LOB);
		clob_locp = NULL;
	} //end of for

	return ret;
}

sword 
TestLobDisableBuffering_blob()
{
	DCILobLocator	*blob_locp = NULL;
	DCIDefine		*pDefine = NULL;
	sword			ret = DCI_SUCCESS;

	typedef struct test_blob_hnd
	{
		DCISvcCtx	*pSvcCtx; 
		DCIError	*pError;
		sword		ret_val;
	}test_blob_hnd;

	test_blob_hnd blob_array[]=
	{
		{NULL, pError, DCI_INVALID_HANDLE},
		{pSvcCtx, NULL, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, DCI_SUCCESS},
	};
	typedef struct test_blob_suite
	{
		char	*str_in;
		sword	ret_val;
	}test_blob_suite;

	test_blob_suite test_blob[] = 
	{
		{"北京人大金仓信息技术股份有限公司", DCI_SUCCESS},
	};

	char	*str_in;

	str_in = test_blob[0].str_in;

	//test handle
	int		i = 0;
	for (i = 0;i < (int)sizeof(blob_array) / (int)sizeof(blob_array[0]); i++)
	{
		drop_table(table_name1);
		create_table_blob(table_name1);
		insert_table_blob(table_name1, str_in, (int)strlen(test_blob[0].str_in));

		blob_locp = select_table_blob(table_name1, pDefine, blob_locp);

		err = DCILobDisableBuffering(blob_array[i].pSvcCtx, blob_array[i].pError, blob_locp);
		if (err != blob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobDisableBuffering_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCILobEnableBuffering(blob_array[i].pSvcCtx, blob_array[i].pError, blob_locp);	
		err = DCILobDisableBuffering(blob_array[i].pSvcCtx, blob_array[i].pError, blob_locp);	
		if (err != blob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobDisableBuffering_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(blob_locp, DCI_DTYPE_LOB);
		blob_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		str_in = test_blob[i].str_in;

		drop_table(table_name1);
		create_table_blob(table_name1);
		insert_table_blob(table_name1, str_in, (int)strlen(test_blob[i].str_in));

		blob_locp = select_table_blob(table_name1, pDefine, blob_locp);

		err = DCILobDisableBuffering(pSvcCtx, pError, blob_locp);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobDisableBuffering_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCILobEnableBuffering(pSvcCtx, pError, blob_locp);
		err = DCILobDisableBuffering(pSvcCtx, pError, blob_locp);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobDisableBuffering_blob: FAILED\n");
			ret = DCI_ERROR;
			continue;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(blob_locp, DCI_DTYPE_LOB);
		blob_locp = NULL;
	} //end of for

	return ret;
}

sword 
TestLobDisableBuffering()
{
	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	if (TestLobDisableBuffering_clob() == DCI_ERROR)
	{
		printf("TestLobDisableBuffering_clob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobDisableBuffering_clob SUCCESS\n");

	if (TestLobDisableBuffering_blob() == DCI_ERROR)
	{
		printf("TestLobDisableBuffering_blob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobDisableBuffering_blob SUCCESS\n");

	err = disconnect(true);

	return DCI_SUCCESS;
}

sword 
TestLobFlushBuffer_clob()
{
	DCILobLocator	*clob_locp = NULL;
	DCIDefine		*pDefine = NULL;
	sword			ret = DCI_SUCCESS;

	typedef struct test_clob_hnd
	{
		DCISvcCtx	*pSvcCtx; 
		DCIError	*pError;
		sword		ret_val;
	}test_clob_hnd;

	test_clob_hnd clob_array[]=
	{
		{NULL, pError, DCI_INVALID_HANDLE},
		{pSvcCtx, NULL, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, DCI_SUCCESS},
	};

	typedef struct test_clob_suite
	{
		char	*str_in;
		sword	ret_val;
	}test_clob_suite;

	test_clob_suite test_clob[] = 
	{
		{"北京人大金仓信息技术股份有限公司", DCI_SUCCESS},
	};

	char	*str_in;

	str_in = test_clob[0].str_in;

	//test handle
	int		i = 0;
	char	bufp[BUFLEN] = "中华人民共和国";
	ub4		amtp = (ub4)strlen(bufp);

	for (i = 0;i < (int)sizeof(clob_array) / (int)sizeof(clob_array[0]); i++)
	{
		drop_table(table_name1);
		create_table_clob(table_name1);
		insert_table_clob(table_name1, str_in);

		clob_locp = select_table_clob(table_name1, pDefine, clob_locp);
		//在没有执行DCILobEnableBuffering和DCILobWrite之前执行DCILobFlushBuffer不论何种情况，都不会成功
		err = DCILobFlushBuffer(clob_array[i].pSvcCtx, clob_array[i].pError, clob_locp, DCI_DEFAULT);
		if (err != clob_array[i].ret_val && err != DCI_ERROR)
		{
			report_error(pError);
			printf("TestLobFlushBuffer_clob: FAILED\n");
			ret = DCI_ERROR;
		}
		err = DCILobEnableBuffering(clob_array[i].pSvcCtx, clob_array[i].pError, clob_locp);

		err = DCILobWrite(pSvcCtx, pError, clob_locp, &amtp, (ub4)1, bufp, (ub4)strlen(bufp), 
			(ub1)DCI_ONE_PIECE, (void *)0, 0, 0, SQLCS_IMPLICIT);

		err = DCILobFlushBuffer(clob_array[i].pSvcCtx, clob_array[i].pError, clob_locp, DCI_DEFAULT);
		if (err != clob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobFlushBuffer_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(clob_locp, DCI_DTYPE_LOB);
		clob_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		str_in = test_clob[i].str_in;

		drop_table(table_name1);
		create_table_clob(table_name1);
		insert_table_clob(table_name1, str_in);

		clob_locp = select_table_clob(table_name1, pDefine, clob_locp);

		err = DCILobFlushBuffer(pSvcCtx, pError, clob_locp, DCI_DEFAULT);
		if (err != DCI_ERROR)
		{
			report_error(pError);
			printf("TestLobFlushBuffer_clob: FAILED\n");
			ret = DCI_ERROR;
		}
		err = DCILobEnableBuffering(pSvcCtx, pError, clob_locp);

		err = DCILobWrite(pSvcCtx, pError, clob_locp, &amtp, (ub4)1, bufp, (ub4)strlen(bufp), 
			(ub1)DCI_ONE_PIECE, (void *)0, 0, 0, SQLCS_IMPLICIT);

		err = DCILobFlushBuffer(pSvcCtx, pError, clob_locp, DCI_DEFAULT);
		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobFlushBuffer_clob: FAILED\n");
			ret = DCI_ERROR;
		}
		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(clob_locp, DCI_DTYPE_LOB);
		clob_locp = NULL;
	} //end of for

	return ret;
}

sword
TestLobFlushBuffer_blob()
{
	DCILobLocator	*blob_locp = NULL;
	DCIDefine		*pDefine = NULL;
	sword			ret = DCI_SUCCESS;

	typedef struct test_blob_hnd
	{
		DCISvcCtx	*pSvcCtx; 
		DCIError	*pError;
		sword		ret_val;
	}test_blob_hnd;

	test_blob_hnd blob_array[]=
	{
		{NULL, pError, DCI_INVALID_HANDLE},
		{pSvcCtx, NULL, DCI_INVALID_HANDLE},
		{pSvcCtx, pError, DCI_SUCCESS},
	};
	typedef struct test_blob_suite
	{
		char	*str_in;
		ub4		str_len;
		sword	ret_val;
	}test_blob_suite;

	test_blob_suite test_blob[] = 
	{
		{"北京人大金仓信息技术股份有限公司", 32, DCI_SUCCESS},
	};

	char	*str_in;
	ub4		str_len;
	int		i = 0;
	char	bufp[BUFLEN] = "中华人民共和国";
	ub4		amtp = (ub4)strlen(bufp);

	str_in = test_blob[0].str_in; 
	str_len = test_blob[0].str_len;

	//test handle
	for (i = 0;i < (int)sizeof(blob_array) / (int)sizeof(blob_array[0]); i++)
	{
		drop_table(table_name1);
		create_table_blob(table_name1);
		insert_table_blob(table_name1, str_in, str_len);

		blob_locp = select_table_blob(table_name1, pDefine, blob_locp);

		err = DCILobFlushBuffer(blob_array[i].pSvcCtx, blob_array[i].pError, blob_locp, DCI_DEFAULT);
		if (err != blob_array[i].ret_val && err != DCI_ERROR)
		{
			report_error(pError);
			printf("TestLobFlushBuffer_blob: FAILED\n");
			ret = DCI_ERROR;
		}
		err = DCILobEnableBuffering(blob_array[i].pSvcCtx, blob_array[i].pError, blob_locp);	

		err = DCILobWrite(pSvcCtx, pError, blob_locp, &amtp, (ub4)1, bufp, (ub4)strlen(bufp), 
			(ub1)DCI_ONE_PIECE, (void *)0, 0, 0, SQLCS_IMPLICIT);

		err = DCILobFlushBuffer(blob_array[i].pSvcCtx, blob_array[i].pError, blob_locp, DCI_DEFAULT);
		if (err != blob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobFlushBuffer_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(blob_locp, DCI_DTYPE_LOB);
		blob_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		str_in = test_blob[i].str_in;

		drop_table(table_name1);
		create_table_blob(table_name1);
		insert_table_blob(table_name1, str_in, test_blob[i].str_len);

		blob_locp = select_table_blob(table_name1, pDefine, blob_locp);

		err = DCILobFlushBuffer(pSvcCtx, pError, blob_locp, DCI_DEFAULT);
		if (err != DCI_ERROR)
		{
			report_error(pError);
			printf("TestLobFlushBuffer_blob: FAILED\n");
			ret = DCI_ERROR;
		}
		err = DCILobEnableBuffering(pSvcCtx, pError, blob_locp);	

		err = DCILobWrite(pSvcCtx, pError, blob_locp, &amtp, (ub4)1, bufp, (ub4)strlen(bufp), 
			(ub1)DCI_ONE_PIECE, (void *)0, 0, 0, SQLCS_IMPLICIT);

		err = DCILobFlushBuffer(pSvcCtx, pError, blob_locp, DCI_DEFAULT);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobFlushBuffer_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(blob_locp, DCI_DTYPE_LOB);
		blob_locp = NULL;
	} //end of for

	return ret;
}

sword
TestLobFlushBuffer()
{
	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	if (TestLobFlushBuffer_clob() == DCI_ERROR)
	{
		printf("TestLobFlushBuffer_clob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobFlushBuffer_clob SUCCESS\n");

	if (TestLobFlushBuffer_blob() == DCI_ERROR)
	{
		printf("TestLobFlushBuffer_blob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobFlushBuffer_blob SUCCESS\n");

	err = disconnect(true);

	return DCI_SUCCESS;
}

sword 
TestLobIsEqual_clob()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	boolean		is_equal = -1;
	sword		ret = DCI_SUCCESS;

	typedef struct test_clob_hnd
	{
		DCIEnv	*pEnv; 
		int		dst_flag;
		int		src_flag;
		sword	ret_val;
	}test_clob_hnd;

	//Two LOB or BFILE locators are equal if and only 
	//if they both refer to the same LOB or BFILE value.

	//Two NULL locators are considered not equal by DCILobIsEqual.

	//Use DCILobAssign to make the two locators refer to the same LOB.

	test_clob_hnd clob_array[]=
	{
		{NULL, 1, 1, DCI_INVALID_HANDLE},
		{pEnv, 0, 1, DCI_SUCCESS},
		{pEnv, 1, 0, DCI_SUCCESS},
		{pEnv, 0, 0, DCI_SUCCESS},
		{pEnv, 1, 1, DCI_SUCCESS},
	};

	typedef struct test_clob_suite
	{
		char	*dst_in;
		char	*src_in;
		sword	ret_val;
	}test_clob_suite;

	test_clob_suite test_clob[] = 
	{
		{"北京人大金仓信息技术股份有限公司", "研发中心工程部", DCI_SUCCESS},
		{"aaa", "bbbbb", DCI_SUCCESS},
		{NULL, "", DCI_SUCCESS},
		{NULL, NULL, DCI_SUCCESS},
	};

	char	*dst_in, *src_in;
	int		i = 0;

	dst_in = test_clob[0].dst_in; 
	src_in = test_clob[0].src_in;

	//test handle
	for (i = 0;i < (int)sizeof(clob_array) / (int)sizeof(clob_array[0]); i++)
	{
		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_clob(table_name2, src_in);

		if (clob_array[i].dst_flag == 1)
			dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);

		if (clob_array[i].src_flag == 1)
			src_locp = select_table_clob(table_name2, pDefine2, src_locp);

		is_equal = -1;
		err = DCILobIsEqual(clob_array[i].pEnv, dst_locp, src_locp, &is_equal);
		if (err != clob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		src_locp = NULL;
		dst_locp = NULL;

		drop_table(table_name1);
		drop_table(table_name2);
	} //end of for


	//test data
	for (i = 0; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		dst_in = test_clob[i].dst_in; 
		src_in = test_clob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_clob(table_name2, src_in);

		if (dst_in != NULL)
			dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);

		if (src_in != NULL)
			src_locp = select_table_clob(table_name2, pDefine2, src_locp);

		//只要DCILobIsEqual的环境句柄为本测试程序的环境句柄，DCILobIsEqual的返回值就是DCI_SUCCESS
		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (err != test_clob[i].ret_val || is_equal != 0)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);
		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (dst_locp == NULL || src_locp == NULL)
		{
			if (err != test_clob[i].ret_val || is_equal != 0)
			{
				report_error(pError);
				printf("TestLobIsEqual_clob: FAILED\n");
				ret = DCI_ERROR;
			}
		}
		else if (err != test_clob[i].ret_val || is_equal != 1)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;

		drop_table(table_name1);
		drop_table(table_name2);
	} //end of for

	return ret;
}

sword
TestLobIsEqual_blob()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	boolean		is_equal = -1;
	sword		ret = DCI_SUCCESS;

	typedef struct test_blob_hnd
	{
		DCIEnv	*pEnv; 
		int		dst_flag;
		int		src_flag;
		sword	ret_val;
	}test_blob_hnd;

	//Two LOB or BFILE locators are equal if and only 
	//if they both refer to the same LOB or BFILE value.

	//Two NULL locators are considered not equal by DCILobIsEqual.

	//Use DCILobAssign to make the two locators refer to the same LOB.

	test_blob_hnd blob_array[]=
	{
		{NULL, 1, 1, DCI_INVALID_HANDLE},
		{pEnv, 0, 1, DCI_SUCCESS},
		{pEnv, 1, 0, DCI_SUCCESS},
		{pEnv, 0, 0, DCI_SUCCESS},
		{pEnv, 1, 1, DCI_SUCCESS},
	};

	typedef struct test_blob_suite
	{
		char	*dst_in;
		char	*src_in;
		sword	ret_val;
	}test_blob_suite;

	test_blob_suite test_blob[] = 
	{
		{"aaaaaaaaaaaa",  "bbbbbbbbbbbbbb", DCI_SUCCESS},
		{"111111111111111", "22222222222222", DCI_SUCCESS},
		{NULL, "", DCI_SUCCESS},
		{NULL, NULL, DCI_SUCCESS},
	};

	char	*dst_in, *src_in;
	int		i = 0;

	dst_in = test_blob[0].dst_in; 
	src_in = test_blob[0].src_in;

	//test handle
	for (i = 0;i < (int)sizeof(blob_array) / (int)sizeof(blob_array[0]); i++)
	{
		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, dst_in, (int)strlen(test_blob[0].dst_in));
		insert_table_blob(table_name2, src_in, (int)strlen(test_blob[0].src_in));

		if (blob_array[i].src_flag == 1)
			dst_locp = select_table_blob(table_name1, pDefine1, dst_locp);

		if (blob_array[i].dst_flag == 1)
			src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		is_equal = -1;
		err = DCILobIsEqual(blob_array[i].pEnv, dst_locp, src_locp, &is_equal);
		if (err != blob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobIsEqual_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		src_locp = NULL;
		dst_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		dst_in = test_blob[i].dst_in;
		src_in = test_blob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, dst_in, (int)strlen(test_blob[0].dst_in));
		insert_table_blob(table_name2, src_in, (int)strlen(test_blob[0].src_in));

		if (dst_in != NULL)
			dst_locp = select_table_blob(table_name1, pDefine1, dst_locp);

		if (src_in != NULL)
			src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		//只要DCILobIsEqual的环境句柄为本测试程序的环境句柄，DCILobIsEqual的返回值就是DCI_SUCCESS
		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (err != test_blob[i].ret_val || is_equal != 0)
		{
			report_error(pError);
			printf("TestLobIsEqual_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (dst_locp == NULL || src_locp == NULL)
		{
			if (err != test_blob[i].ret_val || is_equal != 0)
			{
				report_error(pError);
				printf("TestLobIsEqual_blob: FAILED\n");
				ret = DCI_ERROR;
			}
		}
		else if (err != test_blob[i].ret_val || is_equal != 1)
		{
			report_error(pError);
			printf("TestLobIsEqual_blob: FAILED\n");
			ret = DCI_ERROR;
		}
		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;
	} //end of for

	return ret;
}

sword
TestLobIsEqual_clob_blob()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	boolean		is_equal = -1;
	sword		ret = DCI_SUCCESS;

	typedef struct test_clob_blob_hnd
	{
		DCIEnv	*pEnv; 
		int		dst_flag;
		int		src_flag;
		sword	ret_val;
	}test_clob_blob_hnd;

	//Two LOB or BFILE locators are equal if and only 
	//if they both refer to the same LOB or BFILE value.

	//Two NULL locators are considered not equal by DCILobIsEqual.

	//Use DCILobAssign to make the two locators refer to the same LOB.

	test_clob_blob_hnd clob_blob_array[]=
	{
		{NULL, 1, 1, DCI_INVALID_HANDLE},
		{pEnv, 0, 1, DCI_SUCCESS},
		{pEnv, 1, 0, DCI_SUCCESS},
		{pEnv, 0, 0, DCI_SUCCESS},
		{pEnv, 1, 1, DCI_SUCCESS},
	};

	typedef struct test_clob_blob_suite
	{
		char	*dst_in;
		char	*src_in;
		sword	ret_val;
	}test_clob_blob_suite;

	test_clob_blob_suite test_clob_blob[] = 
	{
		{"北京人大金仓信息技术股份有限公司", "3123213131", DCI_SUCCESS},
		{"北京人大金仓", "bbbbb", DCI_SUCCESS},
		{NULL, "", DCI_SUCCESS},
	};

	char	*dst_in, *src_in;
	int		i = 0;

	dst_in = test_clob_blob[0].dst_in; 
	src_in = test_clob_blob[0].src_in;

	//test handle
	for (i = 0;i < (int)sizeof(clob_blob_array) / (int)sizeof(clob_blob_array[0]); i++)
	{
		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_blob(table_name2, src_in, (int)strlen(test_clob_blob[0].src_in));

		if (clob_blob_array[i].src_flag == 1)
			dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);

		if (clob_blob_array[i].dst_flag == 1)
			src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		is_equal = -1;
		err = DCILobIsEqual(clob_blob_array[i].pEnv, dst_locp, src_locp, &is_equal);
		if (err != clob_blob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		src_locp = NULL;
		dst_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_clob_blob) / (int)sizeof(test_clob_blob[0]); i++)
	{
		dst_in = test_clob_blob[i].dst_in;
		src_in = test_clob_blob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_blob(table_name2, src_in, (int)strlen(test_clob_blob[i].src_in));

		if (dst_in != NULL)
			dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);

		if (src_in != NULL)
			src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (err != test_clob_blob[i].ret_val || is_equal != 0)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (dst_locp == NULL || src_locp == NULL)
		{
			if (err != test_clob_blob[i].ret_val || is_equal != 0)
			{
				report_error(pError);
				printf("TestLobIsEqual_clob_blob: FAILED\n");
				ret = DCI_ERROR;
			}
		}
		else if (err != test_clob_blob[i].ret_val || is_equal != 1)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;
	} //end of for

	return ret;
}

sword 
TestLobIsEqual_clob_enablebuffering()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	boolean		is_equal = -1;
	sword		ret = DCI_SUCCESS;

	typedef struct test_clob_suite
	{
		char	*dst_in;
		char	*src_in;
		sword	ret_val;
	}test_clob_suite;

	test_clob_suite test_clob[] = 
	{
		{"北京人大金仓", "研发中心工程部", DCI_SUCCESS},
		{"aaa", "bbbbb", DCI_SUCCESS},
	};

	char	*dst_in, *src_in;
	int		i = 0;

	//参数 src_locp 启用缓存子系统
	for (i = 0; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		dst_in = test_clob[i].dst_in;
		src_in = test_clob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_clob(table_name2, src_in);

		dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);
		src_locp = select_table_clob(table_name2, pDefine2, src_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (err != test_clob[i].ret_val || is_equal != 0)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (dst_locp == NULL || src_locp == NULL)
		{
			if (err != test_clob[i].ret_val || is_equal != 0)
			{
				report_error(pError);
				printf("TestLobIsEqual_clob: FAILED\n");
				ret = DCI_ERROR;
			}
		}
		else if (err != test_clob[i].ret_val || is_equal != 1)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);
		dst_locp = NULL;
		src_locp = NULL;

		drop_table(table_name1);
		drop_table(table_name2);
	} //end of for

	//参数 dst_locp 启用缓存子系统
	for (i = 0; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		dst_in = test_clob[i].dst_in;
		src_in = test_clob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_clob(table_name2, src_in);

		dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);
		src_locp = select_table_clob(table_name2, pDefine2, src_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (err != test_clob[i].ret_val || is_equal != 0)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (dst_locp == NULL || src_locp == NULL)
		{
			if (err != test_clob[i].ret_val || is_equal != 0)
			{
				report_error(pError);
				printf("TestLobIsEqual_clob: FAILED\n");
				ret = DCI_ERROR;
			}
		}
		else if (err != test_clob[i].ret_val || is_equal != 1)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;

		drop_table(table_name1);
		drop_table(table_name2);
	} //end of for

	//参数 src_locp 和 dst_locp 启用缓存子系统
	for (i = 0; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		dst_in = test_clob[i].dst_in;
		src_in = test_clob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_clob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_clob(table_name2, src_in);

		dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);
		src_locp = select_table_clob(table_name2, pDefine2, src_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (err != test_clob[i].ret_val || is_equal != 0)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (dst_locp == NULL || src_locp == NULL)
		{
			if (err != test_clob[i].ret_val || is_equal != 0)
			{
				report_error(pError);
				printf("TestLobIsEqual_clob: FAILED\n");
				ret = DCI_ERROR;
			}
		}
		else if (err != test_clob[i].ret_val || is_equal != 1)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;

		drop_table(table_name1);
		drop_table(table_name2);
	} //end of for

	return ret;
}

sword 
TestLobIsEqual_blob_enablebuffering()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	boolean		is_equal = -1;
	sword		ret = DCI_SUCCESS;

	typedef struct test_blob_suite
	{
		char	*dst_in;
		char	*src_in;
		sword	ret_val;
	}test_blob_suite;

	test_blob_suite test_blob[] = 
	{
		{"123", "456", DCI_SUCCESS},
		{"aaa", "bbbbb", DCI_SUCCESS},
	};

	char	*dst_in, *src_in;
	int		i = 0;

	//参数 src_locp 启用缓存子系统
	for (i = 0; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		dst_in = test_blob[i].dst_in;
		src_in = test_blob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, dst_in, (int)strlen(dst_in));
		insert_table_blob(table_name2, src_in, (int)strlen(src_in));

		dst_locp = select_table_blob(table_name1, pDefine1, dst_locp);
		src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (err != test_blob[i].ret_val || is_equal != 0)
		{
			report_error(pError);
			printf("TestLobIsEqual_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (dst_locp == NULL || src_locp == NULL)
		{
			if (err != test_blob[i].ret_val || is_equal != 0)
			{
				report_error(pError);
				printf("TestLobIsEqual_blob: FAILED\n");
				ret = DCI_ERROR;
			}
		}
		else if (err != test_blob[i].ret_val || is_equal != 1)
		{
			report_error(pError);
			printf("TestLobIsEqual_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;

		drop_table(table_name1);
		drop_table(table_name2);
	} //end of for

	//参数 dst_locp 启用缓存子系统
	for (i = 0; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		dst_in = test_blob[i].dst_in;
		src_in = test_blob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, dst_in, (int)strlen(dst_in));
		insert_table_blob(table_name2, src_in, (int)strlen(src_in));

		dst_locp = select_table_blob(table_name1, pDefine1, dst_locp);
		src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (err != test_blob[i].ret_val || is_equal != 0)
		{
			report_error(pError);
			printf("TestLobIsEqual_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (dst_locp == NULL || src_locp == NULL)
		{
			if (err != test_blob[i].ret_val || is_equal != 0)
			{
				report_error(pError);
				printf("TestLobIsEqual_blob: FAILED\n");
				ret = DCI_ERROR;
			}
		}
		else if (err != test_blob[i].ret_val || is_equal != 1)
		{
			report_error(pError);
			printf("TestLobIsEqual_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;

		drop_table(table_name1);
		drop_table(table_name2);
	} //end of for

	//参数 src_locp 和 dst_locp 启用缓存子系统
	for (i = 0; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		dst_in = test_blob[i].dst_in;
		src_in = test_blob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_blob(table_name1);
		create_table_blob(table_name2);

		insert_table_blob(table_name1, dst_in, (int)strlen(dst_in));
		insert_table_blob(table_name2, src_in, (int)strlen(src_in));

		dst_locp = select_table_blob(table_name1, pDefine1, dst_locp);
		src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (err != test_blob[i].ret_val || is_equal != 0)
		{
			report_error(pError);
			printf("TestLobIsEqual_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (dst_locp == NULL || src_locp == NULL)
		{
			if (err != test_blob[i].ret_val || is_equal != 0)
			{
				report_error(pError);
				printf("TestLobIsEqual_blob: FAILED\n");
				ret = DCI_ERROR;
			}
		}
		else if (err != test_blob[i].ret_val || is_equal != 1)
		{
			report_error(pError);
			printf("TestLobIsEqual_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;

		drop_table(table_name1);
		drop_table(table_name2);
	} //end of for

	return ret;
}

sword 
TestLobIsEqual_clob_blob_enablebuffering()
{
	DCIDefine	*pDefine1 = NULL;
	DCIDefine	*pDefine2 = NULL;
	boolean		is_equal = -1;
	sword		ret = DCI_SUCCESS;

	typedef struct test_clob_blob_suite
	{
		char	*dst_in;
		char	*src_in;
		sword	ret_val;
	}test_clob_blob_suite;

	test_clob_blob_suite test_clob_blob[] = 
	{
		{"123", "456", DCI_SUCCESS},
		{"aaa", "bbbbb", DCI_SUCCESS},
	};

	char	*dst_in, *src_in;
	int		i = 0;

	//参数 src_locp 启用缓存子系统
	for (i = 0; i < (int)sizeof(test_clob_blob) / (int)sizeof(test_clob_blob[0]); i++)
	{
		dst_in = test_clob_blob[i].dst_in;
		src_in = test_clob_blob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_blob(table_name2, src_in, (int)strlen(src_in));

		dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);
		src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (err != test_clob_blob[i].ret_val || is_equal != 0)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (dst_locp == NULL || src_locp == NULL)
		{
			if (err != test_clob_blob[i].ret_val || is_equal != 0)
			{
				report_error(pError);
				printf("TestLobIsEqual_clob_blob_enablebuffering: FAILED\n");
				ret = DCI_ERROR;
			}
		}
		else if (err != test_clob_blob[i].ret_val || is_equal != 1)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;

		drop_table(table_name1);
		drop_table(table_name2);
	} //end of for

	//参数 dst_locp 启用缓存子系统
	for (i = 0; i < (int)sizeof(test_clob_blob) / (int)sizeof(test_clob_blob[0]); i++)
	{
		dst_in = test_clob_blob[i].dst_in;
		src_in = test_clob_blob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_blob(table_name2, src_in, (int)strlen(src_in));

		dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);
		src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (err != test_clob_blob[i].ret_val || is_equal != 0)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (dst_locp == NULL || src_locp == NULL)
		{
			if (err != test_clob_blob[i].ret_val || is_equal != 0)
			{
				report_error(pError);
				printf("TestLobIsEqual_clob_blob_enablebuffering: FAILED\n");
				ret = DCI_ERROR;
			}
		}
		else if (err != test_clob_blob[i].ret_val || is_equal != 1)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob_blob_enablebuffering: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;

		drop_table(table_name1);
		drop_table(table_name2);
	} //end of for

	//参数 src_locp 和 dst_locp 启用缓存子系统
	for (i = 0; i < (int)sizeof(test_clob_blob) / (int)sizeof(test_clob_blob[0]); i++)
	{
		dst_in = test_clob_blob[i].dst_in;
		src_in = test_clob_blob[i].src_in;

		drop_table(table_name1);
		drop_table(table_name2);

		create_table_clob(table_name1);
		create_table_blob(table_name2);

		insert_table_clob(table_name1, dst_in);
		insert_table_blob(table_name2, src_in, (int)strlen(src_in));

		dst_locp = select_table_clob(table_name1, pDefine1, dst_locp);
		src_locp = select_table_blob(table_name2, pDefine2, src_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		err = DCILobEnableBuffering(pSvcCtx, pError, dst_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (err != test_clob_blob[i].ret_val || is_equal != 0)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCILobAssign(pEnv, pError, src_locp, &dst_locp);

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		err = DCILobEnableBuffering(pSvcCtx, pError, src_locp);
		if (err != DCI_SUCCESS)
		{
			report_error(pError);
			printf("DCILobIsEqual enable buffering failed\n");
		}

		is_equal = -1;
		err = DCILobIsEqual(pEnv, dst_locp, src_locp, &is_equal);
		if (dst_locp == NULL || src_locp == NULL)
		{
			if (err != test_clob_blob[i].ret_val || is_equal != 0)
			{
				report_error(pError);
				printf("TestLobIsEqual_clob_blob: FAILED\n");
				ret = DCI_ERROR;
			}
		}
		else if (err != test_clob_blob[i].ret_val || is_equal != 1)
		{
			report_error(pError);
			printf("TestLobIsEqual_clob_blob: FAILED\n");
			ret = DCI_ERROR;
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(dst_locp, DCI_DTYPE_LOB);
		DCIDescriptorFree(src_locp, DCI_DTYPE_LOB);

		dst_locp = NULL;
		src_locp = NULL;

		drop_table(table_name1);
		drop_table(table_name2);
	} //end of for

	return ret;
}

sword
TestLobIsEqual()
{
	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	if (TestLobIsEqual_clob() == DCI_ERROR)
	{
		printf("TestLobIsEqual_clob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobIsEqual_clob SUCCESS\n");

	if (TestLobIsEqual_blob() == DCI_ERROR)
	{
		printf("TestLobIsEqual_blob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobIsEqual_blob SUCCESS\n");

	if (TestLobIsEqual_clob_blob() == DCI_ERROR)
	{
		printf("TestLobIsEqual_clob_blob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobIsEqual_clob_blob SUCCESS\n");

	if (TestLobIsEqual_clob_enablebuffering() == DCI_ERROR)
	{
		printf("TestLobIsEqual_clob_enablebuffering FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobIsEqual_clob_enablebuffering SUCCESS\n");

	if (TestLobIsEqual_blob_enablebuffering() == DCI_ERROR)
	{
		printf("TestLobIsEqual_blob_enablebuffering FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobIsEqual_blob_enablebuffering SUCCESS\n");

	if (TestLobIsEqual_clob_blob_enablebuffering() == DCI_ERROR)
	{
		printf("TestLobIsEqual_clob_blob_enablebuffering FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobIsEqual_clob_blob_enablebuffering SUCCESS\n");

	err = disconnect(true);

	return DCI_SUCCESS;
}

sword
TestLobLocatorIsInit_clob()
{
	DCILobLocator	*clob_locp = NULL;
	DCIDefine		*pDefine = NULL;
	boolean			is_initialized = -1;
	sword			ret = DCI_SUCCESS;

	typedef struct test_clob_hnd
	{
		DCIEnv	*pEnv; 
		sword	clob_flag;
		sword	ret_val;
	}test_clob_hnd;

	//如果错误句柄为空，执行DCILobLocatorIsInit，程序中断
	//所以避开pError为空的情况(Oracle)
	test_clob_hnd clob_array[]=
	{
		{NULL, 1, DCI_INVALID_HANDLE},
		{pEnv, 0, DCI_INVALID_HANDLE},
		{pEnv, 1, DCI_SUCCESS},
	};

	typedef struct test_clob_suite
	{
		char	*str_in;
		sword	ret_val;
	}test_clob_suite;

	test_clob_suite test_clob[] = 
	{
		{"北京人大金仓信息技术股份有限公司", DCI_SUCCESS},
		{"aaaaaa", DCI_SUCCESS},
		{NULL,DCI_INVALID_HANDLE},
	};
	char	str_out[BUFLEN];
	ub4		amtp = BUFLEN;
	char	*str_in;

	str_in = test_clob[0].str_in;

	//test handle
	int		i = 0;
	for (i = 0;i < (int)sizeof(clob_array) / (int)sizeof(clob_array[0]); i++)
	{
		drop_table(table_name1);
		create_table_clob(table_name1);
		insert_table_clob(table_name1, str_in);

		if (clob_array[i].clob_flag == 1)
			clob_locp = select_table_clob(table_name1, pDefine, clob_locp);

		if (clob_locp != NULL)
		{
			amtp = sizeof(str_out);
			memset(str_out, 0, amtp);

			err = DCILobRead(pSvcCtx, pError, clob_locp, &amtp, (ub4)1, 
				(void *)str_out, (ub4)sizeof(str_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				ret = DCI_ERROR;
			}
		}

		is_initialized = -1;
		err = DCILobLocatorIsInit(clob_array[i].pEnv, pError, clob_locp, &is_initialized);
		if (err != clob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobLocatorIsInit_clob failed\n");
			ret = DCI_ERROR;
		}

		if (is_initialized == 1)
		{
			if (memcmp(str_out, str_in, amtp))
			{
				printf("TestLobLocatorIsInit_clob failed\n");
				ret = DCI_ERROR;
			}
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(clob_locp, DCI_DTYPE_LOB);
		clob_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_clob) / (int)sizeof(test_clob[0]); i++)
	{
		str_in = test_clob[i].str_in;

		drop_table(table_name1);
		create_table_clob(table_name1);
		insert_table_clob(table_name1, str_in);

		if (str_in != NULL)
			clob_locp = select_table_clob(table_name1, pDefine, clob_locp);

		if (clob_locp != NULL)
		{
			amtp = sizeof(str_out);
			memset(str_out, 0, amtp);

			err = DCILobRead(pSvcCtx, pError, clob_locp, &amtp, (ub4)1, 
				(void *)str_out, (ub4)sizeof(str_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				ret = DCI_ERROR;
			}
		}

		is_initialized = -1;
		err = DCILobLocatorIsInit(pEnv, pError, clob_locp, &is_initialized);
		if (err != test_clob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobLocatorIsInit_clob failed\n");
			ret = DCI_ERROR;
		}

		if (is_initialized == 1)
		{
			if (memcmp(str_out, str_in, amtp))
			{
				printf("TestLobLocatorIsInit_clob failed\n");
				ret = DCI_ERROR;
			}
		}
		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(clob_locp, DCI_DTYPE_LOB);
		clob_locp = NULL;
	} //end of for

	return ret;
}

sword
TestLobLocatorIsInit_blob()
{
	DCILobLocator	*blob_locp = NULL;
	DCIDefine		*pDefine = NULL;
	boolean			is_initialized = -1;
	sword			ret = DCI_SUCCESS;

	typedef struct test_blob_hnd
	{
		DCIEnv	*pEnv; 
		sword	blob_flag;
		sword	ret_val;
	}test_blob_hnd;

	test_blob_hnd blob_array[]=
	{
		{NULL, 1, DCI_INVALID_HANDLE},
		{pEnv, 0, DCI_INVALID_HANDLE},
		{pEnv, 1, DCI_SUCCESS},
	};

	typedef struct test_blob_suite
	{
		char	*str_in;
		char	str_len;
		sword	ret_val;
	}test_blob_suite;

	test_blob_suite test_blob[] = 
	{
		{"abcdef", 6, DCI_SUCCESS},
		{"123456", 6, DCI_SUCCESS},
		{NULL, 0, DCI_INVALID_HANDLE},
	};
	char	str_out[BUFLEN];
	ub4		amtp = BUFLEN;
	char	*str_in;

	str_in = test_blob[0].str_in; 
	//test handle
	int		i = 0;
	for (i = 0;i < (int)sizeof(blob_array) / (int)sizeof(blob_array[0]); i++)
	{
		drop_table(table_name1);
		create_table_blob(table_name1);
		insert_table_blob(table_name1, str_in, (int)strlen(str_in));

		if (blob_array[i].blob_flag == 1)
			blob_locp = select_table_blob(table_name1, pDefine, blob_locp);

		if (blob_locp != NULL)
		{
			amtp = sizeof(str_out);
			memset(str_out, 0, amtp);

			err = DCILobRead(pSvcCtx, pError, blob_locp, &amtp, (ub4)1, 
				(void *)str_out, (ub4)sizeof(str_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				ret = DCI_ERROR;
			}
		}

		//如果错误句柄为空，执行DCILobLocatorIsInit，程序中断
		//所以避开pError为空的情况(Oracle)
		is_initialized = -1;
		err = DCILobLocatorIsInit(blob_array[i].pEnv, pError, blob_locp, &is_initialized);
		if (err != blob_array[i].ret_val)
		{
			report_error(pError);
			printf("TestLobLocatorIsInit_blob failed\n");
			ret = DCI_ERROR;
		}

		if (is_initialized == 1)
		{
			if (memcmp(str_out, str_in, amtp))
			{
				printf("TestLobLocatorIsInit_blob failed\n");
				ret = DCI_ERROR;
			}
		}
		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(blob_locp, DCI_DTYPE_LOB);
		blob_locp = NULL;
	} //end of for

	//test data
	for (i = 0; i < (int)sizeof(test_blob) / (int)sizeof(test_blob[0]); i++)
	{
		str_in = test_blob[i].str_in;

		drop_table(table_name1);
		create_table_blob(table_name1);
		insert_table_blob(table_name1, str_in, test_blob[i].str_len);

		if (str_in != NULL)
			blob_locp = select_table_blob(table_name1, pDefine, blob_locp);

		if (blob_locp != NULL)
		{
			amtp = sizeof(str_out);
			memset(str_out, 0, amtp);

			err = DCILobRead(pSvcCtx, pError, blob_locp, &amtp, (ub4)1, 
				(void *)str_out, (ub4)sizeof(str_out), (void *)0, 
				0, (ub2)0, (ub1)SQLCS_IMPLICIT);
			if (err != DCI_SUCCESS)
			{
				report_error(pError);
				ret = DCI_ERROR;
			}
		}

		is_initialized = -1;
		err = DCILobLocatorIsInit(pEnv, pError, blob_locp, &is_initialized);
		if (err != test_blob[i].ret_val)
		{
			report_error(pError);
			printf("TestLobLocatorIsInit_blob failed\n");
			ret = DCI_ERROR;
		}

		if (is_initialized == 1)
		{
			if (memcmp(str_out, str_in, amtp))
			{
				printf("TestLobLocatorIsInit_blob failed\n");
				ret = DCI_ERROR;
			}
		}

		err = DCITransCommit(pSvcCtx, pError, (ub4)DCI_DEFAULT);

		DCIDescriptorFree(blob_locp, DCI_DTYPE_LOB);
		blob_locp = NULL;
	} //end of for

	return ret;
}

sword
TestLobLocatorIsInit()
{
	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	if (TestLobLocatorIsInit_clob() == DCI_ERROR)
	{
		printf("TestLobIsInit_clob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobIsInit_clob SUCCESS\n");

	if (TestLobLocatorIsInit_blob() == DCI_ERROR)
	{
		printf("TestLobIsInit_blob FAILED\n");
		return DCI_ERROR;
	}
	else
		printf("TestLobIsInit_blob SUCCESS\n");

	err = disconnect(true);

	return DCI_SUCCESS;
}

sword
TestLobCharSetForm()
{
	typedef struct att_input 
	{
		DCIEnv*			envp;
		DCIError*		errp;
		DCILobLocator*	lob;
		sb4				ret_value;
		ub1				result;
	}att_input;

	DCIDefine		*dfnhp[2];
	DCILobLocator	*lobp[2];
	DCIStmt			*pStmt = NULL;

	ub1 csfrm = 0;
	int i = 0;

	sword ret = DCI_SUCCESS;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	err = DCIHandleAlloc((dvoid *)pEnv, (dvoid **) &pStmt,
		(ub4)DCI_HTYPE_STMT, (CONST size_t) 0, (dvoid **) 0);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		report_error(pError);
		goto end;
	}

	lobp[0] = (DCILobLocator *) 0;
	lobp[1] = (DCILobLocator *) 0;

	if (alloc_lob_desc(pEnv, &lobp[0]) == DCI_ERROR
		|| alloc_lob_desc(pEnv, &lobp[1]) == DCI_ERROR)
	{
		ret = DCI_ERROR;
		printf("alloc lob failed\n");
		goto free_resource;
	}
	else
	{
		att_input intput_array[]=
		{
			{NULL, NULL, NULL, DCI_INVALID_HANDLE, 0},
			{NULL, NULL, lobp[0], DCI_INVALID_HANDLE, 0},
			{NULL, pError, lobp[0], DCI_INVALID_HANDLE, 0},
			{NULL, pError, NULL, DCI_INVALID_HANDLE, 0},

			{pEnv, NULL, NULL, DCI_INVALID_HANDLE, 0},
			{pEnv, NULL, lobp[0], DCI_INVALID_HANDLE, 0},
			{pEnv, pError, NULL, DCI_INVALID_HANDLE, 0},
			{pEnv, pError, lobp[0], DCI_SUCCESS, 0},

			{NULL, NULL, lobp[1], DCI_INVALID_HANDLE, 0},
			{NULL, pError, lobp[1], DCI_INVALID_HANDLE, 0},
			{pEnv, NULL, lobp[1], DCI_INVALID_HANDLE, 0},
			{pEnv, pError, lobp[1], DCI_SUCCESS, 1},
		};

		create_table(pEnv, pStmt, dfnhp, lobp);

		for (i = 0; i < (int)sizeof(intput_array) / (int)sizeof(intput_array[0]); i++)
		{
			csfrm = 0;

			err = DCILobCharSetForm(intput_array[i].envp, intput_array[i].errp, intput_array[i].lob, &csfrm);
			if (err != intput_array[i].ret_value
				|| csfrm != intput_array[i].result)
			{
				printf("test DCILobCharSetForm failed: cycle_no: %d\n", i + 1);
				ret = DCI_ERROR;
				goto free_resource;
			}
		}
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

	if (lobp[0])
	{
		err = DCIDescriptorFree((dvoid *) lobp[0], (ub4) DCI_DTYPE_LOB);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIDescriptorFree lobp[0] failed\n");
		}
	}

	if (lobp[1])
	{
		err = DCIDescriptorFree((dvoid *) lobp[1], (ub4) DCI_DTYPE_LOB);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIDescriptorFree lobp[1] failed\n");
		}
	}

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}

sword
TestLobCharSetId()
{
	typedef struct att_input 
	{
		DCIEnv*			envp;
		DCIError*		errp;
		DCILobLocator*	lob;
		sb4				ret_value;
		ub2				result;
	}att_input;

	DCIDefine		*dfnhp[2];
	DCILobLocator	*lobp[2];
	DCIStmt			*pStmt = NULL;

	ub2 csid = 0;
	int i = 0;

	sword ret = DCI_SUCCESS;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;	
	}

	err = DCIHandleAlloc((dvoid *)pEnv, (dvoid **) &pStmt,
		(ub4)DCI_HTYPE_STMT, (CONST size_t) 0, (dvoid **) 0);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		report_error(pError);
		goto end;
	}

	lobp[0] = (DCILobLocator *) 0;
	lobp[1] = (DCILobLocator *) 0;

	if (alloc_lob_desc(pEnv, &lobp[0]) == DCI_ERROR
		|| alloc_lob_desc(pEnv, &lobp[1]) == DCI_ERROR)
	{
		ret = DCI_ERROR;
		printf("alloc lob failed\n");
		goto free_resource;
	}
	else
	{
		att_input intput_array[]=
		{
			{NULL, NULL, NULL, DCI_INVALID_HANDLE, 0},
			{NULL, NULL, lobp[0], DCI_INVALID_HANDLE, 0},
			{NULL, pError, lobp[0], DCI_INVALID_HANDLE, 0},
			{NULL, pError, NULL, DCI_INVALID_HANDLE, 0},

			{pEnv, NULL, NULL, DCI_INVALID_HANDLE, 0},
			{pEnv, NULL, lobp[0], DCI_INVALID_HANDLE, 0},
			{pEnv, pError, NULL, DCI_INVALID_HANDLE, 0},
			{pEnv, pError, lobp[0], DCI_SUCCESS, 0},

			{NULL, NULL, lobp[1], DCI_INVALID_HANDLE, 0},
			{NULL, pError, lobp[1], DCI_INVALID_HANDLE, 0},
			{pEnv, NULL, lobp[1], DCI_INVALID_HANDLE, 0},
			{pEnv, pError, lobp[1], DCI_SUCCESS, 1},
		};

		create_table(pEnv, pStmt, dfnhp, lobp);

		for (i = 0; i < (int)sizeof(intput_array) / (int)sizeof(intput_array[0]); i++)
		{
			csid = 0;

			err = DCILobCharSetId(intput_array[i].envp, intput_array[i].errp, intput_array[i].lob, &csid);

			if (err != intput_array[i].ret_value
				|| csid != intput_array[i].result)
			{
				ret = DCI_ERROR;
				printf("test DCILobCharSetId failed cycle_no:%d   !\n", i + 1);
				goto free_resource;
			}
		}
	}
	
free_resource:
	if (pStmt)
	{
		err = DCIHandleFree((void *)pStmt, (const ub4)DCI_HTYPE_STMT);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIHandleFree pStmt failed\n");
		}
	}

	if (lobp[0])
	{
		err = DCIDescriptorFree((dvoid *) lobp[0], (ub4) DCI_DTYPE_LOB);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIDescriptorFree lobp[0] failed\n");
		}
	}
	
	if (lobp[1])
	{
		err = DCIDescriptorFree((dvoid *) lobp[1], (ub4) DCI_DTYPE_LOB);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIDescriptorFree lobp[1] failed\n");
		}
	}
	
end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	return ret;
}


sword
TestTempLobs()
{
	DCILobLocator	*lobp[2] = {NULL, NULL};

	int				blobdata[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0, 12};
	char			clobdata[] = "My god please bless me to win a Shuangseqiu lottery tonight.";
	ub4				amt[2] = {0, 0};
	int				blobdataout[sizeof(blobdata)/sizeof(blobdata[0])];
	char			clobdataout[sizeof(clobdata)];
	DCIBind			*bindp[2];
	DCIDefine		*defp[2];
	DCIStmt			*stmthp = NULL;
	DCIStmt	        *pStmt = NULL;
	sword ret = DCI_SUCCESS;
	boolean			isOpen[2], isTemp[2];

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;	
	}

	err = DCIHandleAlloc((dvoid *)pEnv, (dvoid **) &pStmt,
		(ub4)DCI_HTYPE_STMT, (CONST size_t) 0, (dvoid **) 0);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		report_error(pError);
		goto end;
	}

	lobp[0] = (DCILobLocator *) 0;
	lobp[1] = (DCILobLocator *) 0;

	if (alloc_lob_desc(pEnv, &lobp[0]) == DCI_ERROR
		|| alloc_lob_desc(pEnv, &lobp[1]) == DCI_ERROR)
	{
		ret = DCI_ERROR;
		printf("alloc lob failed\n");
		goto free_resource;
	}
	
	if (DCILobCreateTemporary(pSvcCtx, pError, lobp[0], DCI_DEFAULT, SQLCS_IMPLICIT, DCI_TEMP_BLOB, true, DCI_DURATION_SESSION) ||
		DCILobCreateTemporary(pSvcCtx, pError, lobp[1], DCI_DEFAULT, SQLCS_IMPLICIT, DCI_TEMP_CLOB, true, DCI_DURATION_SESSION))
	{
		ret = DCI_ERROR;
		printf("DCILobCreateTemporary failed.\n");
		report_error(pError);
		goto free_resource;
	}

	DCITransStart(pSvcCtx, pError, 0, 0);

	if (DCILobOpen(pSvcCtx, pError, lobp[0], DCI_LOB_READWRITE) ||
		DCILobOpen(pSvcCtx, pError, lobp[1], DCI_LOB_READWRITE))
	{
		ret = DCI_ERROR;
		printf("DCILobOpen failed.\n");
		report_error(pError);
		goto free_resource;
	}

	
	if (DCILobIsOpen(pSvcCtx, pError, lobp[0], &isOpen[0]) ||
		DCILobIsOpen(pSvcCtx, pError, lobp[1], &isOpen[1]))
	{
		ret = DCI_ERROR;
		printf("DCILobIsOpen failed.\n");
		report_error(pError);
		goto free_resource;
	}

	if (!isOpen[0] || !isOpen[1])
	{
		ret = DCI_ERROR;
		printf("DCILobIsOpen dectected lob not open\n");
		report_error(pError);
		goto free_resource;
	}

	if (DCILobIsTemporary(pEnv, pError, lobp[0], &isTemp[0]) ||
		DCILobIsTemporary(pEnv, pError, lobp[1], &isTemp[1]))
	{
		ret = DCI_ERROR;
		printf("DCILobIsTemporary failed.\n");
		report_error(pError);
		goto free_resource;
	}

	if (!isTemp[0] || !isTemp[1])
	{
		ret = DCI_ERROR;
		printf("DCILobIsTemporary's result failed.\n");
		report_error(pError);
		goto free_resource;
	}

	amt[0] = sizeof(blobdata);
	amt[1] = strlen(clobdata);
	if (DCILobWrite(pSvcCtx, pError, lobp[0], &amt[0], 1, (dvoid*)blobdata, sizeof(blobdata), DCI_ONE_PIECE, NULL, NULL, DCI_DEFAULT, SQLCS_IMPLICIT) ||
		DCILobWrite(pSvcCtx, pError, lobp[1], &amt[1], 1, (dvoid*)clobdata, strlen(clobdata), DCI_ONE_PIECE, NULL, NULL, DCI_DEFAULT, SQLCS_IMPLICIT))
	{
		ret = DCI_ERROR;
		printf("DCILobWrite failed.\n");
		report_error(pError);
		goto free_resource;
	}

	memset(amt, 0, sizeof(amt));
	memset(clobdataout, 0, sizeof(clobdataout));
	memset(blobdataout, 0, sizeof(blobdataout));

	if (DCILobRead(pSvcCtx, pError, lobp[0], &amt[0], 1, (dvoid*)blobdataout, sizeof(blobdataout), NULL, NULL, DCI_DEFAULT, SQLCS_IMPLICIT) ||
		DCILobRead(pSvcCtx, pError, lobp[1], &amt[1], 1, (dvoid*)clobdataout, sizeof(clobdataout), NULL, NULL, DCI_DEFAULT, SQLCS_IMPLICIT))
	{
		ret = DCI_ERROR;
		printf("DCILobRead failed.\n");
		report_error(pError);
		goto free_resource;
	}

	if (DCILobClose(pSvcCtx, pError, lobp[0]) ||
		DCILobClose(pSvcCtx, pError, lobp[1]))
	{
		ret = DCI_ERROR;
		printf("DCILobClose failed.\n");
		report_error(pError);
		goto free_resource;
	}

	if (DCILobIsOpen(pSvcCtx, pError, lobp[0], &isOpen[0]) ||
		DCILobIsOpen(pSvcCtx, pError, lobp[1], &isOpen[1]))
	{
		ret = DCI_ERROR;
		printf("DCILobIsOpen failed.\n");
		report_error(pError);
		goto free_resource;
	}

	if (isOpen[0] || isOpen[1])
	{
		ret = DCI_ERROR;
		printf("DCILobIsOpen dectected closed lob open\n");
		report_error(pError);
		goto free_resource;
	}


	DCITransCommit(pSvcCtx, pError, DCI_DEFAULT);

	/* persists the temporary lobs */
	execute("drop table if exists temp_lob_test");
	execute("create table temp_lob_test(_blob blob, _clob clob)");
//	execute("create table temp_lob_test(_blob bytea, _clob text)");

	if (DCIHandleAlloc(pEnv, (dvoid**)&stmthp, DCI_HTYPE_STMT, 0, NULL))
	{
		ret = DCI_ERROR;
		printf("Failed to alloc stmt handle:");
		report_error(pError);
		goto free_resource;
	}

	if (DCIStmtPrepare(stmthp, pError, (text*)"insert into temp_lob_test values(:blob, :clob)", 
		strlen("insert into temp_lob_test values(:blob, :clob)"), DCI_NTV_SYNTAX, DCI_DEFAULT))
	{
		ret = DCI_ERROR;
		printf("Prepare failed:");
		report_error(pError);
		goto free_resource;
	}

	if (DCIBindByName(stmthp, &bindp[0], pError, (text*)"blob", -1, &lobp[0], sizeof(lobp[0]), SQLT_BLOB, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT) ||
		DCIBindByName(stmthp, &bindp[1], pError, (text*)"clob", -1, &lobp[1], sizeof(lobp[1]), SQLT_CLOB, NULL, NULL, NULL, 0, NULL, DCI_DEFAULT))
	{
		ret = DCI_ERROR;
		printf("BindByName failed:");
		report_error(pError);
		goto free_resource;
	}

	if (DCIStmtExecute(pSvcCtx, stmthp, pError, 1, 0, NULL, NULL, DCI_DEFAULT))
	{
		ret = DCI_ERROR;
		printf("Execute failed:");
		report_error(pError);
		goto free_resource;
	}

	

	if (DCILobFreeTemporary(pSvcCtx, pError, lobp[0]) ||
		DCILobFreeTemporary(pSvcCtx, pError, lobp[1]))
	{
		ret = DCI_ERROR;
		printf("DCILobFreeTemporary failed.\n");
		report_error(pError);
		goto free_resource;
	}

	if (strcmp(clobdataout, clobdata) ||
		memcmp(blobdataout, blobdata, sizeof(blobdata)))
	{
		ret  = DCI_ERROR;
		printf("Data compare failed.\n");
		goto free_resource;
	}

	if (DCIDescriptorFree(lobp[0], DCI_DTYPE_LOB) ||
		DCIDescriptorFree(lobp[1], DCI_DTYPE_LOB))
	{
		ret = DCI_ERROR;
		printf("Descriptor of lobs free failed.");
		report_error(pError);
		goto free_resource;
	}

	lobp[0] = lobp[1] = NULL;
	/* disconnect, and then the session will be an another one, 
	 * temp lobs will be free, but permanent ones will not be. */
	disconnect(true);
	DCIHandleFree(stmthp, DCI_HTYPE_STMT);
	stmthp = NULL;
	
	connect(true);

	if (alloc_lob_desc(pEnv, &lobp[0]) == DCI_ERROR
		|| alloc_lob_desc(pEnv, &lobp[1]) == DCI_ERROR)
	{
		ret = DCI_ERROR;
		printf("alloc lob failed\n");
		goto free_resource;
	}
	if (DCIHandleAlloc(pEnv, (dvoid**)&stmthp, DCI_HTYPE_STMT, 0, NULL))
	{
		ret = DCI_ERROR;
		printf("Alloc stmt failed:");
		report_error(pError);
		goto free_resource;
	}

	if (DCIStmtPrepare(stmthp, pError, (text*)"select _blob, _clob from temp_lob_test", strlen("select _blob, _clob from temp_lob_test"), DCI_NTV_SYNTAX, DCI_DEFAULT))
	{
		ret = DCI_ERROR;
		printf("prepare failed:");
		report_error(pError);
		goto free_resource;
	}

	memset(clobdataout, 0, sizeof(clobdataout));
	memset(blobdataout, 0, sizeof(blobdataout));
	if (DCIDefineByPos(stmthp, &defp[0], pError, 1, &lobp[0], sizeof(lobp[0]), SQLT_BLOB, NULL, NULL, NULL, DCI_DEFAULT) ||
		DCIDefineByPos(stmthp, &defp[1], pError, 2, &lobp[1], sizeof(lobp[1]), SQLT_CLOB, NULL, NULL, NULL, DCI_DEFAULT))
	{
		ret = DCI_ERROR;
		printf("Failed to defineByPos:");
		report_error(pError);
		goto free_resource;
	}
	
	if (DCIStmtExecute(pSvcCtx, stmthp, pError, 1, 0, NULL, NULL, DCI_DEFAULT))
	{
		ret = DCI_ERROR;
		printf("select failed:");
		report_error(pError);
		goto free_resource;
	}

	memset(amt, 0, sizeof(amt));

	if (DCITransStart(pSvcCtx, pError, 0, DCI_DEFAULT))
	{
		ret = DCI_ERROR;
		printf("Failed to start transaction\n");
		report_error(pError);
		goto free_resource;
	}

	if (DCILobRead(pSvcCtx, pError, lobp[0], &amt[0], 1, blobdataout, sizeof(blobdataout), NULL, NULL, 0, 0) ||
		DCILobRead(pSvcCtx, pError, lobp[1], &amt[1], 1, clobdataout, sizeof(clobdataout), NULL, NULL, 0, 0))
	{
		ret = DCI_ERROR;
		printf("failed to read lobs");
		report_error(pError);
		goto free_resource;
	}

	if (DCITransCommit(pSvcCtx, pError, DCI_DEFAULT))
	{
		ret = DCI_ERROR;
		printf("Failed to commit transaction\n");
		report_error(pError);
		goto free_resource;
	}

	if (strcmp(clobdataout, clobdata) ||
		memcmp(blobdataout, blobdata, sizeof(blobdata)))
	{
		ret  = DCI_ERROR;
		printf("Data compare failed.\n");
		goto free_resource;
	}

	if (DCILobIsTemporary(pEnv, pError, lobp[0], &isTemp[0]) ||
		DCILobIsTemporary(pEnv, pError, lobp[1], &isTemp[1]))
	{
		ret = DCI_ERROR;
		printf("DCILobIsTemporary failed.\n");
		report_error(pError);
		goto free_resource;
	}

	if (isTemp[0] || isTemp[1])
	{
		ret = DCI_ERROR;
		printf("DCILobIsTemporary: permanent lob's result not correct.\n");
		report_error(pError);
		goto free_resource;
	}

	if (DCIDescriptorFree(lobp[0], DCI_DTYPE_LOB) ||
		DCIDescriptorFree(lobp[1], DCI_DTYPE_LOB))
	{
		ret = DCI_ERROR;
		printf("Descriptor of lobs free failed.");
		report_error(pError);
		goto free_resource;
	}
	lobp[0] = lobp[1] = NULL;

free_resource:

	if (lobp[0])
	{
		err = DCIDescriptorFree((dvoid *) lobp[0], (ub4) DCI_DTYPE_LOB);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIDescriptorFree lobp[0] failed\n");
			report_error(pError);
		}
	}

	if (lobp[1])
	{
		err = DCIDescriptorFree((dvoid *) lobp[1], (ub4) DCI_DTYPE_LOB);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			printf("DCIDescriptorFree lobp[1] failed\n");
			report_error(pError);
		}
	}

end:
	err = disconnect(true);
	if (err != DCI_SUCCESS)
		ret = DCI_ERROR;

	if(stmthp != NULL)
	{
		DCIHandleFree(stmthp, DCI_HTYPE_STMT);
		stmthp = NULL;
	}
	
	if(pStmt != NULL)
	{
		DCIHandleFree(pStmt, DCI_HTYPE_STMT);
		pStmt = NULL;
	}
	return ret;
}

sword TestDuration()
{
	DCIDuration		dur;
	DCILobLocator	*blob = NULL, *clob = NULL;
	sword			ret = DCI_ERROR;
	char			buf[] = "no zuo no die why you try, no try no high give me five.";
	ub4				amt = sizeof(buf);

	if (connect(true))
		return ret;

	if (DCIDescriptorAlloc(pEnv, (dvoid**)&blob, DCI_DTYPE_LOB, 0, NULL) ||
		DCIDescriptorAlloc(pEnv, (dvoid**)&clob, DCI_DTYPE_LOB, 0, NULL))
	{
		report_error(pError);
		goto free_resource;
	}

	if (DCIDurationBegin(pEnv, pError, pSvcCtx, DCI_DURATION_SESSION, &dur))
	{
		printf("ERROR: Failed to begin duration.\n");
		goto free_resource;
	}

	if (DCILobCreateTemporary(pSvcCtx, pError, blob, 0, 0, DCI_TEMP_BLOB, false, dur) ||
		DCILobCreateTemporary(pSvcCtx, pError, clob, 0, 0, DCI_TEMP_CLOB, false, dur))
	{
		printf("ERROR: Failed to create temp lobs.\n");
		report_error(pError);
		goto free_resource;
	}

	if (DCITransStart(pSvcCtx, pError, 0, DCI_DEFAULT))
	{
		printf("ERROR: Failed to start transaction: ");
		report_error(pError);
		goto free_resource;
	}

	if (DCILobOpen(pSvcCtx, pError, blob, DCI_LOB_READWRITE) ||
		DCILobOpen(pSvcCtx, pError, clob, DCI_LOB_READWRITE))
	{
		printf("ERROR: Failed to open lobs:");
		report_error(pError);
		goto free_resource;
	}

	if (DCILobWrite(pSvcCtx, pError, blob, &amt, 1, buf, amt, DCI_FIRST_PIECE, NULL, NULL, 0, 0) ||
		DCILobWrite(pSvcCtx, pError, clob, &amt, 1, buf, amt, DCI_FIRST_PIECE, NULL, NULL, 0, 0))
	{
		printf("ERROR: Failed to write lobs\n");
		report_error(pError);
		goto free_resource;
	}

	if (DCIDurationEnd(pEnv, pError, pSvcCtx, dur))
	{
		printf("ERROR: Failed to end duration.\n");
		report_error(pError);
		goto free_resource;
	}

	amt = sizeof(buf);
	DCITransRollback(pSvcCtx, pError, DCI_DEFAULT);

	ret = DCI_SUCCESS;
free_resource:
	if (NULL != blob)
		DCIDescriptorFree(blob, DCI_DTYPE_LOB);
	if (NULL != clob)
		DCIDescriptorFree(clob, DCI_DTYPE_LOB);
	disconnect(true);
	return ret;
}

sword stream_write_lob(DCISvcCtx *svchp, DCIError *errhp, DCILobLocator *lobp)
{
	char  *bufp = (char*)malloc(sizeof(char) * BUFLEN);
	ub4   offset = 1;
	ub4   loblen = 0;
	ub4   amtp = FILELEN;  
	ub1   piece;
	sword retval = DCI_SUCCESS;
	ub4   nbytes;       
	ub4   remainder = FILELEN;  

	(void) printf("--> To do streamed write lob, amount = %d\n", FILELEN);

	(void) DCILobGetLength(svchp, errhp, lobp, &loblen);
	(void) printf("Before stream write, LOB length = %d\n\n", loblen);

  	if (FILELEN > BUFLEN)
		nbytes = BUFLEN;
  	else
		nbytes = FILELEN;

  	memset(bufp, 'a', nbytes);

  	remainder -= nbytes; 

	if (remainder == 0)       /* exactly one piece in the file */
	{
		(void) printf("Only one piece, no need for stream write.\n");
		if (retval = DCILobWrite(svchp, errhp, lobp, &amtp, offset, (dvoid *) bufp,
		(ub4) nbytes, DCI_ONE_PIECE, (dvoid *)0,
		(DCICallbackLobWrite) 0,
		(ub2) 0, (ub1) SQLCS_IMPLICIT) != DCI_SUCCESS)
		{
			  (void) printf("ERROR: DCILobWrite(), retval = %d\n", retval);
			  return retval;
		}
	}
  	else                     /* more than one piece */
  	{
	    if (DCILobWrite(svchp, errhp, lobp, &amtp, offset, (dvoid *) bufp,
	    (ub4) BUFLEN, DCI_FIRST_PIECE, (dvoid *)0,
	    (DCICallbackLobWrite) 0,
	    (ub2) 0, (ub1) SQLCS_IMPLICIT) != DCI_NEED_DATA)
	    {
	      	(void) printf("ERROR: DCILobWrite().\n");
	      	return retval;
	    }

		piece = DCI_NEXT_PIECE;

		do
	    {
	      	if (remainder > BUFLEN)
				nbytes = BUFLEN;
	      	else
		    {
				nbytes = remainder;
				piece = DCI_LAST_PIECE;
		    }

	    retval = DCILobWrite(svchp, errhp, lobp, &amtp, offset, (dvoid *) bufp,
	    (ub4) nbytes, piece, (dvoid *)0,
	    (DCICallbackLobWrite) 0,
	    (ub2) 0, (ub1) SQLCS_IMPLICIT);
	    remainder -= nbytes;

	    } while (retval == DCI_NEED_DATA);
  	}

	if (retval != DCI_SUCCESS)
	{
		(void) printf("Error: stream writing LOB.\n");
		return retval;
	}

  	(void) DCILobGetLength(svchp, errhp, lobp, &loblen);
  	(void) printf("After stream write, LOB length = %d\n\n", loblen);
	if(bufp != NULL)
		free(bufp);
  	return retval;
}

sword stream_read_lob(DCISvcCtx *svchp, DCIError *errhp, DCILobLocator *lobp)
{
	ub4     offset = 1;
	ub4     loblen = 0;
	ub1     bufp[BUFLEN];
	ub4     amtp = 0;
	sword   retval;
	ub4     piece = 0;
	ub4     remainder;

	(void) DCILobGetLength(svchp, errhp, lobp, &loblen);
	amtp = loblen;

	(void) printf("--> To stream read LOB, loblen = %d.\n", loblen);

	memset((void *)bufp, '\0', BUFLEN);

	retval = DCILobRead(svchp, errhp, lobp, &amtp, offset, (dvoid *) bufp,
	(loblen < BUFLEN ? loblen : BUFLEN), (dvoid *)0,
	(DCICallbackLobRead) 0,
	(ub2) 0, (ub1) SQLCS_IMPLICIT);

	switch (retval)
	{
	case DCI_SUCCESS:             /* only one piece */
	    (void) printf("stream read %d th piece\n", ++piece);
	    (void) printf("read buf %.1024s \n",  bufp);
	    break;
	case DCI_ERROR:
	    //report_error();
	    break;
	case DCI_NEED_DATA:           /* there are 2 or more pieces */
	    (void) printf("stream read %d th piece, amtp = %d\n", ++piece, amtp);
	    (void) printf("read buf %.1024s \n",  bufp);
	    remainder = loblen;

	    do
	    {
	        memset((void *)bufp, '\0', BUFLEN);
	        amtp = 0;

	        remainder -= BUFLEN;

	        retval = DCILobRead(svchp, errhp, lobp, &amtp, offset, (dvoid *) bufp,
	            (ub4) BUFLEN, (dvoid *)0,
	            (DCICallbackLobRead) 0,
	            (ub2) 0, (ub1) SQLCS_IMPLICIT);

	        /* the amount read returned is undefined for FIRST, NEXT pieces */
	        (void) printf("stream read %d th piece, amtp = %d\n", ++piece, amtp);
	        (void) printf("read buf %.1024s \n",  bufp);

	    } while (retval == DCI_NEED_DATA);
	    break;
	default:
	    (void) printf("Unexpected ERROR: DCILobRead() LOB.\n");
	    break;
	}

	return retval;
}

sword TestLobStream()
{
	DCIStmt *pStmt = (DCIStmt *) NULL;
	DCIDefine   *dfnhp[2];
	DCILobLocator *lobp[2];
	text      *sqlstmt = (text *)"SELECT C2, C3 FROM TAB1";
	sword ret = DCI_SUCCESS;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	err = DCIHandleAlloc((void *)pEnv, (void **)&pStmt, DCI_HTYPE_STMT, 0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = execute("drop table if exists TAB1");
	err = execute("CREATE TABLE TAB1(C1 INT PRIMARY KEY, C2 CLOB, C3 BLOB)");
	err = execute("INSERT INTO TAB1 VALUES(1, EMPTY_CLOB(),  EMPTY_BLOB());");
//	err = execute("CREATE TABLE TAB1(C1 INT PRIMARY KEY, C2 TEXT, C3 BYTEA)");
//	err = execute("INSERT INTO TAB1 VALUES(1, NULL,  NULL);");


	lobp[0] = (DCILobLocator *) 0;
	lobp[1] = (DCILobLocator *) 0;

	alloc_lob_desc(pEnv, &lobp[0]);
	alloc_lob_desc(pEnv, &lobp[1]);

	if (DCIStmtPrepare(pStmt, pError, sqlstmt, (ub4)strlen((char *)sqlstmt),
	(ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		ret = DCI_ERROR;
	    	goto free_resource;
	}

	dfnhp[0] = (DCIDefine *)0;
	dfnhp[1] = (DCIDefine *)0;

	if (DCIDefineByPos(pStmt, &dfnhp[0], pError, (ub4)1,
	(dvoid *)&lobp[0], (sb4)4, (ub2)SQLT_CLOB,
	(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		ret = DCI_ERROR;
        	goto free_resource;
	}
	if (DCIDefineByPos(pStmt, &dfnhp[1], pError, (ub4)2,
	(dvoid *)&lobp[1], (sb4)4, (ub2)SQLT_BLOB,
	(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		ret = DCI_ERROR;
        	goto free_resource;
	}

	if (DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4)1, (ub4)0,
	(CONST DCISnapshot*)0, (DCISnapshot*)0,
	(ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtExecute() select\n");
		ret = DCI_ERROR;
	    	goto free_resource;
	}

	stream_write_lob(pSvcCtx, pError, lobp[0]);
	stream_write_lob(pSvcCtx, pError, lobp[1]);

	stream_read_lob(pSvcCtx, pError, lobp[0]);
	stream_read_lob(pSvcCtx, pError, lobp[1]);

	DCIDescriptorFree((dvoid *) lobp[0], (ub4)DCI_DTYPE_LOB);
	DCIDescriptorFree((dvoid *) lobp[1], (ub4)DCI_DTYPE_LOB);

free_resource:
	err = execute("drop table if exists TAB1");
	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{       
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

end:
        err = disconnect(true);
        return ret;
}

sb4 cbk_write_buffer(dvoid *ctxp, dvoid *bufxp, ub4 *lenp, ub1 *piece)
{
	ub4    nbytes;
	static ub4 len = BUFLEN;     


	if ((FILELEN - len) > BUFLEN)
		nbytes = BUFLEN;
	else
		nbytes = FILELEN - len;

	*lenp = nbytes;

	memset(bufxp, 'a', nbytes);

	len += nbytes;

	if (len == FILELEN)         
	{
		*piece = DCI_LAST_PIECE;
		len = BUFLEN;          
	}
	else
		*piece = DCI_NEXT_PIECE;

	return DCI_CONTINUE;
}

sword callback_write_lob(DCISvcCtx *svchp, DCIError *errhp, DCILobLocator *lobp)
{
	ub4   offset = 1;
	ub4   loblen = 0;
	char  *bufp = (char*)malloc(sizeof(char) * BUFLEN);
	ub4   amtp = FILELEN;
	ub4   nbytes;
	sword retval;

	(void) printf("--> To do callback write lob, amount = %d\n", FILELEN);

	(void) DCILobGetLength(svchp, errhp, lobp, &loblen);
	(void) printf("Before callback write, LOB length = %d\n\n", loblen);

	if (FILELEN > BUFLEN)
		nbytes = BUFLEN;
	else
		nbytes = FILELEN;

	memset(bufp, 'a', nbytes);

	if (FILELEN < BUFLEN)       
	{
		(void) printf("Only one piece, no need for callback write.\n");
		if (retval = DCILobWrite(svchp, errhp, lobp, &amtp, offset, (dvoid *) bufp,
		(ub4) nbytes, DCI_ONE_PIECE, (dvoid *)0,
		(DCICallbackLobWrite) 0,
		(ub2) 0, (ub1) SQLCS_IMPLICIT) != DCI_SUCCESS)
		{
			(void) printf("ERROR: DCILobWrite().\n");
			return retval;
		}
	}
	else                    
	{
		if (retval = DCILobWrite(svchp, errhp, lobp, &amtp, offset, (dvoid *)bufp,
		(ub4)nbytes, DCI_FIRST_PIECE, (dvoid *)0,
		cbk_write_buffer, (ub2) 0, (ub1) SQLCS_IMPLICIT))
		{
			(void) printf("ERROR: DCILobWrite().\n");
			return retval; 
		}
	}

	(void) DCILobGetLength(svchp, errhp, lobp, &loblen);
	(void) printf("After callback write, LOB length = %d\n\n", loblen);
	if(NULL != bufp)
		free(bufp);
	return retval;
}

sb4 cbk_read_buffer(dvoid *ctxp, CONST dvoid *bufxp, ub4 lenp, ub1 piece)
{
	static ub4 piece_count = 0;

	piece_count++;

	switch (piece)
	{
		case DCI_LAST_PIECE:
			(void) printf("callback read the %d th piece\n", piece_count);
			piece_count = 0;
			(void) printf("callback read data length %d \n", lenp);
			(void) printf("callback read data %s \n", bufxp);
			return DCI_CONTINUE;

		case DCI_FIRST_PIECE:
		case DCI_NEXT_PIECE:
			break;
		default:
			(void) printf("callback read error: unkown piece = %d.\n", piece);
			return DCI_ERROR;
	}

	(void) printf("callback read the %d th piece\n", piece_count);
	(void) printf("callback read data length%d \n", lenp);
	(void) printf("callback read data %s \n", bufxp);
	return DCI_CONTINUE;
}

sword callback_read_lob(DCISvcCtx *svchp, DCIError *errhp, DCILobLocator *lobp)
{
	ub4     offset = 1;
	ub4     loblen = 0;
	ub1     bufp[BUFLEN];
	ub4     amtp = 0;
	sword   retval;

	(void) DCILobGetLength(svchp, errhp, lobp, &loblen);
	amtp = loblen;

	(void) printf("--> To stream read LOB, loblen = %d.\n", loblen);

	if(retval = DCILobRead(svchp, errhp, lobp, &amtp, offset, (dvoid *) bufp,
	(ub4) BUFLEN, (dvoid *)bufp,
	(DCICallbackLobRead) cbk_read_buffer,
	(ub2) 0, (ub1) SQLCS_IMPLICIT))
	{
		(void) printf("ERROR: DCILobRead() LOB.\n");
	}
	return retval;
}

sword TestLobCallback()
{
	DCIStmt *pStmt = (DCIStmt *) NULL;
	DCIDefine   *dfnhp[2];
	DCILobLocator *lobp[2];
	text      *sqlstmt = (text *)"SELECT C2, C3 FROM TAB2 WHERE C1=1 FOR UPDATE";
	sword ret = DCI_SUCCESS;

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	err = DCIHandleAlloc((dvoid *)pEnv, (dvoid **) &pStmt,
	(ub4)DCI_HTYPE_STMT, (CONST size_t) 0, (dvoid **) 0);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = execute("drop table if exists TAB2");
	err = execute("CREATE TABLE TAB2(C1 INT PRIMARY KEY, C2 CLOB, C3 BLOB)");
	err = execute("INSERT INTO TAB2 VALUES(1, EMPTY_CLOB(),  EMPTY_BLOB());");
//	err = execute("CREATE TABLE TAB2(C1 INT PRIMARY KEY, C2 TEXT, C3 BYTEA)");
//	err = execute("INSERT INTO TAB2 VALUES(1, NULL,  NULL);");


	lobp[0] = (DCILobLocator *) 0;
	lobp[1] = (DCILobLocator *) 0;

	alloc_lob_desc(pEnv, &lobp[0]);
	alloc_lob_desc(pEnv, &lobp[1]);

	if (DCIStmtPrepare(pStmt, pError, sqlstmt, (ub4)strlen((char *)sqlstmt),
	(ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	dfnhp[0] = (DCIDefine *)0;
	dfnhp[1] = (DCIDefine *)0;

	if (DCIDefineByPos(pStmt, &dfnhp[0], pError, (ub4)1,
	(dvoid *)&lobp[0], (sb4)4, (ub2)SQLT_CLOB,
	(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		ret = DCI_ERROR;
		goto free_resource;
	}
	if (DCIDefineByPos(pStmt, &dfnhp[1], pError, (ub4)2,
	(dvoid *)&lobp[1], (sb4)4, (ub2)SQLT_BLOB,
	(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	if (DCIStmtExecute(pSvcCtx, pStmt, pError, (ub4)1, (ub4)0,
	(CONST DCISnapshot*)0, (DCISnapshot*)0,
	(ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtExecute() select\n");
		ret = DCI_ERROR;
		goto free_resource;
	}

	callback_write_lob(pSvcCtx, pError, lobp[0]);
	callback_write_lob(pSvcCtx, pError, lobp[1]);

	callback_read_lob(pSvcCtx, pError, lobp[0]);
	callback_read_lob(pSvcCtx, pError, lobp[1]);

	DCIDescriptorFree((dvoid *) lobp[0], (ub4)DCI_DTYPE_LOB);
	DCIDescriptorFree((dvoid *) lobp[1], (ub4)DCI_DTYPE_LOB);

free_resource:
	err = execute("drop table if exists TAB2");
	err = DCIHandleFree((dvoid *) pStmt, (ub4) DCI_HTYPE_STMT);
	if (err != DCI_SUCCESS)
	{       
		printf("DCIHandleFree DCI_HTYPE_STMT failed\n");
	}

end:
	err = disconnect(true);
	return ret;
}
