#include <bsp/raspberrypi.h>
#include <bsp/arm-cp15-start.h>

BSP_START_TEXT_SECTION void start_processor(uint32_t cpuid)
{
  void (*cpu_mailbox_write_set_reg)() = (void (*)() )(MAILBOX_WRITE_SET_BASE + 0x10*cpuid); 
  cpu_mailbox_write_set_reg = _start;
}

BSP_START_TEXT_SECTION void raspberrypi_wake_secondary_processors()
{
  uint32_t cpu_count = 4;
  uint32_t cpuid;
  
  for(cpuid=1;cpuid<cpu_count;cpuid++)
  {
    start_processor(cpuid);
  }
}

BSP_START_TEXT_SECTION inline void
start_on_secondary_processor(void)
{
  uint32_t ctrl;

  ctrl = arm_cp15_start_setup_mmu_and_cache(
    0,
    ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z
  );

  arm_cp15_set_domain_access_control(
    ARM_CP15_DAC_DOMAIN(ARM_MMU_DEFAULT_CLIENT_DOMAIN, ARM_CP15_DAC_CLIENT)
  );

  arm_cp15_set_translation_table_base(
    (uint32_t *) bsp_translation_table_base
  );

  ctrl |= ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M;
  arm_cp15_set_control(ctrl);

  _SMP_Start_multitasking_on_secondary_processor();
}

bool _CPU_SMP_Start_processor(uint32_t cpu_index)
{
  (void) cpu_index;

  /* Nothing to do */

  return true;
}

uint32_t _CPU_SMP_Initialize(void)
{
  uint32_t hardware_count = 4;
  uint32_t linker_count = (uint32_t) bsp_processor_count;

  return hardware_count <= linker_count ? hardware_count : linker_count;
}

void _CPU_SMP_Finalize_initialization(uint32_t cpu_count)
{
  /* this definition is incomplete */
  if (cpu_count > 0) {
    rtems_status_code sc;
    
    sc = RTEMS_SUCCESSFUL;
  }
}
 
void _CPU_SMP_Prepare_start_multitasking( void )
{
  /* Do nothing */
}

void _CPU_SMP_Send_interrupt( uint32_t target_processor_index )
{
  
}