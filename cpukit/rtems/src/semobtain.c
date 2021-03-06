/**
 *  @file
 *
 *  @brief RTEMS Obtain Semaphore
 *  @ingroup ClassicSem
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/attrimpl.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/optionsimpl.h>
#include <rtems/rtems/semimpl.h>
#include <rtems/score/coremuteximpl.h>
#include <rtems/score/coresemimpl.h>
#include <rtems/score/thread.h>

#include <rtems/score/interr.h>

rtems_status_code rtems_semaphore_obtain(
  rtems_id        id,
  rtems_option    option_set,
  rtems_interval  timeout
)
{
  Semaphore_Control              *the_semaphore;
  Objects_Locations               location;
  ISR_lock_Context                lock_context;
  Thread_Control                 *executing;
  rtems_attribute                 attribute_set;
  bool                            wait;

  the_semaphore = _Semaphore_Get_interrupt_disable(
    id,
    &location,
    &lock_context
  );
  switch ( location ) {

    case OBJECTS_LOCAL:
      executing = _Thread_Executing;
      attribute_set = the_semaphore->attribute_set;
      wait = !_Options_Is_no_wait( option_set );
#if defined(RTEMS_SMP)
      _Thread_Disable_dispatch();
      if ( _Attributes_Is_multiprocessor_resource_sharing( attribute_set ) ) {
        MRSP_Status mrsp_status;

        _ISR_lock_ISR_enable( &lock_context );
        mrsp_status = _MRSP_Obtain(
          &the_semaphore->Core_control.mrsp,
          executing,
          wait,
          timeout
        );
        _Thread_Enable_dispatch();
        _Objects_Put_for_get_isr_disable( &the_semaphore->Object );
        return _Semaphore_Translate_MRSP_status_code( mrsp_status );
      } else
#endif
      if ( !_Attributes_Is_counting_semaphore( attribute_set ) ) {
        _CORE_mutex_Seize(
          &the_semaphore->Core_control.mutex,
          executing,
          id,
          wait,
          timeout,
          &lock_context
        );
#if defined(RTEMS_SMP)
        _Thread_Enable_dispatch();
#endif
        _Objects_Put_for_get_isr_disable( &the_semaphore->Object );
        return _Semaphore_Translate_core_mutex_return_code(
                  executing->Wait.return_code );
      }

      /* must be a counting semaphore */
      _CORE_semaphore_Seize_isr_disable(
        &the_semaphore->Core_control.semaphore,
        executing,
        id,
        wait,
        timeout,
        &lock_context
      );
#if defined(RTEMS_SMP)
      _Thread_Enable_dispatch();
#endif
      _Objects_Put_for_get_isr_disable( &the_semaphore->Object );
      return _Semaphore_Translate_core_semaphore_return_code(
                  executing->Wait.return_code );

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      return _Semaphore_MP_Send_request_packet(
          SEMAPHORE_MP_OBTAIN_REQUEST,
          id,
          option_set,
          timeout
      );
#endif

    case OBJECTS_ERROR:
      break;

  }

  return RTEMS_INVALID_ID;
}
