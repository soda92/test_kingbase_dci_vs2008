#include "DciTest.h"
#include "testDocCases.h"

DCIEnv *envhp;      //�������
DCIError *errhp;	//������
DCISvcCtx *svchp;	//�����ľ��
DCIStmt *stmthp = (DCIStmt *) NULL; //�����

//��ʼ�����־��
sword 
init_handles(DCIEnv **envhp, DCIError **errhp)
{
	//printf("Environment setup ....\n");

	/* ��ʼ��DCIӦ�û��� */
	if (DCIInitialize(DCI_DEFAULT, (dvoid *)0,
		(dvoid * (*)(dvoid *, size_t))0,
		(dvoid * (*)(dvoid *, dvoid *, size_t))0,
		(void (*)(dvoid *, dvoid *))0))
	{
		printf("FAILED: DCIInitialize()\n");
		return DCI_ERROR;
	}

	/* ��ʼ��������� */
	if (DCIEnvInit((DCIEnv **)envhp, (ub4)DCI_DEFAULT,
		(size_t)0, (dvoid **)0))
	{
		printf("FAILED: DCIEnvInit()\n");
		return DCI_ERROR;
	}

	/* �ӻ�������Ϸ���һ��������Ϣ��� */
	if (DCIHandleAlloc((dvoid *)*envhp, (dvoid **)errhp,
		(ub4)DCI_HTYPE_ERROR, (size_t)0, (dvoid **)0))
	{
		printf("FAILED: DCIHandleAlloc() on errhp\n");
		return DCI_ERROR;
	}

	return DCI_SUCCESS;
}

//ʹ�ø������û����Ϳ����¼��ָ�������ݿ������  
sword
log_on(DCIEnv *envhp, DCIError *errhp,
	   char *uid, char *pwd, char *DbName)
{
	//printf("Logging on as %s  ....\n", uid);
	/* �������ݿ⣬�� DCILogon �з��� Service handle */
	if (DCILogon(envhp, errhp, &svchp, (const DciText *)uid, (ub4)strlen(uid),
		(const DciText *)pwd, (ub4)strlen(pwd), (const DciText *)DbName, (ub4)strlen(DbName)))
	{
		printf("FAILED: DCILogon()\n");
		return DCI_ERROR;
	}
	printf("%s logged on.\n", uid);

	return DCI_SUCCESS;
}

//�Ͽ����ӣ����ͷŸ��־��
sword 
finish_demo(boolean loggedon, DCIEnv *envhp, DCISvcCtx *svchp, 
			DCIError *errhp, DCIStmt *stmthp)
{
	if (stmthp)
		DCIHandleFree((dvoid *)stmthp, (ub4)DCI_HTYPE_STMT);

	printf("logoff ...\n\n");
	if (loggedon)
		DCILogoff(svchp, errhp);

	//printf("Freeing handles ...\n");
	if (svchp)
		DCIHandleFree((dvoid *) svchp, (ub4)DCI_HTYPE_SVCCTX);
	if (errhp)
		DCIHandleFree((dvoid *) errhp, (ub4)DCI_HTYPE_ERROR);
	if (envhp)
		DCIHandleFree((dvoid *) envhp, (ub4)DCI_HTYPE_ENV);

	return DCI_SUCCESS;
}

//��ӡ������Ϣ
void 
report_error_doc(DCIError *errhp)
{
	text  msgbuf[512];
	sb4   errcode = 0;

	DCIErrorGet((dvoid *)errhp, (ub4)1, (text *)NULL, &errcode,
		msgbuf, (ub4)sizeof(msgbuf), (ub4)DCI_HTYPE_ERROR);
	printf("ERROR CODE = %d", errcode);
	printf("%.*s\n", 512, msgbuf);

	return;
}

