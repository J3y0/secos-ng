On Arch Linux, I need to add the flag `-accel kvm`

# Q1

`SGDT` est une instruction qui va stocker l'adresse contenue dans le registre `GDTR` dans l'operande de destination.

Dans `kernel/include/segmem.h`, la macro `get_gdtr` est definie pour retrouver l'adresse de base de la `GDT`.

# Q2

# Q3

RPL is 0, we want to explore the GDT (not the LDT) meaning we have the third bit at 0. Finally, the offset is 1 `CS = 0x8 = 0b1 0 00`. `CS` selector points to 

```
1 [0x0 - 0xffffffff] seg_t: 0xb desc_t: 1 priv: 0 present: 1 avl: 0 longmode: 0 default: 1 gran: 1
```

All other selectors points to: (`XS = 0x10 = 0b10 0 00` -> index is `0b10 = 2`)

```
2 [0x0 - 0xffffffff] seg_t: 0x3 desc_t: 1 priv: 0 present: 1 avl: 0 longmode: 0 default: 1 gran: 1
```

# Q4

All segments selectors except `CS` points to the same segment descriptor. As a result, wew know GRUB use `flat mode` as default segmentation mode.

# Q5, Q6, Q7

See code

# Q8

While loading a code segment in DS, the OS crash and try loading again infinitely. The loop is due to the fact interruptions are not implemented yet: the CPU does not know how to handle the interruption.

While loading a data segment in CS, a General Protection fault is raised.

# Q9

See code

# Q10

There are no errors because we use `ES` segment explicitly, treating dst address as an offset. `_memcpy8` function will copy src array to `ES:dst = ES:0` address.

We can verify all 32 bytes set to `0xff` have been copied correctly with the following code:

```c
for (int i = 0; i < 0x21; i++) {
    debug("%hhx\n", (char)*(unsigned char *)(0x600000 + dst + i));
}
```

# Q11

The copy of 64 bits would exceed the size allocated by the segment descriptor. This is why, a GP fault is raised by doing so.

# Q12

See code

# Q13

When loading new `DS`, `ES`, `FS`, `GS`, no error occurs.

When loading `SS`, a General Protection Fault is raised.

When performing a far jump to update `CS`, a GP Fault is raised.
A mean to switch to ring 3 from ring 0 is to take advantage of `iret` instruction during which CPU changes context.