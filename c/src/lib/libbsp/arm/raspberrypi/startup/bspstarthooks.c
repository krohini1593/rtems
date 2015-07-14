/**
 * @file
 *
 * @ingroup arm_start
 *
 * @brief Rasberry Pi startup code.
 */

/*
 * Copyright (c) 2013. Hesham AL-Matary
 * Copyright (c) 2013 by Alan Cudmore
 * based on work by:
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE
 */

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/raspberrypi.h>
#include <bsp/mm.h>
#include <bsp/arm-cp15-start.h>

BSP_START_TEXT_SECTION void raspberrypi_setup_mmu_and_cache(void)
{   
  uint32_t bsp_initial_mmu_ctrl_set;
  uint32_t bsp_initial_mmu_ctrl_clear;
  
#if (BSP_IS_RPI2 == 1) 
  bsp_initial_mmu_ctrl_clear = ARM_CP15_CTRL_A;
  bsp_initial_mmu_ctrl_set = ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z;  
#else
  bsp_initial_mmu_ctrl_clear = 0;
  bsp_initial_mmu_ctrl_set = ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_S | ARM_CP15_CTRL_XP;  
#endif
  
  bsp_memory_management_initialize(
    bsp_initial_mmu_ctrl_set,
    bsp_initial_mmu_ctrl_clear
  );
}

void BSP_START_TEXT_SECTION bsp_start_hook_0(void)
{ 
  /* Enable SMP in auxiliary control */  
  uint32_t actlr = arm_cp15_get_auxiliary_control();
  actlr |= ARM_CORTEX_A9_ACTL_SMP;
  arm_cp15_set_auxiliary_control(actlr);  
}

void BSP_START_TEXT_SECTION bsp_start_hook_1(void)
{
  bsp_start_copy_sections();
  raspberrypi_setup_mmu_and_cache();
  bsp_start_clear_bss();
}