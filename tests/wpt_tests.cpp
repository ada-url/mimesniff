#include <filesystem>
#include <iostream>
#include <set>

#include "gtest/gtest.h"
#include "mimesniff.h"

#include "simdjson.h"

using namespace simdjson;

#ifndef WPT_DATA_DIR
#define WPT_DATA_DIR "wpt/"
#endif
const char *GENERATED_MIME_TYPES_JSON =
    WPT_DATA_DIR "generated-mime-types.json";
const char *MIME_TYPES_JSON = WPT_DATA_DIR "mime-types.json";

bool file_exists(const char *filename) {
  namespace fs = std::filesystem;
  std::filesystem::path f{filename};
  return std::filesystem::exists(filename);
}

TEST(wpt_tests, generated_mime_types) {
  ondemand::parser parser;

  ASSERT_TRUE(file_exists(GENERATED_MIME_TYPES_JSON));
  padded_string json = padded_string::load(GENERATED_MIME_TYPES_JSON);
  ondemand::document doc = parser.iterate(json);
  try {
    for (auto element : doc.get_array()) {
      if (element.type() == ondemand::json_type::string) {
        std::cout << "   section: " << element.get_string() << std::endl;
      } else if (element.type() == ondemand::json_type::object) {
        ondemand::object object = element.get_object();
        std::string_view input = object["input"];
        std::string_view output{};
        bool has_null_output = object["output"].get(output);

        std::cout << "    input: " << input << std::endl;
        std::cout << "    output: " << output << std::endl;

        auto out = ada::mimesniff::parse_mime_type(input);

        ASSERT_EQ(out.has_value(), !has_null_output);

        if (!has_null_output) {
          ASSERT_EQ(out->serialized(), output);
        }
      }
    }
  } catch (simdjson::simdjson_error &error) {
    std::cerr << "JSON error: " << error.what() << " near "
              << doc.current_location() << " in " << MIME_TYPES_JSON
              << std::endl;
    FAIL();
  }
  SUCCEED();
}
