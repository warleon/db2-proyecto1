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
typedef struct {
  size_t pos;
  size_t size;
} recordPos_t;

// a buffer that holds fixed size data (position and size of a given record)
typedef struct bucket {
  bucket(size_t);
  bucket();
  recordPos_t get(size_t);
  bool add(recordPos_t);
  bool remove(size_t);
  void readBucket(std::ifstream);
  void writeBucket(std::ofstream);

 private:
  std::vector<recordPos_t> buffer;
  std::vector<bool> available;
  size_t capacity;
  void checkPos(size_t);
} bucket_t;

class BucketPool {
  std::string poolFileName = "pool.info";

 public:
  using bucketId_t = size_t;

  // input(pool capacity,bucket record capacity,pool dirname)
  BucketPool(size_t, size_t, fs::path);

 private:
  std::vector<bucket_t *> pool;
  std::vector<bool> dirty;
  std::unordered_map<bucketId_t, size_t> idToIndex;
  size_t bucketSize;

  std::atomic<bucketId_t> lastId;
  fs::path poolDirName;

  bucketId_t create();
  size_t tick();
};