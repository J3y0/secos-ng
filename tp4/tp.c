/* GPLv2 (c) Airbus */
#include <debug.h>
#include <cr.h>
#include <pagemem.h>

void tp() {
	// Q1
	uint32_t cr3 = get_cr3();
	debug("CR3: 0x%x\n", cr3);

	//Q2
	pde32_t *pgd = (pde32_t *)0x600000;
	set_cr3((uint32_t)pgd);

	// Q3
	// uint32_t cr0 = get_cr0();
	// set_cr0(cr0 | CR0_PG);

	// Q4
	pte32_t *ptb1 = (pte32_t *)0x601000;

	// Q5
	for (int i = 0; i < 1024; i++) {
		// Do not right shift of 12 bits because this is already given
		// by the offset in virtual address
		pg_set_entry(&ptb1[i], PG_KRN | PG_RW, (0 << 10 | i));
	}
	// Right shift by 12bits because each PTB occupies 2**12 bytes (1024 entries * 4 bytes)
	// Then, base_PTB = PGD.addr & 0xfffff000
	pg_set_entry(&pgd[0], PG_KRN | PG_RW, page_get_nr(ptb1));

	// Q6
	// uint32_t cr0 = get_cr0();
	// set_cr0(cr0 | CR0_PG);

	pte32_t *ptb2 = (pte32_t *)0x602000;
	for (int i = 0; i < 1024; i++) {
		// Do not right shift of 12 bits because this is already given
		// by the offset in virtual address
		pg_set_entry(&ptb2[i], PG_KRN | PG_RW, (1 << 10 | i));
	}
	// Right shift by 12bits because offset would be given by virtual address
	pg_set_entry(&pgd[1], PG_KRN | PG_RW, page_get_nr(ptb2));

	// Activate pagination
	uint32_t cr0 = get_cr0();
	set_cr0(cr0 | CR0_PG);

	debug("PTB2[2]: 0x%x\n", ptb2[2].raw);

	// Q7
	uint32_t *target  = (uint32_t*)0xc0000000;
	int pgd_idx = pd32_get_idx(target);
	int ptb_idx = pt32_get_idx(target);

	debug("pgd_idx = 0x%x, ptb_idx = 0x%x\n", pgd_idx, ptb_idx);
	// Create new PTB
	pte32_t *ptb3 = (pte32_t *)0x603000;
	pg_set_entry(&ptb3[ptb_idx], PG_KRN | PG_RW, (pgd_idx << 10 | ptb_idx));
	// Add PTB to PGB
	pg_set_entry(&pgd[pgd_idx], PG_KRN | PG_RW, page_get_nr(ptb3));

	set_cr0(cr0 | CR0_PG);
	debug("PGD[0]: 0x%x\n", pgd[0].raw);

	// Q8
	char *str_target1 = (char *)0x700000;
	char *str_target2 = (char *)0x7ff000;
	int ptb_idx_str1 = pt32_get_idx(str_target1);
	int ptb_idx_str2 = pt32_get_idx(str_target2);

	// Located in PGD[1], hence ptb2[...]
	pg_set_entry(&ptb2[ptb_idx_str1], PG_KRN | PG_RW, 0x2000 >> 12);
	pg_set_entry(&ptb2[ptb_idx_str2], PG_KRN | PG_RW, 0x2000 >> 12);

	set_cr0(cr0 | CR0_PG);
	debug("%p: %s\n", str_target1, str_target1);
	debug("%p: %s\n", str_target2, str_target2);

	// Q9
	pg_set_zero(&pgd[0]);
	invalidate(&pgd[0]);
}
