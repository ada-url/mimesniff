#include <map>
#include <optional>
#include <string>

#include "ada/mimesniff/util-inl.h"
#include "ada/mimesniff/util.h"
#include "ada/mimesniff/mimetype.h"

namespace ada::mimesniff {

std::optional<mimetype> parse_mime_type(std::string_view input) {
  // Remove any leading and trailing HTTP whitespace from input.
  trim_http_whitespace(input);

  auto type_end_position = input.find_first_of('/');
  if (type_end_position == std::string_view::npos) {
    return std::nullopt;
  }

  // Let type be the result of collecting a sequence of code points that are not
  // U+002F (/) from input, given position.
  // Optimization opportunity: Copy is only required if type is not
  // lowercased.
  std::string type = std::string(input.substr(0, type_end_position));

  // If type is the empty string or does not solely contain HTTP token code
  // points, then return failure.
  if (type.empty() || !contains_only_http_tokens(type)) {
    return std::nullopt;
  }

  // Remove type from input. (This skips past U+002F (/).)
  input.remove_prefix(type_end_position + 1);

  if (input.empty()) {
    // If the string doesn't have a subtype, early return. This
    // is part of step #9. EG. "text/"
    return std::nullopt;
  }

  auto subtype_end_position = input.find_first_of(';');

  if (subtype_end_position == std::string_view::npos) {
    // A mimetype may not have parameters. EG. "text/plain"
    subtype_end_position = input.size();
  }

  // Let subtype be the result of collecting a sequence of code
  // points that are not U+003B (;) from input, given position.
  std::string_view _subtype = input.substr(0, subtype_end_position);

  // Remove any trailing HTTP whitespace from subtype.
  trim_trailing_http_whitespace(_subtype);

  // Optimization opportunity: Copy is only required if subtype is not
  // lowercased.
  std::string subtype = std::string(_subtype);

  // If subtype is the empty string or does not solely contain
  // HTTP token code points, then return failure.
  if (subtype.empty() || !contains_only_http_tokens(subtype)) {
    return std::nullopt;
  }

  // Remove subtype from input
  input.remove_prefix(subtype_end_position);

  // Let mimeType be a new MIME type record whose type is type, in ASCII
  // lowercase, and subtype is subtype, in ASCII lowercase.
  auto out = mimetype();

  to_lower_ascii(type.data(), type.size());
  to_lower_ascii(subtype.data(), subtype.size());

  out.type = type;
  out.subtype = subtype;

  size_t position{0};

  // While position is not past the end of input:
  while (position < input.size()) {
    // Advance position by 1. (This skips past U+003B (;).)
    position++;

    // Collect a sequence of code points that are HTTP whitespace from input
    // given position.
    while (is_http_whitespace(input[position]) && position < input.size()) {
      position++;
    }

    // Let parameterName be the result of collecting a sequence of code points
    // that are not U+003B (;) or U+003D (=) from input, given position.
    auto parameter_name_ending = input.find_first_of(";=", position);

    std::string parameter_name{};

    if (parameter_name_ending == std::string_view::npos) {
      // If position is past the end of input, then break.
      break;
    } else {
      // Optimization opportunity: Copy is only required if parameter_name is
      // not lowercased.
      parameter_name =
          std::string(input.substr(position, parameter_name_ending - position));
      to_lower_ascii(parameter_name.data(), parameter_name.size());
      position = parameter_name_ending;

      // If the code point at position within input is U+003B (;), then
      // continue.
      if (input[position] == ';') continue;

      // Advance position by 1. (This skips past U+003D (=).)
      position++;
    }

    // Let parameterValue be null.
    std::string parameter_value{};

    // If the code point at position within input is U+0022 ("), then:
    if (input[position] == '"') {
      // Set parameterValue to the result of collecting an HTTP quoted string
      // from input.
      parameter_value = collect_http_quoted_string(input, position);
      // Collect a sequence of code points that are not U+003B (;) from input,
      // given position.
      auto semicolon_index = input.find_first_of(';', position);

      position = semicolon_index == std::string_view::npos ? input.size()
                                                           : semicolon_index;
    } else {
      // Set parameterValue to the result of collecting a sequence of code
      // points that are not U+003B (;) from input, given position.
      auto semicolon_index = input.find_first_of(';', position);

      if (semicolon_index == std::string_view::npos) {
        semicolon_index = input.size();
      }

      std::string_view parameter_value_view =
          input.substr(position, semicolon_index - position);

      // Remove any trailing HTTP whitespace from parameterValue.
      trim_trailing_http_whitespace(parameter_value_view);

      parameter_value = std::string(parameter_value_view);

      // If parameterValue is the empty string, then continue.
      if (parameter_value.empty()) {
        continue;
      }
    }

    // If all of the following are true
    // - parameterName is not the empty string
    // - parameterName solely contains HTTP token code points
    // - parameterValue solely contains HTTP quoted-string token code points
    // - mimeType’s parameters[parameterName] does not exist
    if (!parameter_name.empty() && contains_only_http_tokens(parameter_name) &&
        contains_only_http_quoted_string_tokens(
            std::string_view(parameter_value.data(), parameter_value.size())) &&
        !out.parameters.contains_key(parameter_name)) {
      // then set mimeType’s parameters[parameterName] to parameterValue.
      out.parameters.insert({parameter_name, parameter_value});
    }
  }

  // Return mimeType.
  return out;
}

}  // namespace ada::mimesniff
