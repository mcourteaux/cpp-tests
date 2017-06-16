
#include <stdio.h>

#include <cmath>

#define xstr(a) str(a)
#define str(a) #a

#define LD_LIT(a) LD_LIT_INT(a)
#define LD_LIT_INT(a) a ## l

int main() {

    printf("sizeof(float): %zu\n", sizeof(float));
    printf("sizeof(double): %zu\n", sizeof(double));
    printf("sizeof(long double): %zu\n", sizeof(long double));

    float pi_f = M_PI;
    double pi_d = M_PI;
    long double pi_ld = LD_LIT(M_PI);


    printf("float pi:       %.60f\n", pi_f);
    printf("double pi:      %.60f\n", pi_d);
    printf("long double pi: %.60Lf\n", pi_ld);

    printf("string pi:      %s\n", xstr(M_PI));
    printf("string pi_ld:   %s\n", xstr(LD_LIT(M_PI)));

    return 0;
}
