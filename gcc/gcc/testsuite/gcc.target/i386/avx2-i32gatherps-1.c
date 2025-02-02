/* { dg-do compile } */
/* { dg-options "-mavx2 -O2" } */
/* { dg-final { scan-assembler "vgatherdps\[ \\t\]+\[^\n\]*%xmm\[0-9\]" } } */

#include <immintrin.h>

__m128 x;
float *base;
__m128i idx;

void extern
avx2_test (void)
{
  x = _mm_i32gather_ps (base, idx, 1);
}
