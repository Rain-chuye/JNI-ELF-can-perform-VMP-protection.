.section .text
.global _start
_start:
    // Args: x0 = base, x1 = offset, x2 = size, x3 = key
    // We need to call mprotect(addr, len, PROT_READ|PROT_WRITE|PROT_EXEC)
    // mprotect(x0, x1, x2) -> syscall 226

    // Save registers
    stp x29, x30, [sp, #-16]!
    mov x29, sp

    mov x8, #226        // syscall mprotect
    mov x4, x0          // base
    add x0, x4, x1      // addr = base + offset
    // Align addr to page size (4096)
    and x0, x0, #~0xFFF
    mov x1, x2          // len
    mov x2, #7          // PROT_READ|PROT_WRITE|PROT_EXEC
    svc #0

    // Decryption loop
    add x0, x4, x1      // Current pointer (reusing x1 as offset for a moment, wait)
    // Let's use x5 as pointer, x6 as end
    add x5, x4, x1      // Start
    add x6, x5, x2      // End (oops, I overwrote x2)
    // Let's restart args carefully
