#include <immintrin.h>
#include <chrono>
#include <cmath>
#include <future>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "ttmath/ttmath.h"

size_t num_terms;
std::mutex mut;

template <class scalar>
double part_of_pi_scalar(int num_threads, int thread_id) {
    long long int start = num_terms * thread_id / num_threads;
    long long int stop = num_terms * (thread_id + 1) / num_threads;
    scalar sum = 0.0;
    scalar denom1 = 1.0 + 2.0 * start;
    scalar denom2 = 1.0 + 2.0 * start + 2.0;
    for (int i = start; i < stop; i += 2) {
        sum += 1.0 / denom1;
        sum -= 1.0 / denom2;
        denom1 += 4.0;
        denom2 += 4.0;
    }
    return sum;
}

double part_of_pi_avx_dp(int num_threads, int thread_id) {
    long long int start = num_terms * thread_id / num_threads;
    long long int stop = num_terms * (thread_id + 1) / num_threads;
    // ====================== AVX 256 - 4 doubles ===========================//
    double *data_avx = (double *)_mm_malloc(4 * sizeof(double), 32);
    // The Sum
    __m256d sum;
    __m256d factor;
    __m256d denom;
    __m256d div;
    __m256d denom_step;
    sum = _mm256_xor_pd(sum, sum);

    // Load nominators
    data_avx[0] = 1.0;
    data_avx[1] = -1.0;
    data_avx[2] = 1.0;
    data_avx[3] = -1.0;
    factor = _mm256_load_pd(data_avx);

    // Load denominator starting points
    data_avx[0] = 1.0 + 2.0 * start;
    data_avx[1] = 1.0 + 2.0 * start + 2.0;
    data_avx[2] = 1.0 + 1.0 * (start + stop);
    data_avx[3] = 1.0 + 1.0 * (start + stop) + 2.0;
    denom = _mm256_load_pd(data_avx);

    // Load denom steps
    for (int i = 0; i < 4; ++i) data_avx[i] = 4.0;
    denom_step = _mm256_load_pd(data_avx);

    for (int i = start; i < stop; i += 4) {
        div = _mm256_div_pd(factor, denom);
        sum = _mm256_add_pd(sum, div);
        denom = _mm256_add_pd(denom, denom_step);
    }

    _mm256_store_pd(data_avx, sum);
    double result = 0.0;
    for (int i = 0; i < 4; ++i) result += data_avx[i];
    _mm_free(data_avx);
    return result;
}

