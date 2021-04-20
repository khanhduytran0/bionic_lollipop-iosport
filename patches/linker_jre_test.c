#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "jni.h"

typedef jint JLI_Launch_func(int argc, char ** argv, /* main argc, argc */
        int jargc, const char** jargv,          /* java args */
        int appclassc, const char** appclassv,  /* app classpath */
        const char* fullversion,                /* full version defined */
        const char* dotversion,                 /* dot version defined */
        const char* pname,                      /* program name */
        const char* lname,                      /* launcher name */
        jboolean javaargs,                      /* JAVA_ARGS */
        jboolean cpwildcard,                    /* classpath wildcard*/
        jboolean javaw,                         /* windows-only javaw */
        jint ergo                               /* ergonomics class policy */
);

void (*android_update_LD_LIBRARY_PATH_p)(const char*);
void* (*dlopen_p)(const char *, int);
void* (*dlsym_p)(void *, const char *);
int (*dlclose_p)(void *);
char* (*dlerror_p)(void);
void (*__linker_init_mini_p)(int argc, char** argv);
void* dlsym_darwin_p;
void* dlerror_darwin_p;

// PojavLancher: fixme: are these wrong?
#define FULL_VERSION "1.8.0-internal"
#define DOT_VERSION "1.8"

static const char* const_progname = "java";
static const char* const_launcher = "openjdk";
static const char** const_jargs = NULL;
static const char** const_appclasspath = NULL;
static const jboolean const_javaw = JNI_FALSE;
static const jboolean const_cpwildcard = JNI_TRUE;
static const jint const_ergo_class = 0; // DEFAULT_POLICY

#define BIND_METHOD(HANDLE, NAME) \
    NAME##_p = dlsym(HANDLE, #NAME); \
    if (!NAME##_p) { \
        printf("Warning: %s\n", dlerror());\
    }

int main(int argc, char** argv) {
    setenv("LD_DEBUG", "3", 1);

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
    *((void**) dlsym_darwin_p) = dlsym;
    *((void**) dlerror_darwin_p) = dlerror;

    __linker_init_mini_p(0, NULL);

    printf("%s\n", dlerror_p());
    android_update_LD_LIBRARY_PATH_p(getenv("PWD"));

    void *libjli = dlopen_p(realpath("jre8-arm64-20210410-release/lib/aarch64/jli/libjli.so", NULL), RTLD_LAZY | RTLD_GLOBAL);
    printf("libjli.so = %p: %s\n", libjli, dlerror_p());
    JLI_Launch_func *pJLI_Launch = (JLI_Launch_func *)dlsym_p(libjli, "JLI_Launch");
    printf("JLI_Launch = %p: %s\n", pJLI_Launch, dlerror_p());

    printf("Calling JLI_Launch\n");
    return pJLI_Launch(argc, argv,
                   0, NULL, // sizeof(const_jargs) / sizeof(char *), const_jargs,
                   0, NULL, // sizeof(const_appclasspath) / sizeof(char *), const_appclasspath,
                   FULL_VERSION,
                   DOT_VERSION,
                   *argv, // (const_progname != NULL) ? const_progname : *margv,
                   *argv, // (const_launcher != NULL) ? const_launcher : *margv,
                   (const_jargs != NULL) ? JNI_TRUE : JNI_FALSE,
                   const_cpwildcard, const_javaw, const_ergo_class);

/*
    // sprintf(out_name, "%s/%s", getenv("PWD"), realpath("
    void *test_handle = dlopen_p(realpath("libart.so", 0), RTLD_LAZY);
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
*/
}
