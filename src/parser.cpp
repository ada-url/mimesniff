#include <string>
#include <optional>

#include "ada/mimesniff/util-inl.h"
#include "ada/mimesniff/util.h"
#include "ada/mimesniff/mimetype.h"

namespace ada::mimesniff {

std::optional<mimetype> parse_mime_type(std::string_view input) {
  // 1. Remove any leading and trailing HTTP whitespace from input.
  trim_http_whitespace(input);

  // 2. Let position be a position variable for input, initially
  //    pointing at the start of input.
  size_t position = input.find_first_of('/');

  // 5. If position is past the end of input, then return failure.
  // Also implements part of step 4.
  if (position == std::string_view::npos || position == 0) {
    return std::nullopt;
  }

  // 3. Let type be the result of collecting a sequence of code
  //    points that are not U+002F (/) from input, given position.
  std::string_view type = input.substr(0, position);

  // 4. If type is the empty string or does not solely contain
  //    HTTP token code points, then return failure.
  if (!contains_only_http_tokens(type)) {
    return std::nullopt;
  }

  // 6. Advance position by 1. (This skips past U+002F (/).)
  position++;

  size_t subtype_end_position = input.find_first_of(';');

  if (subtype_end_position == std::string_view::npos) {
    // A mimetype may not have parameters. EG. "text/plain"
    subtype_end_position = input.length();
  } else if (subtype_end_position == 0) {
    // If the string doesn't have a subtype, early return. This
    // is part of step #9. EG. "text/"
    return std::nullopt;
  }

  // 7. Let subtype be the result of collecting a sequence of code
  //    points that are not U+003B (;) from input, given position.
  std::string_view subtype = input.substr(position, subtype_end_position);

  // 8. Remove any trailing HTTP whitespace from subtype.
  trim_trailing_http_whitespace(subtype);

  // 9. If subtype is the empty string or does not solely contain
  //    HTTP token code points, then return failure.
  if (subtype.empty() || !contains_only_http_tokens(subtype)) {
    return std::nullopt;
  }

  // 10. Let mimeType be a new MIME type record whose type is type,
  //     in ASCII lowercase, and subtype is subtype, in ASCII lowercase.
  auto out = mimetype();
  out.type = type;        // TODO: Convert this to ASCII lowercase
  out.subtype = subtype;  // TODO: Convert this to ASCII lowercase

  return out;
}

}  // namespace ada::mimesniff
