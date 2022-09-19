#pragma once

#include <cmath>
#include <fstream>
#include <unordered_map>
#include <vector>

#include "bucket.hpp"
#include "bucketPool.hpp"
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

  hash_t hash(key_t);
  void doubleCapacity();

 public:
  ExtendibleHash(fs::path, size_t, size_t);
  ~ExtendibleHash();
  friend std::ostream& operator<<(std::ostream&, ExtendibleHash&);
  friend std::istream& operator>>(std::istream&, ExtendibleHash&);

  // indexes general records with a file of GenRecords and a file with their
  // corresponding records
  void index(std::string, std::string);

  queryResult_t search(key_t);
  void add(recordMeta, key_t);
  bool remove(key_t);
};