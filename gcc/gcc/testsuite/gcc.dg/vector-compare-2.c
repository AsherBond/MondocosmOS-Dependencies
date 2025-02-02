/* { dg-do compile } */   

/* Test if C_MAYBE_CONST are folded correctly when 
   creating VEC_COND_EXPR.  */

typedef int vec __attribute__((vector_size(16)));

vec i,j;
extern vec a, b, c;

extern int p, q, z;
extern vec foo (int);

vec 
foo (int x)
{
  return  foo (p ? q :z) > a;
}

vec 
bar (int x)
{
  return  b > foo (p ? q :z);
}


