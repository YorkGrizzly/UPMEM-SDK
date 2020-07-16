/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DPU_RUNNER_H
#define DPU_RUNNER_H

#include <pthread.h>
#include <stdint.h>

#include <dpu_error.h>
#include <dpu_types.h>

/**
 * @file dpu_runner.h
 * @brief C API for DPU execution operations.
 */

/**
 * @fn dpu_launch_thread_on_rank
 * @brief Launch a thread on all DPUs of a rank.
 * @param rank the the rank of DPUs
 * @param thread the thread to launch
 * @param should_resume whether the thread PC should be kept intact or set to 0
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_launch_thread_on_rank(struct dpu_rank_t *rank, dpu_thread_t thread, bool should_resume);

/**
 * @fn dpu_launch_thread_on_dpu
 * @brief Launch a thread on a DPU.
 * @param dpu the DPU
 * @param thread the thread to launch
 * @param should_resume whether the thread PC should be kept intact or set to 0
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_launch_thread_on_dpu(struct dpu_t *dpu, dpu_thread_t thread, bool should_resume);

/**
 * @fn dpu_poll_rank
 * @brief Poll the status of all DPUs of a rank, and update the run_context structure inside the rank structure.
 * @param rank the rank of DPUs
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_poll_rank(struct dpu_rank_t *rank);

/**
 * @fn dpu_poll_dpu
 * @brief Poll the status of a DPU.
 * @param rank the rank of DPUs
 * @param slice_id the slice containing the DPU
 * @param member_id the member id of the DPU in its slice
 * @param dpu_is_running whether the DPU is running
 * @param dpu_is_in_fault whether the DPU is in fault
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_poll_dpu(struct dpu_t *dpu, bool *dpu_is_running, bool *dpu_is_in_fault);

/**
 * @fn dpu_get_thread_status_on_rank
 * @brief Fetch the status of a thread on all DPUs of a rank.
 * @param rank the rank of DPUs
 * @param thread the thread to check
 * @param thread_is_running bitfield describing whether the thread is running on each DPU of the rank
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_get_thread_status_on_rank(struct dpu_rank_t *rank, dpu_thread_t thread, dpu_bitfield_t *thread_is_running);

/**
 * @fn dpu_get_thread_status_on_dpu
 * @brief Fetch the status of a thread on a DPU.
 * @param dpu the DPU
 * @param thread the thread to check
 * @param thread_is_running whether the thread is running on the DPU
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_get_thread_status_on_dpu(struct dpu_t *dpu, dpu_thread_t thread, bool *thread_is_running);

/**
 * @fn dpu_get_and_update_notify_status_on_rank
 * @brief Fetch and set the status of a notify bit on all DPUs of a rank.
 * @param rank the rank of DPUs
 * @param thread the notify bit to check
 * @param value the new value of the notify bit
 * @param notify_bit_was_set bitfield describing whether the notify bit was set on each DPU of the rank
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_get_and_update_notify_status_on_rank(struct dpu_rank_t *rank,
    dpu_notify_bit_id_t notify_bit,
    bool value,
    dpu_bitfield_t *notify_bit_was_set);

/**
 * @fn dpu_get_and_update_notify_status_on_dpu
 * @brief Fetch and set the status of a notify bit on a DPU.
 * @param dpu the DPU
 * @param thread the notify bit to check
 * @param value the new value of the notify bit
 * @param notify_bit_was_set whether the notify bit was set on the DPU
 * @return Whether the operation was successful.
 */
dpu_error_t
dpu_get_and_update_notify_status_on_dpu(struct dpu_t *dpu, dpu_notify_bit_id_t notify_bit, bool value, bool *notify_bit_was_set);

typedef struct _dpu_run_context_t {
    dpu_bitfield_t dpu_running[DPU_MAX_NR_CIS];
    dpu_bitfield_t dpu_in_fault[DPU_MAX_NR_CIS];
    uint8_t nb_dpu_running;
} * dpu_run_context_t;

/**
 * @fn dpu_get_run_context
 * @brief Fetches the rank run context.
 * @param rank the unique identifier of the rank
 * @return The pointer on the rank run context.
 */
dpu_run_context_t
dpu_get_run_context(struct dpu_rank_t *rank);

#endif // DPU_RUNNER_H
