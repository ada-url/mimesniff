#ifndef ADA_MIMESNIFF_PARSER_H
#define ADA_MIMESNIFF_PARSER_H
#include <string_view>

namespace ada::mimesniff {
void parse_mime_type(std::string_view input);
}
#endif