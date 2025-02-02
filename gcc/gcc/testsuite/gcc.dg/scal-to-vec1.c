/* { dg-do compile } */
/* { dg-options "-Wno-long-long" } */
#define vector(elcount, type)  \
__attribute__((vector_size((elcount)*sizeof(type)))) type

#define vidx(type, vec, idx) (*((type *) &(vec) + idx))


extern float sfl;
extern int   sint;
extern long long sll;

int main (int argc, char *argv[]) {
    vector(8, short) v0 = {argc, 1,2,3,4,5,6,7};
    vector(8, short) v1;

    vector(4, float) f0 = {1., 2., 3., 4.};
    vector(4, float) f1, f2;

    vector(4, int) i0 = {1,2,3,4};
    vector(4, int) i1, i2;

    
    int     i = 12;
    double  d = 3.;

    v1 = i + v0;        /* { dg-error "conversion of scalar to vector" } */
    v1 = 99999 + v0;    /* { dg-error "conversion of scalar to vector" } */

    f1 = d + f0;        /* { dg-error "conversion of scalar to vector" } */
    f1 = 1.3 + f0;      /* { dg-error "conversion of scalar to vector" } */
    f1 = sll + f0;      /* { dg-error "conversion of scalar to vector" } */
    f1 = ((int)998769576) + f0; /* { dg-error "conversion of scalar to vector" } */

    /* convert.c should take care of this.  */
    i1 = sfl + i0;      /* { dg-error "can't convert value to a vector" } */
    i1 = 1.5 + i0;      /* { dg-error "can't convert value to a vector" } */
    v1 = d + v0;        /* { dg-error "can't convert value to a vector" } */

    return 0;
}
