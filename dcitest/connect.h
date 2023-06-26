#ifndef CONNECT_H
#define CONNECT_H

sword	connect(bool useLogon);
sword	disconnect(bool useLogon);
sword	test_logon();
sword	test_sessionbegin();

sword	connect2(bool useLogon);
sword	test_logon2();
sword	test_sessionbegin2();

sword	connect_UTF16(bool useLogon);
sword	disconnect_UTF16(bool useLogon);

sword   TestCloseSvcSocket();

#endif /* CONNECT_H */
