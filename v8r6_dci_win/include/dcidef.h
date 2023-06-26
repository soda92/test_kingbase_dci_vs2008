/*
 * $Header: dcidfn.h $ 
 */

/* Copyright (c) 2010, BaseSoft Inc.  All rights reserved.  */
 
#ifndef DCIDFN_H
#define DCIDFN_H

#include "dcitypes.h"
#include<math.h>
#include<time.h>
/* Data Types */

#define SQLT_CHR  1	                                     /* character string */
#define SQLT_NUM  2                                               /* numeric */
#define SQLT_INT  3                                               /* integer */
#define SQLT_FLT  4                                 /* Floating point number */
#define SQLT_STR  5                                /* zero terminated string */

#define SQLT_VNU  6                        /* NUM with preceding length byte */
#define SQLT_PDN  7                  /* (ORANET TYPE) Packed Decimal Numeric */
#define SQLT_LNG  8                                                  /* long */
#define SQLT_VCS  9                             /* Variable character string */
#define SQLT_NON  10                      /* Null/empty PCC Descriptor entry */
#define SQLT_RID  11                                                /* rowid */
#define SQLT_DAT  12                                /* date in oracle format */
#define SQLT_VBI  15                                 /* binary in VCS format */
#define SQLT_BFLOAT 21                                /* Native Binary float*/
#define SQLT_BDOUBLE 22                             /* NAtive binary double */
#define SQLT_BIN  23                                  /* binary data(DTYBIN) */
#define SQLT_LBI  24                                          /* long binary */
#define SQLT_UIN  68                                     /* unsigned integer */
//#define SQLT_SLS  91                        /* Display sign leading separate */
#define SQLT_LVC  94                                  /* Longer longs (char) */
#define SQLT_LVB  95                                   /* Longer long binary */
#define SQLT_AFC  96                                      /* Ansi fixed char */
#define SQLT_AVC  97                                        /* Ansi Var char */
//#define SQLT_IBFLOAT  100                           /* binary float canonical */
//#define SQLT_IBDOUBLE 101                          /* binary double canonical */
#define SQLT_CUR  102                                        /* cursor  type */
#define SQLT_RDD  104                                    /* rowid descriptor */
//#define SQLT_LAB  105                                          /* label type */
//#define SQLT_OSL  106                                        /* oslabel type */

//#define SQLT_NTY  108                                   /* named object type */
#define SQLT_REF  110                                            /* ref type */

#define SQLT_CLOB 112                                       /* character lob */
#define SQLT_BLOB 113                                          /* binary lob */
//#define SQLT_BFILEE 114                                   /* binary file lob */
//#define SQLT_CFILEE 115                                /* character file lob */
#define SQLT_RSET 116                                     /* result set type */
//#define SQLT_NCO  122      /* named collection type (varray or nested table) */
//#define SQLT_VST  155                                      /* DCIString type */
#define SQLT_ODT  156                                        /* DCIDate type */

// from C_INTERFACE +7792
#define SQLT_OTIME  180                     /* oracle intenal timestamp type */

/* datetimes and intervals */
#define SQLT_DATE                      184                      /* ANSI Date */
#define SQLT_TIME                      185                           /* TIME */
#define SQLT_TIME_TZ                   186            /* TIME WITH TIME ZONE */
#define SQLT_TIMESTAMP                 187                      /* TIMESTAMP */
#define SQLT_TIMESTAMP_TZ              188       /* TIMESTAMP WITH TIME ZONE */
#define SQLT_INTERVAL_YM               189         /* INTERVAL YEAR TO MONTH */
#define SQLT_INTERVAL_DS               190         /* INTERVAL DAY TO SECOND */
#define SQLT_TIMESTAMP_LTZ             232        /* TIMESTAMP WITH LOCAL TZ */

//#define SQLT_PNTY   241              /* pl/sql representation of named types */

/* CHAR/NCHAR/VARCHAR2/NVARCHAR2/CLOB/NCLOB char set "form" information */
#define SQLCS_IMPLICIT 1     /* for CHAR, VARCHAR2, CLOB w/o a specified set */
#define SQLCS_NCHAR    2                  /* for NCHAR, NCHAR VARYING, NCLOB */
#define SQLCS_EXPLICIT 3   /* for CHAR, etc, with "CHARACTER SET ..." syntax */

#define	MORE_ZERO 1
#define	LESS_ZERO (-1)
#define	ZERO 0
#define	MAX_MONTH 12
#define	MAX_HOUR 24
#define	MAX_MINUTE 60
#define	MAX_SECOND 60
#define	MAX_FSEC 1000000000

/* Param Mode */
enum DCITypeParamMode
{
	DCI_TYPEPARAM_IN = 0,                                                /* in */
	DCI_TYPEPARAM_OUT,                                                  /* out */
	DCI_TYPEPARAM_INOUT,                                             /* in-out */
};
typedef enum DCITypeParamMode DCITypeParamMode;

