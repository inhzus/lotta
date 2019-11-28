//
// Created by suun on 11/28/19.
//

#include "lotta/http/state.h"
namespace lotta::http {

std::map<Method, const char *> MethodMsgs{ // NOLINT(cert-err58-cpp)
    {Method::GET, "GET"},
    {Method::POST, "POST"},
    {Method::PUT, "PUT"},
    {Method::DELETE, "DELETE"},
    {Method::HEAD, "HEAD"},
    {Method::OPTIONS, "OPTIONS"},
    {Method::CONNECT, "CONNECT"},
    {Method::TRACE, "TRACE"},
};

std::map<Status, const char *> StatusMsgs{ // NOLINT(cert-err58-cpp)
    {Status::OK, "OK"},
    {Status::BadRequest, "Bad Request"},
    {Status::NotFound, "Not Found"},
};

}
