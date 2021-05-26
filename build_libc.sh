set -e

unset SDKROOT

export CFLAGS="\
  -I. \
  -Iarch-arm64/include \
  -Iinclude \
  -Ikernel/uapi \
  -Ikernel/uapi/asm-arm64 \
  -Istdio \
  -I../libm/include \
  -DANDROID_SMP \
  -DSKIP_APPLE_DIFF \
  -D_LIBC=1 \
  -Wall -Wextra -Wunused \
"

cd libc

clang -static $CFLAGS -o ../libc_conly.a \
\
  -std=c99 \
\
    ../patches/syscall_clone.c \
    bionic/bindresvport.c \
    bionic/daemon.c \
    bionic/err.c \
    bionic/ether_aton.c \
    bionic/ether_ntoa.c \
    bionic/fts.c \
    bionic/gethostname.c \
    bionic/getpriority.c \
    bionic/if_indextoname.c \
    bionic/if_nametoindex.c \
    bionic/initgroups.c \
    bionic/ioctl.c \
    bionic/isatty.c \
    bionic/memmem.c \
    bionic/pathconf.c \
    bionic/pututline.c \
    bionic/sched_cpualloc.c \
    bionic/sched_cpucount.c \
    bionic/semaphore.c \
    bionic/sigblock.c \
    bionic/siginterrupt.c \
    bionic/sigsetmask.c \
    bionic/system_properties_compat.c \
    stdio/snprintf.c\
    stdio/sprintf.c \
    bionic/getentropy_linux.c \
    bionic/sys_siglist.c \
    bionic/sys_signame.c \
    arch-arm64/bionic/__rt_sigreturn.S \
\
> build_libc.txt 2>&1

#arch-arm64/syscalls/*.S \


    # bionic/dup2.cpp \
    # bionic/pipe.cpp

clang++ -static $CFLAGS -o ../libc_cpp.a \
\
  -std=gnu++11 \
\
    bionic/dl_iterate_phdr_static.cpp \
    bionic/libc_init_static.cpp \
    bionic/abort.cpp \
    bionic/accept.cpp \
    bionic/accept4.cpp \
    bionic/access.cpp \
    bionic/assert.cpp \
    bionic/atof.cpp \
    bionic/bionic_time_conversions.cpp \
    bionic/brk.cpp \
    bionic/c16rtomb.cpp \
    bionic/c32rtomb.cpp \
    bionic/chmod.cpp \
    bionic/chown.cpp \
    bionic/clearenv.cpp \
    bionic/clock.cpp \
    bionic/clone.cpp \
    bionic/__cmsg_nxthdr.cpp \
    bionic/connect.cpp \
    bionic/ctype.cpp \
    bionic/dirent.cpp \
    bionic/epoll_create.cpp \
    bionic/epoll_pwait.cpp \
    bionic/epoll_wait.cpp \
    bionic/__errno.cpp \
    bionic/eventfd_read.cpp \
    bionic/eventfd_write.cpp \
    bionic/ffs.cpp \
    bionic/flockfile.cpp \
    bionic/fork.cpp \
    bionic/fpclassify.cpp \
    bionic/futimens.cpp \
    bionic/getauxval.cpp \
    bionic/getcwd.cpp \
    bionic/getpgrp.cpp \
    bionic/getpid.cpp \
    bionic/gettid.cpp \
    bionic/inotify_init.cpp \
    bionic/lchown.cpp \
    bionic/lfs64_support.cpp \
    bionic/__libc_current_sigrtmax.cpp \
    bionic/__libc_current_sigrtmin.cpp \
    bionic/libc_init_common.cpp \
    bionic/libc_logging.cpp \
    bionic/libgen.cpp \
    bionic/link.cpp \
    bionic/locale.cpp \
    bionic/lstat.cpp \
    bionic/mbrtoc16.cpp \
    bionic/mbrtoc32.cpp \
    bionic/mbstate.cpp \
    bionic/mkdir.cpp \
    bionic/mkfifo.cpp \
    bionic/mknod.cpp \
    bionic/mntent.cpp \
    bionic/NetdClientDispatch.cpp \
    bionic/open.cpp \
    bionic/pause.cpp \
    bionic/poll.cpp \
    bionic/posix_fadvise.cpp \
    bionic/posix_fallocate.cpp \
    bionic/posix_timers.cpp \
    bionic/pthread_atfork.cpp \
    bionic/pthread_attr.cpp \
    bionic/pthread_cond.cpp \
    bionic/pthread_create.cpp \
    bionic/pthread_detach.cpp \
    bionic/pthread_equal.cpp \
    bionic/pthread_exit.cpp \
    bionic/pthread_getcpuclockid.cpp \
    bionic/pthread_getschedparam.cpp \
    bionic/pthread_gettid_np.cpp \
    bionic/pthread_internals.cpp \
    bionic/pthread_join.cpp \
    bionic/pthread_key.cpp \
    bionic/pthread_kill.cpp \
    bionic/pthread_mutex.cpp \
    bionic/pthread_once.cpp \
    bionic/pthread_rwlock.cpp \
    bionic/pthread_self.cpp \
    bionic/pthread_setname_np.cpp \
    bionic/pthread_setschedparam.cpp \
    bionic/pthread_sigmask.cpp \
    bionic/ptrace.cpp \
    bionic/pty.cpp \
    bionic/raise.cpp \
    bionic/rand.cpp \
    bionic/readlink.cpp \
    bionic/reboot.cpp \
    bionic/recv.cpp \
    bionic/rename.cpp \
    bionic/rmdir.cpp \
    bionic/scandir.cpp \
    bionic/sched_getaffinity.cpp \
    bionic/sched_getcpu.cpp \
    bionic/send.cpp \
    bionic/setegid.cpp \
    bionic/__set_errno.cpp \
    bionic/seteuid.cpp \
    bionic/setpgrp.cpp \
    bionic/sigaction.cpp \
    bionic/sigaddset.cpp \
    bionic/sigdelset.cpp \
    bionic/sigemptyset.cpp \
    bionic/sigfillset.cpp \
    bionic/sigismember.cpp \
    bionic/signal.cpp \
    bionic/signalfd.cpp \
    bionic/sigpending.cpp \
    bionic/sigprocmask.cpp \
    bionic/sigsuspend.cpp \
    bionic/sigwait.cpp \
    bionic/socket.cpp \
    bionic/stat.cpp \
    bionic/statvfs.cpp \
    bionic/strcoll_l.cpp \
    bionic/strerror.cpp \
    bionic/strerror_r.cpp \
    bionic/strftime_l.cpp \
    bionic/strsignal.cpp \
    bionic/strtold.cpp \
    bionic/strtold_l.cpp \
    bionic/strtoll_l.cpp \
    bionic/strtoull_l.cpp \
    bionic/strxfrm_l.cpp \
    bionic/stubs.cpp \
    bionic/symlink.cpp \
    bionic/sysconf.cpp \
    bionic/syslog.cpp \
    bionic/system_properties.cpp \
    bionic/tdestroy.cpp \
    bionic/termios.cpp \
    bionic/thread_private.cpp \
    bionic/tmpfile.cpp \
    bionic/umount.cpp \
    bionic/unlink.cpp \
    bionic/utimes.cpp \
    bionic/vdso.cpp \
    bionic/wait.cpp \
    bionic/wchar.cpp \
    bionic/wctype.cpp \
\
>> build_libc.txt 2>&1

echo BUILD SUCCESS