#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <curl/curl.h>
#include <stdbool.h>

typedef size_t (*WriteMemoryCallback)(void *, size_t, size_t, void *);
typedef size_t (*WriteMemoryCallback2)(void *, size_t, size_t, void *);


typedef enum {
    FUNC_WRITE_MEMORY = 0,
    FUNC_WRITE_MEMORY_2 = 1
} DLLFunction;

typedef struct {
    HMODULE hDLL;
    void *funcPtr;
} DLLFunctionHandle;

struct MemoryStruct {
    char *memory;
    size_t size;
};

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

DLLFunctionHandle dllloader(const char *dllName, DLLFunction funcType) {


    DLLFunctionHandle handle = {0};

    handle.hDLL = LoadLibraryA(dllName);
    if (!handle.hDLL) {
        printf("[!] v Could not load DLL: %s\n", dllName);
        return handle;
    }

    //-----------------------------------------------------------------------------------------------


    switch (funcType) {
        case FUNC_WRITE_MEMORY:
            handle.funcPtr = (void *)GetProcAddress(handle.hDLL, "WriteMemoryCallback");
            break;

        case FUNC_WRITE_MEMORY_2:
            handle.funcPtr = (void *)GetProcAddress(handle.hDLL, "WriteMemoryCallback2");
            break;

        default:
            printf("[!] Unknown function type.\n");
            break;
    }

    if (!handle.funcPtr) {
        printf("[!] Could not find requested function in DLL.\n");
        FreeLibrary(handle.hDLL);
        handle.hDLL = NULL;
    }

    return handle;
}

/* -------------------------------------------------------------------------------------------
download file will take your url and either download your file to your disk or the process memory by the bool writeToFile
*/ 

__declspec(dllexport) struct MemoryStruct *DownloadFile(const char *url, bool filewrite, const char *outputPath) {
    DLLFunctionHandle handle = dllloader("helper.dll", FUNC_WRITE_MEMORY);
    if (!handle.hDLL || !handle.funcPtr) {
        printf("[!] Failed to load helper DLL or get function.\n");
        return NULL;
    }

    WriteMemoryCallback callback = (WriteMemoryCallback)handle.funcPtr;
    CURL *curl = curl_easy_init();
    if (!curl) {
        printf("[!] Failed to initialize curl.\n");
        FreeLibrary(handle.hDLL);
        return NULL;
    }

    void *result = NULL;

    curl_easy_setopt(curl, CURLOPT_URL, url);

    if (filewrite) {
        FILE *fp = fopen(outputPath, "wb");
        if (!fp) {
            printf("[!] Failed to open file for writing: %s\n", outputPath);
            curl_easy_cleanup(curl);
            FreeLibrary(handle.hDLL);
            return NULL;
        }

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            fprintf(stderr, "[!] Download failed: %s\n", curl_easy_strerror(res));
        else
            printf("[+] File saved to: %s\n", outputPath);

        fclose(fp);
    } else {
        struct MemoryStruct *chunk = malloc(sizeof(struct MemoryStruct));
        chunk->memory = malloc(1);
        chunk->size = 0;

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)chunk);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "[!] Download failed: %s\n", curl_easy_strerror(res));
            free(chunk->memory);
            free(chunk);
            chunk = NULL;
        } else {
            printf("[+] Downloaded %zu bytes into memory.\n", chunk->size);
            result = chunk;  // return the struct pointer
        }
    }

    curl_easy_cleanup(curl);
    FreeLibrary(handle.hDLL);

    return result;
}

__declspec(dllexport) void freepy(void *thing) {
    free(thing);
}
