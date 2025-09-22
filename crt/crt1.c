#include <features.h>
#include "libc.h"

#define START "_start"

#include "crt_arch.h"

int main();
weak void _init();
weak void _fini();
int __libc_start_main(int (*)(), int, char **,
	void (*)(), void(*)(), void(*)());

hidden void _start_c(long *_p)
{
    (void)_p;
    long p[] = {0, 0, 0, 0, 0};
	int argc = p[0];
	char **argv = (void *)(p+1);
	__libc_start_main(main, argc, argv, _init, _fini, 0);
}