/* Type Code */
#define DCI_TYPECODE_REF         SQLT_REF        /* SQL/OTS OBJECT REFERENCE */
#define DCI_TYPECODE_DATE        SQLT_DAT              /* SQL DATE  OTS DATE */
#define DCI_TYPECODE_SIGNED8     27      /* SQL SIGNED INTEGER(8)  OTS SINT8 */
#define DCI_TYPECODE_REAL        21                /* SQL REAL  OTS SQL_REAL */
#define DCI_TYPECODE_DOUBLE      22  /* SQL DOUBLE PRECISION  OTS SQL_DOUBLE */
#define DCI_TYPECODE_FLOAT       SQLT_FLT      /* SQL FLOAT(P)  OTS FLOAT(P) */
#define DCI_TYPECODE_NUMBER      SQLT_NUM/* SQL NUMBER(P S)  OTS NUMBER(P S) */
#define DCI_TYPECODE_DECIMAL     SQLT_PDN

#define DCI_TYPECODE_SMALLINT    246           /* SQL SMALLINT  OTS SMALLINT */
#define DCI_TYPECODE_INTEGER     SQLT_INT        /* SQL INTEGER  OTS INTEGER */

#define DCI_TYPECODE_RAW         SQLT_LVB          /* SQL RAW(N)  OTS RAW(N) */

#define DCI_TYPECODE_CHAR        SQLT_AFC    /* SQL CHAR(N)  OTS SQL_CHAR(N) */
#define DCI_TYPECODE_VARCHAR     SQLT_CHR
#define DCI_TYPECODE_VARCHAR2    SQLT_VCS

#define DCI_TYPECODE_BLOB        SQLT_BLOB    /* SQL/OTS BINARY LARGE OBJECT */
#define DCI_TYPECODE_CLOB        SQLT_CLOB /* SQL/OTS CHARACTER LARGE OBJECT */

#define DCI_TYPECODE_TIME        SQLT_TIME                   /* SQL/OTS TIME */
#define DCI_TYPECODE_TIME_TZ     SQLT_TIME_TZ             /* SQL/OTS TIME_TZ */
#define DCI_TYPECODE_TIMESTAMP   SQLT_TIMESTAMP         /* SQL/OTS TIMESTAMP */
#define DCI_TYPECODE_TIMESTAMP_TZ  SQLT_TIMESTAMP_TZ /* SQL/OTS TIMESTAMP_TZ */

#define DCI_TYPECODE_TIMESTAMP_LTZ  SQLT_TIMESTAMP_LTZ /* TIMESTAMP_LTZ */

#define DCI_TYPECODE_INTERVAL_YM SQLT_INTERVAL_YM   /* SQL/OTS INTRVL YR-MON */
#define DCI_TYPECODE_INTERVAL_DS SQLT_INTERVAL_DS  /* SQL/OTS INTRVL DAY-SEC */
#define DCI_TYPECODE_UNSIGNED8   SQLT_BIN
#define DCI_TYPECODE_DECIMAL     SQLT_PDN
#define DCI_TYPECODE_OCTET       245                   /* SQL ???  OTS OCTET */
#define DCI_TYPECODE_SIGNED16    28    /* SQL SIGNED INTEGER(16)  OTS SINT16 */
#define DCI_TYPECODE_SIGNED32    29    /* SQL SIGNED INTEGER(32)  OTS SINT32 */

typedef ub2 DCITypeCode;


/* DCI_*_PIECE defines the piece types that are returned or set
*/
#define DCI_ONE_PIECE   0                 /* there or this is the only piece */
#define DCI_FIRST_PIECE 1                        /* the first of many pieces */
#define DCI_NEXT_PIECE  2                         /* the next of many pieces */
#define DCI_LAST_PIECE  3                   /* the last piece of this column */

/*-----------------------------Handle Types----------------------------------*/

#define DCI_HTYPE_ENV            1                     /* environment handle */
#define DCI_HTYPE_ERROR          2                           /* error handle */
#define DCI_HTYPE_SVCCTX         3                         /* service handle */
#define DCI_HTYPE_STMT           4                       /* statement handle */
#define DCI_HTYPE_BIND           5                            /* bind handle */
#define DCI_HTYPE_DEFINE         6                          /* define handle */
#define DCI_HTYPE_DESCRIBE       7                        /* describe handle */
#define DCI_HTYPE_SERVER         8                          /* server handle */
#define DCI_HTYPE_SESSION        9                  /* authentication handle */
#define DCI_HTYPE_TRANS         10                     /* transaction handle */

#define DCI_HTYPE_DIRPATH_CTX   14                    /* direct path context */
#define DCI_HTYPE_DIRPATH_COLUMN_ARRAY 15        /* direct path column array */
#define DCI_HTYPE_DIRPATH_STREAM       16              /* direct path stream */


