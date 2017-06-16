#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

int main(int argc, char **argv) {
    bool cont = false;
    if (argc == 2) {
        if (std::string(argv[1]) == "--cont") {
            cont = true;
        }
    }

    std::cout << "I met this girl, late last year:" << std::endl;

    std::tm tm = {};
    std::stringstream ss("Feb 8 2017 01:00:00");
    ss >> std::get_time(&tm, "%b %d %Y %H:%M:%S");
    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    using day = std::chrono::duration<double, std::ratio<24 * 3600, 1>>;

    do {
        auto now = std::chrono::system_clock::now();
        auto dur = now - tp;

        double days = day(dur).count();

        char buff[100];
        snprintf(buff, sizeof(buff), "%4.5f", days);
        std::string formatted_days = buff;

        std::cout << formatted_days << " days since we got together." << std::endl;

        if (cont) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(864ms);

            // Move cursor up.
            std::cout << "\033[1A";
        }
    } while (cont);

    return 0;
}
