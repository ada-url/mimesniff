#ifndef ADA_MIMESNIFF_PARSER_H
#define ADA_MIMESNIFF_PARSER_H
#include <string_view>
#include <optional>

#include "ada/mimesniff/mimetype.h"

namespace ada::mimesniff {

// The input is a string_view of the form "type/subtype; param1=value1;
// It is expected to be UTF-8 encoded, which includes ASCII.
std::optional<mimetype> parse_mime_type(std::string_view input);

}
#endif