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
    using month = std::chrono::duration<double, std::ratio<24 * 3600 * 365, 12>>;
    using year = std::chrono::duration<double, std::ratio<24 * 3600 * 365, 1>>;

    do {
        auto now = std::chrono::system_clock::now();
        auto dur = now - tp;

        double days = day(dur).count();
        double months = month(dur).count();
        double years = year(dur).count();

        char buff_days[100];
        snprintf(buff_days, sizeof(buff_days), "%4.5f", days);
        std::string formatted_days = buff_days;

        char buff_months[100];
        snprintf(buff_months, sizeof(buff_months), "%4.7f", months);
        std::string formatted_months = buff_months;

        char buff_years[100];
        snprintf(buff_years, sizeof(buff_years), "%4.9f", years);
        std::string formatted_years = buff_years;

        std::cout << formatted_days << " days since we got together." << std::endl;
        std::cout << formatted_months << " months, this is." << std::endl;
        std::cout << formatted_years << " years (soon, this will be useful)." << std::endl;

        if (cont) {
            using namespace std::chrono_literals;
            //std::this_thread::sleep_for(864ms);
            std::this_thread::sleep_for(50ms);

            // Move cursor up.
            std::cout << "\033[1A";
            std::cout << "\033[1A";
            std::cout << "\033[1A";
        }
    } while (cont);

    return 0;
}
