//
// Created by suun on 11/26/19.
//

#ifndef LOTTA_REQUEST_H
#define LOTTA_REQUEST_H

#include "lotta/http/state.h"
#include "lotta/http/headers.h"

namespace lotta {

class Buffer;

namespace http {

class Request {
 public:
  Request() : method_(Method::GET), version_(Version::V11), path_("/") {}
  explicit Request(Buffer *buf);
  ~Request();

  void readBuffer(Buffer *buf);

  [[nodiscard]] Method method() const { return method_; }
  [[nodiscard]] Version version() const { return version_; }
  [[nodiscard]] std::string path() const { return path_; }
  [[nodiscard]] std::string query() const { return query_; }
  Headers &headers() {
    return headers_;
  }
  [[nodiscard]] std::string getHeader(const std::string &key) const {
    return headers_.get(key);
  }
  [[nodiscard]] std::string body() const {
    return body_;
  }

 private:
  Method method_;
  Version version_;
  std::string path_;
  std::string query_;
  Headers headers_;
  std::string body_;
};
}
}

#endif //LOTTA_REQUEST_H
