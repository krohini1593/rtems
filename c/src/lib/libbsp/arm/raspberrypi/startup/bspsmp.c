#include<raspberrypi2.h>

void start_processor(uint32_t cpuid)
{
  uint32_t *write_set_reg = MAILBOX_WRITE_SET_BASE + 0x10*cpuid;
  /* START_ADDRESS to be defined */
  *write_set_reg | = START_ADDRESS;
}

void start_on_secondary_processor()
{
  uint32_t cpu_count = 4;
  uint32_t cpuid;
  
  for(cpuid=1;cpuid<cpu_count;cpuid++)
  {
    start_processor(cpuid);
  }
}