/*-------------------------Descriptor Types----------------------------------*/
#define DCI_DTYPE_LOB 50                                     /* lob  locator */
#define DCI_DTYPE_SNAP 51                             /* snapshot descriptor */
#define DCI_DTYPE_PARAM 53
						 /* a parameter descriptor obtained from DCIParamGet */
#define DCI_DTYPE_ROWID 54								/* rowid descriptor  */

#define DCI_DTYPE_INTERVAL_YM   62                    /* Interval year month */
#define DCI_DTYPE_INTERVAL_DS   63                    /* Interval day second */

#define DCI_DTYPE_DATE          65                                   /* Date */
#define DCI_DTYPE_TIME          66                                   /* Time */
#define DCI_DTYPE_TIME_TZ       67                     /* Time with timezone */
#define DCI_DTYPE_TIMESTAMP     68                              /* Timestamp */
#define DCI_DTYPE_TIMESTAMP_TZ  69                /* Timestamp with timezone */
#define DCI_DTYPE_TIMESTAMP_LTZ 70                /* Timestamp with local tz */

/*-------------------------Object Ptr Types----------------------------------*/
#define DCI_OTYPE_NAME 1                                      /* object name */
#define DCI_OTYPE_REF  2                                       /* REF to TDO */
#define DCI_OTYPE_PTR  3                                       /* PTR to TDO */

/*=============================Attribute Types===============================*/
#define DCI_ATTR_FNCODE  1                          /* the DCI function code */
#define DCI_ATTR_OBJECT   2 /* is the environment initialized in object mode */
#define DCI_ATTR_NONBLOCKING_MODE  3                    /* non blocking mode */
#define DCI_ATTR_SQLCODE  4                                  /* the SQL verb */
#define DCI_ATTR_ENV  5                            /* the environment handle */
#define DCI_ATTR_SERVER           6                     /* the server handle */
#define DCI_ATTR_SESSION          7               /* the user session handle */
#define DCI_ATTR_TRANS            8

#define DCI_ATTR_ROW_COUNT        9             /* the rows processed so far */
#define DCI_ATTR_SQLFNCODE        10        /* the SQL verb of the statement */
#define DCI_ATTR_PREFETCH_ROWS    11  /* sets the number of rows to prefetch */
#define DCI_ATTR_PREFETCH_MEMORY  13   /* TODO:memory limit for rows fetched */
#define DCI_ATTR_PARAM_COUNT      18  /* number of column in the select list */
#define DCI_ATTR_ROWID			  19                            /* the rowid */
#define DCI_ATTR_USERNAME         22                   /* username attribute */
#define DCI_ATTR_PASSWORD         23                   /* password attribute */
#define DCI_ATTR_STMT_TYPE        24                   /* statement type */

#define DCI_ATTR_CHARSET_ID       31                     /* Character Set ID */
#define DCI_ATTR_CHARSET_FORM     32                   /* Character Set Form */

#define DCI_ATTR_LOBEMPTY		45                            /* empty lob ? */
#define DCI_ATTR_NUM_DML_ERRORS         73       /* num of errs in array DML */
#define DCI_ATTR_DML_ROW_OFFSET         74        /* row offset in the array */
#define DCI_ATTR_DATEFORMAT       75           /* default date format string */
#define DCI_ATTR_BUF_SIZE         77                          /* buffer size */
#define DCI_ATTR_DIRPATH_MODE     78        /* mode of direct path operation */

#define DCI_ATTR_NUM_ROWS         81       /* number of rows in column array */
#define DCI_ATTR_COL_COUNT        82              /* columns of column array
processed so far.       */
#define DCI_ATTR_NUM_COLS         102                   /* number of columns */
#define DCI_ATTR_LIST_COLUMNS     103        /* parameter of the column list */
#define DCI_ATTR_LIST_ARGUMENTS   108      /* parameter of the argument list */
#define DCI_ATTR_LIST_SUBPROGRAMS 109    /* parameter of the subprogram list */
#define DCI_ATTR_REF_TDO          110          /* REF to the type descriptor */
#define DCI_ATTR_LINK             111              /* the database link name */

#define DCI_ATTR_TIMESTAMP        119             /* timestamp of the object */
#define DCI_ATTR_NUM_ATTRS        120                /* number of sttributes */
#define DCI_ATTR_NUM_PARAMS       121                /* number of parameters */
#define DCI_ATTR_PTYPE            123           /* type of info described by */
#define DCI_ATTR_PARAM            124                /* parameter descriptor */
#define DCI_ATTR_LTYPE            128                           /* list type */

#define DCI_ATTR_OBJ_NAME         134           /* top level schema obj name */
#define DCI_ATTR_OBJ_SCHEMA       135                         /* schema name */
#define DCI_ATTR_OBJ_ID           136          /* top level schema object id */
#define DCI_ATTR_SERVER_STATUS    143          /* state of the server handle */
#define DCI_ATTR_DIRPATH_INPUT    151      /* input in text or stream format */

