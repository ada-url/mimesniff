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

TEST(wpt_tests, mime_types) {
  ondemand::parser parser;
  size_t counter{0};

  ASSERT_TRUE(file_exists(MIME_TYPES_JSON));
  padded_string json = padded_string::load(MIME_TYPES_JSON);
  ondemand::document doc = parser.iterate(json);
  try {
    for (auto element : doc.get_array()) {
      if (element.type() == ondemand::json_type::string) {
        std::cout << "   section: " << element.get_string() << std::endl;
      } else if (element.type() == ondemand::json_type::object) {
        ondemand::object object = element.get_object();
        auto element_string = std::string(std::string_view(object.raw_json()));
        object.reset();

        // We might want to decode the strings into UTF-8, but some of the
        // strings are not always valid UTF-8 (e.g., you have unmatched
        // surrogates which are forbidden by the UTF-8 spec).
        auto input_element = object["input"];
        auto output_element = object["output"];

        counter++;
      }
    }
  } catch (simdjson::simdjson_error &error) {
    std::cerr << "JSON error: " << error.what() << " near "
              << doc.current_location() << " in " << MIME_TYPES_JSON
              << std::endl;
    FAIL();
  }
  std::cout << "Tests executed = " << counter << std::endl;
  SUCCEED();
}
