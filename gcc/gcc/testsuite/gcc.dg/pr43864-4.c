/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-pre" } */

/* Different stmt order.  */

int f(int c, int b, int d)
{
  int r, r2, e;

  if (c)
    {
      r = b + d;
      r2 = d - b;
    }
  else
    {
      r2 = d - b;
      e = d + b;
      r = e;
    }

  return r - r2;
}

/* { dg-final { scan-tree-dump-times "if " 0 "pre"} } */
/* { dg-final { scan-tree-dump-times "_.*\\\+.*_" 1 "pre"} } */
/* { dg-final { scan-tree-dump-times " - " 2 "pre"} } */
/* { dg-final { cleanup-tree-dump "pre" } } */