#define DCI_ATTR_CURRENT_POSITION 164      /* TODO:for scrollable result sets*/

#define DCI_DIRPATH_INPUT_TEXT     0x01
#define DCI_DIRPATH_INPUT_STREAM   0x02
#define DCI_DIRPATH_INPUT_UNKNOWN  0x04

/* TODO----------------------- Connection Pool Attributes ------------------ */
#define DCI_ATTR_CONN_NOWAIT      178
#define DCI_ATTR_CONN_BUSY_COUNT  179
#define DCI_ATTR_CONN_OPEN_COUNT  180
#define DCI_ATTR_CONN_TIMEOUT     181
#define DCI_ATTR_STMT_STATE       182
#define DCI_ATTR_CONN_MIN         183
#define DCI_ATTR_CONN_MAX         184
#define DCI_ATTR_CONN_INCR        185
/* TODO:end */

#define DCI_ATTR_DIRPATH_OID      187             /* loading into an OID col */
#define DCI_ATTR_BIND_COUNT       190             /* number of bind postions */

#define DCI_ATTR_ROWS_FETCHED     197 /* rows fetched in last call */

/* --------- round trip callback attributes in the process  handle --------- */
#define DCI_ATTR_RESERVED_458             458                    /* reserved */
#define DCI_ATTR_RESERVED_459             459                    /* reserved */
#define DCI_ATTR_SPOOL_AUTH               460  /* Auth handle on pool handle */
#define DCI_ATTR_SHOW_INVISIBLE_COLUMNS   460   /* invisible columns support */
#define DCI_ATTR_INVISIBLE_COL            461   /* invisible columns support */


#define DCI_CRED_RDBMS      1                  /* database username/password */

/*------------------------Error Return Values--------------------------------*/
#define DCI_SUCCESS           0            /* maps to SQL_SUCCESS of SAG CLI */
#define DCI_SUCCESS_WITH_INFO 1             /* maps to SQL_SUCCESS_WITH_INFO */
#define DCI_NO_DATA           100                     /* maps to SQL_NO_DATA */
#define DCI_ERROR             -1                        /* maps to SQL_ERROR */
#define DCI_INVALID_HANDLE    -2               /* maps to SQL_INVALID_HANDLE */
#define DCI_NEED_DATA         99                    /* maps to SQL_NEED_DATA */


#define DCI_CONTINUE -24200    /* Continue with the body of the DCI function */

#define DCI_NTV_SYNTAX 1    /* Use what so ever is the native lang of server */

/*------------------------Scrollable Cursor Fetch Options------------------- 
* For non-scrollable cursor, the only valid (and default) orientation is 
* DCI_FETCH_NEXT
*/
#define DCI_FETCH_CURRENT 0x01               /* refetching current position  */
#define DCI_FETCH_NEXT    0x02                                   /* next row */
#define DCI_FETCH_FIRST   0x04                /* first row of the result set */
#define DCI_FETCH_LAST    0x08             /* the last row of the result set */
#define DCI_FETCH_PRIOR   0x10       /* the previous row relative to current */
#define DCI_FETCH_ABSOLUTE 0x20                /* absolute offset from first */
#define DCI_FETCH_RELATIVE 0x40                /* offset relative to current */


#define DCI_DEFAULT         0x00000000 
/* the default value for parameters and attributes */

#define DCI_THREADED        0x00000001      /* appl. in threaded environment */
#define DCI_OBJECT          0x00000002  /* application in object environment */
#define DCI_UTF16           0x00004000        /* mode for all UTF16 metadata */

/*------------------------ DCILogon2 Modes ----------------------------------*/
#define DCI_LOGON2_SPOOL      0x0001     /* Use session pool */
#define DCI_LOGON2_CPOOL      0x0002     /* Use connection pool */
#define DCI_LOGON2_STMTCACHE  0x0004     /* Use Stmt Caching */
#define DCI_LOGON2_PROXY      0x0008     /* Proxy authentiaction */

/*----------------------- Execution Modes -----------------------------------*/
#define DCI_BATCH_MODE        0x01      /* batch the statement for execution */
#define DCI_EXACT_FETCH       0x02         /* fetch the exact rows specified */
#define DCI_KEEP_FETCH_STATE  0x04                                 /* unused */
#define DCI_STMT_SCROLLABLE_READONLY 0x08     /* if result set is scrollable */
#define DCI_DESCRIBE_ONLY     0x10            /* only describe the statement */
#define DCI_COMMIT_ON_SUCCESS 0x20        /* commit, if successful execution */
#define DCI_NON_BLOCKING      0x40                           /* non-blocking */
#define DCI_BATCH_ERRORS      0x80             /* batch errors in array dmls */
#define DCI_PARSE_ONLY        0x100              /* only parse the statement */
#define DCI_EXACT_FETCH_RESERVED_1 0x200                         /* reserved */
#define DCI_SHOW_DML_WARNINGS 0x400   
/* return DCI_SUCCESS_WITH_INFO for delete/update w/no where clause */


