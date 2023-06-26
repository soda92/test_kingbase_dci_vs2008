#include "DciTest.h"

DCIDirPathCtx *dpctx;
DCIDirPathColArray *dpca;
DCIDirPathStream *dpstr;
DCIParam *columns;

char *buffer;
FILE *csv;
char message[512];

typedef struct _col_def
{
	const char *name;
	ub4 type;
	ub4 size;
} col_def;

static void intToSqlInt(int n, char *buffer)
{
	buffer[0] = n & 0xFF;
	buffer[1] = (n >> 8) & 0xFF;
	buffer[2] = (n >> 16) & 0xFF;
	buffer[3] = (n >> 24) & 0xFF;
}


static int strToSqlInt(const char *s, int size, char *buffer)
{
	int n = 0;
	for (int i = 0; i < size; i++)
	{
		if (s[i] < '0' || s[i] > '9')
		{
			return -1;
		}
		n = n * 10 + s[i] - '0';
	}

	intToSqlInt(n, buffer);

	return 0;
}

static inline int isValid(col_def &colDef)
{
	return colDef.type != 0;
}

static int loadRows( ub4 rowCount)
{
	int ret = DCI_SUCCESS;
	for (ub4 offset = 0; offset < rowCount;)
	{
		err = DCIDirPathStreamReset(dpstr, pError);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			return ret;
		}

		err = DCIDirPathColArrayToStream(dpca, dpctx, dpstr, pError, rowCount,offset);
		if (err != DCI_SUCCESS && err != DCI_CONTINUE)
		{
			ret = DCI_ERROR;
			return ret;
		}

		err = DCIDirPathLoadStream(dpctx, dpstr, pError);
		if (err != DCI_SUCCESS)
		{
			ret = DCI_ERROR;
			return ret;
		}

		if (err == DCI_SUCCESS)
		{
			offset = rowCount;
		} 
		else
		{
			ub4 temp;
			err = DCIAttrGet(dpca, DCI_HTYPE_DIRPATH_COLUMN_ARRAY, &temp, 0, DCI_ATTR_ROW_COUNT, pError);
			if (err != DCI_SUCCESS)
			{
				return DCI_ERROR;
			}
			offset += temp;
		}
	}

	return DCI_SUCCESS;
}


int loadCSV(col_def *colDefs, const char **csvValue)
{

	ub4 maxRowCount = 0;
	err = DCIAttrGet(dpca, DCI_HTYPE_DIRPATH_COLUMN_ARRAY, &maxRowCount, 0, DCI_ATTR_NUM_ROWS, pError);
	printf("DCI_HTYPE_DIRPATH_COLUMN_ARRAY.DCI_ATTR_NUM_ROWS = %d\r\n", maxRowCount);

	int rowSize = 0;
	for (int i = 0; isValid(colDefs[i]); i++)
	{
		rowSize += colDefs[i].size;
	}

	if ((buffer = (char *) malloc(rowSize * maxRowCount)) == NULL)
	{
		return DCI_ERROR;
	}

	char *current = buffer;
	const char* line;
	int row = 0;
	int i = 0;

	while(csvValue[i] != NULL)
	{
		line = csvValue[i];
		int len = strlen(line);
		int col = 0;
		i++;
		for (const char *p = line; p < line + len;)
		{
			const char *comma = strchr(p, ',');
			const char *next;
			ub4 size;
			if (comma != NULL)
			{
				size = comma - p;
				next = comma + 1;
			} else
			{
				size = line + len - p;
				if (size > 0 && p[size - 1] == '\n') size--;
				if (size > 0 && p[size - 1] == '\r') size--;
				next = line + len;
			}

			if (colDefs[col].type == SQLT_INT)
			{
				if (strToSqlInt(p, size, current))
				{
					printf("Not a number : \"%s\"\r\n", p);
					return DCI_ERROR;
				}
				size = colDefs[col].size;
			} else if (colDefs[col].type == SQLT_CHR)
			{
				strncpy(current, p, size);
			} else
			{
				printf("Unsupported type : %s\r\n", colDefs[col].type);
				return DCI_ERROR;
			}

			err = DCIDirPathColArrayEntrySet(dpca, pError, row, col, (ub1 *) current, size,
				DCI_DIRPATH_COL_COMPLETE);
			if (err != DCI_SUCCESS)
			{
				return DCI_ERROR;
			}

			p = next;
			current += size;
			col++;
		}

		row++;
		if (row == maxRowCount)
		{
			if (loadRows(row))
			{
				return DCI_ERROR;
			}
			current = buffer;
			row = 0;
		}
	}

	if (row > 0 && loadRows(row))
	{
		return DCI_ERROR;
	}

	free(buffer);
	buffer = NULL;

	return DCI_SUCCESS;
}


