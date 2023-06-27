#ifndef ADA_MIMESNIFF_MIMETYPE_H
#define ADA_MIMESNIFF_MIMETYPE_H

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

#include "ada/mimesniff/util-inl.h"
#include "ada/mimesniff/util.h"

namespace ada::mimesniff {

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
  std::vector<std::pair<std::string, std::string>> parameters{};

  // The essence of a MIME type mimeType is mimeType’s type, followed by U+002F
  // (/), followed by mimeType’s subtype.
  std::string essence() const noexcept { return type + "/" + subtype; }

  /**
   * @see https://mimesniff.spec.whatwg.org/#serializing-a-mime-type
   */
  std::string serialized() const noexcept {
    std::string base = essence();

    for (const auto &i : parameters) {
      base += ';';
      base += i.first + "=";
      if (i.second.empty() || !contains_only_http_tokens(i.second)) {
        // Precede each occurrence of U+0022 (") or U+005C (\) in value with
        // U+005C (\).
        base += '\"';
        for (char c : i.second) {
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
        base += i.second;
      }
    }
    return base;
  }
};

}  // namespace ada::mimesniff

#endif  // ADA_MIMESNIFF_MIMETYPE_H
