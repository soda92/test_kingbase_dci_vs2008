/*
 * $Header: dciapi.h $
 */

/* Copyright (c) 2010, BaseSoft Inc.  All rights reserved.  */

/* 
   NAME 
     dciapi.h - Database Call Interface - Ansi Prototypes

*/

#ifndef DCIAPI_H
#define DCIAPI_H

#include <stddef.h>

#include "dcidef.h"
#include "dcitypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

sword   DCIInitialize   (ub4 mode, dvoid *ctxp, 
                 dvoid *(*malocfp)(dvoid *ctxp, size_t size),
                 dvoid *(*ralocfp)(dvoid *ctxp, dvoid *memptr, size_t newsize),
                 void   (*mfreefp)(dvoid *ctxp, dvoid *memptr) );

sword   DCIEnvCreate (DCIEnv **envp, ub4 mode, dvoid *ctxp,
					  dvoid *(*malocfp)(dvoid *ctxp, size_t size),
					  dvoid *(*ralocfp)(dvoid *ctxp, dvoid *memptr, size_t newsize),
					  void   (*mfreefp)(dvoid *ctxp, dvoid *memptr),
					  size_t xtramem_sz, dvoid **usrmempp);

sword   DCIHandleAlloc(CONST dvoid *parenth, dvoid **hndlpp, CONST ub4 type, 
                       CONST size_t xtramem_sz, dvoid **usrmempp);

sword   DCIHandleFree(dvoid *hndlp, CONST ub4 type);


sword   DCIDescriptorAlloc(CONST dvoid *parenth, dvoid **descpp, 
                           CONST ub4 type, CONST size_t xtramem_sz, 
                           dvoid **usrmempp);

sword   DCIDescriptorFree(dvoid *descp, CONST ub4 type);

sword   DCIEnvInit (DCIEnv **envp, ub4 mode, 
                    size_t xtramem_sz, dvoid **usrmempp);

sword   DCIServerAttach  (DCIServer *srvhp, DCIError *errhp,
                          CONST DciText *dblink, sb4 dblink_len, ub4 mode);

sword   DCIServerDetach  (DCIServer *srvhp, DCIError *errhp, ub4 mode);

sword   DCISessionBegin  (DCISvcCtx *svchp, DCIError *errhp, DCISession *usrhp,
                          ub4 credt, ub4 mode);

sword   DCISessionEnd   (DCISvcCtx *svchp, DCIError *errhp, DCISession *usrhp, 
                         ub4 mode);

sword   DCILogon (DCIEnv *envhp, DCIError *errhp, DCISvcCtx **svchp, 
                  CONST DciText *username, ub4 uname_len, 
                  CONST DciText *password, ub4 passwd_len, 
                  CONST DciText *dbname, ub4 dbname_len);

sword   DCILogon2(DCIEnv *envhp, DCIError *errhp, DCISvcCtx **svchp, 
                  CONST DciText *username, ub4 uname_len, 
                  CONST DciText *password, ub4 passwd_len, 
                  CONST DciText *dbname, ub4 dbname_len, ub4 mode);

sword   DCILogoff (DCISvcCtx *svchp, DCIError *errhp);


sword   DCIStmtPrepare   (DCIStmt *stmtp, DCIError *errhp, CONST DciText *stmt,
                          ub4 stmt_len, ub4 language, ub4 mode);

sword   DCIBindByPos  (DCIStmt *stmtp, DCIBind **bindp, DCIError *errhp,
                       ub4 position, dvoid *valuep, sb4 value_sz,
                       ub2 dty, dvoid *indp, ub2 *alenp, ub2 *rcodep,
                       ub4 maxarr_len, ub4 *curelep, ub4 mode);

sword   DCIBindByName   (DCIStmt *stmtp, DCIBind **bindp, DCIError *errhp,
                         CONST DciText *placeholder, sb4 placeh_len, 
                         dvoid *valuep, sb4 value_sz, ub2 dty, 
                         dvoid *indp, ub2 *alenp, ub2 *rcodep, 
                         ub4 maxarr_len, ub4 *curelep, ub4 mode);

typedef sb4 (*DCICallbackInBind)(dvoid *ictxp, DCIBind *bindp, ub4 iter,
                                  ub4 index, dvoid **bufpp, ub4 *alenp,
                                  ub1 *piecep, dvoid **indp);

typedef sb4 (*DCICallbackOutBind)(dvoid *octxp, DCIBind *bindp, ub4 iter,
                                 ub4 index, dvoid **bufpp, ub4 **alenp,
                                 ub1 *piecep, dvoid **indp,
                                 ub2 **rcodep);

