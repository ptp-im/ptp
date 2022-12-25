/*!
 * toolbox.
 * decimal_formatter.cpp
 *
 * \date 11/30/2019
 * \author Eduard Maximovich (edward.vstock@gmail.com)
 * \link   https://github.com/edwardstock
 */
#include "ptp_toolbox/strings/decimal_formatter.h"

#include "ptp_toolbox/strings.hpp"

#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>
ptp_toolbox::strings::decimal_formatter::decimal_formatter(int num) {
    std::stringstream ss;
    ss << num;
    m_num = ss.str();
}
ptp_toolbox::strings::decimal_formatter::decimal_formatter(double num) {
    std::stringstream ss;
    ss << num;
    m_num = ss.str();
}
ptp_toolbox::strings::decimal_formatter::decimal_formatter(float num) {
    std::stringstream ss;
    ss << num;
    m_num = ss.str();
}
ptp_toolbox::strings::decimal_formatter::decimal_formatter(std::string num)
    : m_num(std::move(num)) {
}
ptp_toolbox::strings::decimal_formatter::decimal_formatter(const char* num)
    : m_num(std::string(num)) {
}
std::string ptp_toolbox::strings::decimal_formatter::operator()(const std::string& num) {
    m_num = num;
    return format();
}
ptp_toolbox::strings::decimal_formatter& ptp_toolbox::strings::decimal_formatter::set_delimiter(const std::string& delimiter) {
    m_delimiter = delimiter;
    return *this;
}
ptp_toolbox::strings::decimal_formatter& ptp_toolbox::strings::decimal_formatter::set_delimiter(char delimiter) {
    std::stringstream ss;
    ss << delimiter;
    m_delimiter = ss.str();
    return *this;
}
ptp_toolbox::strings::decimal_formatter& ptp_toolbox::strings::decimal_formatter::set_decimals_group(size_t num) {
    m_decimals = num;
    return *this;
}
ptp_toolbox::strings::decimal_formatter& ptp_toolbox::strings::decimal_formatter::set_max_precision(size_t max_precision) {
    m_max_precision = max_precision;
    return *this;
}
ptp_toolbox::strings::decimal_formatter& ptp_toolbox::strings::decimal_formatter::set_min_precision(size_t min_precision) {
    m_min_precision = min_precision;
    return *this;
}
ptp_toolbox::strings::decimal_formatter& ptp_toolbox::strings::decimal_formatter::set_max_fractions(size_t max_fractions) {
    m_max_precision = max_fractions;
    return *this;
}
ptp_toolbox::strings::decimal_formatter& ptp_toolbox::strings::decimal_formatter::set_min_fractions(size_t min_fractions) {
    m_min_precision = min_fractions;
    return *this;
}

static size_t count_real_precision(const std::string& fraction) {
    size_t n = fraction.size();
    for (size_t i = fraction.size(); i > 0; i--) {
        const char& v = fraction[i - 1];
        if (v != '0') {
            break;
        } else {
            n--;
        }
    }
    return n;
}

std::string ptp_toolbox::strings::decimal_formatter::format() const {
    if (m_num.empty()) {
        throw std::runtime_error("Empty number passed to decimal formatter");
    }
    std::pair<std::string, std::string> lr = ptp_toolbox::strings::split_pair(m_num, '.');

    std::stringstream out;
    std::stack<std::string> parts;
    size_t fr_size_real = count_real_precision(lr.second);
    if (m_min_precision > 0) {
        // if fractial size less than min required fractions, add trailing zeroes
        if (fr_size_real < m_min_precision) {
            parts.push(ptp_toolbox::strings::repeat('0', m_min_precision - fr_size_real));
            parts.push(lr.second.substr(0, fr_size_real));
        } else if (fr_size_real >= m_min_precision) {
            if (fr_size_real > m_max_precision) {
                parts.push(lr.second.substr(0, m_max_precision));
            } else {
                parts.push(lr.second);
            }
        }

        parts.push(".");
    }

    size_t prev_i;
    for (size_t i = lr.first.size(); i > 0;) {
        prev_i = i;
        if (m_decimals >= i) {
            i = 0;
        } else {
            i -= m_decimals;
        }

        size_t len = (i == 0) ? prev_i : m_decimals;
        std::string part = lr.first.substr(i, len);

        parts.push(std::move(part));
        if (i != 0) {
            parts.push(std::string(m_delimiter));
        }
    }

    while (!parts.empty()) {
        out << parts.top();
        parts.pop();
    }

    return out.str();
}

std::ostream& operator<<(std::ostream& os, const ptp_toolbox::strings::decimal_formatter& formatter) {
    os << formatter.format();
    return os;
}
