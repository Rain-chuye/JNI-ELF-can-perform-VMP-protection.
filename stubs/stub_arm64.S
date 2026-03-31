.section .text
.global decrypt_stub
decrypt_stub:
    // x0: base, x1: offset, x2: size, x3: key
    stp x29, x30, [sp, #-48]!
    mov x29, sp

    add x9, x0, x1      // x9 = start addr
    and x10, x9, #~4095 // x10 = page_start
    add x11, x9, x2     // x11 = end addr
    add x11, x11, #4095
    and x11, x11, #~4095 // x11 = page_end
    sub x11, x11, x10    // x11 = page_len

    // Save state
    str x9, [sp, #16]
    str x11, [sp, #24]
    str x2, [sp, #32]
    str x3, [sp, #40]
    str x10, [sp, #48] // Wait, out of bounds

    // mprotect(page_start, page_len, 7)
    mov x0, x10
    mov x1, x11
    mov x2, #7
    mov x8, #226
    svc #0

    ldr x9, [sp, #16]
    ldr x2, [sp, #32]
    ldr x3, [sp, #40]

    // Loop
    cbz x2, .Ldone
.Lloop:
    ldrb w12, [x9]
    eor w12, w12, w3
    strb w12, [x9], #1
    subs x2, x2, #1
    b.ne .Lloop

.Ldone:
    // mprotect(page_start, page_len, 5)
    ldr x0, [sp, #48] // Wait, I messed up the stack
    // Let's simplify. I'll write the shellcode manually or use a very simple C.
