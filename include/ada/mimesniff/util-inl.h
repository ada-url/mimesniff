#ifndef ADA_MIMESNIFF_UTIL_INL_H
#define ADA_MIMESNIFF_UTIL_INL_H

#include <cstring>
#include <string>
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

constexpr inline void trim_trailing_http_whitespace(std::string_view& input) {
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
  for (const char c : view) {
    // if it is not a token, return false
    if (!is_http_token(c)) {
      return false;
    }
  }
  return true;
}

constexpr static inline bool is_http_quoted_string_token(const char c) {
  // An HTTP quoted-string token code point is U+0009 TAB, a code point in the
  // range U+0020 SPACE to U+007E (~), inclusive, or a code point in the range
  // U+0080 through U+00FF (ÿ), inclusive.
  return (c == '\t' || (c >= ' ' && c <= '~') || (c >= '\x80' && c <= '\xFF'));
}

constexpr inline bool contains_only_http_quoted_string_tokens(
    std::string_view view) {
  for (const char c : view) {
    if (!is_http_quoted_string_token(c)) {
      return false;
    }
  }
  return true;
}

inline std::string collect_http_quoted_string(std::string_view input,
                                              size_t& position) {
  std::string value{};

  // TODO: Assert: the code point at position within input is U+0022 (").

  position++;

  while (true) {
    // Append the result of collecting a sequence of code points that are not
    // U+0022 (") or U+005C (\) from input, given position, to value.
    auto end_index = input.find_first_of("\"\\", position);

    // If position is past the end of input, then break.
    if (end_index == std::string_view::npos) {
      break;
    }

    value.append(input.substr(position, end_index));
    position = end_index;

    // Let quoteOrBackslash be the code point at position within input.
    auto quote_or_backslash = input[position];

    // Advance position by 1.
    position++;

    // If quoteOrBackslash is U+005C (\), then:
    if (quote_or_backslash == '\\') {
      // If position is past the end of input, then append U+005C (\) to value
      // and break.
      if (position >= input.size()) {
        value += "\\";
        break;
      }

      // Append the code point at position within input to value.
      value += input[position];

      // Advance position by 1.
      position++;
    } else {
      // TODO: Assert: quoteOrBackslash is U+0022 (").
      break;
    }
  }

  return value;
}

constexpr bool to_lower_ascii(char* input, size_t length) noexcept {
  auto broadcast = [](uint8_t v) -> uint64_t {
    return 0x101010101010101ull * v;
  };
  uint64_t broadcast_80 = broadcast(0x80);
  uint64_t broadcast_Ap = broadcast(128 - 'A');
  uint64_t broadcast_Zp = broadcast(128 - 'Z' - 1);
  uint64_t non_ascii = 0;
  size_t i = 0;

  for (; i + 7 < length; i += 8) {
    uint64_t word{};
    memcpy(&word, input + i, sizeof(word));
    non_ascii |= (word & broadcast_80);
    word ^=
        (((word + broadcast_Ap) ^ (word + broadcast_Zp)) & broadcast_80) >> 2;
    memcpy(input + i, &word, sizeof(word));
  }
  if (i < length) {
    uint64_t word{};
    memcpy(&word, input + i, length - i);
    non_ascii |= (word & broadcast_80);
    word ^=
        (((word + broadcast_Ap) ^ (word + broadcast_Zp)) & broadcast_80) >> 2;
    memcpy(input + i, &word, length - i);
  }
  return non_ascii == 0;
}

}  // namespace ada::mimesniff
#endif