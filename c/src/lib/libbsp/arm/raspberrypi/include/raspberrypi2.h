/**
 * @name Mailbox Register Defines
 *
 * @{
 */

#define MAILBOX_IRQ_CTRL_BASE 0x4000005C
#define MAILBOX_WRITE_SET_BASE 0x4000008C
#define MAILBOX_READ_CLEAR_BASE 0x400000CC

/** @} */

/**
 * @brief Raspberry Pi 2 SMP functions
 */
void start_on_secondary_processor();
void start_processor(uint32_t cpuid);

