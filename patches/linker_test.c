#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define TEST_EXECZ

void (*android_update_LD_LIBRARY_PATH_p)(const char*);

void* (*dlopen_p)(const char *, int);
void* (*dlsym_p)(void *, const char *);
int (*dlclose_p)(void *);
char* (*dlerror_p)(void);
int (*entry_start_p)(void);
void* (*__linker_init_mini_p)(int argc, char** argv);
void* dlsym_darwin_p;
void* dlerror_darwin_p;

#define BIND_METHOD(HANDLE, NAME) \
    NAME##_p = dlsym(HANDLE, #NAME); \
    if (!NAME##_p) { \
        printf("Warning: %s\n", dlerror());\
    }

int main(int argc, char** argv) {
    void *linker = dlopen("linker64", RTLD_LAZY);
    if (!linker) {
        printf("Error: %s\n", dlerror());
        return -1;
    }
    BIND_METHOD(linker, android_update_LD_LIBRARY_PATH);
    BIND_METHOD(linker, dlopen);
    BIND_METHOD(linker, dlsym);
    BIND_METHOD(linker, dlclose);
    BIND_METHOD(linker, dlerror);
    BIND_METHOD(linker, dlsym_darwin);
    BIND_METHOD(linker, dlerror_darwin);
    BIND_METHOD(linker, __linker_init_mini);
    if (dlsym_darwin_p) *((void**) dlsym_darwin_p) = dlsym;
    if (dlerror_darwin_p) *((void**) dlerror_darwin_p) = dlerror;

#ifdef TEST_EXEC
    if (argc == 1) {
      printf("Usage: %s [executable] [args...]\n", argv[0]);
      return -1;
    }
    
    entry_start_p = __linker_init_mini_p(argc - 1, &argv[1]);
    return entry_start_p();
#else

    __linker_init_mini_p(0, NULL);

    printf("%s\n", dlerror_p());
    android_update_LD_LIBRARY_PATH_p(getenv("PWD"));
    // sprintf(out_name, "%s/%s", getenv("PWD"), realpath("
    void *test_handle = dlopen_p(realpath("libminecraftpe.so", 0), RTLD_LAZY);
    // dlopen_p("/usr/android/system/lib64/libtest.so", RTLD_LAZY);
    if (!test_handle) {
        printf("%s\n", dlerror_p());
        return -1;
    }

    printf("loaded libtest = %p\n", test_handle);
    void(*test_func)();
    test_func = dlsym_p(test_handle, "helloTest");
    if (!test_func) {
        printf("%s\n", dlerror_p());
        return -1;
    }
    printf("loaded helloTest = %p\n", test_func);

    test_func();
#endif
}
