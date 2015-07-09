#include <bsp/raspberrypi.h>
//#include <bsp/arm-a9mpcore-start.h>
//#include <rtems/score/smpimpl.h>

#include <libcpu/arm-cp15.h>
#include <bsp/arm-cp15-start.h>
#include <bsp.h>
//#include <bsp/start.h>
//#include <bsp/arm-a9mpcore-regs.h>

//extern uint32_t _start;
void start_processor(uint32_t cpuid)
{
  
  uint32_t *write_set_reg = MAILBOX_WRITE_SET_BASE + 0x10*cpuid;
  /* START_ADDRESS to be defined */
 // *write_set_reg  = _start;
}

void start_secondary_processors()
{
  uint32_t cpu_count = 4;
  uint32_t cpuid;
  
  for(cpuid=1;cpuid<cpu_count;cpuid++)
  {
    start_processor(cpuid);
  }
}

void raspberrypi_start_on_secondary_processor()
{
  uint32_t ctrl;

  arm_a9mpcore_start_set_vector_base();

  ctrl = arm_cp15_start_setup_mmu_and_cache(
    0,
    ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z
  );

  arm_cp15_set_domain_access_control(
    ARM_CP15_DAC_DOMAIN(ARM_MMU_DEFAULT_CLIENT_DOMAIN, ARM_CP15_DAC_CLIENT)
  );

  /* FIXME: Sharing the translation table between processors is brittle */
  arm_cp15_set_translation_table_base(
    (uint32_t *) bsp_translation_table_base
  );

  ctrl |= ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M;
  arm_cp15_set_control(ctrl);

  _SMP_Start_multitasking_on_secondary_processor();
}

bool _CPU_SMP_Start_processor(uint32_t cpu_index)
{
  /*
   * Wait for secondary processor to complete its basic initialization so that
   * we can enable the unified L2 cache.
   */
  return _Per_CPU_State_wait_for_non_initial_state(cpu_index, 0);
}