sword   DCIBindDynamic   (DCIBind *bindp, DCIError *errhp, dvoid *ictxp,
                          DCICallbackInBind icbfp, dvoid *octxp,
                          DCICallbackOutBind ocbfp);

sword	DCIStmtGetBindInfo(DCIStmt *stmtp, DCIError *errhp, ub4 size, 
							ub4 startloc,
							sb4 *found, DciText *bvnp[], ub1 bvnl[],
							DciText *invp[], ub1 inpl[], ub1 dupl[],
							DCIBind *hndl[]);

sword   DCIBindArrayOfStruct   (DCIBind *bindp, DCIError *errhp, 
                                ub4 pvskip, ub4 indskip,
                                ub4 alskip, ub4 rcskip);

sword   DCIStmtExecute  (DCISvcCtx *svchp, DCIStmt *stmtp, DCIError *errhp, 
                         ub4 iters, ub4 rowoff, CONST DCISnapshot *snap_in, 
                         DCISnapshot *snap_out, ub4 mode);

sword   DCIDefineByPos  (DCIStmt *stmtp, DCIDefine **defnp, DCIError *errhp,
                         ub4 position, dvoid *valuep, sb4 value_sz, ub2 dty,
                         dvoid *indp, ub2 *rlenp, ub2 *rcodep, ub4 mode);

sword   DCIDefineArrayOfStruct  (DCIDefine *defnp, DCIError *errhp, ub4 pvskip,
                                 ub4 indskip, ub4 rlskip, ub4 rcskip);

sword   DCIStmtFetch   (DCIStmt *stmtp, DCIError *errhp, ub4 nrows, 
                        ub2 orientation, ub4 mode);
/* bug#19687 */
sword   DCIStmtFetch2   (DCIStmt *stmtp, DCIError *errhp, ub4 nrows, 
                        ub2 orientation, sb4 fetchOffset, ub4 mode);

sword   DCIDescribeAny  (DCISvcCtx *svchp, DCIError *errhp, 
                         dvoid *objptr, 
                         ub4 objnm_len, ub1 objptr_typ, ub1 info_level,
                         ub1 objtyp, DCIDescribe *dschp);

sword   DCIParamGet (CONST dvoid *hndlp, ub4 htype, DCIError *errhp, 
                     dvoid **parmdpp, ub4 pos);

sword   DCIParamSet(dvoid *hdlp, ub4 htyp, DCIError *errhp, CONST dvoid *dscp,
                    ub4 dtyp, ub4 pos);

sword   DCITransStart  (DCISvcCtx *svchp, DCIError *errhp, 
                        uword timeout, ub4 flags );

sword   DCITransDetach  (DCISvcCtx *svchp, DCIError *errhp, ub4 flags );

sword   DCITransCommit  (DCISvcCtx *svchp, DCIError *errhp, ub4 flags);

sword   DCITransRollback  (DCISvcCtx *svchp, DCIError *errhp, ub4 flags);

sword   DCIErrorGet   (dvoid *hndlp, ub4 recordno, DciText *sqlstate,
                       sb4 *errcodep, DciText *bufp, ub4 bufsiz, ub4 type);

sword   DCILobGetLength  (DCISvcCtx *svchp, DCIError *errhp, 
                          DCILobLocator *locp,
                          ub4 *lenp);

typedef sb4 (*DCICallbackLobRead)(dvoid *ctxp, CONST dvoid *bufp,
								  ub4 len, ub1 piece);

typedef sb4 (*DCICallbackLobWrite)(dvoid *ctxp, dvoid *bufp, 
								   ub4 *lenp, ub1 *piece);

sword   DCILobRead  (DCISvcCtx *svchp, DCIError *errhp, DCILobLocator *locp,
                     ub4 *amtp, ub4 offset, dvoid *bufp, ub4 bufl, dvoid *ctxp,
                     DCICallbackLobRead cbfp, ub2 csid, ub1 csfrm);

sword   DCILobWrite  (DCISvcCtx *svchp, DCIError *errhp, DCILobLocator *locp,
                      ub4 *amtp, ub4 offset, dvoid *bufp, ub4 buflen, ub1 piece,
                      dvoid *ctxp, DCICallbackLobWrite cbfp, ub2 csid,
                      ub1 csfrm);

sword   DCIAttrGet (CONST dvoid *trgthndlp, ub4 trghndltyp, 
                    dvoid *attributep, ub4 *sizep, ub4 attrtype, 
                    DCIError *errhp);

