#include <stdio.h>
extern "C"
{
#include "dci.h"
#include "dciapi.h"
#include "dcidef.h"
}
#include <string>

#ifndef nullptr
#define nullptr NULL
#endif

int main()
{
    std::string user("system");
    std::string password("system");
    std::string database("qt4_test");
    int err = DCIInitialize((ub4)DCI_DEFAULT, nullptr, nullptr, nullptr, nullptr);
    if (err != DCI_SUCCESS)
    {
        printf("DCIInitialize failed\n");
        return err;
    }
    DCIEnv* pEnv = nullptr;
    err = DCIEnvCreate(&pEnv, DCI_DEFAULT, nullptr, nullptr, nullptr, nullptr, 0, nullptr);
    if (err != DCI_SUCCESS)
    {
        printf("DCIEnvCreate failed\n");
        return err;
    }

    // DCIHandle
}