double part_of_pi_avx_sp_reverse(int num_threads, int thread_id) {
    long long int start = num_terms * thread_id / num_threads;
    long long int stop = num_terms * (thread_id + 1) / num_threads;
    float *data_avx = (float *)_mm_malloc(8 * sizeof(float), 32);
    // The Sum
    __m256 sum;
    __m256 factor;
    __m256 denom;
    __m256 div;
    __m256 denom_step;
    __m256 denom_step_large;
    __m256 neg1;
    sum = _mm256_xor_ps(sum, sum);

    // Load nominators
    for (int i = 0; i < 8; ++i) data_avx[i] = 1.0;
    factor = _mm256_load_ps(data_avx);

    // Load denominator starting points
    data_avx[0] = 1.0 + 2.0 * start;
    data_avx[1] = 1.0 + 2.0 * start + 4.0;
    data_avx[2] = 1.0 + 1.0 * (start + stop);
    data_avx[3] = 1.0 + 1.0 * (start + stop) + 4.0;
    data_avx[4] = 1.0 + 0.5 * (3 * start + stop);
    data_avx[5] = 1.0 + 0.5 * (3 * start + stop) + 4.0;
    data_avx[6] = 1.0 + 0.5 * (start + 3 * stop);
    data_avx[7] = 1.0 + 0.5 * (start + 3 * stop) + 4.0;
    denom = _mm256_load_ps(data_avx);

    for (int i = 0; i < 8; ++i) data_avx[i] = 1.0 * (stop - start);
    neg1 = _mm256_load_ps(data_avx);
    denom = _mm256_add_ps(neg1, denom);

    // Load denom steps
    for (int i = 0; i < 8; ++i) data_avx[i] = 2.0;
    denom_step = _mm256_load_ps(data_avx);
    for (int i = 0; i < 8; ++i) data_avx[i] = 6.0;
    denom_step_large = _mm256_load_ps(data_avx);

    // Load neg1
    for (int i = 0; i < 8; ++i) data_avx[i] = -1.0;
    neg1 = _mm256_load_ps(data_avx);

    for (int i = start; i < stop; i += 8) {
        denom = _mm256_sub_ps(denom, denom_step_large);
        div = _mm256_div_ps(neg1, denom);
        sum = _mm256_add_ps(sum, div);

        denom = _mm256_sub_ps(denom, denom_step);
        div = _mm256_div_ps(factor, denom);
        sum = _mm256_add_ps(sum, div);
    }

    _mm256_store_ps(data_avx, sum);
    double result = 0.0;
    for (int i = 0; i < 8; ++i) result += data_avx[i];
    _mm_free(data_avx);
    return result;
}
double part_of_pi_avx_sp(int num_threads, int thread_id) {
    long long int start = num_terms * thread_id / num_threads;
    long long int stop = num_terms * (thread_id + 1) / num_threads;
    // ====================== AVX 256 - 8 floats ============================//
    float *data_avx = (float *)_mm_malloc(8 * sizeof(float), 32);
    // The Sum
    __m256 sum;
    __m256 factor;
    __m256 denom;
    __m256 div;
    __m256 denom_step;
    __m256 denom_step_large;
    __m256 neg1;
    sum = _mm256_xor_ps(sum, sum);

    // Load nominators
    for (int i = 0; i < 8; ++i) data_avx[i] = 1.0;
    factor = _mm256_load_ps(data_avx);

    // Load neg1
    for (int i = 0; i < 8; ++i) data_avx[i] = -1.0;
    neg1 = _mm256_load_ps(data_avx);

    // Load denominator starting points
    data_avx[0] = 1.0 + 2.0 * start;
    data_avx[1] = 1.0 + 2.0 * start + 4.0;
    data_avx[2] = 1.0 + 1.0 * (start + stop);
    data_avx[3] = 1.0 + 1.0 * (start + stop) + 4.0;
    data_avx[4] = 1.0 + 0.5 * (3 * start + stop);
    data_avx[5] = 1.0 + 0.5 * (3 * start + stop) + 4.0;
    data_avx[6] = 1.0 + 0.5 * (start + 3 * stop);
    data_avx[7] = 1.0 + 0.5 * (start + 3 * stop) + 4.0;
    denom = _mm256_load_ps(data_avx);

    // Load denom steps
    for (int i = 0; i < 8; ++i) data_avx[i] = 2.0;
    denom_step = _mm256_load_ps(data_avx);
    for (int i = 0; i < 8; ++i) data_avx[i] = 6.0;
    denom_step_large = _mm256_load_ps(data_avx);

    for (int i = start; i < stop; i += 8) {
        div = _mm256_div_ps(factor, denom);
        denom = _mm256_add_ps(denom, denom_step);
        sum = _mm256_add_ps(sum, div);
        div = _mm256_div_ps(neg1, denom);
        denom = _mm256_add_ps(denom, denom_step_large);
        sum = _mm256_add_ps(sum, div);
    }

    _mm256_store_ps(data_avx, sum);
    double result = 0.0;
    for (int i = 0; i < 8; ++i) result += data_avx[i];
    _mm_free(data_avx);
    return result;
}

template <class Scalar>
Scalar arctan(Scalar x, int num_terms) {
    Scalar s = 0.0;
    Scalar x_pow = x;
    Scalar x2 = x * x;
    int k = 1;
    for (int i = 0; i < num_terms; ++i) {
        s += x_pow / Scalar(k);
        k += 2;
        x_pow *= -x2;
    }
    return s;
}

template <class Scalar>
Scalar calc_pi_mt(int num_terms) {
    num_terms /= 4;
    //π  = 176·arctan(1/57) + 28·arctan(1/239) - 48·arctan(1/682) +
    //96·arctan(1/12943)
    std::future<Scalar> p1 = std::async(std::launch::async, &arctan<Scalar>,
                                        Scalar(1.0) / Scalar(57.0), num_terms);
    std::future<Scalar> p2 = std::async(std::launch::async, &arctan<Scalar>,
                                        Scalar(1.0) / Scalar(239.0), num_terms);
    std::future<Scalar> p3 = std::async(std::launch::async, &arctan<Scalar>,
                                        Scalar(1.0) / Scalar(682.0), num_terms);
    std::future<Scalar> p4 =
        std::async(std::launch::async, &arctan<Scalar>,
                   Scalar(1.0) / Scalar(12943.0), num_terms);
    Scalar s = 0.0;
    s += Scalar(176.0) * p1.get();
    s += Scalar(28.00) * p2.get();
    s += Scalar(-48.0) * p3.get();
    s += Scalar(96.0) * p4.get();
    return s;
}

