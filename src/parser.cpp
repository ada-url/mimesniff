#include <string>
#include <optional>

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
  std::string_view type = input.substr(0, type_end_position);

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

  // 7. Let subtype be the result of collecting a sequence of code
  //    points that are not U+003B (;) from input, given position.
  std::string_view subtype = input.substr(0, subtype_end_position);

  // 8. Remove any trailing HTTP whitespace from subtype.
  trim_trailing_http_whitespace(subtype);

  // 9. If subtype is the empty string or does not solely contain
  //    HTTP token code points, then return failure.
  if (subtype.empty() || !contains_only_http_tokens(subtype)) {
    return std::nullopt;
  }

  // Remove subtype from input
  input.remove_prefix(subtype_end_position);

  // Let mimeType be a new MIME type record whose type is type, in ASCII
  // lowercase, and subtype is subtype, in ASCII lowercase.
  auto out = mimetype();
  out.type = type;        // TODO: Convert this to ASCII lowercase
  out.subtype = subtype;  // TODO: Convert this to ASCII lowercase

  size_t position{0};

  // 11. While position is not past the end of input:
  while (position < input.size()) {
    // Advance position by 1. (This skips past U+003B (;).)
    position++;

    // Collect a sequence of code points that are HTTP whitespace from input
    // given position.
    while (is_http_whitespace(input[position])) {
      position++;
    }

    // Let parameterName be the result of collecting a sequence of code points
    // that are not U+003B (;) or U+003D (=) from input, given position.
    size_t parameter_name_ending = input.find_first_of(";=", position);
    std::string_view parameter_name =
        input.substr(position, parameter_name_ending);
    position += parameter_name_ending;
    // TODO: Set parameterName to parameterName, in ASCII lowercase.

    // If position is not past the end of input, then:
    if (position == std::string_view::npos) {
      // If the code point at position within input is U+003B (;), then
      // continue.
      if (input[position] == ';') continue;

      // Advance position by 1. (This skips past U+003D (=).)
      position++;
    }

    // If position is past the end of input, then break.
    if (position >= input.size()) {
      break;
    }

    // Let parameterValue be null.
    std::string_view parameter_value{};

    // If the code point at position within input is U+0022 ("), then:
    if (input[position] == '"') {
      // Set parameterValue to the result of collecting an HTTP quoted string
      // from input, given position and true.
    }
  }

  return out;
}

}  // namespace ada::mimesniff
