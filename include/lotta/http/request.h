//
// Created by suun on 11/26/19.
//

#ifndef LOTTA_REQUEST_H
#define LOTTA_REQUEST_H

#include "lotta/http/state.h"
#include <map>

namespace lotta {

class Buffer;

namespace http {

class Request {
 public:
  explicit Request(Buffer *buf);
  ~Request();

  [[nodiscard]] Method method() const { return method_; }
  [[nodiscard]] Version version() const { return version_; }
  [[nodiscard]] std::string path() const { return path_; }
  [[nodiscard]] std::string query() const { return query_; }
  [[nodiscard]] std::map<std::string, std::string> headers() const {
    return headers_;
  }
  [[nodiscard]] bool hasHeader(const std::string &key) const {
    return headers_.find(key) != headers_.end();
  }
  [[nodiscard]] std::string getHeader(const std::string &key) const {
    return headers_.find(key)->second;
  }
  [[nodiscard]] std::string body() const {
    return body_;
  }

 private:
  Method method_;
  Version version_;
  std::string path_;
  std::string query_;
  std::map<std::string, std::string> headers_;
  std::string body_;
};
}
}

#endif //LOTTA_REQUEST_H
