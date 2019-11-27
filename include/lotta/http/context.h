//
// Created by suun on 11/27/19.
//

#ifndef LOTTA_CONTEXT_H
#define LOTTA_CONTEXT_H

#include "lotta/utils/noncopyable.h"
#include <memory>

namespace lotta {

class Buffer;

namespace http {

class Request;
class Response;

class Context : utils::noncopyable {
 public:

  Context();
  ~Context();

  void setRequest(const Request &request);
  void setResponse(const Response &response);

  [[nodiscard]] std::shared_ptr<const Request> request() const { return request_; }
  [[nodiscard]] std::shared_ptr<Response> response() { return response_; }

 private:
  std::shared_ptr<Request> request_;
  std::shared_ptr<Response> response_;
};
}
}

#endif //LOTTA_CONTEXT_H
