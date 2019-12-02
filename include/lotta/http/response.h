//
// Created by suun on 11/26/19.
//

#ifndef LOTTA_RESPONSE_H
#define LOTTA_RESPONSE_H

#include "lotta/http/headers.h"
#include "lotta/http/state.h"

namespace lotta::http {

class Response {
 public:
  Response() :
      status_(Status::OK),
      version_(Version::V11),
      headers_{
          {"Connection", "Keep-Alive"},
          {"Content-Length", "0"}
      } {}

  void setStatus(Status);
  void setBody(std::string);
  [[nodiscard]] std::string toString() const;
 private:
  Status status_;
  Version version_;
  Headers headers_;
  std::string body_;
};

}

#endif //LOTTA_RESPONSE_H
