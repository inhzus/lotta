//
// Created by suun on 11/18/19.
//

#ifndef LOTTA_BUFFER_H
#define LOTTA_BUFFER_H

#include <functional>
#include <string>
#include <vector>
#include <cassert>
#include <cstring>
#include <climits>
#include <endian.h>
#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/ostr.h"

namespace lotta {

template<typename T>
T swapEndian(T v) {
#if __BYTE_ORDER == __BIG_ENDIAN
  return v;
#else
  static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");
  union {
    T u;
    unsigned char u8[sizeof(T)];
  } src, dest;
  src.u = v;
  for (size_t k = 0; k < sizeof(T); k++)
    dest.u8[k] = src.u8[sizeof(T) - k - 1];
  return dest.u;
#endif
}

struct Slice {
  const char *data;
  size_t n;

  Slice() : data(nullptr), n() {}

  explicit Slice(const std::string &s) :
      data(s.data()), n(static_cast<size_t>(s.size())) {}
  Slice(const char *data, size_t n) : data(data), n(n) {}

  [[nodiscard]] std::string toString() const {
    return std::string(data, n);
  }

  [[nodiscard]] bool empty() const {
    return n == 0;
  }

  [[nodiscard]] const char *begin() const {
    return data;
  }
  [[nodiscard]] const char *end() const {
    return data + n;
  }

  bool operator==(const char *str) {
    return strlen(str) == n && strncmp(str, data, n) == 0;
  }

  bool operator==(const std::string &s) {
    return s.size() == n && strncmp(s.data(), data, n) == 0;
  }

  friend std::ostream &operator<<(std::ostream &os, const Slice &slice) {
    os.write(slice.data, slice.n);
    return os;
  }
};

class Buffer {
 public:
  static const size_t kInitLen;

  explicit Buffer(size_t n = Buffer::kInitLen) :
      idx_(0) {
    data_.reserve(n);
  }
//
//  template<typename T, std::enable_if_t<
//      std::is_constructible_v<std::string, T>, int> = 0>
//  explicit Buffer(T &&v) : // NOLINT(bugprone-forwarding-reference-overload)
//      idx_(0) {
//    std::string s(std::forward<std::string>(v));
//    data_.reserve(Buffer::kInitLen);
//    data_.assign(s.begin(), s.end());
//  }

  void swap(Buffer &b) {
    data_.swap(b.data_);
    std::swap(idx_, b.idx_);
  }

  [[nodiscard]] Slice toSlice() const {
    return Slice{peek(), readable()};
  }

  [[nodiscard]] std::string toString() const {
    return std::string(peek(), readable());
  };

  Slice retrieveUntil(const char *delim) {
    return retrieveUntil(delim, strlen(delim));
  }

  Slice retrieveUntil(const char *delim, size_t n) {
    auto it = std::search(peek(), end(), delim, delim + n);
    if (it == end()) {
      return Slice{nullptr, 0};
    } else {
      Slice ret = retrieve(it - peek());
      retrieve(n);
      return ret;
    }
  }

  template<typename T, std::enable_if_t<std::is_unsigned_v<T>, int> = 0>
  T retrieve() {
    Slice slice = retrieve(sizeof(T));
    const T *ptr = reinterpret_cast<const T *>(slice.data);
    return swapEndian(*ptr);
  }

  Slice retrieve() {
    return retrieve(readable());
  }

  Slice retrieve(size_t n) {
    assert(n <= readable());
    Slice ret{peek(), n};
    idx_ += n;
    return ret;
  }

  void revoke(size_t n) {
    assert(n >= idx_);
    idx_ -= n;
  }

  void append(Slice slice) {
    append(slice.data, slice.n);
  }

  void append(const char *data) {
    append(data, strlen(data));
  }

  template<typename T>
  void append(T v) {
    v = swapEndian(v);
    append(static_cast<const char *>(&v), sizeof(T));
  }

  void append(const char *data, size_t n) {
    makeSpace(n);
    data_.insert(data_.end(), data, data + n);
  }

  void discard(size_t n) {
    assert(n <= readable());
    data_.resize(data_.size() - n);
  }

  ssize_t readFrom(int fd);

  char *peek() { return begin() + idx_; }
  [[nodiscard]] const char *peek() const { return begin() + idx_; }

  [[nodiscard]] size_t readable() const {
    assert(data_.size() >= idx_);
    return data_.size() - idx_;
  }
  [[nodiscard]] size_t writable() const {
    return data_.capacity() - data_.size();
  }

 private:

  char *begin() { return &*data_.begin(); }
  [[nodiscard]] const char *begin() const { return &*data_.begin(); }

  char *end() { return &*data_.end(); }
  [[nodiscard]] const char *end() const { return &*data_.end(); }

  void makeSpace(size_t n) {
    if (readable() == 0 && writable() == 0) {
      idx_ = 0;
    }
    if (n <= writable()) { return; }
    if (readable() + n > data_.capacity()) { return; }
    std::copy(peek(), end(), begin());
    data_.resize(readable());
    idx_ = 0;
  }

  size_t idx_;
  std::vector<char> data_;
};

}

#endif //LOTTA_BUFFER_H
