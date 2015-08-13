/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#define ARM_CP15_TEXT_SECTION BSP_START_TEXT_SECTION

#include <bsp.h>
#include <bsp/mm.h>
#include <bsp/start.h>
#include <bsp/arm-cp15-start.h>
#include <bsp/arm-a9mpcore-start.h>

BSP_START_DATA_SECTION const arm_cp15_start_section_config
arm_cp15_start_mmu_config_table[] = {
  ARMV7_CP15_START_DEFAULT_SECTIONS,
  {
    .begin = 0x10000000U,
    .end = 0x10020000U,
    .flags = ARMV7_MMU_DEVICE
  }, {
    .begin = 0x1f000000U,
    .end = 0x20000000U,
    .flags = ARMV7_MMU_DEVICE
  }, {
    .begin = 0x4e000000U,
    .end = 0x4f000000U,
    .flags = ARMV7_MMU_DEVICE
  }
};

const size_t arm_cp15_start_mmu_config_table_size =
  RTEMS_ARRAY_SIZE(arm_cp15_start_mmu_config_table);

BSP_START_TEXT_SECTION static void setup_mmu_and_cache(void)
{
  uint32_t bsp_initial_mmu_ctrl_set = ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z;
  uint32_t bsp_initial_mmu_ctrl_clear = ARM_CP15_CTRL_A;
  
  bsp_memory_management_initialize(
    bsp_initial_mmu_ctrl_set,
    bsp_initial_mmu_ctrl_clear
  );
}

BSP_START_TEXT_SECTION void bsp_start_hook_0(void)
{
#ifdef RTEMS_SMP
  uint32_t cpu_id = arm_cortex_a9_get_multiprocessor_cpu_id();

  /*
   * QEMU jumps to the entry point of the ELF file on all processors.  Prevent
   * a SMP_FATAL_MULTITASKING_START_ON_INVALID_PROCESSOR this way.
   */
  if ( cpu_id >= rtems_configuration_get_maximum_processors() ) {
    while (true) {
      _ARM_Wait_for_event();
    }
  }
#endif

  arm_a9mpcore_start_hook_0();
}

BSP_START_TEXT_SECTION void bsp_start_hook_1(void)
{
  arm_a9mpcore_start_hook_1();
  bsp_start_copy_sections();
  setup_mmu_and_cache();
  bsp_start_clear_bss();
}