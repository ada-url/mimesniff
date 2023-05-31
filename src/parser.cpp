#include "util.cpp"

#include <string>

namespace ada::mimesniff {
void parse_mime_type (std::string_view input) {
  // step 1
  trim_http_whitespace(input);

  // step 2
  int position = input.find_first_of('/');

  // step 5
  if (position == std::string_view::npos || position == input.length()) {
    return;
  }

  // step 3
  std::string_view type = input.substr(0, position);

  // step 4
  if (!contains_only_http_tokens(type)) {
    return;
  }

  // step 6
  position++;
}
}