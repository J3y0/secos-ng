/* GPLv2 (c) Airbus */
#include <debug.h>
#include <segmem.h>
#include <string.h>

void userland() {
   asm volatile ("mov %eax, %cr0");
}

void print_gdt_content(gdt_reg_t gdtr_ptr) {
    seg_desc_t* gdt_ptr;
    gdt_ptr = (seg_desc_t*)(gdtr_ptr.addr);
    int i=0;
    while ((uint32_t)gdt_ptr < ((gdtr_ptr.addr) + gdtr_ptr.limit)) {
        uint32_t start = gdt_ptr->base_3<<24 | gdt_ptr->base_2<<16 | gdt_ptr->base_1;
        uint32_t end;
        if (gdt_ptr->g) {
            end = start + ( (gdt_ptr->limit_2<<16 | gdt_ptr->limit_1) <<12) + 4095;
        } else {
            end = start + (gdt_ptr->limit_2<<16 | gdt_ptr->limit_1);
        }
        debug("%d ", i);
        debug("[0x%x ", start);
        debug("- 0x%x] ", end);
        debug("seg_t: 0x%x ", gdt_ptr->type);
        debug("desc_t: %d ", gdt_ptr->s);
        debug("priv: %d ", gdt_ptr->dpl);
        debug("present: %d ", gdt_ptr->p);
        debug("avl: %d ", gdt_ptr->avl);
        debug("longmode: %d ", gdt_ptr->l);
        debug("default: %d ", gdt_ptr->d);
        debug("gran: %d ", gdt_ptr->g);
        debug("\n");
        gdt_ptr++;
        i++;
    }
}

void display_segment_registers() {
    debug("CS: 0x%x\n", get_seg_sel(cs));
    debug("DS: 0x%x\n", get_ds());
    debug("ES: 0x%x\n", get_es());
    debug("FS: 0x%x\n", get_fs());
    debug("GS: 0x%x\n", get_gs());
    debug("SS: 0x%x\n", get_ss());
}


void tp() {
    // Q1
	gdt_reg_t gdtr_ptr;
    get_gdtr(gdtr_ptr);

    // Q2
    print_gdt_content(gdtr_ptr);

    // Q3
    display_segment_registers();

    // Q5
    seg_desc_t new_gdt[6];

    // First entry is null entry
    new_gdt[0] = (seg_desc_t){
        .limit_1 = 0x0,
        .base_1 = 0x0,
        .base_2 = 0x0,
        .type = 0x0,
        .s = 0x0,
        .dpl = 0x0,
        .p = 0x0,
        .limit_2 = 0x0,
        .avl = 0x0,
        .l = 0x0,
        .d = 0x0,
        .g = 0x0,
        .base_3 = 0x0,
    };

    new_gdt[1] = (seg_desc_t){
        .limit_1 = 0xffff,
        .base_1 = 0x0000,
        .base_2 = 0x00,
        .type = SEG_DESC_CODE_XRA,
        .s = 0x1,
        .dpl = 0x0,
        .p = 0x1,
        .limit_2 = 0xf,
        .avl = 0x1,
        .l = 0x0,
        .d = 0x1,
        .g = 0x1,
        .base_3 = 0x00,
    };

    new_gdt[2] = (seg_desc_t){
        .limit_1 = 0xffff,
        .base_1 = 0x0000,
        .base_2 = 0x00,
        .type = SEG_DESC_DATA_RWA,
        .s = 0x1,
        .dpl = 0x0,
        .p = 0x1,
        .limit_2 = 0xf,
        .avl = 0x1,
        .l = 0x0,
        .d = 0x1,
        .g = 0x1,
        .base_3 = 0x00,
    };

    // Q6
    gdt_reg_t new_gdtr_ptr;
    new_gdtr_ptr.addr = (offset_t)new_gdt;
    new_gdtr_ptr.limit = sizeof(new_gdt) - 1;
    set_gdtr(new_gdtr_ptr);

    debug(
        "\nGDT Addr: 0x%x, GDT Limit: 0x%x\n",
        (unsigned int)new_gdtr_ptr.addr,
        (unsigned int)new_gdtr_ptr.limit
    );

    set_cs(gdt_krn_seg_sel(1));
    set_ds(gdt_krn_seg_sel(2));
    display_segment_registers();

    // Q7
    get_gdtr(new_gdtr_ptr);
    print_gdt_content(new_gdtr_ptr);

    // Q8
    // set_ds(gdt_krn_seg_sel(1)); // set DS with code segment
    // set_cs(gdt_krn_seg_sel(2)); // set CS with data segment

    // Q9
    char  src[64];
    char *dst = 0;

    memset(src, 0xff, 64);

    new_gdt[3] = (seg_desc_t){
        .limit_1 = 0x40,
        .base_1 = 0x0000,
        .base_2 = 0x60,
        .type = SEG_DESC_DATA_RWA,
        .s = 0x1,
        .dpl = 0x0,
        .p = 0x1,
        .limit_2 = 0x0,
        .avl = 0x1,
        .l = 0x0,
        .d = 0x1,
        .g = 0x0,
        .base_3 = 0x00,
    };
    debug("\n");
    print_gdt_content(new_gdtr_ptr);

    // Q10
    set_es(gdt_krn_seg_sel(3));
    debug("ES: 0x%x\n", get_es());
    _memcpy8(dst, src, 32);

    // Display copied bytes for debug
    for (int i = 0; i < 0x21; i++) {
        debug("%hhx", (char)*(unsigned char *)(0x600000 + dst + i));
    }

    // Q11
    // _memcpy8(dst, src, 64);

    // Q12
    new_gdt[4] = (seg_desc_t){
        .limit_1 = 0xffff,
        .base_1 = 0x0000,
        .base_2 = 0x00,
        .type = SEG_DESC_CODE_XRA,
        .s = 0x1,
        .dpl = 0x3,
        .p = 0x1,
        .limit_2 = 0xf,
        .avl = 0x1,
        .l = 0x0,
        .d = 0x1,
        .g = 0x1,
        .base_3 = 0x00,
    };

    new_gdt[5] = (seg_desc_t){
        .limit_1 = 0xffff,
        .base_1 = 0x0000,
        .base_2 = 0x00,
        .type = SEG_DESC_DATA_RWA,
        .s = 0x1,
        .dpl = 0x3,
        .p = 0x1,
        .limit_2 = 0xf,
        .avl = 0x1,
        .l = 0x0,
        .d = 0x1,
        .g = 0x1,
        .base_3 = 0x00,
    };

    debug("\n");
    print_gdt_content(new_gdtr_ptr);

    // Q13
    // Set DS, ES, FS, GS
    set_ds(gdt_usr_seg_sel(5));
    set_es(gdt_usr_seg_sel(5));
    set_fs(gdt_usr_seg_sel(5));
    set_gs(gdt_usr_seg_sel(5));

    // Set SS
    // set_ss(gdt_usr_seg_sel(5));

    // Set CS with a far jump
    // fptr32_t fptr;
    // fptr.segment = gdt_usr_seg_sel(4);
    // fptr.offset = (uint32_t)userland;
    // farjump(fptr);
}
