/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>

extern info_t   *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;

char * get_multiboot_mmtype(multiboot_uint32_t type) {
/*
in grub_mbi.h
  #define MULTIBOOT_MEMORY_AVAILABLE              1
  #define MULTIBOOT_MEMORY_RESERVED               2
  #define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
  #define MULTIBOOT_MEMORY_NVS                    4
*/
   switch (type) {
   case MULTIBOOT_MEMORY_AVAILABLE:
      return "MULTIBOOT_MEMORY_AVAILABLE";
   case MULTIBOOT_MEMORY_RESERVED:
      return "MULTIBOOT_MEMORY_RESERVED";
   case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
      return "MULTIBOOT_MEMORY_ACPI_RECLAIMABLE";
   case MULTIBOOT_MEMORY_NVS:
      return "MULTIBOOT_MEMORY_NVS";
   }

   return "UNKNOWN";
}


void tp() {
   debug("kernel mem [0x%p - 0x%p]\n", &__kernel_start__, &__kernel_end__);
   debug("MBI flags 0x%x\n", info->mbi->flags);

   multiboot_memory_map_t* entry = (multiboot_memory_map_t*)info->mbi->mmap_addr;
   while((uint32_t)entry < (info->mbi->mmap_addr + info->mbi->mmap_length)) {
      // Q2
      debug(
         "[0x%x - 0x%x] %s\n",
         (unsigned int)entry->addr,
         (unsigned int)(entry->addr + entry->len - 1),
         get_multiboot_mmtype(entry->type)
      );

      entry++;
   }

   // Q3
   int *ptr_in_available_mem;
   ptr_in_available_mem = (int*)0x0;
   debug("Available mem (0x0): before: 0x%x ", *ptr_in_available_mem); // read
   *ptr_in_available_mem = 0xaaaaaaaa;                           // write
   debug("after: 0x%x\n", *ptr_in_available_mem);                // check

   int *ptr_in_reserved_mem;
   ptr_in_reserved_mem = (int*)0xf0000;
   debug("Reserved mem (at: 0xf0000):  before: 0x%x ", *ptr_in_reserved_mem); // read
   *ptr_in_reserved_mem = 0xaaaaaaaa;                           // write
   debug("after: 0x%x\n", *ptr_in_reserved_mem);                // check

   // Q4
   int *ptr_outside_mem;
   ptr_outside_mem = (int*)0x1000000ff;
   debug("Outside mem (at: 0x1000000ff):  before: 0x%x ", *ptr_outside_mem); // read
   *ptr_outside_mem = 0xaaaaaaaa;                           // write
   debug("after: 0x%x\n", *ptr_outside_mem);                // check
}
