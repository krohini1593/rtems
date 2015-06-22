/*
 * Copyright (c) 2013 Hesham AL-Matary.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#define ARM_CP15_TEXT_SECTION BSP_START_TEXT_SECTION

#include <bsp/start.h>
#include <bsp/arm-cp15-start.h>
#include <bsp/linker-symbols.h>
#include <bsp/mm.h>

#if (BSP_IS_RPI2 == 1)
BSP_START_TEXT_SECTION void bsp_memory_management_initialize(void)
{  
  /* Enable SMP in auxiliary control */
  uint32_t actlr = arm_cp15_get_auxiliary_control();
  actlr |= ARM_CORTEX_A9_ACTL_SMP;
  arm_cp15_set_auxiliary_control(actlr);

  uint32_t ctrl = arm_cp15_start_setup_mmu_and_cache(
    ARM_CP15_CTRL_A,
    ARM_CP15_CTRL_AFE| ARM_CP15_CTRL_Z
  );
  
  arm_cp15_start_setup_translation_table_and_enable_mmu_and_cache(
    ctrl,
    (uint32_t *) bsp_translation_table_base,
    ARM_MMU_DEFAULT_CLIENT_DOMAIN,
    &arm_cp15_start_mmu_config_table[0],
    arm_cp15_start_mmu_config_table_size
  );
}
#else
BSP_START_TEXT_SECTION void bsp_memory_management_initialize(void)
{
  uint32_t ctrl = arm_cp15_get_control();

  ctrl |= ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_S | ARM_CP15_CTRL_XP;

  arm_cp15_start_setup_translation_table_and_enable_mmu_and_cache(
    ctrl,
    (uint32_t *) bsp_translation_table_base,
    ARM_MMU_DEFAULT_CLIENT_DOMAIN,
    &arm_cp15_start_mmu_config_table[0],
    arm_cp15_start_mmu_config_table_size
  );
}
#endif