sword   DCIAttrSet (dvoid *trgthndlp, ub4 trghndltyp, dvoid *attributep,
                    ub4 size, ub4 attrtype, DCIError *errhp);


sword   DCIDirPathDataSave( DCIDirPathCtx *dpctx, DCIError *errhp, ub4 action );

sword   DCIDirPathFinish( DCIDirPathCtx *dpctx, DCIError  *errhp );

sword   DCIDirPathFlushRow( DCIDirPathCtx *dpctx, DCIError  *errhp );

sword   DCIDirPathPrepare( DCIDirPathCtx *dpctx, DCISvcCtx *svchp,
				  DCIError  *errhp );

sword   DCIDirPathLoadStream( DCIDirPathCtx *dpctx, DCIDirPathStream *dpstr,
					 DCIError      *errhp );

sword   DCIDirPathColArrayEntryGet( DCIDirPathColArray *dpca, DCIError *errhp,
						   ub4 rownum, ub2 colIdx, ub1 **cvalpp, ub4 *clenp,
						   ub1 *cflgp );

sword   DCIDirPathColArrayEntrySet( DCIDirPathColArray *dpca, DCIError *errhp,
						   ub4 rownum, ub2 colIdx, ub1 *cvalp, ub4 clen,
						   ub1 cflg );

sword   DCIDirPathColArrayRowGet( DCIDirPathColArray *dpca, DCIError *errhp,
						 ub4 rownum, ub1 ***cvalppp, ub4 **clenpp,
						 ub1 **cflgpp );

sword   DCIDirPathColArrayReset( DCIDirPathColArray *dpca, DCIError *errhp );

sword   DCIDirPathColArrayToStream( DCIDirPathColArray *dpca,  DCIDirPathCtx *dpctx,
						   DCIDirPathStream   *dpstr, DCIError      *errhp,
						   ub4 rowcnt, ub4 rowoff );

sword   DCIDirPathStreamReset( DCIDirPathStream *dpstr, DCIError *errhp );

// Lob Functions
sword	DCILobAssign(DCIEnv *envhp, DCIError *errhp, CONST DCILobLocator *src_locp, DCILobLocator **dst_locp);

sword	DCILobAppend(DCISvcCtx *svchp, DCIError *errhp, DCILobLocator *dst_locp, DCILobLocator *src_locp);

sword	DCILobCopy(DCISvcCtx *svchp, DCIError *errhp, DCILobLocator *dst_locp, 
				 DCILobLocator *src_locp, ub4 amount, ub4 dst_offset, ub4 src_offset);

sword	DCILobErase(DCISvcCtx *svchp, DCIError *errhp, DCILobLocator *locp, ub4 *amount, ub4 offset);

sword	DCILobEnableBuffering(DCISvcCtx *svchp, DCIError *errhp, DCILobLocator *locp);

sword	DCILobDisableBuffering(DCISvcCtx *svchp, DCIError *errhp, DCILobLocator *locp);

sword	DCILobFlushBuffer(DCISvcCtx *svchp, DCIError *errhp, DCILobLocator *locp, ub4 flag);

sword	DCILobIsEqual(DCIEnv *envhp, CONST DCILobLocator *x, CONST DCILobLocator *y, boolean *is_equal);

sword	DCILobLocatorIsInit(DCIEnv *envhp, DCIError *errhp, CONST DCILobLocator *locp, boolean *is_initialized);

sword	DCILobCharSetForm(DCIEnv *envhp, DCIError *errhp, CONST DCILobLocator *locp, ub1 *csfrm);

sword	DCILobCharSetId(DCIEnv *envhp, DCIError *errhp, CONST DCILobLocator *locp, ub2 *csid);

sword	DCILobTrim(DCISvcCtx *svchp, DCIError *errhp, DCILobLocator *locp, ub4 newlen);


sword	DCIServerVersion(dvoid *hndlp, DCIError *errhp, text *bufp, ub4 bufsz, ub1 hndltype);

sword DCIDateTimeConstruct(void *hndl, 
						  DCIError *err, 
						  DCIDateTime *datetime, 
						  sb2 year, ub1 month, ub1 day, ub1 hour, ub1 min, ub1 sec, ub4 fsec, 
						  DciText *timezone, size_t timezone_length);
sword DCIDateTimeGetDate (void               *hndl,
						  DCIError           *err, 
						  const DCIDateTime  *datetime,
						  sb2                *year, 
						  ub1                *month, 
						  ub1                *day );

