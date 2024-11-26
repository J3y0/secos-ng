# Q1, Q2, Q3, Q4

See code

# Q5

Inside `tp2/`, we can run the following command:

```
objdump -D kernel.elf --disassemble=bp_handler
```

```
00303f65 <bp_handler>:
303f65:	55                   	push   %ebp
303f66:	89 e5                	mov    %esp,%ebp
303f68:	83 ec 08             	sub    $0x8,%esp
303f6b:	83 ec 0c             	sub    $0xc,%esp
303f6e:	68 50 49 30 00       	push   $0x304950
303f73:	e8 58 f1 ff ff       	call   3030d0 <printf>
303f78:	83 c4 10             	add    $0x10,%esp
303f7b:	90                   	nop
303f7c:	c9                   	leave
303f7d:	c3                   	ret
```

As we can see, the last instruction of this function is `ret`. The problem with this naive implementation is that the program does not retrieve `EIP` after popping the stack but `EFLAGS` or `SS` if a privilege change occured. Indeed, `pusha` was executed before `bp_handler`, hence this stack state.

This is why we need to call `iret` instead. This instruction would correctly understand what is on the stack after an interrupt call.

# Q7

```
ebp-0x4: 0x303f9e
```

```
$ objdump -D kernel.elf --disassemble=bp_trigger

00303f97 <bp_trigger>:
303f97:	55                   	push   %ebp
303f98:	89 e5                	mov    %esp,%ebp
303f9a:	83 ec 08             	sub    $0x8,%esp
303f9d:	cc                   	int3
303f9e:	83 ec 0c             	sub    $0xc,%esp
303fa1:	68 94 49 30 00       	push   $0x304994
303fa6:	e8 25 f1 ff ff       	call   3030d0 <printf>
303fab:	83 c4 10             	add    $0x10,%esp
303fae:	90                   	nop
303faf:	c9                   	leave
303fb0:	c3                   	ret
```

We can notice the address stored at `ebp - 0x4` is the address of the instruction following `int3` call. This value represents the `eip` value stored during context switch that happened due to `int3` (during the handling of the interruption).

# Q8

General registers are not stored during this process. Yet, there are very important when restoring the context back to the normal execution.

We can use `pusha`/`popa` instructions to store them on the stack.

# Q9

The handler should end with the instruction `iret` to restore properly the context of the execution before the interruption handling.

# Q11

C does not suit well for developping interrupt handling. Indeed, defining a simple handler would naturally use instructions for basic functions such as `ret` which is not desired in this case.

In ASM, one could directly use `iret`.