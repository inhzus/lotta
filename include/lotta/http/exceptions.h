//
// Created by suun on 11/27/19.
//

#ifndef LOTTA_EXCEPTIONS_H
#define LOTTA_EXCEPTIONS_H

#include <exception>
#include <string>

namespace lotta::http {

class HttpException : public std::exception {
 public:
  HttpException() noexcept = default;
  explicit HttpException(std::string msg) noexcept : msg_(std::move(msg)) {}
  [[nodiscard]] virtual int code() const noexcept = 0;
  [[nodiscard]] std::string msg() const noexcept { return msg_; }
 private:
  std::string msg_;
};

#define __LOTTA_HTTP_HTTP_EXCEPTION(name_, code_, what_) \
class name_##Exception : public HttpException { \
 public: \
  name_##Exception() noexcept = default; \
  explicit name_##Exception(std::string msg) noexcept : \
      HttpException(std::move(msg)) {} \
  [[nodiscard]] int code() const noexcept override { \
    return code_; \
  } \
  [[nodiscard]] const char *what() const noexcept override { \
    return what_; \
  } \
}

__LOTTA_HTTP_HTTP_EXCEPTION(BadRequest, 400, "Bad Request");
__LOTTA_HTTP_HTTP_EXCEPTION(NotFound, 404, "Not Found");

}

#endif //LOTTA_EXCEPTIONS_H
