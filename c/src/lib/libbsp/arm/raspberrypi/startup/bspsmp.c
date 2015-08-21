#include <bsp/raspberrypi.h>
#include <libcpu/arm-cp15.h>
#include <bsp/arm-cp15-start.h>
#include <bsp.h>
#include <bsp/start.h>
#include <bsp/irq.h>
#include <bsp/linker-symbols.h>
#include <rtems/score/smpimpl.h>

BSP_START_TEXT_SECTION void raspberrypi_wake_secondary_processors()
{
  __asm__ volatile (
   "ldr r2 , =_start\n"
   "ldr r1 , =0x4000009C\n"
   "str r2 , [r1]\n"
   "ldr r1 , =0x400000AC\n"
   "str r2 , [r1]\n"
   "ldr r1 , =0x400000BC\n"
   "str r2 , [r1]\n"
  ); 
} 

BSP_START_TEXT_SECTION inline void start_on_secondary_processor(void)
{
  uint32_t ctrl;
   
  /* Includes support only for mailbox 3 interrupt. 
   * Further interrupt support has to be added. This will have to be integrated
   * with existing interrupt support for Raspberry Pi */
   raspberrypi_IPI_initialize();
  
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
  
}
 
void _CPU_SMP_Prepare_start_multitasking( void )
{
  /* Do nothing */
}

void _CPU_SMP_Send_interrupt( uint32_t target_processor_index )
{
  /* Generates IPI */
  uint32_t *target_mb_write = (uint32_t *)(BCM2836_MAILBOX_3_WRITE_SET_BASE + 0x10 * target_processor_index);
  *target_mb_write = 0x1;
}

void raspberrypi_IPI_initialize(void)
{
  uint32_t cpuid = arm_cortex_a9_get_multiprocessor_cpu_id();
  uint32_t *mb_read_clr = (uint32_t *)(BCM2836_MAILBOX_3_READ_CLEAR_BASE + 0x10 * cpuid);
  uint32_t *mb_write_set = (uint32_t *)(BCM2836_MAILBOX_3_WRITE_SET_BASE + 0x10 * cpuid);
  uint32_t *mb_irq_ctrl = (uint32_t *)(BCM2836_MAILBOX_IRQ_CTRL_BASE + 0x4 * cpuid);
  
  /* reset mailbox 3 contents to zero */
  *mb_read_clr = *mb_write_set;
  
  *mb_irq_ctrl |= 0x8;    
}

void raspberrypi_IPI_handler(void)
{
  uint32_t cpuid = arm_cortex_a9_get_multiprocessor_cpu_id();
  uint32_t *irq_src_reg = (uint32_t *)(BCM2836_IRQ_SOURCE_REG_BASE + 0x4 * cpuid);
  uint32_t *mb_irq_ctrl = (uint32_t *)(BCM2836_MAILBOX_IRQ_CTRL_BASE + 0x4 * cpuid);
  uint32_t *mb_read_clr = (uint32_t *)(BCM2836_MAILBOX_3_READ_CLEAR_BASE + 0x10 * cpuid);
  
  if ((*irq_src_reg & 0x80) && (*mb_read_clr == 0x01 )){
    
    /* writing zero to mailbox clears the interrupt */
    *mb_read_clr = 0x01;
    
    _SMP_Inter_processor_interrupt_handler();
  }
}