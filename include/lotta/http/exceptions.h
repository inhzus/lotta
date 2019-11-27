//
// Created by suun on 11/27/19.
//

#ifndef LOTTA_EXCEPTIONS_H
#define LOTTA_EXCEPTIONS_H

#include <exception>

namespace lotta::http {

class HttpException : public std::exception {
 public:
  [[nodiscard]] virtual int code() const = 0;
};

class NotFoundException : public HttpException {
 public:
  [[nodiscard]] int code() const override {
    return 404;
  }
  [[nodiscard]] const char *what() const noexcept override {
    return "Not Found";
  }
};

}

#endif //LOTTA_EXCEPTIONS_H
