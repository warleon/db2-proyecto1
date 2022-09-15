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
  void readBucket(std::ifstream &);
  void writeBucket(std::ofstream &);

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
  using mutex_t = std::recursive_mutex;  // TODO change to normal mutex and
                                         // instantiate one per function
  using lock_t = std::lock_guard<mutex_t>;

  // input(pool capacity,bucket record capacity,pool dirname)
  BucketPool(size_t, size_t, fs::path);
  ~BucketPool();

 private:
  std::vector<bucket_t *> pool;
  std::vector<bool> dirty, clock;
  std::unordered_map<bucketId_t, size_t> idToPos;
  std::unordered_map<size_t, bucketId_t> posToId;
  size_t bucketSize;
  size_t capacity;
  mutex_t mutex;

  std::atomic<bucketId_t> lastId;
  size_t clockCount;
  fs::path poolDirName;

  // checks if id exists in the pool
  bool fetched(bucketId_t);

  // checks if pool is full
  bool isFull();

  // creates a path to the requested bucket
  fs::path makeBucketPath(bucketId_t);

  // creates a new bucket in the filesystem returns the bucket id
  bucketId_t create();

  // advances the clock
  size_t tick();

  // returns the evicted position
  size_t evict();

  // returns a pointer to the requested bucket
  bucket_t *fetch(bucketId_t);

  void readPool(std::ifstream &);
  void writePool(std::ofstream &);
};