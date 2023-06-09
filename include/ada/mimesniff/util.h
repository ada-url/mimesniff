#ifndef ADA_MIMESNIFF_UTIL_H
#define ADA_MIMESNIFF_UTIL_H

#include <string>
#include <string_view>

namespace ada::mimesniff {

constexpr inline void trim_http_whitespace(std::string_view& input);

constexpr inline void trim_trailing_http_whitespace(std::string_view& input);

constexpr inline bool is_http_whitespace(char c);

/**
 * Assuming that the view is UTF-8 encoded, we return true if its characters are
 * U+0009 TAB, a code point in the range U+0020 SPACE to U+007E (~) or in the
 * range U+0080 through U+00FF (ÿ), inclusive. We return false otherwise.
 * @see https://mimesniff.spec.whatwg.org/#http-token-code-point
 */
constexpr inline bool contains_only_http_tokens(std::string_view view);
/**
 * Assuming that the view is UTF-8 encoded, we return a byte value with the most
 * significant bit set to 0 if its characters are
 * U+0009 TAB, a code point in the range U+0020 SPACE to U+007E (~) or in the
 * range U+0080 through U+00FF (ÿ), inclusive. The 3rd bit is set to 1 if an
 * uppercase ASCII letter was found.
 * @see https://mimesniff.spec.whatwg.org/#http-token-code-point
 */
constexpr inline uint8_t http_tokens_map(std::string_view view);

/**
 * @see https://mimesniff.spec.whatwg.org/#http-quoted-string-token-code-point
 */
constexpr inline bool contains_only_http_quoted_string_tokens(
    std::string_view view);

inline std::string collect_http_quoted_string(std::string_view& input);

/**
 * Lowers the string in-place, assuming that the content is ASCII.
 * Return true if the content was ASCII.
 */
constexpr bool to_lower_ascii(char* input, size_t length) noexcept;

/**
 * Lowers the ASCII letters in the string in-place. This function is optimized
 * for short inputs (<= 8 characters).
 */
constexpr void to_lower_ascii_short(char* input, size_t length) noexcept;
}  // namespace ada::mimesniff
#endif
