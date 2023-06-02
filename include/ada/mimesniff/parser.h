#ifndef ADA_MIMESNIFF_PARSER_H
#define ADA_MIMESNIFF_PARSER_H
#include <string_view>
#include <optional>

#include "ada/mimesniff/mimetype.h"

namespace ada::mimesniff {

std::optional<mimetype> parse_mime_type(std::string_view input);

}
#endif