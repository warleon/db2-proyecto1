#include <fstream>
#include <unordered_map>
#include <vector>

#include "genRecord.hpp"

template <class bucketPool_t>
class ExtendibleHash {
  using hash_t = size_t;
  using key_t = std::string;
  using queryResult_t = std::vector<Record>;

  std::unordered_map<hash_t, typename bucketPool_t::bucketId_t> hashToBucket;
  bucketPool_t pool;
  std::string filename;
  GenRecordInfo recordInfo;
  size_t key_index;

  key_t recordToKey(Record);
  hash_t hash(key_t);

 public:
  ExtendibleHash(GenRecordInfo, size_t, bucketPool_t);
  ExtendibleHash();
  ~ExtendibleHash();
  void writeIndex(std::string);
  void readIndex(std::string);

  queryResult_t search(key_t);
  key_t add(Record);
  bool remove(key_t);
};