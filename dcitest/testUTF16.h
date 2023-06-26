#ifndef TESTUTF16_H
#define TESTUTF16_H

int ascii_to_utf16(char *src, char *dst);
int utf16_to_utf16(char *src_LE, char *dst, int len_bytes);

sword test_UTF16_logon();
sword test_UTF16_execute(char *sql);

sword test_UTF16_insert();
sword test_UTF16_insert_batch();
sword test_UTF16_fetch();
sword test_UTF16_fetch_batch();

sword test_UTF16();

sword test_UTF16_statement_chinese();

ub4	ucs2bytelen(const char *ucs2str, ub4 inputbytelen);
sword test_UTF16_AttrGet_NAME();

extern char	User_UTF16[NAME_LEN * 2];
extern char	Pwd_UTF16[NAME_LEN * 2];
extern char	DbName_UTF16[NAME_LEN * 2];
extern int test_utf16_run;

//static int bigendian = 0; repace it with BYTE_ORDER

#endif //TESTUTF16_H