sword DCIDateTimeGetTime (void          *hndl, 
						  DCIError      *err, 
						  DCIDateTime   *datetime, 
						  ub1           *hour,
						  ub1           *min, 
						  ub1           *sec, 
						  ub4           *fsec );

sword DCIIntervalGetDaySecond (void *hndl, 
							  DCIError           *err, 
							  sb4                *dy, 
							  sb4                *hr,
							  sb4                *mm, 
							  sb4                *ss, 
							  sb4                *fsec, 
							  const DCIInterval  *interval);
sword DCIIntervalGetYearMonth ( void               *hndl, 
							   DCIError           *err, 
							   sb4                *yr,
							   sb4                *mnth,
							   const DCIInterval  *interval );
sword DCIIntervalFromText(void           *hndl, 
						 DCIError	    *err, 
						 const DciText  *inpstring,
						 size_t         str_len,
						 DCIInterval    *result);

sword
DCIIntervalAssign( void					*hndl, 
					DCIError 			*err, 
					const DCIInterval 	*inpinter, 
					DCIInterval 		*outinter );

sword
DCIIntervalSetDaySecond ( void 				*hndl, 
								DCIError 			*err, 
								sb4 				dy,
								sb4 				hr,
								sb4 				mm, 
								sb4 				ss, 
								sb4 				fsec, 
								DCIInterval 			*result );

sword
DCIIntervalSetYearMonth ( void 				*hndl, 
								DCIError 			*err, 
								sb4 				yr,
								sb4 				mnth,
								DCIInterval 			*result );

sword
DCIIntervalAdd(void *hndl, DCIError *err, DCIInterval *addend1, 
						DCIInterval *addend2, DCIInterval *result );

sword
DCIIntervalToText ( void 				*hndl,
						DCIError 			*err,
						const DCIInterval 	*interval,
						ub1 				lfprec,
						ub1 				fsprec,
						DciText 			*buffer, 
						size_t 				buflen,
						size_t 				*resultlen );

sword 
DCINumberToText(DCIError *err, CONST DCINumber *number, 
				CONST DciText *fmt, ub4 fmt_length,
				CONST DciText *nls_params, ub4 nls_p_length,
				ub4 *buf_size, DciText *buf);

sword
DCINumberFromText(DCIError *err, CONST DciText *str, 
				  ub4 str_length, CONST DciText *fmt, ub4 fmt_length, 
				  CONST DciText *nls_params, ub4 nls_p_length, 
				  DCINumber *number);

sword
DCINumberToInt(DCIError *err, CONST DCINumber *number,
			   uword rsl_length, uword rsl_flag, dvoid *rsl);

sword
DCINumberFromInt(DCIError *err, CONST dvoid *inum, uword inum_length,
				 uword inum_s_flag, DCINumber *number);

sword 
DCINumberToReal(DCIError *err, CONST DCINumber *number,
				uword rsl_length, dvoid *rsl);

sword 
DCINumberFromReal(DCIError *err, CONST dvoid *rnum,
				  uword rnum_length, DCINumber *number);
sword DCIDateTimeFromText(void          	*hndlp, 
						  DCIError			*errhp,
						  const DciText 	*date_str ,
						  size_t 			dstr_length,
						  const DciText 	*fmt,	
						  ub1 				fmt_length, 
						  const DciText 	*lang_name, 
						  size_t 			lang_length, 
						  DCIDateTime   	*datetime );
sword DCIDateTimeToText(void            	*hndlp,
						DCIError 			*errhp, 
						const DCIDateTime   *datetime, 
						const DciText 		*fmt, 
						ub1 				fmt_length, 
						ub1 				fsprec, 
						const DciText 		*lang_name, 
						size_t 				lang_length, 
						ub4 				*bufsz, 
						DciText 			*bufp);

sword
DCIDateFromText(DCIError *errhp, const DciText *date_str, size_t dstr_length, const DciText *fmt,
				ub1 fmt_length, const DciText *lang_name, size_t lang_length, DCIDate *date);

sword
DCIDateToText(DCIError *errhp, const DCIDate *date, const DciText *fmt, ub1 fmt_length, 
				const DciText *lang_name, size_t lang_length, ub4 *bufsz, DciText *bufp);

sword
DCIStmtGetPieceInfo(DCIStmt *stmtp, DCIError *errhp, void **hndlpp,
							  ub4 *typep, ub1 *in_outp, ub4 *iterp, ub4 *idxp,
							  ub1 *piecep);

