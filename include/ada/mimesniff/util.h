#include <string>

namespace ada::mimesniff {
void trim_http_whitespace(std::string_view& input);

bool is_http_whitespace(const char c);

bool contains_only_http_tokens(std::string_view view);
}  // namespace ada::mimesniff
