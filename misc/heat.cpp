#include <immintrin.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <thread>

int main(int argc, char **argv) {
    int cores;
    if (argc == 2) {
        cores = std::atoi(argv[1]);
    } else {
        cores = std::thread::hardware_concurrency();
    }
    printf("cores: %d\n", cores);
    for (int i = 0; i < cores; ++i) {
        printf("start core: %d\n", i);
        std::thread t([i]() {
            float init[0x20];
            for (int i = 0; i < 0x20; ++i) {
                init[i] = i * 0.4f;
            }
            __m256 a, b;
            a = _mm256_xor_ps(a, a);
            b = _mm256_xor_ps(b, b);
            a += _mm256_load_ps(init);
            b += _mm256_load_ps(init + 8);
            while (1) {
                sqrtf(rand() / 10000);
                if (i & 1) {
                    b = _mm256_add_ps(a, b);
                    a = _mm256_sub_ps(a, b);
                }
            }
        });
        if (i == cores - 1) {
            t.join();
        } else {
            t.detach();
        }
    }
    return 0;
}
