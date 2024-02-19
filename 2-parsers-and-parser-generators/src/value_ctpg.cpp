#include <array>
#include <charconv>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "ctpg/ctpg.hpp"

using namespace ctpg;
using namespace ctpg::buffers;

// nterms
constexpr nterm<double> value("value");
constexpr nterm<double> date_value("date_value");
constexpr nterm<double> time_value("time_value");
constexpr nterm<double> date_time_value("date_time_value");

// patterns
constexpr char period_pattern[] = "AM|am|PM|pm";
constexpr char month_pattern[] = "[A-Za-z]+";
//"[AaPp][Mm]?";//"AM | am | PM | pm";
constexpr char number_pattern[] = "[0]?[1-9][0-9]*";

constexpr char sep_pattern[] = "-|/";
constexpr char currency_pattern[] =
    "$";  // wide char seems to be unsupported (e.g. €)
/* constexpr char space_pattern[] = "[\\x20]+"; //unsupported? */
constexpr regex_term<number_pattern> number("number");

constexpr regex_term<period_pattern> period("period");
constexpr regex_term<sep_pattern> sep("sep");
constexpr regex_term<month_pattern> month("month");
constexpr regex_term<currency_pattern> currency("currency");
/* constexpr regex_term<space_pattern> space("space"); */

// Utilities and Callables
double to_int(std::string_view sv) {
    double i = 0;
    std::from_chars(sv.data(), sv.data() + sv.size(), i);
    return i;
}

// Utilities and Callables
double to_double(std::string_view sv) {
    double i = 0;
    std::from_chars(sv.data(), sv.data() + sv.size(), i);
    return i;
}

const inline std::unordered_map<std::string, int> months = {
    {"jan", 1},       {"janu", 1},     {"janua", 1},    {"januar", 1},
    {"january", 1},   {"feb", 2},      {"febr", 2},     {"febru", 2},
    {"februa", 2},    {"februar", 2},  {"february", 2}, {"mar", 3},
    {"marc", 3},      {"march", 3},    {"apr", 4},      {"apri", 4},
    {"april", 4},     {"may", 5},      {"jun", 6},      {"june", 6},
    {"jul", 7},       {"july", 7},     {"aug", 8},      {"augu", 8},
    {"augus", 8},     {"august", 8},   {"sep", 9},      {"sept", 9},
    {"septe", 9},     {"septem", 9},   {"septemb", 9},  {"septembe", 9},
    {"september", 9}, {"oct", 10},     {"octo", 10},    {"octob", 10},
    {"octobe", 10},   {"october", 10}, {"nov", 11},     {"nove", 11},
    {"novem", 11},    {"novemb", 11},  {"novembe", 11}, {"november", 11},
    {"dec", 12},      {"dece", 12},    {"decem", 12},   {"decemb", 12},
    {"decembe", 12},  {"december", 12}};

auto toLower = [](std::string s) -> std::string {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
};

double set_hour_min(std::string_view hour, char sep, std::string_view min) {
    const double DAY_SECONDS = 86'400;
    double divisor = (to_int(min) * 60) + (to_int(hour) * 60 * 60);
    return (divisor / DAY_SECONDS);
}

double set_hour_min_sec(std::string_view hour, char sep, std::string_view min,
                        char sep2, std::string_view sec) {
    const double DAY_SECONDS = 86'400;
    double divisor =
        to_int(sec) + (to_int(min) * 60) + (to_int(hour) * 60 * 60);

    // Decimal can be > 1
    return (divisor / DAY_SECONDS);  // Any time field can be greater than
                                     // normal, Excel supports this
}

double set_hour_min_sec_period(std::string_view hour, char sep,
                               std::string_view min, char sep2,
                               std::string_view sec, std::string_view period) {
    const double DAY_SECONDS = 86'400;

    //
    double hour_val{to_int(hour)}, min_val{to_int(min)}, sec_val{to_int(sec)};

    // Calculation for period here:
    bool isPM = false;
    if (period == "pm" || period == "PM") isPM = true;

    if (isPM) {
        if (hour_val < 12)
            hour_val += 12;
        else if (hour_val > 12) {
            return -1;
        }  // don't put PM in 24HR format
    } else if (hour_val > 12) {
        return -1;
    }  // Time after 12 can't be AM
    else if (hour_val == 12) {
        hour_val = 0.0;
    }

    double divisor = sec_val + (min_val * 60) + (hour_val * 60 * 60);
    // Decimal can be > 1
    return (divisor / DAY_SECONDS);  // Any time field can be greater than
                                     // normal, Excel supports this
}

