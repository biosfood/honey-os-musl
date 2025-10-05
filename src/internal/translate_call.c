//
// Created by lukas on 9/22/25.
//
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <pthread.h>

typedef enum {
        HONEY_SYS_RUN = 0,
        HONEY_SYS_FORK = 23,
        HONEY_SYS_PTHREAD_CREATE = 25,
        HONEY_SYS_OPEN = 26,
        HONEY_SYS_READ = 27,
        HONEY_SYS_WRITE = 28,
        HONEY_SYS_FILE_CREATE = 29,
        HONEY_SYS_MMAP = 30,
        HONEY_SYS_MUNMAP = 31,
        HONEY_SYS_CLOSE = 32,
        HONEY_SYS_STAT = 33,
        HONEY_SYS_EXEC = 34,
} SyscallIds;

// same as in kernel
enum FileType {
        FILE_TYPE_DIRECTORY = 0,
        FILE_TYPE_SYMLINK = 1,
        FILE_TYPE_FILE = 2,
        FILE_TYPE_FIFO = 3,
        FILE_TYPE_SOCKET = 4,
        FILE_TYPE_LINK = 5,
};

#define PTR(x) ((void *)(uintptr_t)(x))
#define U32(x) ((uint32_t)(uintptr_t)(x))

int _open_(const char *path, const int oflag, ...);
extern uint32_t syscall_impl(uint32_t function, uint32_t parameter0,
                             uint32_t parameter1, uint32_t parameter2,
                             uint32_t parameter3);

int _close_(int fildes) {
        return (int)syscall_impl(HONEY_SYS_CLOSE, fildes, 0, 0, 0);
}

uint32_t get_last_slash_position(char *string) {
        uint32_t result = strlen(string);
        if (!result) {
                return 0;
        }
        result--;
        while (result && string[result] != '/') {
                result--;
        }
        return result;
}

int _create_(const char *_path, int type) {
        char path[255];
        strcpy(path, _path);
        uint32_t last_slash_position = get_last_slash_position(path);
        if (last_slash_position == 0) {
                int fd = _open_("/", 0);
                if (fd < 0) {
                        return -1;
                }
                int result = (int)syscall_impl(HONEY_SYS_FILE_CREATE, fd,
                                               U32(path + 1), type, 0);
                close(fd);
                return result;
        }
        path[last_slash_position] = 0;
        int fd = _open_(path, 0);
        path[last_slash_position] = '/';
        if (fd < 0) {
                return -1;
        }
        int result =
            (int)syscall_impl(HONEY_SYS_FILE_CREATE, fd,
                              U32(path + last_slash_position + 1), type, 0);
        close(fd);
        return result;
}

int _mkfifo_(const char *path, mode_t mode) {
        return _create_(path, FILE_TYPE_FIFO);
}

int _mkdir_(const char *path, mode_t mode) {
        return _create_(path, FILE_TYPE_DIRECTORY);
}

int _fstat_(int fildes, struct stat *buf) {
        return (int)syscall_impl(HONEY_SYS_STAT, fildes, U32(buf), 0, 0);
}

int _stat_(const char *restrict path, struct stat *restrict buf) {
        int open_result = _open_(path, 0);
        if (open_result < 0) {
                return open_result;
        }
        return _fstat_(open_result, buf);
}

int _pthread_create_(pthread_t *restrict thread,
                     const pthread_attr_t *restrict attr,
                     void *(*start_routine)(void *), void *restrict arg) {
        return (int)syscall_impl(HONEY_SYS_PTHREAD_CREATE, U32(thread),
                                 U32(attr), U32(start_routine), U32(arg));
}

ssize_t _read_(const int filedes, void *buffer, size_t nbyte) {
        return (int)syscall_impl(HONEY_SYS_READ, filedes, U32(buffer), nbyte,
                                 0);
}

ssize_t _pread_(const int filedes, void *buffer, size_t nbyte, off_t offset) {
        return (int)syscall_impl(HONEY_SYS_READ, filedes, U32(buffer), nbyte,
                                 offset);
}

