#include <curl/curl.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

struct MemoryStruct {
  char *memory;
  size_t size;
};

__declspec(dllexport) size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
  if (ptr == NULL) {
    /* out of memory! */
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0; // Null-terminate the string

  return realsize;
}


__declspec(dllexport)size_t WriteMemoryCallback2(void *contents, size_t size, size_t nmemb, void *userp) {
  FILE *fp = (FILE *)userp;  // `userp` will be your open FILE*
  size_t written = fwrite(contents, size, nmemb, fp);
  return written;
}

