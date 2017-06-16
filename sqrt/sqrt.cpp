
#include <iostream>
#include <iomanip>

int main() {
    double val = 10;
    double sqrt = 1;
    std::cout << std::setprecision(20);
    for (int i = 0; i < 11; ++i) {
        sqrt = (val + sqrt * sqrt) / (2.0 * sqrt);
        std::cout << "sqrt(" << val << ") = " << sqrt << std::endl;
    }
    std::cout << "sqrt(" << val << ") = " << sqrt << std::endl;
    return 0;
}
