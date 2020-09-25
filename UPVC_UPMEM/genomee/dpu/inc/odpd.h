/**
 * Copyright 2016-2019 - Dominique Lavenier & UPMEM
 */

#ifndef __INTEGRATION_ODPD_H__
#define __INTEGRATION_ODPD_H__

#include <stdint.h>

/**
 * @brief Compares two sequences of symbols to assign a score.
 *
 * Optimal Dynamic Programming Diagonal. The sequences are expressed a byte streams, i.e. 4
 * nucleotides per bytes.
 *
 * @param s1           The first vector
 * @param s2           The second vector
 * @param max_score    Any score above this threshold is good
 * @param nbr_sym_len  The number of symbols in s1 and s2
 * @return A score
 */
int odpd(const uint8_t *s1, const uint8_t *s2, int max_score, unsigned int nbr_sym_len);

#define NB_BYTES_TO_SYMS(len, delta) (((len) - (delta)) << 2)
#define NB_ITEMS_PER_MATRIX(nbr_sym_len) ((nbr_sym_len + 2) << 1)
#define SIZEOF_MATRIX(nbr_sym_len) (NB_ITEMS_PER_MATRIX(nbr_sym_len) << 2)

#endif /* __INTEGRATION_ODPD_H__ */