/*=======================Describe Handle Parameter Attributes ===============*/

/* Attributes common to Columns and Stored Procs */
#define DCI_ATTR_DATA_SIZE      1                /* maximum size of the data */
#define DCI_ATTR_DATA_TYPE      2     /* the SQL type of the column/argument */
#define DCI_ATTR_DISP_SIZE      3                        /* the display size */
#define DCI_ATTR_NAME           4         /* the name of the column/argument */
#define DCI_ATTR_PRECISION      5                /* precision if number type */
#define DCI_ATTR_SCALE          6                    /* scale if number type */
#define DCI_ATTR_IS_NULL        7                            /* is it null ? */
#define DCI_ATTR_TYPE_NAME      8
/* name of the named data type or a package name for package private types */
#define DCI_ATTR_SCHEMA_NAME    9             /* the schema name */
#define DCI_ATTR_SUB_NAME       10      /* type name if package private type */
#define DCI_ATTR_POSITION       11
/* relative position of col/arg in the list of cols/args */


/* Only Stored Procs */
#define DCI_ATTR_LEVEL          211            /* level for structured types */
#define DCI_ATTR_HAS_DEFAULT    212                   /* has a default value */
#define DCI_ATTR_IOMODE         213                         /* in, out inout */
#define DCI_ATTR_RADIX          214                       /* returns a radix */
#define DCI_ATTR_NUM_ARGS       215             /* total number of arguments */

#define DCI_ATTR_TYPECODE                  216       /* object or collection */


#define DCI_ATTR_CHAR_USED                 285      /* char length semantics */
#define DCI_ATTR_CHAR_SIZE                 286                /* char length */

/*---------------------------End Describe Handle Attributes -----------------*/


/*-----------------------Handle Definitions----------------------------------*/
typedef struct DCIEnv           DCIEnv;            /* DCI environment handle */
typedef struct DCIError         DCIError;                /* DCI error handle */
typedef struct DCISvcCtx        DCISvcCtx;             /* DCI service handle */
typedef struct DCIStmt          DCIStmt;             /* DCI statement handle */
typedef struct DCIBind          DCIBind;                  /* DCI bind handle */
typedef struct DCIDefine        DCIDefine;              /* DCI Define handle */
typedef struct DCIDescribe      DCIDescribe;          /* DCI Describe handle */
typedef struct DCIServer        DCIServer;              /* DCI Server handle */
typedef struct DCISession       DCISession;     /* DCI Authentication handle */
typedef struct DCITrans         DCITrans;          /* DCI Transaction handle */

typedef struct DCIDirPathCtx      DCIDirPathCtx;                  /* context */
typedef struct DCIDirPathColArray DCIDirPathColArray;        /* column array */
typedef struct DCIDirPathStream   DCIDirPathStream;                /* stream */

/*-----------------------Descriptor Definitions------------------------------*/
typedef struct DCISnapshot      DCISnapshot;      /* DCI snapshot descriptor */
typedef struct DCILobLocator    DCILobLocator; /* DCI Lob Locator descriptor */
typedef struct DCIParam         DCIParam;        /* DCI PARameter descriptor */
typedef struct DCIDateTime      DCIDateTime;      /* DCI DateTime descriptor */
typedef struct DCIInterval      DCIInterval;      /* DCI Interval descriptor */
typedef struct DCIRowid			DCIRowid;			 /* DCI Rowid descriptor */

typedef ub4 DCILobOffset;
typedef ub4 DCILobLength;
typedef ub2 DCIDuration;

enum DCILobMode
{
	DCI_LOBMODE_READONLY = 1,                                     /* read-only */
	DCI_LOBMODE_READWRITE = 2             /* read_write for internal lobs only */
};
typedef enum DCILobMode DCILobMode;


/*--------------------------- LOB open modes --------------------------------*/
#define DCI_LOB_READONLY 1              /* readonly mode open for ILOB types */
#define DCI_LOB_READWRITE 2                /* read write mode open for ILOBs */

/*--------------------------- DCI Statement Types ---------------------------*/
#define  DCI_STMT_UNKNOWN 0                               /* other statement */ 
#define  DCI_STMT_SELECT  1                              /* select statement */
#define  DCI_STMT_UPDATE  2                              /* update statement */
#define  DCI_STMT_DELETE  3                              /* delete statement */
#define  DCI_STMT_INSERT  4                              /* Insert Statement */
#define  DCI_STMT_CREATE  5                              /* create statement */
#define  DCI_STMT_DROP    6                                /* drop statement */
#define  DCI_STMT_ALTER   7                               /* alter statement */
#define  DCI_STMT_BEGIN   8                   /* begin ... (pl/sql statement)*/
#define  DCI_STMT_DECLARE 9                /* declare .. (pl/sql statement ) */
/*---------------------------------------------------------------------------*/

