#ifndef ADA_MIMESNIFF_UTIL_INL_H
#define ADA_MIMESNIFF_UTIL_INL_H
#include <algorithm>
#include <string_view>

#include "ada/mimesniff/util.h"
namespace ada::mimesniff {

// https://fetch.spec.whatwg.org/#http-whitespace
constexpr inline bool is_http_whitespace(const char c) {
  return c == '\r' || c == '\n' || c == '\t' || c == ' ';
}

constexpr inline void trim_http_whitespace(std::string_view& input) {
  while (!input.empty() && is_http_whitespace(input.front())) {
    input.remove_prefix(1);
  }
  while (!input.empty() && is_http_whitespace(input.back())) {
    input.remove_suffix(1);
  }
}

// alphanum = 1, symbols = 2, other = 0
constexpr static uint8_t http_tokens[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 2, 2, 2, 2, 0, 0, 2, 2, 0, 2, 2, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 2, 2,
    2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

constexpr static inline bool is_http_token(const char c) {
  return http_tokens[uint8_t(c)];
}

constexpr inline bool contains_only_http_tokens(std::string_view view) {
  // An HTTP token code point is U+0021 (!), U+0023 (#), U+0024 ($),
  // U+0025 (%), U+0026 (&), U+0027 ('), U+002A (*), U+002B (+), U+002D (-),
  // U+002E (.), U+005E (^), U+005F (_), U+0060 (`), U+007C (|), U+007E (~), or
  // an ASCII alphanumeric.
  return (std::all_of(view.begin(), view.end(), is_http_token));
}
}  // namespace ada::mimesniff
#endif