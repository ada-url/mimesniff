#include <iostream>

#include "ada_mimesniff.cpp"
#include "ada_mimesniff.h"

using namespace std;

int main(int, char *[]) {
  ada::mimesniff::parse_mime_type("text/plain");

  return EXIT_SUCCESS;
}
