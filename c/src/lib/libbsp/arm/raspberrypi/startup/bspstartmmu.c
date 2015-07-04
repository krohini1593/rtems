#define ARM_CP15_TEXT_SECTION BSP_START_TEXT_SECTION

#include <bsp/start.h>
#include <bsp/arm-cp15-start.h>
#include <bsp/linker-symbols.h>
#include <libcpu/arm-cp15.h>
#include <bsp/mm.h>
#include <bsp.h>

uint32_t bsp_initial_mmu_ctrl_set;
uint32_t bsp_initial_mmu_ctrl_clear;
uint32_t domain_set;

void raspberrypi_setup_mmu_and_cache(void)
{   
#if (BSP_IS_RPI2 == 1)
  /* Enable SMP in auxiliary control */
  uint32_t actlr = arm_cp15_get_auxiliary_control();
  actlr |= ARM_CORTEX_A9_ACTL_SMP;
  arm_cp15_set_auxiliary_control(actlr);  
  bsp_initial_mmu_ctrl_clear = ARM_CP15_CTRL_A;
  bsp_initial_mmu_ctrl_set = ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z;  
#else
  initial_ctrl_clear = 0;
  initial_ctrl_set = ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_S | ARM_CP15_CTRL_XP;  
#endif
  domain_set = ARM_MMU_DEFAULT_CLIENT_DOMAIN;
  
  bsp_memory_management_initialize();
}