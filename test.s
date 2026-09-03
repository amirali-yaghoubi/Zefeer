    .global main
    .text
main:
    str x30, [sp, #-16]!
    sub sp, sp, #32
    mov w0, #8
    str w0, [sp, #0]
    ldr w0, [sp, #0]
    mov w1, #4
    cmp w0, w1
    mov w0, #0
    mov w1, #1
    csel w0, w1, w0, ge
    str w0, [sp, #4]
    ldr w0, [sp, #4]
    cmp w0, #0
    bne L0
    b L1
L0:
L3:
    ldr w0, [sp, #0]
    mov w1, #3
    cmp w0, w1
    mov w0, #0
    mov w1, #1
    csel w0, w1, w0, gt
    str w0, [sp, #8]
    ldr w0, [sp, #8]
    mov w1, #0
    cmp w0, w1
    mov w0, #0
    mov w1, #1
    csel w0, w1, w0, eq
    str w0, [sp, #12]
    ldr w0, [sp, #12]
    cmp w0, #0
    bne L4
    ldr w1, [sp, #0]
    ldr x0, =fmt
    bl printf
    ldr w0, [sp, #0]
    mov w1, #1
    sub w0, w0, w1
    str w0, [sp, #16]
    ldr w0, [sp, #16]
    str w0, [sp, #0]
    b L3
L4:
L1:
    add sp, sp, #32
    ldr x30, [sp], #16
    ret
.data
fmt: .string "%d\n"
