#ifndef ADA_MIMESNIFF_MIMETYPE_H
#define ADA_MIMESNIFF_MIMETYPE_H

#include <vector>
#include <string>
#include <string_view>
#include <algorithm>

#include "ada/mimesniff/util-inl.h"
#include "ada/mimesniff/util.h"

namespace ada::mimesniff {

struct key_value final {
  key_value() = default;
  key_value(std::string_view key, std::string_view value) noexcept
      : stored_key{key}, stored_value{value} {}
  const std::string_view key() const noexcept { return stored_key; }
  const std::string_view value() const noexcept { return stored_value; }

 private:
  std::string stored_key{};
  std::string stored_value{};
};

struct parameter_list final {
  parameter_list() = default;

  typedef std::vector<key_value>::iterator iterator;
  typedef std::vector<key_value>::const_iterator const_iterator;
  iterator begin() noexcept { return stored_parameters.begin(); }
  iterator end() noexcept { return stored_parameters.end(); }
  const_iterator begin() const noexcept { return stored_parameters.begin(); }
  const_iterator end() const noexcept { return stored_parameters.end(); }
  const_iterator cbegin() const noexcept { return stored_parameters.cbegin(); }
  const_iterator cend() const noexcept { return stored_parameters.cend(); }
  bool empty() const noexcept { return stored_parameters.empty(); }
  bool contains_key(std::string_view key) const noexcept {
    return std::find_if(stored_parameters.begin(), stored_parameters.end(),
                        [&key](const key_value &kv) {
                          return kv.key() == key;
                        }) != stored_parameters.end();
  }
  void insert(key_value &&kv) noexcept { stored_parameters.emplace_back(kv); }
  void insert(const key_value &kv) noexcept {
    stored_parameters.emplace_back(kv);
  }

 private:
  std::vector<key_value> stored_parameters{};
};

struct mimetype {
  mimetype() = default;
  mimetype(const mimetype &m) = default;
  mimetype(mimetype &&m) noexcept = default;
  mimetype &operator=(mimetype &&m) noexcept = default;
  mimetype &operator=(const mimetype &m) = default;
  ~mimetype() = default;

  // A MIME type’s type is a non-empty ASCII string.
  std::string type{};

  // A MIME type’s subtype is a non-empty ASCII string.
  std::string subtype{};

  // A MIME type’s parameters is an ordered map whose keys are ASCII
  // strings and values are strings limited to HTTP quoted-string token code
  // points. It is initially empty.
  parameter_list parameters{};

  // The essence of a MIME type mimeType is mimeType’s type, followed by U+002F
  // (/), followed by mimeType’s subtype.
  std::string essence() const noexcept { return type + "/" + subtype; }

  /**
   * @see https://mimesniff.spec.whatwg.org/#serializing-a-mime-type
   */
  std::string serialized() const noexcept {
    std::string base = essence();

    for (const key_value &i : parameters) {
      base += ';';
      base += std::string(i.key()) + "=";
      if (i.value().empty() || !contains_only_http_tokens(i.value())) {
        // Precede each occurrence of U+0022 (") or U+005C (\) in value with
        // U+005C (\).
        base += '\"';
        for (char c : i.value()) {
          switch (c) {
            case '"':
            case '\\':
              base += '\\';
              [[fallthrough]];
            default:
              base += c;
          }
        }
        base += '\"';
      } else {
        base += std::string(i.value());
      }
    }
    return base;
  }
};

}  // namespace ada::mimesniff

#endif  // ADA_MIMESNIFF_MIMETYPE_H
