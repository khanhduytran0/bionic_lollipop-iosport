#include <dlfcn.h>

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// #include <sys/prctl.h>
#include <sys/syscall.h>
#include <fcntl.h>

#include <sys/mman.h>

#include "private/KernelArgumentBlock.h"

// Basic syscalls translation

// see man(2) prctl, specifically the section about PR_GET_NAME
#define MAX_TASK_NAME_LEN (16)

// Unused! Just for ignore compiler error
#define MAP_NONE 0
#define MAP_READ 0
#define MAP_WRITE 0
#define MAP_EXEC 0
#define MAP_NONE_DARWIN 0
#define MAP_READ_DARWIN 0
#define MAP_WRITE_DARWIN 0
#define MAP_EXEC_DARWIN 0
#define PROT_EXECUTABLE 0
#define PROT_SHARED 0
#define PROT_PRIVATE 0
#define PROT_ANON 0
#define PROT_FILE 0
#define PROT_FIXED 0
#define PROT_NORESERVE 0
#define PROT_JIT 0
#define PROT_SHARED_DARWIN 0
#define PROT_PRIVATE_DARWIN 0
#define PROT_ANON_DARWIN 0
#define PROT_FILE_DARWIN 0
#define PROT_FIXED_DARWIN 0
#define PROT_NORESERVE_DARWIN 0
#define PROT_JIT_DARWIN 0

#define PROT_NONE_DARWIN 0x00
#define PROT_READ_DARWIN 0x01
#define PROT_WRITE_DARWIN 0x02
#define PROT_EXEC_DARWIN 0x04
#define MAP_SHARED_DARWIN 0x0001
#define MAP_PRIVATE_DARWIN 0x0002
#define MAP_ANON_DARWIN 0x1000
#define MAP_FILE_DARWIN 0x0000
#define MAP_FIXED_DARWIN 0x0010
#define MAP_NORESERVE_DARWIN 0x0040
#define MAP_JIT 0x0800

#define PORT_CUSTOM(NAME, FLAG, FLAG_NEW) \
    isprot = &NAME == &prot; \
    flag = isprot ? PROT_##FLAG : MAP_##FLAG; \
    if (NAME & flag) { \
        /* printf("mmap: using %s_%s\n", isprot ? "PROT" : "MAP", #FLAG); */ \
        NAME##_converted |= isprot ? PROT_##FLAG_NEW : MAP_##FLAG_NEW; \
    }

