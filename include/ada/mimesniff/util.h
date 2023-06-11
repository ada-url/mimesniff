#ifndef ADA_MIMESNIFF_UTIL_H
#define ADA_MIMESNIFF_UTIL_H

#include <string>
#include <string_view>

namespace ada::mimesniff {

constexpr inline void trim_http_whitespace(std::string_view& input);

constexpr inline void trim_trailing_http_whitespace(std::string_view& input);

constexpr inline bool is_http_whitespace(const char c);

constexpr static inline bool is_http_token(const char c);

constexpr inline bool contains_only_http_tokens(std::string_view view);

constexpr static inline bool is_http_quoted_string_token(const char c);

/**
 * @see https://mimesniff.spec.whatwg.org/#http-quoted-string-token-code-point
 */
constexpr inline bool contains_only_http_quoted_string_tokens(
    std::string_view view);

inline std::string collect_http_quoted_string(std::string_view input,
                                              size_t& position,
                                              bool extract_value = false);

}  // namespace ada::mimesniff
#endif