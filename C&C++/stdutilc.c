#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <curl/curl.h>
#include <stdbool.h>
#include "stdutilc.h"

/*---------------------------------------------------------------------------------------------------
dllloader is used by C|C++ now it called by 
    DLLFunctionHandle handle = dllloader("yourdll.dll", your func indefier);
    if (!handle.hDLL || !handle.funcPtr) {
        return;
    }
    your typedef callback = (your typedef)handle.funcPtr;
    typedef return val (*typedef name)(args);
-----------------------------------------------------------------------------------------------------
*/ 

typedef MemoryStruct *(*downloadFile)(const char *, bool, const char *);

DLLFunctionHandle dllloader(const char *dllName, char *funcname) {


    DLLFunctionHandle handle = {0};

    handle.hDLL = LoadLibraryA(dllName);
    if (!handle.hDLL) {
        printf("[!] v Could not load DLL: %s\n", dllName);
        return handle;
    }

    //-----------------------------------------------------------------------------------------------

    handle.funcPtr = (void *)GetProcAddress(handle.hDLL, funcname);

    if (!handle.funcPtr) {
        printf("[!] Could not find requested function in DLL.\n");
        FreeLibrary(handle.hDLL);
        handle.hDLL = NULL;
    }

    return handle;
}

MemoryStruct *DownloadFile(const char *url, bool filewrite, const char *outputPath){

    DLLFunctionHandle handle = dllloader("stdutil.dll", "DownloadFile");
    if (!handle.hDLL || !handle.funcPtr) {
        printf("[!] Failed to load helper DLL or get function.\n");
        return;
    }
    downloadFile func = (downloadFile)handle.funcPtr;
    MemoryStruct *strt = func(urll, writetofile, op);
    return strt;
}