ssize_t _write_(const int filedes, void *buffer, size_t nbyte) {
        return (int)syscall_impl(HONEY_SYS_WRITE, filedes, U32(buffer), nbyte,
                                 0);
}

ssize_t _pwrite_(const int filedes, void *buffer, size_t nbyte, off_t offset) {
        return (int)syscall_impl(HONEY_SYS_WRITE, filedes, U32(buffer), nbyte,
                                 offset);
}

pid_t _fork_() {
        return (int)syscall_impl(HONEY_SYS_FORK, 0, 0, 0, 0);
}

int _open_(const char *path, const int oflag, ...) {
        int retval = (int)syscall_impl(HONEY_SYS_OPEN, U32(path), oflag, 0, 0);
        if (retval > 0 || !(oflag & O_CREAT)) {
                return retval;
        }
        retval = _create_(path, FILE_TYPE_FILE);
        if (retval < 0) {
                return retval;
        }
        return (int)syscall_impl(HONEY_SYS_OPEN, U32(path), oflag, 0, 0);
}

int _execv_(const char *path, char *const argv[]) {
        int fd = open(path, 0);
        return (int)syscall_impl(HONEY_SYS_EXEC, fd, 0, 0, 0);
}

int _writev_(int fildes, const struct iovec *iov, int iovcnt) {
        int total = 0;
        for (uint32_t i = 0; i < iovcnt; i++) {
                total += _write_(fildes, iov[i].iov_base, iov[i].iov_len);
        }
        return total;
}

int pthread_create(pthread_t *restrict thread, const pthread_attr_t *restrict attr,
                          void *(*start_routine)(void *), void *restrict arg) {
        return (int)syscall_impl(HONEY_SYS_PTHREAD_CREATE, U32(thread), U32(attr), U32(start_routine), U32(arg));
}


int _execve_(const char *path, char *const argv[], char *const env[]) {
        (void)argv;
        (void)env;
        int fd = open(path, 0);
        return (int)syscall_impl(HONEY_SYS_EXEC, fd, 0, 0, 0);
}

typedef struct {
        void *addr;
        size_t len;
        int prot;
        int flags;
        int filedes;
        off_t off;
} __attribute__((packed)) MmapArgs;

void *_mmap_(void *addr, const size_t len, const int prot, const int flags,
           const int fildes, const off_t off) {
        if (!len) {
                return NULL;
        }
        MmapArgs args = {addr, len, prot, flags, fildes, off};
        return PTR(syscall_impl(HONEY_SYS_MMAP, U32(&args), 0, 0, 0));
}

int _munmap_(void *addr, size_t len) {
        return (int)syscall_impl(HONEY_SYS_MUNMAP, U32(addr), len, 0, 0);
}

uintptr_t tp;

long translate_call(long n, long a1, long a2, long a3, long a4, long a5,
                    long a6) {
        switch (n) {
        case SYS_open:
        case SYS_openat:
                return _open_(PTR(a1), a2);
        case SYS_write:
                return _write_(a1, PTR(a2), a3);
        case SYS_writev:
                return _writev_(a1, PTR(a2), a3);
        case SYS_read:
                return _read_(a1, PTR(a2), a3);
        case SYS_execve:
                return _execve_(PTR(a1), PTR(a2), PTR(a3));
        case SYS_mmap:
                return U32(_mmap_(PTR(a1), a2, a3, a4, a5, a6));
        case SYS_mmap2:
                return U32(_mmap_(PTR(a1), a2, a3, a4, a5, a6 << 12));
        case SYS_munmap:
                return _munmap_(PTR(a1), a2);
        case SYS_fork:
                return _fork_();
        case SYS_mkdir:
                return _mkdir_(PTR(a1), a2);
        case SYS_mknod:
                if (a2&S_IFIFO) {
                        return _create_(PTR(a1), FILE_TYPE_FIFO);
                }
                break;
        case SYS_close:
                return _close_(a1);
        case SYS_set_thread_area:
                tp = a1;
                return 0;
        case SYS_get_thread_area:
                return tp;
        case 1:
                return (long)syscall_impl(0, 0, 0, 0, 0);
        default:;
        }
        return 0;
}
