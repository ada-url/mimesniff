#ifndef ADA_MIMESNIFF_UTIL_H
#define ADA_MIMESNIFF_UTIL_H

#include <string>
#include <string_view>

namespace ada::mimesniff {

constexpr inline void trim_http_whitespace(std::string_view& input);

constexpr inline void trim_trailing_http_whitespace(std::string_view& input);

constexpr inline bool is_http_whitespace(char c);

constexpr static inline bool is_http_token(char c);

/**
 * Assuming that the view is UTF-8 encoded, we return true if its characters are
 * U+0009 TAB, a code point in the range U+0020 SPACE to U+007E (~) or in the
 * range U+0080 through U+00FF (ÿ), inclusive. We return false otherwise.
 * @see https://mimesniff.spec.whatwg.org/#http-token-code-point
 */
constexpr inline bool contains_only_http_tokens(std::string_view view);

constexpr static inline bool is_http_quoted_string_token(char c);

/**
 * @see https://mimesniff.spec.whatwg.org/#http-quoted-string-token-code-point
 */
constexpr inline bool contains_only_http_quoted_string_tokens(
    std::string_view view);

inline std::string collect_http_quoted_string(std::string_view input,
                                              size_t& position);

/**
 * Lowers the string in-place, assuming that the content is ASCII.
 * Return true if the content was ASCII.
 */
constexpr bool to_lower_ascii(char* input, size_t length) noexcept;

}  // namespace ada::mimesniff
#endif
