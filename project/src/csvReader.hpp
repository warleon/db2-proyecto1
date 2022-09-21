#pragma once
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>

#include "genRecord.hpp"

class CSV {
  static const size_t pageSize = 4096;
  std::string record;

 public:
  Record header, line;
  GenRecordInfo headerInfo, lineInfo;
  std::ifstream file;

  CSV(std::string, std::string sep = ",");
  ~CSV();
  bool parseLine(const char *);
  std::pair<dtype, size_t> parseToken(char *);
  void castToken(char *, char *, size_t);
};