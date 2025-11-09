#include <windows.h>
#include <iostream>
#include <jni.h>
#include <string>


#include <stdutil.h>

typedef struct {
    HMODULE hDLL;
    void *funcPtr;
} DLLFunctionHandle;

typedef enum {
    download = 0,
    FUNC_WRITE_MEMORY_2 = 1
} DLLFunction;

struct MemoryStruct {
    char *memory;
    size_t size;
};


//---------------------------------------------------------------------------------------------------------

typedef MemoryStruct *(*DownloadFile)(const char *, bool, const char *);


//---------------------------------------------------------------------------------------------------------

DLLFunctionHandle dllloader(const std::string dllName, DLLFunction funcType) {


    DLLFunctionHandle handle = {0};
   
    const char *dllcharprt = dllName.c_str();

    handle.hDLL = LoadLibraryA(dllcharprt);
    if (!handle.hDLL) {
        return handle;
    }

    //-----------------------------------------------------------------------------------------------


    switch (funcType) {
        case download:
            handle.funcPtr = (void *)GetProcAddress(handle.hDLL, "DownloadFile");
            break;

        case FUNC_WRITE_MEMORY_2:
            handle.funcPtr = (void *)GetProcAddress(handle.hDLL, "WriteMemoryCallback2");
            break;

        default:
            printf("not a function that is callable");
            break;
    }

    if (!handle.funcPtr) {
        FreeLibrary(handle.hDLL);
        handle.hDLL = NULL;
    }

    return handle;
}


JNIEXPORT void JNICALL Java_stdutil_frepy(JNIEnv *env, jobject, jobject var1){
    free(var1);
}

JNIEXPORT jstring JNICALL Java_stdutil_downloadfile(JNIEnv *env, jobject, jstring url, jboolean filewrite, jstring outputPath){

    std::string nativeString = env->GetStringUTFChars(url, nullptr);
    const char *urll = nativeString.c_str();
    bool writetofile = (bool)(filewrite);
    std::string opt = env->GetStringUTFChars(outputPath, nullptr);
    const char *op = opt.c_str();

    DLLFunctionHandle handle = dllloader("helper.dll", download);
    DownloadFile func = (DownloadFile)handle.funcPtr;
    MemoryStruct *strt = func(urll, writetofile, op);
    std::string htnt= strt->memory;

    jstring ret = env->NewStringUTF(htnt.c_str());;
    return ret;
}
