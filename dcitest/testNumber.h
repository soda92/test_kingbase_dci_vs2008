#ifndef TESTNUMBER_H
#define TESTNUMBER_H
#include <cstdio>
#include <cstdlib>

#ifdef WIN32
#include <windows.h>
#endif

#include "DciTest.h"

#if _MSC_VER >= 1400
#pragma warning(disable : 4996)
#endif

sword TestNumberFetch();
sword TestNumberInsert();

sword TestNumberBatchFetch();
sword TestNumberBatchInsert();

sword TestDciNumberFromInt();
sword TestDciNumberToInt();

sword TestDciNumberFromReal();
sword TestDciNumberToReal();

sword TestDciNumberFromText();
sword TestDciNumberToText();


sword TestDciNumber();

#endif /* TESTNUMBER_H */