#define PORT(NAME, FLAG) \
    PORT_CUSTOM(NAME, FLAG, FLAG##_DARWIN);

#define UNSUPPORTED(NAME, FLAG) \
    if (NAME & FLAG) { \
        /* printf("mmap %s warning: %s is unsupported on Darwin\n", #NAME, #FLAG); */ \
        NAME##_converted |= FLAG; \
    }

#define UNSUPPORTED_ERR(NAME, FLAG) \
    if (NAME & FLAG) { \
        printf("mmap %s error: %s is unsupported on Darwin\n", #NAME, #FLAG); \
        errno = EOPNOTSUPP; \
        if (1) return MAP_FAILED; \
    }

    // PORT_CUSTOM(NAME, FLAG, FLAG);

extern "C" {

void* mmap64(void *addr, size_t length, int prot, int flags, int fd, off64_t offset) {
    static void* (*darwin_mmap_p)(void *, unsigned long, int, int, int, long long);
    if (!darwin_mmap_p) darwin_mmap_p = (void *(*)(void *, unsigned long, int, int, int, long long)) dlsym(RTLD_NEXT, "mmap");
    // printf("addr=%p\n", darwin_mmap_p);
    if (!darwin_mmap_p) {
        printf("%s\n", dlerror());
        return NULL;
    }

    int isprot;
    int flag;
    
    int prot_converted = 0;
    int flags_converted = 0;

    PORT(prot, NONE);
    PORT(prot, READ);
    PORT(prot, WRITE);
    PORT(prot, EXEC);

    PORT(flags, SHARED);
    // PORT_CUSTOM(flags, MAP_SHARED_VALIDATE, MAP_SHARED_DARWIN|MAP_PRIVATE_DARWIN);
    PORT(flags, PRIVATE);
    // UNSUPPORTED(flags, MAP_32BIT);
    PORT(flags, ANON);
    UNSUPPORTED(flags, MAP_DENYWRITE);
    PORT_CUSTOM(flags, EXECUTABLE, JIT); // FIXME: is it correct?
    PORT(flags, FILE);
    PORT(flags, FIXED);
    // UNSUPPORTED(flags, MAP_FIXEDNOREPLACE);
    UNSUPPORTED(flags, MAP_GROWSDOWN);
    UNSUPPORTED(flags, MAP_HUGETLB);
    // UNSUPPORTED(flags, MAP_HUGE_2MB);
    // UNSUPPORTED(flags, MAP_HUGE_1GB);
    UNSUPPORTED(flags, MAP_LOCKED);
    UNSUPPORTED(flags, MAP_NONBLOCK);
    PORT(flags, NORESERVE);
    UNSUPPORTED(flags, MAP_POPULATE);
    UNSUPPORTED(flags, MAP_STACK);
    // UNSUPPORTED(flags, MAP_SYNC);
    UNSUPPORTED(flags, MAP_UNINITIALIZED);

    // prot_converted |= PROT_WRITE;
    // flags_converted |= MAP_JIT;

    printf("mmap(addr=%p, length=%d, prot=%d, flags=%d, fd=%d, offset=%d)\n", addr, length, prot_converted, flags_converted, fd, offset);

    // printf("mmap length=%lfM\n", length / 1048576.0);

    return darwin_mmap_p(addr, (long unsigned int) length, prot_converted, flags_converted, fd, (long long) offset);
}
void* mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    return mmap64(addr, length, prot, flags, fd, (off64_t) offset);
}

ssize_t pread64(int fd, void *buf, size_t count, off_t offset) {
    char *data = (char*) malloc(count);
    if (offset < 0 || data == NULL) {
        if (offset < 0) {
            errno = EINVAL;
        }
        return -1;
    }

    if (read(fd, data, count) == -1) {
        return -1;
    }

    size_t remaining = count - offset;
    if ((size_t) offset > count) {
        remaining = 0;
    }
    size_t n = count;
    if (n > remaining) {
        n = remaining;
    }

    memcpy(buf, data + offset, n);
    free(data);
    return n;
}

ssize_t pread(int fd, void* buf, size_t count, off_t offset) {
  return pread64(fd, buf, count, (off64_t) offset);
}

ssize_t pwrite64(int fd, const void *buf, size_t count, off_t offset) {
    if (offset < 0) {
        errno = EINVAL;
        return -1;
    }

    size_t remaining = count - offset;
    if ((size_t) offset > count) {
        remaining = 0;
    }
    size_t n = count;
    if (n > remaining) {
        n = remaining;
    }

    return write(fd, (const char*) buf + offset, n);
}

ssize_t pwrite(int fd, const void* buf, size_t byte_count, off_t offset) {
  return pwrite64(fd, buf, byte_count, (off64_t) offset);
}

int
pthread_getname_np(pthread_t thread, char *threadname, size_t len);
/*
int main(int argc, char** argv) {
    //FILE* logFile = fopen("/var/mobile/Documents/linker_log.txt", "w");
    //int log_fd = fileno(logFile);
    //dup2(log_fd, 1);
    //dup2(log_fd, 2);
    //close(log_fd);

    void* start = mmap(idk, 1, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (start == MAP_FAILED) {
        printf("FKING MMAP FAILED: %d\n", errno);
    }

    void *handle = dlopen("/usr/android/system/lib64/libtest.so", RTLD_LAZY);
    if (handle == NULL) {
        printf("dlopen failed: %s\n", dlerror());
    }

    printf("libtest handle = %p\n", handle);
}
*/

void _libc_init_tls(KernelArgumentBlock& args) {
    printf("FIXME not yet implemented _libc_init_tls\n");
}

int gettid() {
    return syscall(372);
    // SYS_thread_selfid
}

int prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {
    switch (option) {
        case 16: // PR_GET_NAME
            return pthread_getname_np(pthread_self(), (char*) arg2, MAX_TASK_NAME_LEN);

        default:
            printf("FIXME not yet implemented prtcl(option=%p)\n", option);
            return -1;
    }
}

int tgkill(int tgid, int tid, int sig) {
    /*
     * bionic calls:
     * - tgid: getpid()
     * - tid: gettid(), unused by this impl
     * - sig: signal
     */
    
    return kill(tgid, sig);
}

}
