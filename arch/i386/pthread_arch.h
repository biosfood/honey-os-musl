static inline uintptr_t __get_tp()
{
    return 0;
	uintptr_t tp;
	__asm__ ("movl %%gs:0,%0" : "=r" (tp) );
	return tp;
}

#define MC_PC gregs[REG_EIP]