/*--------------------------- DCI Parameter Types ---------------------------*/
#define DCI_PTYPE_UNK                 0                         /* unknown   */
#define DCI_PTYPE_TABLE               1                         /* table     */
#define DCI_PTYPE_VIEW                2                         /* view      */
#define DCI_PTYPE_PROC                3                         /* procedure */
#define DCI_PTYPE_FUNC                4                         /* function  */
#define DCI_PTYPE_PKG                 5                         /* package   */
#define DCI_PTYPE_TYPE                6                 /* user-defined type */
#define DCI_PTYPE_SYN                 7                         /* synonym   */
#define DCI_PTYPE_SEQ                 8                         /* sequence  */
#define DCI_PTYPE_COL                 9                         /* column    */
#define DCI_PTYPE_ARG                 10                        /* argument  */
#define DCI_PTYPE_LIST                11                        /* list      */
#define DCI_PTYPE_TYPE_ATTR           12    /* user-defined type's attribute */
#define DCI_PTYPE_TYPE_COLL           13        /* collection type's element */
#define DCI_PTYPE_TYPE_METHOD         14       /* user-defined type's method */
#define DCI_PTYPE_TYPE_ARG            15   /* user-defined type method's arg */
#define DCI_PTYPE_TYPE_RESULT         16/* user-defined type method's result */
#define DCI_PTYPE_SCHEMA              17                           /* schema */
#define DCI_PTYPE_DATABASE            18                         /* database */
#define DCI_PTYPE_RULE                19                             /* rule */
#define DCI_PTYPE_RULE_SET            20                         /* rule set */
#define DCI_PTYPE_EVALUATION_CONTEXT  21               /* evaluation context */
#define DCI_PTYPE_TABLE_ALIAS         22                      /* table alias */
#define DCI_PTYPE_VARIABLE_TYPE       23                    /* variable type */
#define DCI_PTYPE_NAME_VALUE          24                  /* name value pair */

/*---------------------------------------------------------------------------*/

/*----------------------------- DCI List Types ------------------------------*/
#define DCI_LTYPE_UNK           0                               /* unknown   */
#define DCI_LTYPE_COLUMN        1                             /* column list */
#define DCI_LTYPE_ARG_PROC      2                 /* procedure argument list */
#define DCI_LTYPE_ARG_FUNC      3                  /* function argument list */
#define DCI_LTYPE_SUBPRG        4                         /* subprogram list */
#define DCI_LTYPE_TYPE_ATTR     5                          /* type attribute */
#define DCI_LTYPE_TYPE_METHOD   6                             /* type method */
#define DCI_LTYPE_TYPE_ARG_PROC 7    /* type method w/o result argument list */
#define DCI_LTYPE_TYPE_ARG_FUNC 8      /* type method w/result argument list */
#define DCI_LTYPE_SCH_OBJ       9                      /* schema object list */
#define DCI_LTYPE_DB_SCH        10                   /* database schema list */
#define DCI_LTYPE_TYPE_SUBTYPE  11                           /* subtype list */
#define DCI_LTYPE_TABLE_ALIAS   12                       /* table alias list */
#define DCI_LTYPE_VARIABLE_TYPE 13                     /* variable type list */
#define DCI_LTYPE_NAME_VALUE    14                        /* name value list */

/*----- values for cflg argument to DCIDirpathColArrayEntrySet -----*/
#define DCI_DIRPATH_COL_COMPLETE 0                /* column data is complete */
#define DCI_DIRPATH_COL_NULL     1                         /* column is null */
#define DCI_DIRPATH_COL_PARTIAL  2                 /* column data is partial */

/*----- values for action parameter to DCIDirPathDataSave -----*/
#define DCI_DIRPATH_DATASAVE_SAVEONLY 0              /* data save point only */
#define DCI_DIRPATH_DATASAVE_FINISH   1           /* execute finishing logic */
#define DCI_DIRPATH_DATASAVE_PARTIAL  2       
      /* save portion of input data (before space error occurred) and finish */

/*--------------------------- OBJECT INDICATOR ------------------------------*/

typedef sb2 DCIInd;
/*
* DCIInd -- a variable of this type contains (null) indicator information
*/

#define DCI_IND_NOTNULL (DCIInd)0                                /* not NULL */
#define DCI_IND_NULL (DCIInd)(-1)                                    /* NULL */
#define DCI_IND_BADNULL (DCIInd)(-2)                             /* BAD NULL */
#define DCI_IND_NOTNULLABLE (DCIInd)(-3)                     /* not NULLable */

