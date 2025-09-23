#define __SYSCALL_LL_E(x) \
((union { long long ll; long l[2]; }){ .ll = x }).l[0], \
((union { long long ll; long l[2]; }){ .ll = x }).l[1]
#define __SYSCALL_LL_O(x) __SYSCALL_LL_E((x))

#if SYSCALL_NO_TLS
#define SYSCALL_INSNS "int $128"
#else
#define SYSCALL_INSNS "call *%%gs:16"
#endif

#define SYSCALL_INSNS_12 "xchg %%ebx,%%edx ; " SYSCALL_INSNS " ; xchg %%ebx,%%edx"
#define SYSCALL_INSNS_34 "xchg %%ebx,%%edi ; " SYSCALL_INSNS " ; xchg %%ebx,%%edi"

extern long translate_call(long n, long a1, long a2, long a3, long a4, long a5, long a6);

static inline long __syscall0(long n)
{
    return translate_call(n, 0, 0, 0, 0, 0, 0);
}

static inline long __syscall1(long n, long a1)
{
    return translate_call(n, a1, 0, 0, 0, 0, 0);
}

static inline long __syscall2(long n, long a1, long a2)
{
    return translate_call(n, a1, a2, 0, 0, 0, 0);
}

static inline long __syscall3(long n, long a1, long a2, long a3)
{
    return translate_call(n, a1, a2, a3, 0, 0, 0);
}

static inline long __syscall4(long n, long a1, long a2, long a3, long a4)
{
    return translate_call(n, a1, a2, a3, a4, 0, 0);
}

static inline long __syscall5(long n, long a1, long a2, long a3, long a4, long a5)
{
    return translate_call(n, a1, a2, a3, a4, a5, 0);
}

static inline long __syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6)
{
    return translate_call(n, a1, a2, a3, a4, a5, a6);
}

#define VDSO_USEFUL
#define VDSO_CGT32_SYM "__vdso_clock_gettime"
#define VDSO_CGT32_VER "HONEYOS_2_prealpha"
#define VDSO_CGT_SYM "__vdso_clock_gettime64"
#define VDSO_CGT_VER "HONEYOS_2_prealpha"