double set_hour_min_period(std::string_view hour, char sep,
                           std::string_view min, std::string_view period) {
    const double DAY_SECONDS = 86'400;

    //
    double hour_val{to_int(hour)}, min_val{to_int(min)};

    // Calculation for period here:
    bool isPM = false;
    if (period == "pm" || period == "PM") isPM = true;

    if (isPM) {
        if (hour_val < 12)
            hour_val += 12;
        else if (hour_val > 12) {
            return -1;
        }  // don't put PM in 24HR format
    } else if (hour_val > 12) {
        return -1;
    }  // Time after 12 can't be AM
    else if (hour_val == 12) {
        hour_val = 0.0;
    }

    double divisor = (min_val * 60) + (hour_val * 60 * 60);
    // Decimal can be > 1
    return (divisor / DAY_SECONDS);  // Any time field can be greater than
                                     // normal, Excel supports this
}

double set_hour_period(std::string_view hour, std::string_view period) {
    const double DAY_SECONDS = 86'400;

    //
    double hour_val{to_int(hour)};

    // Calculation for period here:
    bool isPM = false;
    if (period == "pm" || period == "PM") isPM = true;

    if (isPM) {
        if (hour_val < 12)
            hour_val += 12;
        else if (hour_val > 12) {
            return -1;
        }  // don't put PM in 24HR format
    } else if (hour_val > 12) {
        return -1;
    }  // Time after 12 can't be AM
    else if (hour_val == 12) {
        hour_val = 0.0;
    }

    // Decimal can be > 1
    return ((hour_val * 60 * 60) /
            DAY_SECONDS);  // Any time field can be greater than
                           // normal, Excel supports this
}

// Date helper

int print_month(std::string_view sv) {
    std::string data(sv);  // Use the correct constructor
    auto it = months.find(toLower(data));
    if (it != months.end()) {
        return it->second;
    } else {
        return -3;  // random error value, -ve is error
    }
}

// Data validation
// Utilities
struct date_vals {
    int day_i{0};
    int month_i{0};
    int year_i{0};
};

inline bool is_leapyear(int y) {
    if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) return true;
    return false;
};

int current_year() {
    auto now = ::time(nullptr);
    auto tm = *localtime(&now);
    return tm.tm_year + 1900;
}

