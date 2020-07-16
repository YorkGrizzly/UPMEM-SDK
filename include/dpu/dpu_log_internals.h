/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_LOG_INTERNALS_H
#define DPU_LOG_INTERNALS_H

#include <dpu_error.h>
#include <dpu_types.h>

/**
 * @file dpu_log_internals.h
 * @brief Internals functions for logging of the toolchain (needed for LLDB).
 */

#define DPU_LOG_FORMAT_HEADER ("=== DPU#0x%x ===\n")

typedef dpu_error_t (*dpu_log_print_fct_t)(void *arg, const char *format, ...);

dpu_error_t
dpulog_read_for_dpu_(struct dpu_t *dpu, dpu_log_print_fct_t print_fct, void *arg);

dpu_error_t
dpulog_read_and_display_contents_of(void *input, size_t input_size, dpu_log_print_fct_t print_fct, void *print_fct_arg);

dpu_error_t
dpulog_c_print_fct(void *arg, const char *fmt, ...);

#endif /* DPU_LOG_INTERNALS_H */
