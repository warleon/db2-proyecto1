#pragma once

#include <atomic>
#include <exception>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fs = std::filesystem;
// TODO change fetch implementation to demiss the reques of the same bucket by 2
// different threads use thread yield

// a buffer that holds fixed size data (position and size of a given record)
template <class bucket_t>
class BucketPool {
 public:
  using bucketId_t = size_t;
  using mutex_t = std::recursive_mutex;  // TODO change to normal mutex and
                                         // instantiate one per function
  using lock_t = std::lock_guard<mutex_t>;

 private:
  std::string poolFileName = "pool.info";
  size_t capacity;
  size_t bucketSize;
  mutex_t mutex;
  size_t clockCount;
  std::atomic<bucketId_t> lastId;
  std::vector<bucket_t *> pool;
  std::vector<bool> dirty, clock;
  std::unordered_map<bucketId_t, size_t> idToPos;
  std::unordered_map<size_t, bucketId_t> posToId;
  fs::path poolDirName;

 public:
  // input(pool capacity,bucket record capacity,pool dirname)
  BucketPool(size_t, size_t, fs::path);
  ~BucketPool();

  // returns a pointer to the requested bucket
  bucket_t *fetch(bucketId_t);

  // creates a new bucket in the filesystem returns the bucket id
  bucketId_t create();

 private:
  // checks if id exists in the pool
  bool fetched(bucketId_t);

  // checks if pool is full
  bool isFull();

  // creates a path to the requested bucket
  fs::path makeBucketPath(bucketId_t);

  // advances the clock
  size_t tick();

  // returns the evicted position
  size_t evict();

  void readPool(std::ifstream &);
  void writePool(std::ofstream &);
};