sword ExecOneSQLWithoutResult()
{
	int logged_on = FALSE;
	char sql[1024];

	//��ʼ�����־��
	if (init_handles(&envhp, &errhp))
	{
		printf("FAILED: init_handles()\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}
	//��¼�����ݿ����
	if (log_on(envhp, errhp, User, Pwd, DbName))
	{
		printf("FAILED: log_on()\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}

	logged_on = TRUE;
	if (DCIHandleAlloc((dvoid *)envhp, (dvoid **)&stmthp,
		(ub4)DCI_HTYPE_STMT, (CONST size_t)0, (dvoid **)0))
	{
		printf("FAILED: alloc statement handle\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}

	//׼��һ���������
	strcpy(sql, "drop table if exists T;"); 
	if (DCIStmtPrepare(stmthp, errhp, (unsigned char *)sql, (ub4)strlen((char *)sql),
		(ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}
	//�ǲ�ѯ������䣬iters��������>=1
	if (DCIStmtExecute(svchp, stmthp, errhp, (ub4)1, (ub4)0,
		(CONST DCISnapshot*)0, (DCISnapshot*)0,
		(ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtExecute() select\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}

	//׼��һ���������
	strcpy(sql, "create table T(c1 int, c2 real, c3 char(100 byte), c4 varchar(100 byte), c5 float, c6 numeric(22,7), c7 date);"); 
	if (DCIStmtPrepare(stmthp, errhp, (unsigned char *)sql, (ub4)strlen((char *)sql),
		(ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}
	//�ǲ�ѯ������䣬iters��������>=1
	if (DCIStmtExecute(svchp, stmthp, errhp, (ub4)1, (ub4)0,
		(CONST DCISnapshot*)0, (DCISnapshot*)0,
		(ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtExecute() select\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}

	strcpy(sql, "insert into T values (1, 1.1, 'a', 'abc', 1.2, 1.3, 'January 8, 1999');"); 
	if (DCIStmtPrepare(stmthp, errhp, (unsigned char *)sql, (ub4)strlen((char *)sql),
		(ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}

	if (DCIStmtExecute(svchp, stmthp, errhp, (ub4)1, (ub4)0,
		(CONST DCISnapshot*)0, (DCISnapshot*)0,
		(ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtExecute() select\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}

	//�ǳ����ݿ���񣬲�������־��
	return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
}

/* ���а󶨵���ʽ����ȡ���ݣ�ÿһ�е�ֵ�������Ĵ����һ��
 * rows������ʾÿ�λ�ȡ����������
 * �ٶ���T�ĽṹΪ
 * T(C1 INT, C2 REAL, C3 CHAR(100 BYTE), C4 VARCHAR(100 BYTE), 
 *  C5 FLOAT, C6 NUMERIC(22,7), C7 TIME)
 */
#define CHAR_SIZE 101
sword
Select_ColumnBind(ub2 rows)
{
	text    *sqlstmt = (text*)"select C1, C2, C3, C4, C5, C6, C7 from T";
	int     *c1 = (int*) malloc (sizeof(int) * rows);
	float   *c2 = (float*) malloc (sizeof(float) * rows);
	DCIDate *c7 = (DCIDate*) malloc (sizeof(DCIDate) * rows);
	char    *c3 = (char*) malloc (sizeof(char) * rows * CHAR_SIZE);
	char    *c4 = (char*) malloc (sizeof(char) * rows * CHAR_SIZE);
	float   *c5 = (float*) malloc (sizeof(float) * rows);
	DCINumber   *c6 = (DCINumber*) malloc (sizeof(DCINumber) * rows);
	DCIDefine *bndhp[7];
	ub4     stmrow, stmrowEx;

	//׼����ѯ���
	if (DCIStmtPrepare(stmthp, errhp, sqlstmt, (ub4)strlen((char *)sqlstmt),
		(ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	
	memset(c1, 0, sizeof(int) * rows);
	memset(c2, 0, sizeof(int) * rows);
	memset(c3, 0, rows * CHAR_SIZE);
	memset(c4, 0, rows * CHAR_SIZE);
	memset(c5, 0, sizeof(float)*rows);
	memset(c6, 0, sizeof(DCINumber)*rows);
	memset(c7, 0, sizeof(DCIDate)*rows);

	//�󶨻����������Ӧ������
	if (DCIDefineByPos(stmthp, &bndhp[0], errhp, 1,
		(dvoid *)c1, (sb4)sizeof(int), (ub2)SQLT_INT,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineByPos(stmthp, &bndhp[1], errhp, 2,
		(dvoid *)c2, (sb4)sizeof(float), (ub2)SQLT_FLT,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}	
	if (DCIDefineByPos(stmthp, &bndhp[2], errhp, 3,
		(dvoid *)c3, (sb4)CHAR_SIZE, (ub2)SQLT_CHR,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineByPos(stmthp, &bndhp[3], errhp, 4,
		(dvoid *)c4, (sb4)CHAR_SIZE, (ub2)SQLT_STR,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineByPos(stmthp, &bndhp[4], errhp, 5,
		(dvoid *)c5, (sb4)sizeof(float), (ub2)SQLT_FLT,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineByPos(stmthp, &bndhp[5], errhp, 6,
		(dvoid *)c6, (sb4)sizeof(DCINumber), (ub2)SQLT_VNU,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineByPos(stmthp, &bndhp[6], errhp, 7,
		(dvoid *)c7, (sb4)sizeof(DCIDate), (ub2)SQLT_ODT,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	//ָ��ÿһ�е�ÿ��ֵ�ļ������
	if (DCIDefineArrayOfStruct(bndhp[0], errhp, sizeof(int),
		0, 0, 0))
	{
		printf("FAILED: DCIDefineArrayOfStruct()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineArrayOfStruct(bndhp[1], errhp, sizeof(float),
		0, 0, 0))
	{
		printf("FAILED: DCIDefineArrayOfStruct()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineArrayOfStruct(bndhp[2], errhp, CHAR_SIZE,
		0, 0, 0))
	{
		printf("FAILED: DCIDefineArrayOfStruct()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineArrayOfStruct(bndhp[3], errhp, CHAR_SIZE,
		0, 0, 0))
	{
		printf("FAILED: DCIDefineArrayOfStruct()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineArrayOfStruct(bndhp[4], errhp, sizeof(float),
		0, 0, 0))
	{
		printf("FAILED: DCIDefineArrayOfStruct()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineArrayOfStruct(bndhp[5], errhp, sizeof(DCINumber),
		0, 0, 0))
	{
		printf("FAILED: DCIDefineArrayOfStruct()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineArrayOfStruct(bndhp[6], errhp, sizeof(DCIDate),
		0, 0, 0))
	{
		printf("FAILED: DCIDefineArrayOfStruct()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}

	//ִ�в�ѯ���
	if (DCIStmtExecute(svchp, stmthp, errhp, (ub4)0, (ub4)0,
		(CONST DCISnapshot*)0, (DCISnapshot*)0,
		(ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtExecute() select\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	
	stmrowEx = 0;
	while (1) 
	{
		if (DCIStmtFetch(stmthp, errhp, rows, DCI_FETCH_NEXT, 0) == DCI_ERROR) 
		{
			report_error_doc(errhp);
			break;
		}

		//��ȡ�����ȡ�ܹ����ļ�¼������
		DCIAttrGet(stmthp, DCI_HTYPE_STMT, &stmrow, 0, DCI_ATTR_ROW_COUNT, errhp);
		if(stmrow == stmrowEx)//û�����б���ȡ����ô����
			break;
		printf("��ȡ����%d�м�¼\n", stmrow - stmrowEx);

		//
		//����������ﴦ���ȡ��������
		//
		for (int i = 0; i < rows; i++)
		{
			printf("*************��%d��****************\n", i + 1);
			printf("c1: %d\n", c1[i]);
			printf("c2: %f\n", c2[i]);
			printf("c3: %s\n", c3 + CHAR_SIZE * i);
			printf("c4: %s\n", c4 + CHAR_SIZE * i);
			char c6fmt[] = "999999999999999999999.9999999"; // 22��9.7��9
			char c6str[255];
			int outlen;
			DCINumberToText(errhp, &c6[i], (DciText*)c6fmt, (ub4)strlen(c6fmt), NULL, 0, (ub4*)&outlen, (DciText*)c6str);
			c6str[outlen] = 0;
			printf("c5: %f\n", c5[i]);
			printf("c6: %s\n", c6str);
			printf("c7: %d-%d-%d %d:%d:%d\n", (int)c7[i].DCIDateYYYY, c7[i].DCIDateMM, c7[i].DCIDateDD,
				c7[i].DCIDateTime.DCITimeHH, c7[i].DCIDateTime.DCITimeMI, c7[i].DCIDateTime.DCITimeSS);
		}          
		stmrowEx = stmrow;
	}

	if (DCITransCommit(svchp,  errhp,(ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCITransCommit() select\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}

	free(c1);
	free(c2);
	free(c3);
	free(c4);
	free(c5);
	free(c6);
	free(c7);
	return DCI_SUCCESS;
}

/* �������а󶨵ķ�ʽ����ȡ��¼��ֵ�Ĵ��������Ȼ������ʽ
 * rows������ʾÿ�λ�ȡ����������
 * �ٶ���T�ĽṹΪ
 * T(C1 INT, C2 REAL, C3 CHAR(100 BYTE), C4 VARCHAR(100 BYTE), 
 * C5 FLOAT, C6 NUMERIC(22,7), C7 DATETIME)
 */
sword
Select_RowBind(ub2 rows)
{
	text        *sqlstmt = (text*)"select C1, C2, C3, C4, C5, C6, C7 from T";
	char        *p;
	DCIDefine   *bndhp[7];
	int         *c1;
	float       *c2;
	char        *c3;
	char        *c4;
	float       *c5;
	DCINumber       *c6;
	DCIDate     *c7;
	ub4         stmrow, stmrowEx;

	//�����һ�м�¼���ܴ�С���Ա���������ÿ��ֵ�����ļ����С
	int row_size = sizeof(int)+sizeof(float)+sizeof(DCIDate) + CHAR_SIZE \
		+ CHAR_SIZE + sizeof(DCINumber) + sizeof(float);

	p = (char*) malloc (sizeof(char) * row_size * rows);
	memset(p, 0, row_size * rows);

	if (DCIStmtPrepare(stmthp, errhp, sqlstmt, (ub4)strlen((char *)sqlstmt),
		(ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	
	c1 = (int*)p;
	c2 = (float*)((char*)c1 + sizeof(int));
	c3 = ((char*)c2 + sizeof(float));
	c4 = (char*)c3 + CHAR_SIZE;
	c5 = (float*)((char*)c4 + CHAR_SIZE);
	c6 = (DCINumber*)((char*)c5 + sizeof(float));
	c7 = (DCIDate*)((char*)c6 + sizeof(DCINumber));

	if (DCIDefineByPos(stmthp, &bndhp[0], errhp, 1,
		(dvoid *)c1, (sb4)4, (ub2)SQLT_INT,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineByPos(stmthp, &bndhp[1], errhp, 2,
		(dvoid *)c2, (sb4)4, (ub2)SQLT_FLT,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}	
	if (DCIDefineByPos(stmthp, &bndhp[2], errhp, 3,
		(dvoid *)c3, (sb4)CHAR_SIZE, (ub2)SQLT_CHR,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineByPos(stmthp, &bndhp[3], errhp, 4,
		(dvoid *)c4, (sb4)CHAR_SIZE, (ub2)SQLT_STR,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineByPos(stmthp, &bndhp[4], errhp, 5,
		(dvoid *)c5, (sb4)4, (ub2)SQLT_FLT,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineByPos(stmthp, &bndhp[5], errhp, 6,
		(dvoid *)c6, (sb4)4, (ub2)SQLT_VNU,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineByPos(stmthp, &bndhp[6], errhp, 7,
		(dvoid *)c7, (sb4)sizeof(DCIDate), (ub2)SQLT_ODT,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	//����ÿ�е�ֵ��ż����С�����Ĵ�СΪһ�м�¼�Ĵ�С
	if (DCIDefineArrayOfStruct(bndhp[0], errhp, row_size,
		0, 0, 0))
	{
		printf("FAILED: DCIDefineArrayOfStruct()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineArrayOfStruct(bndhp[1], errhp, row_size,
		0, 0, 0))
	{
		printf("FAILED: DCIDefineArrayOfStruct()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineArrayOfStruct(bndhp[2], errhp, row_size,
		0, 0, 0))
	{
		printf("FAILED: DCIDefineArrayOfStruct()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineArrayOfStruct(bndhp[3], errhp, row_size,
		0, 0, 0))
	{
		printf("FAILED: DCIDefineArrayOfStruct()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineArrayOfStruct(bndhp[4], errhp, row_size,
		0, 0, 0))
	{
		printf("FAILED: DCIDefineArrayOfStruct()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineArrayOfStruct(bndhp[5], errhp, row_size,
		0, 0, 0))
	{
		printf("FAILED: DCIDefineArrayOfStruct()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineArrayOfStruct(bndhp[6], errhp, row_size,
		0, 0, 0))
	{
		printf("FAILED: DCIDefineArrayOfStruct()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}

	if (DCIStmtExecute(svchp, stmthp, errhp, (ub4)0, (ub4) 0,
		(CONST DCISnapshot*)0, (DCISnapshot*) 0,
		(ub4) DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtExecute() select\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	
	stmrowEx = 0;
	while (1) 
	{
		if (DCIStmtFetch(stmthp, errhp, rows, DCI_FETCH_NEXT, 0) == DCI_ERROR)
			break;

		//��ȡ�����ȡ�ܹ����ļ�¼������
		DCIAttrGet(stmthp, DCI_HTYPE_STMT, &stmrow, 0, DCI_ATTR_ROW_COUNT, errhp);
		if(stmrow == stmrowEx)//û�����б���ȡ����ô����
			break;

		printf("��ȡ����%d�м�¼\n", stmrow - stmrowEx);

		//
		//����������ﴦ���ȡ��������
		//
		for (int i = 0; i < rows; i++)
		{
			int *c1tmp = (int*)((char*)c1 + i * row_size);
			float *c2tmp = (float*)((char*)c2 + i * row_size);
			char *c3tmp = c3 + i * row_size, *c4tmp = c4 + i * row_size;
			float *c5tmp = (float*)((char*)c5 + i * row_size);
			DCINumber *c6tmp = (DCINumber *)((char*)c6 + i * row_size);
			DCIDate *c7tmp = (DCIDate*)((char*)c7 + i * row_size);
			printf("*************��%d��****************\n", i + 1);
			printf("c1: %d\n", *c1tmp);
			printf("c2: %f\n", *c2tmp);
			printf("c3: %s\n", c3tmp);
			printf("c4: %s\n", c4tmp);
			char c6fmt[] = "999999999999999999999.9999999"; // 22��9.7��9
			char c6str[255];
			int outlen = sizeof(c6str);
			DCINumberToText(errhp, c6tmp, (DciText*)c6fmt, (ub4)strlen(c6fmt), NULL, 0, (ub4*)&outlen, (DciText*)c6str);
			c6str[outlen] = 0;
			printf("c5: %f\n", *c5tmp);
			printf("c6: %s\n", c6str);
			printf("c7: %d-%d-%d %d:%d:%d\n", c7tmp->DCIDateYYYY, c7tmp->DCIDateMM, c7tmp->DCIDateDD, 
				c7tmp->DCIDateTime.DCITimeHH, c7tmp->DCIDateTime.DCITimeMI, c7tmp->DCIDateTime.DCITimeSS);
		}        
		stmrowEx = stmrow;
	}

	if (DCITransCommit(svchp, errhp, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtExecute() select\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}

	free(p);
	return DCI_SUCCESS;
}

sword ExecOneSQLWithResults()
{
	int logged_on = FALSE;
	
	//��ʼ�����־��
	if (init_handles(&envhp, &errhp))
	{
		printf("FAILED: init_handles()\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}

	//��¼�����ݿ����
	if (log_on(envhp, errhp, User, Pwd, DbName))
	{
		printf("FAILED: log_on()\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}
	logged_on = TRUE;
	if (DCIHandleAlloc((dvoid *)envhp, (dvoid **)&stmthp,
		(ub4)DCI_HTYPE_STMT, (CONST size_t)0, (dvoid **) 0))
	{
		printf("FAILED: alloc statement handle\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}

	//�ֱ����к��еķ�ʽ��ȡ100������
	Select_ColumnBind(1);
	Select_RowBind(1); 

	//�ǳ����ݿ���񣬲�������־��
	return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
}

#define COMMENT(x) (void) fprintf(stdout,"\nCOMMENT: %s\n", x)
//��ȡһ�����ֶ�
void
Read_from_loc(DCISvcCtx *svchp, DCIError *errhp, DCILobLocator *lobp)
{
	ub1	*buf;
	ub4	readsize = 0;
	ub4	amtp;
	ub4	lenp=0;
	sb4	err;

	COMMENT("Read from the locator.");
	/* ��ȡ���ֶε��ܳ��� */
	if (DCILobGetLength(svchp, errhp, lobp, &lenp) != DCI_SUCCESS){
		COMMENT("Read_from_loc: DCILobGetLength");
		return;
	}
	else
	{
		printf("  Length of Locator is %d\n", lenp);
		buf = (ub1 *)malloc(sizeof(ub1) * (lenp + 1));//����� CLOB ����ô��Ҫ������һ���ֽ�������ſս�β
		buf[lenp] = '\0';
	}

	//printf("\n------------------------------------------------------------------\n");

	/* Read the locator */
	do
	{
		amtp=1024;		
		err = DCILobRead(svchp, errhp, lobp, &amtp, 1,
			buf+readsize, lenp+1-readsize , (dvoid *) 0,
			(sb4 (*)(dvoid *, const dvoid *, ub4, ub1))0, (ub2)0, (ub1)SQLCS_IMPLICIT);
		if (err == DCI_SUCCESS || err == DCI_NEED_DATA)
			readsize +=amtp;//�ۼ�ÿ�εõ������ݳ���
		else
			COMMENT("Read_from_loc : DCILobRead");
	}
	while (err == DCI_NEED_DATA);
	//�Ѿ����꣬����������ﴦ��õ������ݣ������ܳ��ȴ���� readsize ����

	free(buf);
}

//����һ�����ֶδ��������ṹָ��
sword 
alloc_lob_desc(DCIEnv *envhp, DCILobLocator **lobsrc)
{
	if (DCIDescriptorAlloc(envhp, (dvoid **)lobsrc,
		(ub4)DCI_DTYPE_LOB,
		(size_t)0, (dvoid **)0) != DCI_SUCCESS )
	{		
		printf("FAILED: init_handles()\n");
		return DCI_SUCCESS; 
	}

	return DCI_ERROR;
}

int ReadLargeObject()
{
	int             logged_on = FALSE;
	DCIDefine       *dfnhp[2];
	DCILobLocator   *lobp[2];
	text            *sqlstmt = (text *)"SELECT C2,C3  FROM TAB2";

	//��ʼ�����־��
	if (init_handles(&envhp, &errhp))
	{
		printf("FAILED: init_handles()\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}
	//��¼�����ݿ����
	if (log_on(envhp, errhp, User, Pwd, DbName))
	{
		printf("FAILED: log_on()\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}

	logged_on = TRUE;
	if (DCIHandleAlloc((dvoid *)envhp, (dvoid **) &stmthp,
		(ub4)DCI_HTYPE_STMT, (CONST size_t) 0, (dvoid **) 0))
	{
		printf("FAILED: alloc statement handle\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}

	//��ṹΪ��CREATE TABLE TAB2(C1 INT PRIMARY KEY, C2 CLOB, C3 BLOB)
	lobp[0] = (DCILobLocator *)0;
	lobp[1] = (DCILobLocator *)0;
	alloc_lob_desc(envhp, &lobp[0]);
	alloc_lob_desc(envhp, &lobp[1]);

	if (DCIStmtPrepare(stmthp, errhp, sqlstmt, (ub4)strlen((char *)sqlstmt),
		(ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}

	dfnhp[0] = (DCIDefine *) 0;
	dfnhp[1] = (DCIDefine *) 0;

	if (DCIDefineByPos(stmthp, &dfnhp[0], errhp, (ub4)1,
		(dvoid *)&lobp[0], (sb4)4, (ub2)SQLT_CLOB,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineByPos(stmthp, &dfnhp[1], errhp, (ub4)2,
		(dvoid *)&lobp[1], (sb4)4, (ub2)SQLT_BLOB,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}

	if (DCIStmtExecute(svchp, stmthp, errhp, (ub4)0, (ub4)0,
		(CONST DCISnapshot*)0, (DCISnapshot*)0,
		(ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtExecute() select\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	while (1)
	{
		if (DCIStmtFetch(stmthp, errhp, 1, DCI_FETCH_NEXT, 0) != DCI_SUCCESS)
			break;

		//��ȡ��ǰ�α�λ�õĴ�����¼
		Read_from_loc(svchp, errhp, lobp[0]);
		Read_from_loc(svchp, errhp, lobp[1]);
	}

	DCIDescriptorFree((dvoid *) lobp[0], (ub4) DCI_DTYPE_LOB);
	DCIDescriptorFree((dvoid *) lobp[1], (ub4) DCI_DTYPE_LOB);

	//�ǳ����ݿ���񣬲�������־��
	return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
}

#define WRITESIZE	1024*32
void
write_from_loc(DCISvcCtx *svchp, DCIError *errhp, DCILobLocator *lobp)
{
	char  *buf = (char*)malloc(sizeof(char) * 1024);
	ub4   writesize = 0;
	ub4   amtp;
	//ub4   lenp=0;
	sb4   err;

	COMMENT("Write from the locator.");
	memset(buf, 'a', 1024);

	//printf("\n------------------------------------------------------------------\n");
	do
	{
		amtp=1024;

		err = DCILobWrite(svchp, errhp, lobp, &amtp, 1,
			(dvoid *)buf, (ub4)1024, 0, (dvoid *)0, 0, (ub2) 0, (ub1) SQLCS_IMPLICIT);
		if (err == DCI_SUCCESS)
			writesize +=amtp;
	}
	while (writesize<WRITESIZE);

	//printf("\n------------------------------------------------------------------\n");
	//printf("\n");

	free(buf);
}

int WriteLargeObject()
{
	int		logged_on = FALSE;
	DCIDefine		*dfnhp[2];
	DCILobLocator	*lobp[2];
	text			*sqlstmt = (text *)"SELECT C2, C3 FROM TAB2 WHERE C1=1 FOR UPDATE";
	char			sql[1024];

	//��ʼ�����־��
	if (init_handles(&envhp, &errhp))
	{
		printf("FAILED: init_handles()\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}
	//��¼�����ݿ����
	if (log_on(envhp, errhp, User, Pwd, DbName))
	{
		printf("FAILED: log_on()\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}
	logged_on = TRUE;
	if (DCIHandleAlloc((dvoid *)envhp, (dvoid **) &stmthp,
		(ub4)DCI_HTYPE_STMT, (CONST size_t)0, (dvoid **)0))
	{
		printf("FAILED: alloc statement handle\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}

	//��ṹΪ��CREATE TABLE TAB2(C1 INT PRIMARY KEY, C2 CLOB, C3 BLOB)
	lobp[0] = (DCILobLocator *)0;
	lobp[1] = (DCILobLocator *)0;

	alloc_lob_desc(envhp, &lobp[0]);
	alloc_lob_desc(envhp, &lobp[1]);

	strcpy(sql, "DROP TABLE IF EXISTS TAB2;");
	if (DCIStmtPrepare(stmthp, errhp, (unsigned char *)sql, (ub4)strlen((char *)sql),
		(ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}
	if (DCIStmtExecute(svchp, stmthp, errhp, (ub4)1, (ub4)0,
		(CONST DCISnapshot*)0, (DCISnapshot*)0,
		(ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtExecute() select\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}

	strcpy(sql, "CREATE TABLE TAB2(C1 INT PRIMARY KEY, C2 CLOB, C3 BLOB);");
	if (DCIStmtPrepare(stmthp, errhp, (unsigned char *)sql, (ub4)strlen((char *)sql),
		(ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}
	if (DCIStmtExecute(svchp, stmthp, errhp, (ub4)1, (ub4)0,
		(CONST DCISnapshot*)0, (DCISnapshot*)0,
		(ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtExecute() select\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}

	strcpy(sql, "INSERT INTO TAB2 VALUES(1, EMPTY_CLOB(),  EMPTY_BLOB());");
	//�ȸ����ֶβ���һ��NULL
	if (DCIStmtPrepare(stmthp, errhp, (unsigned char *)sql, (ub4)strlen((char *)sql),
		(ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}
	if (DCIStmtExecute(svchp, stmthp, errhp, (ub4)1, (ub4)0,
		(CONST DCISnapshot*)0, (DCISnapshot*)0,
		(ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtExecute() select\n");
		return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
	}

	//��λ���´��ֶ�
	if (DCIStmtPrepare(stmthp, errhp, sqlstmt, (ub4)strlen((char *)sqlstmt),
		(ub4)DCI_NTV_SYNTAX, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtPrepare() select\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}

	dfnhp[0] = (DCIDefine *)0;
	dfnhp[1] = (DCIDefine *)0;

	if (DCIDefineByPos(stmthp, &dfnhp[0], errhp, (ub4)1,
		(dvoid *)&lobp[0], (sb4)4, (ub2)SQLT_CLOB,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}
	if (DCIDefineByPos(stmthp, &dfnhp[1], errhp, (ub4)2,
		(dvoid *)&lobp[1], (sb4)4, (ub2)SQLT_BLOB,
		(dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIDefineByPos()\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}

	if (DCIStmtExecute(svchp, stmthp, errhp, (ub4)1, (ub4)0,
		(CONST DCISnapshot*)0, (DCISnapshot*)0,
		(ub4)DCI_DEFAULT))
	{
		printf("FAILED: DCIStmtExecute() select\n");
		report_error_doc(errhp);
		return DCI_ERROR;
	}

	write_from_loc(svchp, errhp, lobp[0]);
	write_from_loc(svchp, errhp, lobp[1]);

	DCIDescriptorFree((dvoid *) lobp[0], (ub4)DCI_DTYPE_LOB);
	DCIDescriptorFree((dvoid *) lobp[1], (ub4)DCI_DTYPE_LOB);

	//�ǳ����ݿ���񣬲�������־��
	return finish_demo(logged_on, envhp, svchp, errhp, stmthp);
}

sword TestDocCases()
{
	int errcode = DCI_SUCCESS;

	errcode = ExecOneSQLWithoutResult();
	if (errcode == DCI_ERROR)
		goto end;
		
	errcode = ExecOneSQLWithResults();
	if (errcode == DCI_ERROR)
		goto end;

	errcode = WriteLargeObject();
	if (errcode == DCI_ERROR)
		goto end;

	errcode = ReadLargeObject();
	if (errcode == DCI_ERROR)
		goto end;

end:
	return errcode;
}
