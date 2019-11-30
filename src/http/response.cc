//
// Created by suun on 11/28/19.
//

#include "lotta/http/response.h"
#include "lotta/http/state.h"
#include "spdlog/fmt/fmt.h"
#include <sstream>

namespace lotta::http {

void Response::setStatus(Status status) {
  status_ = status;
}

void Response::setBody(std::string body) {
  body_ = std::move(body);
  headers_["Content-Length"] = fmt::format("{}", body_.size());
}

std::string Response::toString() const {
  std::stringstream ss;

  ss << "HTTP/1.1 " << static_cast<int>(status_) << " " <<
     StatusMsgs[status_] << "\r\n";
//  ss << fmt::format("HTTP/1.1 {} {}\r\n", status_, StatusMsgs[status_]);
  for (const auto &header : headers_) {
    ss << header.first << ": " << header.second << "\r\n";
//    ss << fmt::format("{}: {}\r\n", header.first, header.second);
  }
  ss << "\r\n" << body_;
  return ss.str();
}

}
