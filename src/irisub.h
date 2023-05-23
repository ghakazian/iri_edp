// irisub.h Header file with the stub used to call the Fortran entrypoint of the IRI model

#include<stdbool.h>

typedef int ft_integer;
typedef float ft_real;
typedef int ft_logical;

extern int iri_web_for_c(ft_integer *jmag, ft_integer *jf, ft_real *alati, ft_real *along, ft_integer *iyyyy, ft_integer *mmdd, ft_integer *iut,
    ft_real *dhour, ft_real *height, ft_real *h_tec_max, ft_integer *ivar, ft_real *vbeg, ft_real *vend, ft_real *vstp, ft_real *a, ft_real *b);