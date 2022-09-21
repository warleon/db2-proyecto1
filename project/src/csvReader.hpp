#pragma once
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>

#include "genRecord.hpp"

class CSV {
  static const size_t pageSize = 4096;
  char buffer[pageSize];
  std::string record;

  void readPage();

 public:
  Record header, line;
  GenRecordInfo headerInfo, lineInfo;
  std::ifstream file;

  CSV(std::string);
  ~CSV();
  void parseLine(const char *sep = ",");
  std::pair<dtype, size_t> parseToken(char *);
  void castToken(char *, char *, size_t);
};