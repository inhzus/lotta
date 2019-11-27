//
// Created by suun on 11/26/19.
//

#ifndef LOTTA_STATE_H
#define LOTTA_STATE_H

namespace lotta::http {

enum class Method {
  Get,
  Post,
  PUT,
  DELETE
};

enum class Version {
  V09,
  V10,
  V11,
  V20,
  V30
};

}

#endif //LOTTA_STATE_H
