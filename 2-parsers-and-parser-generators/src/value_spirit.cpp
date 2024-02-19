#include <algorithm>
#include <boost/spirit/home/x3.hpp>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

using namespace boost::spirit;
namespace x3 = boost::spirit::x3;

// Globals
inline struct datetime_vals {
    int year_i{0};
    int month_i{0};
    int day_i{0};
    double hour{0.0};
    double min{0.0};
    double sec{0.0};
    std::optional<bool> period{std::nullopt};  // PM is true
    bool date_available{false};
} dt;

inline std::optional<double> final_value{0};

// Global temporary buffers
inline double d_buf{0.0};
inline int i_buf{0};

inline void set_date_available() { dt.date_available = true; }

inline void reset_dt() { dt = {0, 0, 0, 0, 0, 0, std::nullopt, false}; }

// Setters
auto set_hour = [](auto& ctx) { dt.hour = x3::_attr(ctx); };
auto set_min = [](auto& ctx) { dt.min = x3::_attr(ctx); };
auto set_sec = [](auto& ctx) { dt.sec = x3::_attr(ctx); };
auto set_year = [](auto& ctx) { dt.year_i = x3::_attr(ctx); };
auto set_month = [](auto& ctx) { dt.month_i = x3::_attr(ctx); };
auto set_day = [](auto& ctx) { dt.day_i = x3::_attr(ctx); };

auto set_year_or_day = []() {
    if (dt.day_i > 31) {
        dt.year_i = dt.day_i;
        dt.day_i = 1;  // tm.tm_mday;
    } else {
        auto now = ::time(nullptr);
        auto tm = *localtime(&now);
        dt.year_i = tm.tm_year + 1900;
    }
};

// Utilities
auto toLower = [](std::string s) -> std::string {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
};

inline bool is_leapyear(int y) {
    return ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0);
};

inline double timevalue_val() {
    // Bounds Checks, -1 is error
    if (dt.hour < 0 || dt.min < 0 || dt.sec < 0) return -1;

    // Dealing with decimal sec above 3dp
    std::string ss{std::to_string(dt.sec)};
    if (ss.find('.') != std::string::npos) {
        auto dec = ss.substr(ss.find('.'));
        auto left = ss.substr(0, ss.find('.'));
        if (dec.length() > 3) {
            dec = dec.substr(0, 4);
            ss = left.append(dec);
        }
    }
    dt.sec = std::stod(ss);

    // min and sec must not both be >59
    if (dt.min > 59 && dt.sec > 59) return -1;

    // Hour can be above 24 but not when min or sec is above 60
    if (dt.hour > 24 && dt.min > 59 || dt.hour > 24 && dt.sec > 59) return -1;

    const double DAY_SECONDS = 86'400;  // number of seconds in a day

    if (dt.period.has_value()) {
        if (dt.period.value()) {
            if (dt.hour < 12)
                dt.hour += 12;
            else if (dt.hour > 12)
                return -1;  // don't put PM in 24HR format
        } else if (dt.hour == 12)
            dt.hour = 0.0;

        else if (dt.hour > 12)
            return -1;  // Time after 12 can't be AM
    }

    double divisor = dt.sec + (dt.min * 60) + (dt.hour * 60 * 60);

    // Decimal can be > 1
    return (divisor / DAY_SECONDS);  // Any time field can be greater than
                                     // normal, Excel supports this
};

