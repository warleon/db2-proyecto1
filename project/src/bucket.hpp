#pragma once

#include <fstream>
#include <string>
#include <unordered_map>

#include "genRecord.hpp"

struct recordMeta {
  size_t pos;
  GenRecordInfo info;
};

std::ostream &operator<<(std::ostream &, recordMeta &);
std::istream &operator>>(std::istream &, recordMeta &);

struct bucket {
  using key_t = std::string;
  bucket(size_t);
  bucket();
  recordMeta get(key_t);
  bool add(key_t, recordMeta);
  void remove(key_t);

  size_t localDeph;

  std::unordered_map<key_t, recordMeta> buffer;
  size_t capacity;
  void checkKey(key_t);
};

std::ostream &operator<<(std::ostream &, bucket &);
std::istream &operator>>(std::istream &, bucket &);