template <class Scalar>
Scalar calc_pi(int num_terms) {
    num_terms /= 4;
    //π  = 176·arctan(1/57) + 28·arctan(1/239) - 48·arctan(1/682) +
    //96·arctan(1/12943)
    Scalar p1 = arctan<Scalar>(Scalar(1.0) / Scalar(57.0), num_terms);
    Scalar p2 = arctan<Scalar>(Scalar(1.0) / Scalar(239.0), num_terms);
    Scalar p3 = arctan<Scalar>(Scalar(1.0) / Scalar(682.0), num_terms);
    Scalar p4 = arctan<Scalar>(Scalar(1.0) / Scalar(12943.0), num_terms);
    Scalar s = 0.0;
    s += Scalar(176.0) * p1;
    s += Scalar(28.00) * p2;
    s += Scalar(-48.0) * p3;
    s += Scalar(96.0) * p4;
    return s;
}

void benchmark(std::string desc, int num_threads,
               double (*technique)(int, int)) {
    std::cout << "Benchmarking '" << desc << "' on " << num_threads
              << " threads..." << std::endl;
    auto start = std::chrono::system_clock::now();
    std::vector<std::future<double>> futs;
    for (int i = 0; i < num_threads; ++i) {
        std::future<double> fut =
            std::async(std::launch::async, technique, num_threads, i);
        futs.push_back(std::move(fut));
    }
    double pi = 0.0;
    for (int i = 0; i < num_threads; ++i) {
        pi += futs[i].get();
    }
    pi *= 4.0;
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> dur = end - start;
    std::cout << "  Time: " << dur.count() << std::endl;
    std::cout << "  Result: " << std::setprecision(14) << pi << std::endl;
    std::cout << "  M_PI  : " << std::setprecision(14) << M_PI << std::endl;
    double diff = std::abs(M_PI - pi);
    std::cout << "  Decimals: " << std::setprecision(3)
              << -(std::log(diff) / std::log(10.0)) << "" << std::endl;
}

template <class Scalar>
void benchmark_smart_atan(int num_terms, std::string desc,
                          Scalar (*func)(int)) {
    std::cout << "Benchmarking 'smart atan: " << desc << "'" << std::endl;
    auto start = std::chrono::system_clock::now();
    Scalar pi = func(num_terms);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> dur = end - start;
    std::cout << "  Time: " << std::setprecision(6) << dur.count() << std::endl;
    std::cout << "  Result: " << std::setprecision(15) << pi << std::endl;
}

void test_arctan() {
    for (double d = 0.0; d < 1.0; d += 0.01) {
        std::cout << d << "   "
                  << -std::log(std::abs(std::atan(d) - arctan<double>(d, 500)))
                  << std::endl;
    }
}

int main(int argc, char **argv) {
    num_terms = 10 * 1000 * 1000;
    int num_threads = std::thread::hardware_concurrency();
    if (argc == 2) {
        num_threads = std::atoi(argv[1]);
    }

    benchmark("scalar float version", 1, &part_of_pi_scalar<float>);
    benchmark("scalar float version MT", num_threads, &part_of_pi_scalar<float>);
    std::cout << std::endl << std::endl;
    benchmark("scalar double version", 1, &part_of_pi_scalar<double>);
    benchmark("scalar double version MT", num_threads, &part_of_pi_scalar<double>);
    std::cout << std::endl << std::endl;
    benchmark("AVX float version", num_threads, &part_of_pi_avx_sp);
    benchmark("AVX float rev version", num_threads, &part_of_pi_avx_sp_reverse);
    benchmark("AVX double version", num_threads, &part_of_pi_avx_dp);

    std::cout << std::endl << std::endl;
    benchmark_smart_atan<float>(20, "single threaded <float>", &calc_pi<float>);
    benchmark_smart_atan<float>(20, "multi threaded <float>",
                                &calc_pi_mt<float>);

    std::cout << std::endl << std::endl;
    benchmark_smart_atan<double>(20, "single threaded <double>",
                                 &calc_pi<double>);
    benchmark_smart_atan<double>(20, "multi threaded <double>",
                                 &calc_pi_mt<double>);

    std::cout << std::endl << std::endl;
    typedef ttmath::Big<30, 80> bigdec;
    benchmark_smart_atan<bigdec>(1000, "single threaded <bigdec>",
                                 &calc_pi<bigdec>);
    std::cout << std::endl;
    benchmark_smart_atan<bigdec>(1000, "multi  threaded <bigdec>",
                                 &calc_pi_mt<bigdec>);

    return 0;
}