inline double datetime_val() {
    double time_val = timevalue_val();
    if (time_val == -1)
        return -1;  // propagate error

    else if (!dt.date_available)
        return time_val;

    int days_in_months[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // CONVERT SHORTHAND YEAR TO FULL YEAR
    if (dt.year_i > 0 && dt.year_i < 30)
        dt.year_i += 2000;
    else if (dt.year_i >= 30 && dt.year_i < 100)
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
    double serial = (dt.year_i - 1900) * 365 + dt.day_i + count +
                    time_val;  // timeval was added too

    // ADD MONTHS UP TO CURRENT MONTH
    for (int a = 1; a < dt.month_i; a++) serial += days_in_months[a];

    return serial > 59 ? serial + 1 : serial;
}

// final value setters
inline void set_datetime_val() {
    final_value = datetime_val();
    if (final_value == -1) final_value = std::nullopt;
}

auto set_percent_val = [](auto& ctx) {
    final_value = final_value.value() * 0.01;
};

inline void flip_final() {
    if (final_value.value() < 0) {
        final_value = std::nullopt;
        return;
    }
    final_value = final_value.value() * -1;
}

// Month map
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

// Rules
x3::rule<class sep> const sep;
x3::rule<class month> const month = "month";
x3::rule<class period> const period = "period";
x3::rule<class parse_time> const parse_time = "parse_time";
x3::rule<class date> const date = "date";
x3::rule<class datetime> const datetime = "datetime";

// Integers, Exponents, Decimals, Percentage, Currency
x3::rule<class currency_symbol> const currency_symbol = "currency_symbol";
x3::rule<class percentage> const percentage = "percentage";
x3::rule<class number> const number = "number";
x3::rule<class value_sans_datetime> const value_sans_datetime =
    "value_sans_datetime";
x3::rule<class value> const value = "value";

// Rules Definitions
const auto sep_def = x3::lit('/') | x3::lit('-');

const auto period_def = ((x3::char_('A') | x3::char_('a') | x3::char_('P') |
                          x3::char_('p'))[([](auto& ctx) {
                             char c = boost::get<char>(_attr(ctx));
                             dt.period = (c == 'p' || c == 'P') ? true : false;
                         })] >>
                         (x3::char_('M') | x3::char_('m')));

const auto month_def = (+x3::char_("A-Za-z"))[([](auto& ctx) {
    std::string s(_attr(ctx));
    auto it = months.find(toLower(s));
    if (it != months.end()) {
        dt.month_i = it->second; /*set the month variable to the correct index*/
    } else {
        dt.month_i = -1;  // error x3::_pass(ctx) = false;
    }
})];

const auto parse_time_def =
    ((x3::double_[set_hour] >> x3::lit(':') >> x3::double_[set_min] >>
      x3::lit(':') >> x3::double_[set_sec] >> -(+x3::lit(' ') >> period)) |
     (x3::double_[set_hour] >> x3::lit(':') >> x3::double_[set_min] >>
      -(+x3::lit(' ') >> period)) |
     (x3::double_[set_hour] >> +x3::lit(' ') >> period));

const auto date_def =
    ((month >> +x3::lit(' ') >> x3::int_[set_day] >> *x3::lit(' ') >>
      -x3::lit(',') >> +x3::lit(' ') >> x3::int_[set_year]) |
     (month >> +x3::lit(' ') >> x3::int_[set_day])[set_year_or_day] |
     (x3::int_[set_year] >> sep >> x3::int_[set_month] >> sep >>
      x3::int_[set_day])[([]() {
         // Assume ymd if year >=1900, MDY otherwise
         int swapy{0}, swapm(0);
         if (dt.year_i < 31 || dt.year_i < 1899) {
             // MDY
             swapy = dt.year_i;
             swapm = dt.month_i;
             dt.month_i = swapy;  // M becomes Y
             swapy = dt.day_i;
             dt.year_i = dt.day_i;  // Y become d
             dt.day_i = swapm;      // D becomes M
             // MDY}
         }
     })] |
     (x3::int_[set_day] >> sep >> month >> sep >> x3::int_[set_year]) |
     (x3::int_[set_day] >> +x3::lit(' ') >> month >> +x3::lit(' ') >>
      x3::int_[set_year]) |
     (x3::int_[set_month] >> sep >> x3::int_[set_day])[set_year_or_day] |
     (x3::int_[set_day] >> +x3::lit(' ') >> month)[set_year_or_day] |
     (x3::int_[set_day] >> sep >> month)[set_year_or_day]);

const auto datetime_def =
    (x3::eps[reset_dt] >> x3::lexeme[parse_time] >>
     x3::no_skip[+x3::lit(' ')] >> x3::lexeme[date])[set_date_available] |
    (x3::eps[reset_dt] >> x3::lexeme[date] >>
     (x3::no_skip[+x3::lit(' ')] >>
      x3::lexeme[parse_time]))[set_date_available] |
    (x3::eps[reset_dt] >> x3::lexeme[parse_time] >> x3::lit(',') >>
     (x3::no_skip[+x3::lit(' ')] >> x3::lexeme[date]))[set_date_available] |
    (x3::eps[reset_dt] >> x3::lexeme[date] >> x3::lit(',') >>
     (x3::no_skip[+x3::lit(' ')] >>
      x3::lexeme[parse_time]))[set_date_available] |
    (x3::eps[reset_dt] >> x3::lexeme[parse_time]) |
    (x3::eps[reset_dt] >> x3::lexeme[date])[set_date_available];

// Visitor for Comma separated Numbers
/*
 *
 */
inline struct number_retrieve
    : public boost::static_visitor<>  // Visitor used in the boost variant
{
    std::optional<double> state;

    void operator()(const double& val) { state = val; }

    void operator()(const int& val) { state = val; }

    void operator()(const boost::fusion::deque<
                    boost::optional<boost::variant<char, char>>, std::string,
                    std::vector<std::string>, boost::optional<std::string>,
                    boost::optional<int>>& val) {
        std::string s;

        // Add optional leading minus
        if (at_c<0>(val).has_value() &&
            boost::get<char>(at_c<0>(val).value()) == '-')
            s += "-";

        // Add required part
        s += at_c<1>(val);

        // Add optional groups
        for (const auto& it : at_c<2>(val)) s += it;

        // Add optional fraction
        if (at_c<3>(val).has_value()) {
            const auto& fractionS = at_c<3>(val).value();
            if (!fractionS.empty()) s += "." + fractionS;
        }

        // Add optional exponential part
        if (at_c<4>(val).has_value()) {
            const auto& exp = at_c<4>(val).value();
            s += "e" + std::to_string(exp);
        }

        state = std::stod(s);  // to decimal
    }
} num_retrieve;

const auto number_def =
    ((-(x3::char_('-') | x3::char_('+')) >> +x3::digit >>
      *(x3::lit(',') >> x3::repeat(3, x3::inf)[x3::digit]) >>
      -(x3::lit('.') >> x3::no_skip[*(x3::digit)]) >>
      -((x3::lit('e') | x3::lit('E')) >> x3::int_)) |
     x3::double_ | x3::int_)[([](auto& ctx) {
        boost::apply_visitor(num_retrieve, _attr(ctx));
        final_value = num_retrieve.state;
    })];

const auto currency_symbol_def =
    x3::lit('$');  // | x3::standard_wide::lit(L'€');

const auto percentage_def = x3::lit('%');

const auto value_sans_datetime_def =
    x3::lit('(') >>
        (currency_symbol >> number)[set_percent_val] >> x3::lit(
                                                            ')')[flip_final] |
    x3::lit('(') >>
        (number >> percentage)[set_percent_val] >> x3::lit(')')[flip_final] |
    x3::lit('(') >> number >> x3::lit(')')[flip_final] |
    (number >> percentage)[set_percent_val] |
    (currency_symbol >> number) | number;

const auto value_def = datetime[set_datetime_val] | value_sans_datetime;

// Definitions
BOOST_SPIRIT_DEFINE(value, value_sans_datetime, datetime, parse_time, date,
                    month, period, sep, currency_symbol, percentage, number);

/************************
 * For Debug Logging
 *
 */

void print_dt(const datetime_vals& dt) {
    std::cout << "\n**********************************\nThe elements of "
                 "datetime struct are:\n";
    std::cout << "Year:   " << dt.year_i << "\n";
    std::cout << "Month:  " << dt.month_i << "\n";
    std::cout << "Day:    " << dt.day_i << "\n";
    std::cout << "Hour:   " << dt.hour << "\n";
    std::cout << "Min:    " << dt.min << "\n";
    std::cout << "Sec:    " << dt.sec << "\n";
    std::cout << "Period: ";
    if (dt.period.has_value())
        std::cout << (dt.period.value() ? "PM" : "AM") << "\n";
    else
        std::cout << "Not supplied\n";
}

inline std::optional<double> value_parse(std::string_view value_string) {
    // std::nullopt is an error
    double val{0.0};

    auto it = value_string.begin();

    if (x3::phrase_parse(it, value_string.end(), value, x3::ascii::space) &&
        it == value_string.end()) {
        if (!final_value.has_value()) {
            return std::nullopt;  // propagate
        }
        return final_value.value();
    } else
        return std::nullopt;
}

int main() {
    // Data
    // datetime_vals dt;

    std::cout << "*********Value Parser (Boost Spirit) **********\n";
    std::cout << "Input a string to be parsed:\n";

    std::string ss;

    while (std::getline(std::cin, ss)) {
        std::optional<double> res = value_parse(ss);  // s
        if (!res.has_value()) {
            std::cout << "Unsuccessful parse\n";
        } else {
            std::cout << "Successful parse: " << res.value() << std::endl;
        }
        std::cout << "\nInput another string to be parsed:\n";

        /*
        To debug DateTime rule
        auto it = s.begin();

        double time_val{0.0};

        if(x3::phrase_parse(it, s.end(), datetime, x3::ascii::space) && it ==
        s.end())
        {
          time_val = timevalue_val();

          if(dt.date_available && !isValidDate(dt))
          {
            std::cout << "Date Available but wrongly formatted or wrong!!!\n";
            print_dt(dt);
          }
          if(time_val == -1) {
              std::cout << "Wrong time, error!!!\n";
              print_dt(dt);
          }
          else {
              std::cout << "Parse succesful, correct timevalue: " << time_val <<
        "\n"; print_dt(dt);
          }
        }
        else {
          std::string rest(it, s.end());
          std::cout << "NO Match!(Check the format of the String), Error!!!!\n";
        print_dt(dt); std::cout << "Parsing stopped at:\n"; std::cout << rest <<
        "\n";
        }

        */
    }
}
