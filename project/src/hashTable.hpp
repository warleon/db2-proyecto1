#pragma once

#include <cmath>
#include <fstream>
#include <unordered_map>
#include <vector>

#include "bucket.hpp"
#include "bucketPool.cpp"
#include "genRecord.hpp"

class ExtendibleHash {
  using hash_t = size_t;
  using key_t = bucket::key_t;
  using queryResult_t = recordMeta;
  using pool_t = BucketPool<bucket>;

  std::string filename = "hash.info";

  fs::path dirName;
  fs::path poolDirName;
  pool_t pool;

  size_t globalDepth;
  std::vector<pool_t::bucketId_t> directory;

  std::hash<key_t> keyToHash;
  size_t hashToIndex(hash_t);
  void doubleCapacity();

 public:
  ExtendibleHash(fs::path home, size_t bucketSize = 32, size_t poolSize = 1024);
  ~ExtendibleHash();
  friend std::ostream& operator<<(std::ostream&, ExtendibleHash&);
  friend std::istream& operator>>(std::istream&, ExtendibleHash&);

  // generates a key from the record
  key_t getKey(Record, GenRecordInfo, size_t);

  // indexes general records with a file of GenRecords  a file with their
  // corresponding records and the key position in the record info
  void index(std::string, std::string, size_t);

  queryResult_t search(key_t);
  void add(recordMeta, key_t);
  void remove(key_t);
};