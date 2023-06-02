#ifndef ADA_MIMESNIFF_MIMETYPE_H
#define ADA_MIMESNIFF_MIMETYPE_H

#include <map>
#include <string_view>

namespace ada::mimesniff {

struct mimetype {
  mimetype() = default;
  mimetype(const mimetype &m) = default;
  mimetype(mimetype &&m) noexcept = default;
  mimetype &operator=(mimetype &&m) noexcept = default;
  mimetype &operator=(const mimetype &m) = default;
  ~mimetype() = default;

  // A MIME type’s type is a non-empty ASCII string.
  std::string_view type{};

  // A MIME type’s subtype is a non-empty ASCII string.
  std::string_view subtype{};

  // A MIME type’s parameters is an ordered map whose keys are ASCII
  // strings and values are strings limited to HTTP quoted-string token code
  // points. It is initially empty.
  std::map<std::string_view, std::string_view> parameters{};

  // The essence of a MIME type mimeType is mimeType’s type, followed by U+002F
  // (/), followed by mimeType’s subtype.
  std::string essence() const noexcept {
    return std::string(type) + "/" + std::string(subtype);
  }
};

}  // namespace ada::mimesniff

#endif  // ADA_MIMESNIFF_MIMETYPE_H
