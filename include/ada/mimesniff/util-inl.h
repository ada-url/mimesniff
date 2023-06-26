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

// alphanum/lower = 1, alphaupper = 4, symbols = 2, other = 128
constexpr static uint8_t http_tokens_map_table[256] = {
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 2, 128, 2, 2, 2, 2, 2, 128, 128, 2, 2, 128, 2, 2, 128, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 128, 128, 128, 128, 128, 128, 128, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 128, 128, 128, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 128, 2, 128, 2, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128};


constexpr inline uint8_t http_tokens_map(std::string_view view) {
  // An HTTP token code point is U+0021 (!), U+0023 (#), U+0024 ($),
  // U+0025 (%), U+0026 (&), U+0027 ('), U+002A (*), U+002B (+), U+002D (-),
  // U+002E (.), U+005E (^), U+005F (_), U+0060 (`), U+007C (|), U+007E (~), or
  // an ASCII alphanumeric.
  uint8_t token = 0;
  for (const char c : view) {
    token |= http_tokens_map_table[uint8_t(c)];
  }
  return token;
}
constexpr inline bool contains_only_http_tokens(std::string_view view) {
  // An HTTP token code point is U+0021 (!), U+0023 (#), U+0024 ($),
  // U+0025 (%), U+0026 (&), U+0027 ('), U+002A (*), U+002B (+), U+002D (-),
  // U+002E (.), U+005E (^), U+005F (_), U+0060 (`), U+007C (|), U+007E (~), or
  // an ASCII alphanumeric.
  return !(http_tokens_map(view) & 128);
}

constexpr inline bool contains_only_http_quoted_string_tokens(
    std::string_view view) {
  for (size_t i = 0; i < view.size(); i++) {
    const uint8_t c(view[i]);
    // An HTTP quoted-string token code point is U+0009 TAB, a code point in the
    // range U+0020 SPACE to U+007E (~) and...
    if (c == '\t' || (c >= ' ' && c <= '~')) {
      continue;
    }
    // We need to check for the range U+0080 through U+00FF (ÿ), inclusive.
    // Doing so depends on the encoding.
    // Let us assume UTF-8.
    if (i + 1 == view.size()) {
      return false;
    }
    const uint8_t c2(view[i + 1]);
    i++;
    if ((c == 0xC2 && c2 >= 0x80 && c2 <= 0xBF) ||
        (c == 0xC3 && c2 >= 0x80 && c2 <= 0xBF)) {
      continue;
    }
    return false;
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
      value.append(input.substr(position));
      break;
    }

    value.append(input.substr(position, end_index - position));
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

constexpr void to_lower_ascii_short(char* input, size_t length) noexcept {
  for(size_t i = 0; i < length; i++) {
    char c = input[i] | 0x20;
    if((c >= 'a') && (c <= 'z')) { input[i] = c; }
  }
  return;
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
