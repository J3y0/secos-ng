/* GPLv2 (c) Airbus */
#include <debug.h>
#include <segmem.h>

#define code_0_seg_idx 1
#define data_0_seg_idx 2
#define code_3_seg_idx 3
#define data_3_seg_idx 4
#define tss_seg_idx    5

#define code_0_seg gdt_krn_seg_sel(code_0_seg_idx)
#define data_0_seg gdt_krn_seg_sel(data_0_seg_idx)
#define code_3_seg gdt_usr_seg_sel(code_3_seg_idx)
#define data_3_seg gdt_usr_seg_sel(data_3_seg_idx)
#define tss_seg    gdt_krn_seg_sel(tss_seg_idx)

seg_desc_t GDT[6];
tss_t TSS;

void add_entry_gdt(uint32_t idx, uint32_t type, uint32_t dpl) {
   GDT[idx] = (seg_desc_t){
      .limit_1 = 0xffff,
      .base_1 = 0x0000,
      .base_2 = 0x00,
      .type = type,
      .s = 0x1,
      .dpl = dpl,
      .p = 0x1,
      .limit_2 = 0xf,
      .avl = 0x1,
      .l = 0x0,
      .d = 0x1,
      .g = 0x1,
      .base_3 = 0x00,
   };
}

void add_tss_entry(uint32_t idx) {
   offset_t tss_ptr = (offset_t)&TSS;
   uint32_t tss_size = sizeof(TSS);

   GDT[idx] = (seg_desc_t){
      .limit_1 = (uint16_t)tss_size, // limit is TSS size
      .base_1 = (uint16_t)tss_ptr, // base is TSS addr
      .base_2 = (uint8_t)(tss_ptr >> 0x10),
      .type = SEG_DESC_SYS_TSS_AVL_32,
      .s = 0x0, // 0 as it is for TSS (system descriptor)
      .dpl = 0x0,
      .p = 0x1,
      .limit_2 = (tss_size >> 0x10) && 0xf,
      .avl = 0x1,
      .l = 0x0,
      .d = 0x1,
      .g = 0x0,
      .base_3 = (uint8_t)(tss_ptr >> 0x18),
   };
}

void init_gdt() {
   GDT[0].raw = 0ULL;
   add_entry_gdt(code_0_seg_idx, SEG_DESC_CODE_XR, 0x0);
   add_entry_gdt(data_0_seg_idx, SEG_DESC_DATA_RW, 0x0);
   add_entry_gdt(code_3_seg_idx, SEG_DESC_CODE_XR, 0x3);
   add_entry_gdt(data_3_seg_idx, SEG_DESC_DATA_RW, 0x3);

   /*
      Init TSS
      
      SS defined on ring 0 segment (if r3 -> r0, takes esp and ss from TSS for r0)
   */
   TSS.s0.ss = gdt_krn_seg_sel(2);
   TSS.s0.esp = get_ebp(); // This runs in r0 -> takes the base of the current stack
   add_tss_entry(tss_seg_idx);

   // Set GDTR
   gdt_reg_t gdtr_ptr;
   gdtr_ptr.addr = (offset_t)GDT;
   gdtr_ptr.limit = sizeof(GDT) - 1;
   set_gdtr(gdtr_ptr);

   // Set TR
   set_tr(tss_seg); // segment selector pointing to TSS entry in GDT

   // Set Segment Registers
   set_cs(code_0_seg);
   set_ds(data_0_seg);
   set_es(data_0_seg);
   set_fs(data_0_seg);
   set_gs(data_0_seg);
   set_ss(data_0_seg);
}

void userland() {
   asm volatile ("mov %eax, %cr0");
}

void tp() {
   // Q1
   init_gdt();

   /*
      Q2 - push values to simulate a ring3 -> ring0 transition
      so iret returns to ring3
   */
   // Push SS
   asm volatile("push %0"::"i"(data_3_seg)); // on ring 3 segment
   // Push ESP
   asm volatile("push %esp");
   // Push EFLAGS
   asm volatile("pushfl");
   // Push fictive CS
   seg_sel_t new_cs;
   new_cs.raw = code_3_seg; // on ring 3 segment
   asm volatile("push %0"::"m"(new_cs));
   // Push fictive EIP
   uint32_t eip = (unsigned int)userland; // continue execution after iret at the start of userland
   asm volatile("push %0"::"m"(eip));
   // Q3
   asm volatile("iret");
}
