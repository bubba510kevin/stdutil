#ifndef stdlib_H
#define stdlib_H

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct {
    HMODULE hDLL;
    void *funcPtr;
} DLLFunctionHandle;

struct MemoryStruct {
    char *memory;
    size_t size;
};

DLLFunctionHandle dllloader(const char *dllName, char *funcname);
MemoryStruct *DownloadFile(const char *url, bool filewrite, const char *outputPath);

#ifdef __cplusplus
}
#endif

#endif