/*------------------------ Transaction Start Flags --------------------------*/
#define DCI_TRANS_NEW          0x00000001 /* starts a new transaction branch */
#define DCI_TRANS_JOIN         0x00000002 /* join an existing transaction */
#define DCI_TRANS_RESUME       0x00000004 /* resume this transaction */
#define DCI_TRANS_STARTMASK    0x000000ff

#define DCI_TRANS_READONLY     0x00000100 /* starts a readonly transaction */
#define DCI_TRANS_READWRITE    0x00000200 /* starts a read-write transaction */ 

#define DCI_TRANS_SERIALIZABLE 0x00000400 /* starts a serializable transaction */
#define DCI_TRANS_ISOLMASK     0x0000ff00

#define DCI_TRANS_LOOSE        0x00010000 /* a loosely coupled branch */
#define DCI_TRANS_TIGHT        0x00020000 /* a tightly coupled branch */
#define DCI_TRANS_TYPEMASK     0x000f0000

#define DCI_TRANS_NOMIGRATE    0x00100000 /* non migratable transaction */
/*---------------------------------------------------------------------------*/

/*------------------------Bind and Define Options----------------------------*/
#define DCI_SB2_IND_PTR       0x00000001                           /* unused */
#define DCI_DATA_AT_EXEC      0x00000002             /* data at execute time */
#define DCI_DYNAMIC_FETCH     0x00000002                /* fetch dynamically */
#define DCI_PIECEWISE         0x00000004          /* piecewise DMLs or fetch */
#define DCI_DEFINE_RESERVED_1 0x00000008                         /* reserved */
#define DCI_BIND_RESERVED_2   0x00000010                         /* reserved */
#define DCI_DEFINE_RESERVED_2 0x00000020                         /* reserved */
#define DCI_BIND_SOFT         0x00000040              /* soft bind or define */
#define DCI_DEFINE_SOFT       0x00000080              /* soft bind or define */
#define DCI_BIND_RESERVED_3   0x00000100                         /* reserved */
#define DCI_IOV               0x00000200   /* For scatter gather bind/define */
/*---------------------------------------------------------------------------*/

/*------------------------Piece Information----------------------------------*/
#define DCI_PARAM_IN			1                     /* in parameter */
#define DCI_PARAM_OUT			2                    /* out parameter */
/*---------------------------------------------------------------------------*/

/* ----- Temporary attribute value for UCS2/UTF16 character set ID -------- */ 
#define DCI_UCS2ID            1000                        /* UCS2 charset ID */
#define DCI_UTF16ID           1000                       /* UTF16 charset ID */


/*---------------------------LOB TYPES---------------------------------------*/
#define DCI_TEMP_BLOB		1			/* LOB type - BLOB ------------------*/
#define DCI_TEMP_CLOB		2			/* LOB type - CLOB ------------------*/

/*--------------------------- FILE open modes -------------------------------*/
#define DCI_FILE_READONLY 1             /* readonly mode open for FILE types */
/*---------------------------------------------------------------------------*/
/*--------------------------- LOB open modes --------------------------------*/
#define DCI_LOB_READONLY 1              /* readonly mode open for ILOB types */
#define DCI_LOB_READWRITE 2                /* read write mode open for ILOBs */
#define DCI_LOB_WRITEONLY     3         /* Writeonly mode open for ILOB types*/
#define DCI_LOB_APPENDONLY    4       /* Appendonly mode open for ILOB types */
#define DCI_LOB_FULLOVERWRITE 5                 /* Completely overwrite ILOB */
#define DCI_LOB_FULLREAD      6                 /* Doing a Full Read of ILOB */

/*----------------------- LOB Buffering Flush Flags -------------------------*/
#define DCI_LOB_BUFFER_FREE   1 
#define DCI_LOB_BUFFER_NOFREE 2
/*---------------------------------------------------------------------------*/

/*---------------------------LOB Option Types -------------------------------*/
#define DCI_LOB_OPT_COMPRESS     1                    /* SECUREFILE Compress */
#define DCI_LOB_OPT_ENCRYPT      2                     /* SECUREFILE Encrypt */
#define DCI_LOB_OPT_DEDUPLICATE  4                 /* SECUREFILE Deduplicate */
#define DCI_LOB_OPT_ALLOCSIZE    8             /* SECUREFILE Allocation Size */
#define DCI_LOB_OPT_CONTENTTYPE 16                /* SECUREFILE Content Type */
#define DCI_LOB_OPT_MODTIME     32           /* SECUREFILE Modification Time */

/*---------------------------------------------------------------------------*/
/*TODO------------------------- Statement States ----------------------------*/

#define DCI_STMT_STATE_INITIALIZED  0x0001
#define DCI_STMT_STATE_EXECUTED     0x0002
#define DCI_STMT_STATE_END_OF_FETCH 0x0003
/*TODO:end*/

/*---------------- Server Handle Attribute Values ---------------------------*/