sword   
DCIStmtSetPieceInfo(void *hndlp, ub4 type, DCIError *errhp, const void *bufp,
							  ub4 *alenp, ub1 piece, const void *indp, ub2 *rcodep); 

sword 
DCICloseSvcSocket(DCISvcCtx *svchp);

sword 
DCINumberMod(DCIError			*err,
			 const DCINumber	*number1, 
			 const DCINumber	*number2,
			 DCINumber			*result );
sword 
DCINumberAdd(DCIError			*err,
			 const DCINumber	*number1, 
			 const DCINumber	*number2,
			 DCINumber			*result );
sword 
DCINumberDiv(DCIError			*err,
			 const DCINumber	*number1, 
			 const DCINumber	*number2,
			 DCINumber			*result);
sword 
DCINumberAssign(DCIError			*err,
				const DCINumber		*from, 
				DCINumber			*to );
sword 
DCINumberMul(DCIError			*err,
			 const DCINumber	*number1, 
			 const DCINumber	*number2,
			 DCINumber			*result );
sword 
DCINumberIsInt(DCIError			*err, 
			   const DCINumber	*number,
			   boolean			*result );

void 
DCIDateGetDate(const DCIDate 	*date,
				sb2 				*year, 
				ub1 				*month, 
				ub1	 			*day);

void
DCIDateGetTime (const DCIDate 	*date,
				ub1				*hour,
				ub1				*min, 
				ub1				*sec );

void
DCIDateSetDate (DCIDate 	*date, 
				sb2			year, 
				ub1			month, 
				ub1			day );

void
DCIDateSetTime (DCIDate 	*date,
				ub1			hour, 
				ub1			min, 
				ub1			sec );

sword 
DCIDateSysDate(DCIError *err, DCIDate *sys_date);

sword
DCIDateAssign(    DCIError *err, const DCIDate *from, 
                        DCIDate *to    );

sword
DCIDateTimeSysTimeStamp(dvoid *hndl, DCIError *err, 
                              DCIDateTime *sys_date);

sword
DCIDateTimeAssign(dvoid *hndl, DCIError *err, CONST DCIDateTime *from, 
                       	DCIDateTime *to);

sword 
DCILobCreateTemporary(	DCISvcCtx          *svchp,
					  DCIError           *errhp,
					  DCILobLocator      *locp,
					  ub2                csid,
					  ub1                csfrm,
					  ub1                lobtype,
					  boolean            cache,
					  DCIDuration        duration);
sword 
DCILobFreeTemporary( DCISvcCtx         *svchp,
					DCIError           *errhp,
					DCILobLocator      *locp);
sword 
DCILobOpen ( DCISvcCtx        *svchp,
			DCIError         *errhp, 
			DCILobLocator    *locp, 
			ub1              mode );
sword 
DCILobClose ( DCISvcCtx      *svchp,
			 DCIError       *errhp, 
			 DCILobLocator  *locp );
sword 
DCILobIsTemporary(DCIEnv            *envhp,
				DCIError          *errhp,
				DCILobLocator     *locp,
				boolean           *is_temporary);

sword 
DCILobIsOpen (DCISvcCtx      *svchp,
			DCIError         *errhp, 
			DCILobLocator    *locp, 
			boolean          *flag );

sword 
DCIDurationEnd (DCIEnv           *env, 
				DCIError           *err, 
				const DCISvcCtx    *svc,
				DCIDuration        duration);

sword 
DCIDurationBegin(DCIEnv             *env,
				 DCIError            *err,
				 const DCISvcCtx     *svc, 
				 DCIDuration         parent,
				 DCIDuration         *duration);
				 
sword 
DCITransDetach ( DCISvcCtx    *svchp,
				DCIError     *errhp,
				ub4          flags );
sword 
DCITransPrepare(DCISvcCtx    *svchp, 
				DCIError     *errhp,
				ub4          flags );
sword 
DCIRowidToChar (DCIRowid	*rowidDesc,
				DciText		*outbfp,
				ub2			*outbflp,
				DCIError	*errhp );

sword DCINumberAbs(DCIError *err, CONST DCINumber *number,
				DCINumber *result);

sword DCINumberNeg(DCIError *err, CONST DCINumber *number,
				DCINumber *result);

sword DCINumberSub(DCIError *err, CONST DCINumber *number1,
				CONST DCINumber *number2, DCINumber *result);

#ifdef __cplusplus
}
#endif

#endif  /* DCIAPI_H */
