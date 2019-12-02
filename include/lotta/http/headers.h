//
// Created by suun on 12/2/19.
//

#ifndef LOTTA_HEADERS_H
#define LOTTA_HEADERS_H

#include <string>
#include <vector>

namespace lotta::http {

class Headers {
 public:
  Headers() = default;
  Headers(const std::initializer_list<std::pair<std::string, std::string>> &l) :
      data_(l) {}
  ~Headers() = default;

  [[nodiscard]] std::string get(const std::string &key) const {
    for (const auto &p : data_) {
      if (p.first == key) { return p.second; }
    }
    return std::string();
  }

  void append(std::string key, std::string val) {
    data_.emplace_back(std::move(key), std::move(val));
  }

  void set(const std::string &key, const std::string &val) {
    for (auto &p : data_) {
      if (p.first == key) {
        p.second = val;
        return;
      }
    }
    append(key, val);
  }

  [[nodiscard]] const std::vector<std::pair<std::string, std::string>> &
  raw() const {
    return data_;
  }
 private:
  std::vector<std::pair<std::string, std::string>> data_;
};

}

#endif //LOTTA_HEADERS_H
