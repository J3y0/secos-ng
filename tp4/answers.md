# Q1, Q2

See code

# Q3

The CPU triple faults continuously... We cannot activate pagination yet. Indeed, the PGD is empty.

# Q4

See code

# Q5

```
$ readelf -e kernel.elf

[...]

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000094 0x00300000 0x00300000 0x0000c 0x0000c RWE 0x4
  LOAD           0x000000 0x00300010 0x00300010 0x00000 0x02000 RW  0x10
  LOAD           0x0000b0 0x00302010 0x00302010 0x02920 0x03550 RWE 0x20

[...]
```

According to `readelf` output, we have to map a total of `0x302010 + 0x3550 = 0x305560` addresses.

We can compute the number of entries in PGD necessary:

`0x305560 >> (10 + 12) = 0`. Only one entry in PGD is necessary.

By only shifting of `12`, we deduce the number of entries in the PTB which is about `773` in that case.

To identity map, we can map pages starting from address `0x0` and keep allocating pages until we reach
our final index. PTB[i] would receive base address equal to `((d << 10 | i) << 12)` where `d is the index of PGD` and `i is the index of the entry in the PTB`.

# Q6

Page Fault because we access a unmapped address (`0x601000` virtual address of the PTB).

We need to map more addresses, containing virtual address `0x601000`. We will add an entry to the PGD.
As a result, all address until `0x800000` would be mapped.

# Q7

Need to map one page to `0xc0000000` virtual address that accesses physical address `0x600000-0x601000`.

# Q8

For `0x700000`, `PGD_idx = 0x1 and PTB_idx = 0x300`.
For `0x7ff000`, `PGD_idx = 0x1 and PTB_idx = 0x3ff`.

# Q9

The CPU triple faults once again continuously...

This is because text section is stored in a page accessible through first entry of PGD.
Hence, when the code is trying to access the next instruction after we zeroed out `PGD[0]`,
it faults and doesn't know how to recover.