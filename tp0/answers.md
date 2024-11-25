# Q1

In `entry.s`, there is the following code:

```asm
.section .stack, "aw", @nobits
.align 16
.space 0x2000

.text
.globl entry
.type  entry,"function"
...
```

It basically means it allocates 0x2000 bytes to the section named `stack`.

Also, in `start.c`, we have an array of `uint32_t` defining `mbh`. As there are 3 values, it means we need a size of `0xc`.
However, the stack needs to be aligned on 0x10 bytes. Then, we need to pad `mbh` section to be aligned on `0x10`.

The above explains why the entrypoint is at `0x302010` instead of `0x300000`.

# Q2

The furthest address available is `0xffffffff`. This is because we are in CPU real mode (Max 1MB).

# Q3

The behavior is coherent ! We cannot write in reserved memory, but we can in available memory.

# Q4

It looks like it is possible to read and write outside available physical space.

It may be because options such as segmentation or pagination are not implemented yet.