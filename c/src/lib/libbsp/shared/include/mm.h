/**
 *  @file
 *
 *  @ingroup shared_mm
 *
 *  @brief MM Support
 */

/*
 * Copyright (c) 2013 Hesham AL-Matary.
 * Copyright (c) 2013 Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef __LIBBSP_MM_H
#define __LIBBSP_MM_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup shared_mm MM Support
 *
 *  @ingroup shared_include
 *
 *  @brief MM Support Package
 */

void bsp_memory_management_initialize(
  uint32_t bsp_initial_mmu_ctrl_set,
  uint32_t bsp_initial_mmu_ctrl_clear,
  uint32_t domain_set
);

#ifdef __cplusplus
}
#endif
#endif