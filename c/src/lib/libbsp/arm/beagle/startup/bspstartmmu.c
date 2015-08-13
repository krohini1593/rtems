/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * Copyright (c) 2014 Chris Johns.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/mm.h>
#include <bsp/start.h>
#include <bsp/arm-cp15-start.h>

#define ARM_SECTIONS       4096          /* all sections needed to describe the
                                            virtual address space */
#define ARM_SECTION_SIZE   (1024 * 1024) /* how much virtual memory is described
                                            by one section */

//static uint32_t pagetable[ARM_SECTIONS] __attribute__((aligned (1024*16)));

BSP_START_DATA_SECTION const arm_cp15_start_section_config
arm_cp15_start_mmu_config_table[] = {
  ARMV7_CP15_START_DEFAULT_SECTIONS,
  {
    .begin = 0x40000000U,
    .end = 0x4FFFFFFFU,
    .flags = ARMV7_MMU_DEVICE
  }
};

/*
 * Make weak and let the user override.
 */
BSP_START_TEXT_SECTION void beagle_setup_mmu_and_cache(void) __attribute__ ((weak));

BSP_START_TEXT_SECTION void beagle_setup_mmu_and_cache(void)
{
  /* turn mmu off first in case it's on */
  uint32_t bsp_initial_mmu_ctrl_clear = ARM_CP15_CTRL_M | ARM_CP15_CTRL_A; 
  uint32_t bsp_initial_mmu_ctrl_set = ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z;
  
    bsp_memory_management_initialize(
    bsp_initial_mmu_ctrl_set,
    bsp_initial_mmu_ctrl_clear
  );
}

const size_t arm_cp15_start_mmu_config_table_size =
  RTEMS_ARRAY_SIZE(arm_cp15_start_mmu_config_table);