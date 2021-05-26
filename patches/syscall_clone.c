#include <sched.h>
#include <stdio.h>

int clone(int (*fn)(void *), void *stack, int flags, void *arg, ...) {
    printf("clone() unimplemented\n");
    return -1;
}
