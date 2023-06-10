#include "mimesniff.h"
#include "gtest/gtest.h"
#include <cstdlib>
#include <iostream>

TEST(basic_tests, valid_type_and_subtype) {
  auto r = ada::mimesniff::parse_mime_type("text/plain");
  ASSERT_TRUE(r.has_value());
  ASSERT_EQ(r->type, "text");
  ASSERT_EQ(r->subtype, "plain");
  SUCCEED();
}
