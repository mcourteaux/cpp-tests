#include <cassert>
#include <cstdio>
#include <cstdlib>

void usage() {
    puts("mod_mult_inv <n> <x>");
    puts("    Computes the inverse of x, modulo n");
    puts("    x * x^-1 = 1 (mod n)");
}

int main(int argc, char **argv) {
    if (argc != 3) {
        usage();
        return 1;
    }

    int n = std::atoi(argv[1]);
    int x = std::atoi(argv[2]);

    printf("n = %d\nx = %d\n", n, x);
    printf("Computing %d^-1 (mod %d)\n", x, n);

    /* The algo here */
    int A1, A2, A3;
    int B1, B2, B3;
    int C1, C2, C3;
    int r, q;

    /* Initialization */
    A1 = B2 = 1;
    A2 = B1 = 0;
    C1 = C2 = C3 = 0;

    printf("%6s %6s + %6s %6s = %6s      %6s = %6s %6s + %6s\n", "n", "A1", "x",
           "A2", "A3", "A3", "q", "B3", "r");
    printf("\n");
    do {
        /* Solve: A3 = q*B3 + r */
        A3 = n * A1 + x * A2;
        B3 = n * B1 + x * B2;
        q = A3 / B3;
        r = A3 % B3;

        /* Next line */
        C1 = A1 - q * B1;
        C2 = A2 - q * B2;
        C3 = r;
        assert(A3 - q * B3 == r);

        printf("%6d %6d + %6d %6d = %6d      %6d = %6d %6d + %6d\n", n, A1, x,
               A2, A3, A3, q, B3, r);

        /* Shift lines up */
        A1 = B1;
        A2 = B2;
        A3 = B3;
        B1 = C1;
        B2 = C2;
        B3 = C3;
    } while (r > 1);
    printf("%6d %6d + %6d %6d = %6d      %6d = %6d %6d + %6d\n", n, B1, x, B2,
           B3, B3, q, C3, r);

    puts("");
    if (r == 1) {
        printf("Result: %6d\n", C2);
        printf("%6d * %6d = %6d = %6d\n", C2, x, C2 * x, (C2 * x) % n);
    } else {
        printf("No solution!\n");
    }

    return 0;
}
