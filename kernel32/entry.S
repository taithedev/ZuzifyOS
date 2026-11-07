.section .multiboot2,"a"
.balign 8
multiboot2_header:
    .long 0xe85250d6
    .long 0
    .long multiboot2_header_end - multiboot2_header
    .long -(0xe85250d6 + 0 + (multiboot2_header_end - multiboot2_header))
    .short 0
    .short 0
    .long 8
multiboot2_header_end:

.section .bss
.balign 16
stack:
    .skip 16384
stack_end:

.section .text
.global _start
.extern kmain32
_start:
    cli
    movl $stack_end,%esp
    pushl %ebx
    pushl %eax
    call kmain32
1:  hlt
    jmp 1b
