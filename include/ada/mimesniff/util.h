#ifndef ADA_MIMESNIFF_UTIL_H
#define ADA_MIMESNIFF_UTIL_H

#include <string_view>

namespace ada::mimesniff {

constexpr inline void trim_http_whitespace(std::string_view& input);

constexpr inline void trim_trailing_http_whitespace(std::string_view& input);

constexpr inline bool is_http_whitespace(const char c);

constexpr inline bool contains_only_http_tokens(std::string_view view);

}  // namespace ada::mimesniff
#endif