/* DCI_ATTR_SERVER_STATUS */
#define DCI_SERVER_NOT_CONNECTED        0x0 
#define DCI_SERVER_NORMAL               0x1 

#define  DCI_DURATION_INVALID 0xFFFF                     /* Invalid duration */
#define  DCI_DURATION_BEGIN (DCIDuration)10
                                           /* beginning sequence of duration */
#define  DCI_DURATION_NULL (DCIDuration)(DCI_DURATION_BEGIN-1)
                                                            /* null duration */
#define  DCI_DURATION_DEFAULT (DCIDuration)(DCI_DURATION_BEGIN-2) /* default */
#define  DCI_DURATION_USER_CALLBACK (DCIDuration)(DCI_DURATION_BEGIN-3)
#define  DCI_DURATION_NEXT (DCIDuration)(DCI_DURATION_BEGIN-4)
                                                    /* next special duration */
#define  DCI_DURATION_SESSION (DCIDuration)(DCI_DURATION_BEGIN)
                                                  /* the end of user session */
#define  DCI_DURATION_TRANS (DCIDuration)(DCI_DURATION_BEGIN+1)
                                              /* the end of user transaction */
#define DCI_STILL_EXECUTING -3123                   /* DCI would block error */											  
#define DCI_ATTR_NONBLOCKING_MODE  3                    /* non blocking mode */	

#define  DCI_DURATION_INVALID 0xFFFF                     /* Invalid duration */
#define  DCI_DURATION_BEGIN (DCIDuration)10
                                           /* beginning sequence of duration */
#define  DCI_DURATION_NULL (DCIDuration)(DCI_DURATION_BEGIN-1)
                                                            /* null duration */
#define  DCI_DURATION_DEFAULT (DCIDuration)(DCI_DURATION_BEGIN-2) /* default */
#define  DCI_DURATION_USER_CALLBACK (DCIDuration)(DCI_DURATION_BEGIN-3)
#define  DCI_DURATION_NEXT (DCIDuration)(DCI_DURATION_BEGIN-4)
                                                    /* next special duration */
#define  DCI_DURATION_SESSION (DCIDuration)(DCI_DURATION_BEGIN)
                                                  /* the end of user session */
#define  DCI_DURATION_TRANS (DCIDuration)(DCI_DURATION_BEGIN+1)
                                              /* the end of user transaction */
/******************************************************************************
**  DO NOT USE DCI_DURATION_CALL. IT  IS UNSUPPORTED                         **
**  WILL BE REMOVED/CHANGED IN A FUTURE RELEASE                              **
******************************************************************************/
#define  DCI_DURATION_CALL (DCIDuration)(DCI_DURATION_BEGIN+2)
                                       /* the end of user client/server call */
#define  DCI_DURATION_STATEMENT (DCIDuration)(DCI_DURATION_BEGIN+3)

/* This is to be used only during callouts.  It is similar to that 
of DCI_DURATION_CALL, but lasts only for the duration of a callout.
Its heap is from PGA */
#define  DCI_DURATION_CALLOUT (DCIDuration)(DCI_DURATION_BEGIN+4)

#define  DCI_DURATION_LAST DCI_DURATION_CALLOUT 
                                             /* last of predefined durations */
#define  DCI_DURAION_USER_MAX	(DCIDuration)65481			  /* The Max one.*/



/* This is not being treated as other predefined durations such as 
   SESSION, CALL etc, because this would not have an entry in the duration
   table and its functionality is primitive such that only allocate, free,
   resize memory are allowed, but one cannot create subduration out of this
*/
#define  DCI_DURATION_PROCESS (DCIDuration)(DCI_DURATION_BEGIN-5) 

/*
   SESSION, CALL etc, because this would not have an entry in the duration
   table and its functionality is primitive such that only allocate, free,
   resize memory are allowed, but one cannot create subduration out of this
*/
#define  DCI_DURATION_PROCESS (DCIDuration)(DCI_DURATION_BEGIN-5) 

/*
 * DCIDuration - DCI object duration
 *
 * A client can specify the duration of which an object is pinned (pin
 * duration) and the duration of which the object is in memory (allocation
 * duration).  If the objects are still pinned at the end of the pin duration,
 * the object cache manager will automatically unpin the objects for the
 * client. If the objects still exist at the end of the allocation duration,
 * the object cache manager will automatically free the objects for the client.
 *
 * Objects that are pinned with the option DCI_DURATION_TRANS will get unpinned
 * automatically at the end of the current transaction.
 *
 * Objects that are pinned with the option DCI_DURATION_SESSION will get
 * unpinned automatically at the end of the current session (connection).
 *
 * The option DCI_DURATION_NULL is used when the client does not want to set
 * the pin duration.  If the object is already loaded into the cache, then the
 * pin duration will remain the same.  If the object is not yet loaded, the
 * pin duration of the object will be set to DCI_DURATION_DEFAULT.
 */
										  
#endif  /* DCIDFN_H */
