.text
.globl syscall_impl
.type syscall_impl, @function

/* uint32_t syscall_impl(uint32_t function,
 *                       uint32_t parameter0,
 *                       uint32_t parameter1,
 *                       uint32_t parameter2,
 *                       uint32_t parameter3);
 *
 * Calling convention: cdecl (args on stack):
 *   [esp]   : return addr (to caller)
 *   [esp+4] : function
 *   [esp+8] : parameter0
 *   [esp+12]: parameter1
 *   [esp+16]: parameter2
 *   [esp+20]: parameter3
 */

syscall_impl:
    pusha

    subl    $128, %esp
    movups  %xmm0, 0(%esp)
    movups  %xmm1, 16(%esp)
    movups  %xmm2, 32(%esp)
    movups  %xmm3, 48(%esp)
    movups  %xmm4, 64(%esp)
    movups  %xmm5, 80(%esp)
    movups  %xmm6, 96(%esp)
    movups  %xmm7, 112(%esp)


    movl    164(%esp), %eax    /* function -> eax */
    movl    168(%esp), %ebx    /* parameter0 -> ebx */
    movl    172(%esp), %ecx    /* parameter1 -> ecx */
    movl    176(%esp), %edx    /* parameter2 -> edx */
    movl    180(%esp), %esi    /* parameter3 -> esi */

    push    $end
    mov     %esp, %edi

    sysenter

end:
    /* Restore SSE registers */
    movups  0(%esp), %xmm0
    movups  16(%esp), %xmm1
    movups  32(%esp), %xmm2
    movups  48(%esp), %xmm3
    movups  64(%esp), %xmm4
    movups  80(%esp), %xmm5
    movups  96(%esp), %xmm6
    movups  112(%esp), %xmm7
    
    /* Deallocate the 128 bytes */
    addl    $128, %esp

    movl    %eax, 28(%esp)
    popa
    ret

    .size syscall_impl, .-syscall_impl
