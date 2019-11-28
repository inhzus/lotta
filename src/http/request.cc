//
// Created by suun on 11/28/19.
//

#include "lotta/http/request.h"
#include "lotta/buffer.h"
#include "lotta/http/exceptions.h"
#include "lotta/utils/logging.h"

namespace lotta::http {

Request::Request(Buffer *buf) {
  const char *CRLF = "\r\n", *SP = " ";
  Slice slice;
  slice = buf->retrieveUntil(SP);  // method slice
  if (slice == "GET") {
    method_ = Method::GET;
  } else if (slice == "POST") {
    method_ = Method::POST;
  } else if (slice == "PUT") {
    method_ = Method::PUT;
  } else if (slice == "DELETE") {
    method_ = Method::DELETE;
  } else if (slice == "OPTIONS") {
    method_ = Method::OPTIONS;
  } else if (slice == "HEAD") {
    method_ = Method::HEAD;
  } else if (slice == "TRACE") {
    method_ = Method::TRACE;
  } else if (slice == "CONNECT") {
    method_ = Method::CONNECT;
  } else {
    SPDLOG_WARN("unrecognized method: {}", slice);
    throw BadRequestException();
  }
  slice = buf->retrieveUntil(SP);  // path slice
  if (slice.empty()) {
    SPDLOG_WARN("path empty");
    throw BadRequestException();
  }
  path_ = slice.toString();
  slice = buf->retrieveUntil(CRLF);  // version slice
  if (slice.n < 5) {
    SPDLOG_WARN("unrecognized version: {}", slice);
  }
  slice = {slice.data + 5, slice.n - 5};
  if (slice == "0.9") {
    version_ = Version::V09;
  } else if (slice == "1.0") {
    version_ = Version::V10;
  } else if (slice == "1.1") {
    version_ = Version::V11;
  } else if (slice == "2.0") {
    version_ = Version::V20;
  } else if (slice == "3.0") {
    version_ = Version::V30;
  } else {
    SPDLOG_WARN("unrecognized version: {}", slice);
    throw BadRequestException();
  }
  const char *COLON = ": ";
  while (true) {
    slice = buf->retrieveUntil(CRLF);
    if (slice.empty()) { break; }
    auto colon = std::search(
        slice.begin(), slice.end(), COLON, COLON + strlen(COLON));
    if (colon == slice.end()) {
      SPDLOG_WARN("not-formatted header: {}", slice);
      throw BadRequestException();
    }
    headers_[std::string(slice.begin(), colon)] =
        std::string(colon + 2, slice.end());
  }
  body_ = buf->retrieve().toString();
}

Request::~Request() = default;

}

