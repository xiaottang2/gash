/*
 * util.cc -- Implementation of the utility class
 *
 * Author: Xiaoting Tang <tang_xiaoting@brown.edu>
 * Copyright: Xiaoting Tang (2018)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
  * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "util.h"
#include <cstring>

namespace gashgc {

  static block seed;

  void Timer::tic(const char* event_name) {
    if (m_ticking) {

      toc();

      tic(event_name);

    } else {

      m_ticking = true;

      clock_gettime(CLOCK_MONOTONIC, &m_last_time);

      m_curr_name = event_name;

    }
  }

  void Timer::toc() {

    clock_gettime(CLOCK_MONOTONIC, &m_curr_time);

    m_elapsed_time = (m_curr_time.tv_sec - m_last_time.tv_sec) * 1000000000 +
      (m_curr_time.tv_nsec - m_last_time.tv_nsec);

    Event event(string(m_curr_name), m_elapsed_time);

    m_records.insert(make_pair(m_nrecords++, event));

    m_ticking = false;

  }

  void Timer::report() {

    for (const auto& it : m_records) {
      cout << it.second.m_name << ", ";
    }

    cout << "\n";

    for (const auto& it : m_records) {
      cout << it.second.m_elapsed_time << ", ";
    }

  }

  void split(string str, char* delim, StringVec& ret) {

    char* tok = strtok(str.c_str(), delim);

    while (tok) {

      ret.push_back(string(tok));

    }

  }

  void srand_sse(u32 seed) {

    seed = _mm_set_epi32(seed, seed * 3, seed * 7, seed * 11);

  }

  block random_block() {

        block seed_split;
        block multiplier;
        block adder;
        block mod_mask;
        block sra_mask;
        // block sseresult;
        static const unsigned int mult[4] = {214013, 17405, 214013, 69069};
        static const unsigned int gadd[4] = {2531011, 10395331, 13737667, 1};
        static const unsigned int mask[4] = {0xFFFFFFFF, 0, 0xFFFFFFFF, 0};
        static const unsigned int masklo[4] = { 0x00007FFF, 0x00007FFF, 0x00007FFF, 0x00007FFF };

        adder = _mm_load_si128((block *) gadd);
        multiplier = _mm_load_si128((block *) mult);
        mod_mask = _mm_load_si128((block *) mask);
        sra_mask = _mm_load_si128( (block*) masklo);
        seed_split = _mm_shuffle_epi32(seed, _MM_SHUFFLE(2, 3, 0, 1));

        seed = _mm_mul_epu32(seed, multiplier);
        multiplier = _mm_shuffle_epi32(multiplier, _MM_SHUFFLE(2, 3, 0, 1));
        seed_split = _mm_mul_epu32(seed_split, multiplier);

        seed = _mm_and_si128(seed, mod_mask);
        seed_split = _mm_and_si128(seed_split, mod_mask);
        seed_split = _mm_shuffle_epi32(seed_split, _MM_SHUFFLE(2, 3, 0, 1));
        seed = _mm_or_si128(seed, seed_split);
        seed = _mm_add_epi32(seed, adder);

        return seed;
  }

  bool block_eq(block& a, block& b) {

    return memcmp(&a, &b, 16) == 0;

  }

  string block2hex(block& a) {

    char buf[100];

    int cx;

    i64 *v64val = (i64 *) &a;

    cx = snprintf(buf, 100, "%.16llx %.16llx\n", (u64) v64val[1], (u64) v64val[0]);

    if (cx > 0 && cx < 100) {

      return string(buf);

    }

    FATAL("Buffer overflow for block2hex()");
  }

  string block2dec(block& a) {

    char buf[100];

    int cx;

    i64 *v64val = (i64 *) &a;

    cx = snprintf(buf, 100, "%lld %lld\n", (u64) v64val[1], (u64) v64val[0]);

    if (cx > 0 && cx < 100) {

      return string(buf);

    }

    FATAL("Buffer overflow for block2dec()");
  }

  // u32 eval_gate(int a, int b, int func) {


  // }
}
