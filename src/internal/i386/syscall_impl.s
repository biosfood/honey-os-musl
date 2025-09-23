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
    /* save regs */
    push    %ebp
    push    %edi
    push    %esi
    push    %ebx

    movl    20(%esp), %eax    /* function -> eax */
    movl    24(%esp), %ebx    /* parameter0 -> ebx */
    movl    28(%esp), %ecx    /* parameter1 -> ecx */
    movl    32(%esp), %edx    /* parameter2 -> edx */
    movl    36(%esp), %esi    /* parameter3 -> esi */

    push    $end
    mov     %esp, %edi

    sysenter

end:
    /* restore saved regs */
    pop     %ebx
    pop     %esi
    pop     %edi
    pop     %ebp
    ret

    .size syscall_impl, .-syscall_impl
