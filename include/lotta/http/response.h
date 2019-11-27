//
// Created by suun on 11/26/19.
//

#ifndef LOTTA_RESPONSE_H
#define LOTTA_RESPONSE_H

#include <map>

namespace lotta::http {

class Response {

 private:
  Method method_;
  Version version_;
  std::string path_;
  std::string query_;
  std::map<std::string, std::string> headers_;
  std::string body_;
};

}

#endif //LOTTA_RESPONSE_H
