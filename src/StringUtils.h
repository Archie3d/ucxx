#ifndef UCXX_STRINGUTILS_H
#define UCXX_STRINGUTILS_H

//
// Useful string utility functions
//

#include <string>
#include <sstream>

namespace ucxx {

/**
 * @brief Remove whitespace characters at the right.
 * If whitespace characters are not provided the default onces will be used,
 * which are " \t\n\r\f\v"
 * @param s String to be trimmed.
 * @param t C-style string containing whitespace characters.
 * @return Input string after the trimming.
 */
std::string& rtrim(std::string &s, const char *t = 0);

/**
 * @brief Remove whitespace characters at the left.
 * If whitespace characters are not provided the default onces will be used,
 * which are " \t\n\r\f\v"
 * @param s String to be trimmed.
 * @param t C-style string containing whitespace characters.
 * @return Input string after the trimming.
 */
std::string& ltrim(std::string &s, const char *t = 0);

/**
 * @brief Remove whitespace characters at both ends of the string.
 * If whitespace characters are not provided the default onces will be used,
 * which are " \t\n\r\f\v"
 * @param s String to be trimmed.
 * @param t C-style string containing whitespace characters.
 * @return Input string after the trimming.
 */
std::string& trim(std::string &s, const char *t = 0);

/**
 * @brief Convert a number to its string representation.
 * @param number Number to convert.
 * @return Textual representation.
 */
template <typename T>
std::string numberToString(T number)
{
    std::ostringstream ss;
    ss << number;
    return ss.str();
}

/**
 * @brief Parse a string into a number.
 * @param s String to be parsed.
 * @return Parsed number.
 */
template <typename T>
T stringToNumber(const std::string &s)
{
    std::istringstream ss(s);
    T res;
    ss >> res;
    return res;
}

} // namespace ucxx

#endif // UCXX_STRINGUTILS_H
