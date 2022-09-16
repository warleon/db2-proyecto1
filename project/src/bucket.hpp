#include <fstream>
#include <unordered_map>

#include "genRecord.hpp"

struct recordMeta {
  size_t pos;
  GenRecordInfo info;
};

std::ostream &operator<<(std::ostream &, recordMeta &);
std::istream &operator>>(std::istream &, recordMeta &);
template <typename key_t>
struct bucket {
  bucket(size_t);
  bucket();
  recordMeta get(key_t);
  bool add(key_t, recordMeta);
  void remove(key_t);
  void readBucket(std::ifstream &);
  void writeBucket(std::ofstream &);

  size_t localDegree;

 private:
  std::unordered_map<key_t, recordMeta> buffer;
  size_t capacity;
  void checkKey(key_t);
};