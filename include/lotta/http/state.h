//
// Created by suun on 11/26/19.
//

#ifndef LOTTA_STATE_H
#define LOTTA_STATE_H

#include <map>

namespace lotta::http {

enum class Method {
  OPTIONS,
  GET,
  HEAD,
  POST,
  PUT,
  DELETE,
  TRACE,
  CONNECT,
};

extern std::map<Method, const char *> MethodMsgs;

enum class Version {
  V09,
  V10,
  V11,
  V20,
  V30,
};

enum class Status {
  OK = 200,
  BadRequest = 400,
  NotFound = 404,
};

extern std::map<Status, const char *> StatusMsgs;

}

#endif //LOTTA_STATE_H
