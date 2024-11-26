/* GPLv2 (c) Airbus */
#include <debug.h>
#include <intr.h>

void bp_handler() {
	// Q8
	asm volatile("pusha");

	// Q7
	uint32_t eip;
	asm volatile ("mov 4(%%ebp), %0":"=r"(eip));
	printf("$ebp-0x4: 0x%x\n", eip);
	debug("[bp_handler] #BP handling\n");

	// Q8
	asm volatile("popa");
	// Q9
	asm volatile("leave; iret");
}

void bp_trigger() {
	asm volatile("int3");
	debug("After bp_trigger() call\n");
}

void tp() {
	// Q1
	idt_reg_t idtr_info;
	get_idtr(idtr_info);
	debug(
		"IDTR address: 0x%08x, limit: 0x%x\n",
		(unsigned int)idtr_info.addr,
		idtr_info.limit
	);

	// Q3
	int_desc_t *bp_desc = &idtr_info.desc[3];
	bp_desc->offset_1 = (uint16_t)((uint32_t)bp_handler);
	bp_desc->offset_2 = (uint16_t)((uint32_t)bp_handler >> 16);

	// Q4
	bp_trigger();
}