sword TestDirpath()
{

	char *table = "test_DirPath";
	ub1	dirpathinput = DCI_DIRPATH_INPUT_STREAM;
	ub2				cols = 1;
	int				buf_size = 64 *1024;
	const char *csvValue[] =
	{
		"1,10,asdfghjkl",
		"2,20,qwertyuiop\n",
		"3,300,value3",
		"5,500,value5",
		NULL
	};

	const int SQL_LEN = 1024;
	char sql[SQL_LEN];

	sword ret = DCI_SUCCESS;


	col_def colDefs[] = {
		{"ID",     SQLT_INT, 4},
		{"NUM",    SQLT_INT, 4},
		{"VALUE1", SQLT_CHR, 60},
		{NULL, 0,            0}
	};

	err = connect(true);
	if (err != DCI_SUCCESS)
	{
		printf("connect failed\n");
		return DCI_ERROR;
	}

	//drop table
	memset(sql, 0, SQL_LEN);
	sprintf(sql, "DROP TABLE if exists %s ", table);
	err = execute(sql);


	//create table for test
	memset(sql, 0, SQL_LEN);
	sprintf(sql, "CREATE TABLE %s (ID int primary key, NUM int, VALUE1 char(60))", table);
	err = execute(sql);

	err = DCIHandleAlloc((void *)pEnv, (void **)&dpctx, DCI_HTYPE_DIRPATH_CTX, (size_t)0, NULL);
	if (err != DCI_SUCCESS)
	{
		ret = err;
		goto end;
	}

	err = DCIAttrSet((void *)dpctx, (ub4)DCI_HTYPE_DIRPATH_CTX, (dvoid *)table, (ub4)strlen(table), (ub4)DCI_ATTR_NAME, pError);
	if (err != DCI_SUCCESS)
	{
		ret = err;
		goto end;
	}

	err = DCIAttrSet(dpctx, DCI_HTYPE_DIRPATH_CTX,&buf_size,0, DCI_ATTR_BUF_SIZE, pError);
	err = DCIAttrSet(dpctx, DCI_HTYPE_DIRPATH_CTX, (dvoid *) &dirpathinput, (ub4) 0, DCI_ATTR_DIRPATH_INPUT, pError);

	for (cols = 0; isValid(colDefs[cols]); cols++);
	err = DCIAttrSet(dpctx, DCI_HTYPE_DIRPATH_CTX, &cols, sizeof(ub2), DCI_ATTR_NUM_COLS, pError);
	if (err != DCI_SUCCESS)
	{
		ret = err;
		goto end;
	}

	err = DCIAttrGet((dvoid *)dpctx,DCI_HTYPE_DIRPATH_CTX,(dvoid *)&columns, (ub4 *)0,DCI_ATTR_LIST_COLUMNS, pError);
	if (err != DCI_SUCCESS)
	{
		DciText sqlstate[6] = "", msg[256] = "";
		sb4		errcode = 0;

		DCIErrorGet(pError, 1, sqlstate, &errcode, msg, 256, DCI_HTYPE_ERROR);
		printf("DCIAttrGet DCI_ATTR_LIST_COLUMNS failed: %d %s\n", errcode, msg);
		ret = err;
		goto end;
	}

	for (int i = 0; i < cols; i++)
	{
		col_def &colDef = colDefs[i];
		DCIParam *column;

		err = DCIParamGet(columns, DCI_DTYPE_PARAM, pError, (void **) &column, i + 1);
		if (err != DCI_SUCCESS)
		{
			ret = err;
			goto end;
		}

		err = DCIAttrSet(column, DCI_DTYPE_PARAM, (void *) colDef.name, strlen(colDef.name), DCI_ATTR_NAME,
			pError);
		if (err != DCI_SUCCESS)
		{
			ret = err;
			goto end;
		}

		err = DCIAttrSet(column, DCI_DTYPE_PARAM, &colDef.type, sizeof(ub4), DCI_ATTR_DATA_TYPE, pError);
		if (err != DCI_SUCCESS)
		{
			ret = err;
			goto end;
		}

		err = DCIAttrSet(column, DCI_DTYPE_PARAM, &colDef.size, sizeof(ub4), DCI_ATTR_DATA_SIZE, pError);
		if (err != DCI_SUCCESS)
		{
			ret = err;
			goto end;
		}

		/*
		DCIAttrSet(column, DCI_DTYPE_PARAM, &colDefs[i].precision, sizeof(ub4), DCI_ATTR_PRECISION, err);

		DCIAttrSet(column, DCI_DTYPE_PARAM, &colDefs[i].scale, sizeof(ub4), DCI_ATTR_SCALE, err);
		*/

		DCIDescriptorFree(column, DCI_DTYPE_PARAM);
	}

	err = DCIDirPathPrepare(dpctx, pSvcCtx, pError);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = DCIHandleAlloc(dpctx, (void **) &dpca, DCI_HTYPE_DIRPATH_COLUMN_ARRAY, (size_t) 0, (void **) 0);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;

	}

	err = DCIHandleAlloc(dpctx, (void **) &dpstr, DCI_HTYPE_DIRPATH_STREAM, (size_t) 0, (dvoid **) 0);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}

	err = loadCSV(colDefs, csvValue);
	if (err != DCI_SUCCESS)
	{
		ret = DCI_ERROR;
		goto end;
	}


end:

	DCIDirPathFinish(dpctx, pError);
	DCIHandleFree(dpstr, DCI_HTYPE_DIRPATH_STREAM);
	DCIHandleFree(dpca, DCI_HTYPE_DIRPATH_COLUMN_ARRAY);
	DCIHandleFree(dpctx, DCI_HTYPE_DIRPATH_CTX);

	disconnect(true);

	return ret;

}
