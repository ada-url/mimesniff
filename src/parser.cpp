#include <iostream>
#include <map>
#include <optional>
#include <string>

#include "ada/mimesniff/util-inl.h"
#include "ada/mimesniff/util.h"
#include "ada/mimesniff/mimetype.h"

namespace ada::mimesniff {

std::optional<mimetype> parse_mime_type(std::string_view input) {
  // Let mimeType be a new MIME type record whose type is type, in ASCII
  // lowercase, and subtype is subtype, in ASCII lowercase.
  auto out = mimetype();

  // Remove any leading and trailing HTTP whitespace from input.
  trim_http_whitespace(input);

  auto type_end_position = input.find_first_of('/');
  if (type_end_position == std::string_view::npos) {
    return std::nullopt;
  }

  // Let type be the result of collecting a sequence of code points that are not
  // U+002F (/) from input, given position.
  std::string_view type = input.substr(0, type_end_position);
  uint8_t type_map = http_tokens_map(type);
  // If type is the empty string or does not solely contain HTTP token code
  // points, then return failure.
  if (type.empty() || (type_map & 128)) {
    return std::nullopt;
  }

  out.type = type;

  if (type_map & 4) {  // containers uppercase letters
    to_lower_ascii(out.type.data(), out.type.size());
  }

  // Remove type from input. (This skips past U+002F (/).)
  input.remove_prefix(type_end_position + 1);

  // Optimization: No need to check for string_view::npos subtype_end_position
  // is used as a second parameter to `std::string_view::substr`.
  auto subtype_end_position = input.find_first_of(';');

  // Let subtype be the result of collecting a sequence of code
  // points that are not U+003B (;) from input, given position.
  std::string_view subtype = input.substr(0, subtype_end_position);

  // Remove any trailing HTTP whitespace from subtype.
  trim_trailing_http_whitespace(subtype);

  // Optimization opportunity: Copy is only required if subtype is not
  // lowercased.
  uint8_t subtype_map = http_tokens_map(subtype);

  // If subtype is the empty string or does not solely contain
  // HTTP token code points, then return failure.
  if (subtype.empty() || (subtype_map & 128)) {
    return std::nullopt;
  }
  out.subtype = subtype;
  if (subtype_map & 4) {  // containers uppercase letters
    to_lower_ascii(out.subtype.data(), out.subtype.size());
  }

  // Remove subtype from input
  input.remove_prefix(subtype_end_position);

  // Reserve memory
  out.parameters.reserve(2);

  // While position is not past the end of input:
  while (!input.empty()) {
    // Advance position by 1. (This skips past U+003B (;).)
    input.remove_prefix(1);

    // Collect a sequence of code points that are HTTP whitespace from input
    // given position.
    while (!input.empty() && is_http_whitespace(input[0])) {
      input.remove_prefix(1);
    }

    // Let parameterName be the result of collecting a sequence of code points
    // that are not U+003B (;) or U+003D (=) from input, given position.
    auto parameter_name_ending = input.find_first_of(";=");

    if (parameter_name_ending == std::string_view::npos) {
      // Parameter name needs to end with either `;` or `=`
      // If position is past the end of input, then break.
      break;
    }

    // Optimization opportunity: Copy is only required if parameter_name is
    // not lowercased.
    std::string parameter_name =
        std::string(input.substr(0, parameter_name_ending));
    to_lower_ascii_short(parameter_name.data(), parameter_name.size());
    input.remove_prefix(parameter_name_ending);

    // If the code point at position within input is U+003B (;), then
    // continue.
    if (!input.empty() && input[0] == ';') continue;

    // Advance position by 1. (This skips past U+003D (=).)
    input.remove_prefix(1);

    // Let parameterValue be null.
    std::string parameter_value;

    // If the code point at position within input is U+0022 ("), then:
    if (input[0] == '"') {
      // Set parameterValue to the result of collecting an HTTP quoted string
      // from input.
      parameter_value = collect_http_quoted_string(input);
      // Collect a sequence of code points that are not U+003B (;) from input,
      // given position.
      input.remove_prefix(input.find_first_of(';'));
    } else {
      // Set parameterValue to the result of collecting a sequence of code
      // points that are not U+003B (;) from input, given position.
      auto semicolon_index = input.find_first_of(';');

      std::string_view parameter_value_view = input.substr(0, semicolon_index);

      // Remove any trailing HTTP whitespace from parameterValue.
      trim_trailing_http_whitespace(parameter_value_view);

      // If parameterValue is the empty string, then continue.
      if (parameter_value_view.empty()) {
        continue;
      }

      parameter_value = std::string(parameter_value_view);
    }

    // If all of the following are true
    // - parameterName is not the empty string
    // - parameterName solely contains HTTP token code points
    // - parameterValue solely contains HTTP quoted-string token code points
    // - mimeType’s parameters[parameterName] does not exist
    if (!parameter_name.empty() && contains_only_http_tokens(parameter_name) &&
        contains_only_http_quoted_string_tokens(parameter_value) &&
        std::find_if(out.parameters.begin(), out.parameters.end(),
                     [&parameter_name](const auto& param) {
                       return param.first == parameter_name;
                     }) == out.parameters.end()) {
      // then set mimeType’s parameters[parameterName] to parameterValue.
      out.parameters.emplace_back(parameter_name, parameter_value);
    }
  }

  // Return mimeType.
  return out;
}

}  // namespace ada::mimesniff
