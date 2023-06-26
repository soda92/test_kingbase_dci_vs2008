#include "DciTest.h"

#define	STRLEN	128
#define PIECELEN	5
#define EXPLEN	11

typedef struct TestPieceData
{
	char	typeVal[STRLEN+1];
	char	inVal[PIECELEN+1];
	int		inLen;
	char	expVal[STRLEN+1];
	int		expLen;
}TestPieceData;

TestPieceData TestPiece[][2] = 
{
	{
		{"blob", "", 0, "", 0},
		{"clob", "", 0, "", 0},
	},
	{
		{"bytea", "", 0, "", 0},
		{"text", "", 0, "", 0},
	},
	{
		{"blob", "67890", 12, "678906789067", 12},
		{"clob", "fghij", 12, "fghijfghijfg", 12},
	},	
	{
		{"bytea", "67890", 15, "678906789067890", 15},
		{"text", "fghij", 15, "fghijfghijfghij", 15},
	}
};

sword
TestPieceMultiColsMultiRows()
{
	ub4		i, j, failed = 0;
	DCIStmt	*pStmt = NULL;
	DCIBind	*pBind = NULL;
	DCIDefine *pDefine = NULL;
	char	sql[512] = "";
	int		in_int1 = 123;
	int		in_int2 = 456;
	char	in_char[] = "abc";

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
		failed++;
		printf("DCIHandleAlloc DCI_HTYPE_STMT failed\n");
		goto end;
	}

	for (i = 0; i < sizeof(TestPiece) / sizeof(TestPiece[0]); i++)
	{
		ub4 	type = 0;
		ub1 	in_out = 0;
		ub4 	iter = 0;
		ub4 	idx = 0;
		ub1 	piece = 0;

		ub4 	alen = 0;
		sb2 	ind = 0;
		ub2 	rcode = 0;

		ub4 	sz_remain1 = 0;
		ub4		sz_remain2 = 0;

		char	pieceOut1[STRLEN+1] = "";
		char	pieceOut2[STRLEN+1] = "";

		int		out_int1 = 0;
		int		out_int2 = 0;
		char	out_char[STRLEN+1] = "";

		sprintf(sql, "drop table tbind");
		err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			failed++;
			report_error(pError);
			goto end;
		}
		err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);

		sprintf(sql, "create table tbind(a int, b %s, c int, d %s, e varchar(20))", TestPiece[i][0].typeVal, TestPiece[i][1].typeVal);
		err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			failed++;
			report_error(pError);
			goto end;
		}
		err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			failed++;
			report_error(pError);
			goto end;
		}

		sprintf(sql, "insert into tbind values(:1, :2, :3, :4, :5)");
		err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			failed++;
			report_error(pError);
			goto end;
		}

		err = DCIBindByPos(pStmt, &pBind, pError, 1, &in_int1, sizeof(in_int1), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			failed++;
			report_error(pError);
			goto end;
		}

		err = DCIBindByPos(pStmt, &pBind, pError, 2, (void *)0, TestPiece[i][0].inLen, SQLT_BIN, 0, 0, 0, 0, 0, DCI_DATA_AT_EXEC);
		if (err != DCI_SUCCESS)
		{
			failed++;
			report_error(pError);
			goto end;
		}

		err = DCIBindByPos(pStmt, &pBind, pError, 3, &in_int2, sizeof(in_int2), SQLT_INT, 0, 0, 0, 0, 0, DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			failed++;
			report_error(pError);
			goto end;
		}

		err = DCIBindByPos(pStmt, &pBind, pError, 4, (void *)0, TestPiece[i][1].inLen, SQLT_CHR, 0, 0, 0, 0, 0, DCI_DATA_AT_EXEC);
		if (err != DCI_SUCCESS)
		{
			failed++;
			report_error(pError);
			goto end;
		}

		err = DCIBindByPos(pStmt, &pBind, pError, 5, &in_char, sizeof(in_char), SQLT_CHR, 0, 0, 0, 0, 0, DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			failed++;
			report_error(pError);
			goto end;
		}

		sz_remain1 = TestPiece[i][0].expLen;
		sz_remain2 = TestPiece[i][1].expLen;

		while (1)
		{
			err = DCIStmtExecute(pSvcCtx, pStmt, pError, 1, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
			if (err == DCI_SUCCESS && sz_remain1 == 0 && sz_remain2 == 0)
				break;

			if (err != DCI_NEED_DATA)
			{
				failed++;
				printf("DCIStmtExecute failed\n");
				goto end;
			}

			err = DCIStmtGetPieceInfo(pStmt, pError, (void **)&pBind, &type, &in_out, &iter, &idx, &piece);
			if (err != DCI_SUCCESS)
			{
				failed++;
				report_error(pError);
				goto end;
			}

			if (sz_remain1 > 0)
			{
				alen = (ub4)strlen(TestPiece[i][0].inVal);
				if (alen >= sz_remain1)
				{
					alen = sz_remain1;
					piece = DCI_LAST_PIECE;
				}

				err = DCIStmtSetPieceInfo(pBind, type, pError, TestPiece[i][0].inVal, &alen, piece, &ind, &rcode);
				if (err != DCI_SUCCESS)
				{
					failed++;
					report_error(pError);
					goto end;
				}

				sz_remain1 = sz_remain1 - alen;
			}
			else
			{
				alen = (ub4)strlen(TestPiece[i][1].inVal);
				if (alen >= sz_remain2)
				{
					alen = sz_remain2;
					piece = DCI_LAST_PIECE;
				}

				err = DCIStmtSetPieceInfo(pBind, type, pError, TestPiece[i][1].inVal, &alen, piece, &ind, &rcode);
				if (err != DCI_SUCCESS)
				{
					failed++;
					report_error(pError);
					goto end;
				}

				sz_remain2 = sz_remain2 - alen;
			}
		}


		sprintf(sql, "select * from tbind");
		err = DCIStmtPrepare(pStmt, pError, (const DciText*) sql, (ub4) strlen((char*)sql), (ub4) DCI_NTV_SYNTAX, (ub4) DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			failed++;
			report_error(pError);
			goto end;
		}

		err = DCIDefineByPos(pStmt, &pDefine, pError, 1, &out_int1, sizeof(out_int1), (ub2)SQLT_INT, (void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			failed++;
			report_error(pError);
			goto end;
		}

		err = DCIDefineByPos(pStmt, &pDefine, pError, 2, (void *)0, TestPiece[i][0].expLen, (ub2)SQLT_BIN, (void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DYNAMIC_FETCH);
		if (err != DCI_SUCCESS)
		{
			failed++;
			report_error(pError);
			goto end;
		}

		err = DCIDefineByPos(pStmt, &pDefine, pError, 3, &out_int2, sizeof(out_int2), (ub2)SQLT_INT, (void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			failed++;
			report_error(pError);
			goto end;
		}

		err = DCIDefineByPos(pStmt, &pDefine, pError, 4, (void *)0, TestPiece[i][1].expLen, (ub2)SQLT_CHR, (void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DYNAMIC_FETCH);
		if (err != DCI_SUCCESS)
		{
			failed++;
			report_error(pError);
			goto end;
		}

		err = DCIDefineByPos(pStmt, &pDefine, pError, 5, &out_char, sizeof(out_char), (ub2)SQLT_CHR, (void *)NULL, (ub2 *)0, (ub2 *)0, (ub4)DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			failed++;
			report_error(pError);
			goto end;
		}

		err = DCIStmtExecute(pSvcCtx, pStmt, pError, 0, 0, (DCISnapshot *)0, (DCISnapshot *)0, DCI_DEFAULT);
		if (err != DCI_SUCCESS)
		{
			failed++;
			report_error(pError);
			goto end;
		}

		sz_remain1 = TestPiece[i][0].expLen;
		sz_remain2 = TestPiece[i][1].expLen;
		while (1)
		{
			err = DCIStmtFetch(pStmt, pError, (ub4)1, (ub2)DCI_FETCH_NEXT, (ub4)DCI_DEFAULT);
			if (err == DCI_SUCCESS && sz_remain1 == 0 && sz_remain2 == 0)
				break;

			if (err != DCI_NEED_DATA)
			{
				failed++;
				printf("DCIStmtExecute failed\n");
				goto end;
			}

			err = DCIStmtGetPieceInfo(pStmt, pError, (void **)&pDefine, &type, &in_out, &iter, &idx, &piece);
			if (err != DCI_SUCCESS)
			{
				failed++;
				report_error(pError);
				goto end;
			}

			if (sz_remain1 > 0) 
			{
				alen = PIECELEN;
				if (alen >= sz_remain1)
					alen = sz_remain1;

				err = DCIStmtSetPieceInfo(pDefine, type, pError, pieceOut1 + TestPiece[i][0].expLen - sz_remain1, &alen, piece, &ind, &rcode);
				if (err != DCI_SUCCESS)
				{
					failed++;
					report_error(pError);
					goto end;
				}

				sz_remain1 = sz_remain1 - alen;
			}
			else
			{
				alen = PIECELEN;
				if (alen >= sz_remain2)
					alen = sz_remain2;

				err = DCIStmtSetPieceInfo(pDefine, type, pError, pieceOut2 + TestPiece[i][1].expLen - sz_remain2, &alen, piece, &ind, &rcode);
				if (err != DCI_SUCCESS)
				{
					failed++;
					report_error(pError);
					goto end;
				}

				sz_remain2 = sz_remain2 - alen;
			}
		}

		if (strcmp(pieceOut1, TestPiece[i][0].expVal) != 0
			|| strcmp(pieceOut2, TestPiece[i][1].expVal) != 0
			|| strcmp(in_char, out_char) != 0
			|| in_int1 != out_int1
			|| in_int2 != out_int2)
			failed++;
	}

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
	if (failed == 0)
		return DCI_SUCCESS;
	else
	{
		printf("failed %d\n", failed);
		return DCI_ERROR;
	}
}
