#include "ada/mimesniff/util.h"

namespace ada::mimesniff {

void trim_http_whitespace(std::string_view& input) noexcept {
  while (!input.empty() && is_http_whitespace(input.front())) {
    input.remove_prefix(1);
  }
  while (!input.empty() && is_http_whitespace(input.back())) {
    input.remove_suffix(1);
  }
}

// https://fetch.spec.whatwg.org/#http-whitespace
bool is_http_whitespace(const char c) noexcept {
  return c == '\r' || c == '\n' || c == '\t' || c == ' ';
}

}
