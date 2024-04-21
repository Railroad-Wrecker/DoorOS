// -----------------------------------boot.S -------------------------------------

/* • The Arm Cortex-A72 on Raspberry Pi 4 has four processing cores. At the present, we only 
want our code to run on the master core, so we check the processor ID and either run our 
code (master) or hang in an infinite loop (slave).
• We need to tell our OS how to access the stack. We think of the stack as temporary storage 
space used by currently-executing code, like a scratchpad. We need to set memory aside for it 
and store a pointer to it.
• We also need to initialize the BSS section. This is the area in memory where uninitialized 
variables will be stored. It’s more efficient to initialize everything to zero here, rather than take 
up space in our kernel image doing it explicitly.
• Finally, we can jump to our main() routine in C! */

.section ".text.boot" // Make sure the linker puts this at the start of the kernel image

.global _start // Execution starts here

_start:
    // Check processor ID is zero (executing on main core), else hang
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    // We're not on the main core, so hang in an infinite wait loop
1:  wfe
    b       1b
2: // We're on the main core!

    // Set stack to start below our code
    ldr     x1, =_start
    mov     sp, x1

    // Clean the BSS section
    ldr     x1, =__bss_start    // Start address
    ldr     w2, =__bss_size     // Size of the section
3:  cbz     w2, 4f              // Quit loop if zero
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b             // Loop if non-zero

    // Jump to our main() routine in C (make sure it doesn't return)
4:  bl      main
    // In case it does return, halt the master core too
    b       1b