inline double datetime_val(date_vals dt) {
    int days_in_months[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int curr_year_last_2digits = current_year() % 100;

    // CONVERT SHORTHAND YEAR TO FULL YEAR
    if (dt.year_i > 0 && dt.year_i < curr_year_last_2digits)
        dt.year_i += current_year() - curr_year_last_2digits;
    else if (dt.year_i >= curr_year_last_2digits && dt.year_i < 100)
        dt.year_i += 1900;

    // DAYS IN YEAR
    if (is_leapyear(dt.year_i)) days_in_months[2] = 29;

    // ERR CHECK
    if (dt.month_i < 1 || dt.month_i > 12 || dt.day_i < 0 ||
        dt.day_i > days_in_months[dt.month_i] || dt.year_i < 1900)
        return -1;

    // ADD EXTRA DAY PER LEAPYEAR FROM 1900 - DATE
    int count = 0;
    for (int i = 1900; i < dt.year_i; i++)
        if (is_leapyear(i)) count++;

    // COUNT DAYS ASSUMING REGULAR YEARS AND ADD LEAPYEAR DAYS
    double serial = (dt.year_i - 1900) * 365 + dt.day_i + count;  // + time_val;
    // timeval was added too

    // ADD MONTHS UP TO CURRENT MONTH
    for (int a = 1; a < dt.month_i; a++) serial += days_in_months[a];

    return serial > 59 ? serial + 1 : serial;
}

// Swaps year or day based on the correct format
// dt is an out variable
void set_year_or_day(date_vals& dt) {
    if (dt.year_i < 31 && dt.day_i > 1900) {
        std::swap(dt.year_i, dt.day_i);
    } else if (!((dt.year_i > 1900 || dt.year_i < 100) && dt.day_i < 31)) {
        dt.day_i = -1;
    }
};

// Handles date validation and calculates the full date value
double date_calc(int d, int m, int y = current_year(),
                 bool flip_likely = false) {
    date_vals dt = {d, m, y};
    if (flip_likely) set_year_or_day(dt);
    return datetime_val(dt);
}

double month_number_comma_number(std::string_view month, std::string_view day,
                                 char comma, std::string_view year) {
    int m{print_month(month)};
    if (m == -1) return -1;
    return date_calc(to_int(day), m, to_int(year));
}
double number_sep_number_sep_number(std::string_view year, std::string_view sep,
                                    std::string_view month,
                                    std::string_view sep2,
                                    std::string_view day) {
    return date_calc(to_int(day), to_int(month), to_int(year), true);
}
double number_sep_month_sep_number(std::string_view year, std::string_view sep,
                                   std::string_view month,
                                   std::string_view sep2,
                                   std::string_view day) {
    int m{print_month(month)};
    if (m == -1) return -1;
    return date_calc(to_int(day), m, to_int(year), true);
}
double number_sep_number(std::string_view month, std::string_view sep,
                         std::string_view day) {
    return date_calc(to_int(day), to_int(month));
}
double number_space_month(std::string_view day, /* std::string_view space, */
                          std::string_view month) {
    int m{print_month(month)};
    if (m == -1) return -1;
    return date_calc(to_int(day), m);
}
double number_sep_month(std::string_view day, std::string_view sep,
                        std::string_view month) {
    int m{print_month(month)};
    if (m == -1) return -1;
    return date_calc(to_int(day), m);
}

// Datetime rules
/* Time  date
date time
time, date
date, time
time
date */

double date_time_calc(double date, double time) { return date + time; }
double date_time(double date, double time) {
    return date_time_calc(date, time);
}
double time_date(double time, double date) {
    return date_time_calc(date, time);
}
double date_sep_time(double date, char sep, double time) {
    return date_time_calc(date, time);
}
double time_sep_date(double time, char sep, double date) {
    return date_time_calc(date, time);
}
double time_alone(double time) { return date_time_calc(0, time); }
double date_alone(double date) { return date_time_calc(date, 0); }

// Values
// datetime
// ($ number)
// ($ number)%
// (number)
// number percentage
// $number
// number

// double value_calc(std::string_view date_time);
double bracket_currency_number(char bracket, std::string_view currency,
                               std::string_view number, char close_bracket) {
    return -1 * to_int(number);
}
double bracket_currency_number_percent(char bracket, std::string_view currency,
                                       std::string_view number,
                                       char close_bracket, char percent) {
    return -1 * to_int(number) * 0.01;
}
double bracket_number(char bracket, std::string_view number,
                      char close_bracket) {
    return -1 * to_int(number);
}
double number_percent(std::string_view number, char percent) {
    return to_int(number) * 0.01;
}
double currency_number(std::string_view currency, std::string_view number) {
    return to_int(number);
}

constexpr parser p(
    value,
    terms(':', ',', '(', ')', '%',
          /*Terms listed in order of precedence*/ /* space, */ currency, sep,
          number, period, month),
    nterms(value, date_time_value, date_value, time_value),
    rules(
        time_value(month) >= print_month,
        /* time_value */
        time_value(number, ':', number) >= set_hour_min,
        time_value(number, ':', number, period) >= set_hour_min_period,
        time_value(number, period) >= set_hour_period,
        time_value(number, ':', number, ':', number, period) >=
            set_hour_min_sec_period,
        time_value(number, ':', number, ':', number) >= set_hour_min_sec,
        time_value(number, period) >= set_hour_period,

        /* Date */
        date_value(month, number, ',', number) >= month_number_comma_number,
        date_value(number, sep, number, sep, number) >=
            number_sep_number_sep_number,
        date_value(number, sep, month, sep, number) >=
            number_sep_month_sep_number,
        date_value(number, sep, number) >= number_sep_number,
        date_value(number, month) >= number_space_month,  // Learn
                                                          // how
                                                          // to
                                                          // handle
                                                          // spaces
                                                          // !!
        date_value(number, sep, month) >= number_sep_month,

        /* Date-time*/
        date_time_value(date_value, time_value) >= date_time,
        date_time_value(time_value, date_value) >= time_date,
        date_time_value(date_value, ',', time_value) >= date_sep_time,
        date_time_value(time_value, ',', date_value) >= time_sep_date,
        date_time_value(date_value) >= date_alone,
        date_time_value(time_value) >= time_alone,

        /* Final value*/
        value(date_time_value) >= [](double val) { return val; },
        value(currency, number) >= currency_number,
        value('(', currency, number, ')') >= bracket_currency_number,
        value('(', currency, number, ')', '%') >=
            bracket_currency_number_percent,
        value('(', number, ')') >= bracket_number,
        value(number, '%') >= number_percent, value(number) >= to_int));

int main(int argc, char* argv[]) {
    std::cout << "*********Value Parser**********\n";
    std::cout << "Input a string to be parsed:\n";
    std::string input;
    while (std::getline(std::cin, input)) {
        auto res =
            p.parse(/* ctpg::parse_options{}.set_skip_whitespace(), */
                    ctpg::buffers::string_buffer(input.data()), std::cerr);
        bool success = res.has_value();
        if (success)
            std::cout << "Successful parse: " << res.value() << std::endl;
        else
            std::cout << "Unsuccessful parse\n";

        std::cout << "\nInput another string to be parsed:\n";
    }
}
