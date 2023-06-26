#ifndef TESTPIECE_H
#define TESTPIECE_H
#include <cstdio>
#include <cstdlib>

#ifdef WIN32
#include <windows.h>
#endif

#include "DciTest.h"

#if _MSC_VER >= 1400
#pragma warning(disable : 4996)
#endif

sword TestPieceMultiColsMultiRows();

#endif /* TESTPIECE_H */
