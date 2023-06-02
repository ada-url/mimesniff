#include <iostream>

#include "ada_mimesniff.h"
#include "ada_mimesniff.cpp"

using namespace std;

int main(int, char *[]) {
  ada::mimesniff::parse_mime_type("text/plain");

  return EXIT_SUCCESS;
}
