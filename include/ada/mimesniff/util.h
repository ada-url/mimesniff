namespace ada::mimesniff {
  bool is_http_whitespace(const char c);
  void trim_http_whitespace(std::string_view& input);
}
