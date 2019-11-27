//
// Created by suun on 11/27/19.
//

#include "lotta/http/context.h"
#include "lotta/http/request.h"
#include "lotta/http/response.h"

namespace lotta::http {

Context::Context() = default;

void Context::setRequest(const Request &request) {
  request_ = std::make_shared<Request>(request);
}

void Context::setResponse(const Response &response) {
  response_ = std::make_shared<Response>(response);
}

}
