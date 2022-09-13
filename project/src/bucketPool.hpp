#include <fstream>
#include <string>
#include <utility>
#include <vector>

typedef struct {
  size_t pos;
  size_t size;
} recordPos_t;

// a buffer that holds fixed size data (position and size of a given record)
typedef struct bucket {
  bucket(size_t);
  bool add(recordPos_t);
  bool remove(recordPos_t);
  void writeBuff(std::ofstream);
  void readBucket(std::ifstream);
  void dump(std::ofstream);
  bucket *split();

 private:
  std::vector<recordPos_t> buffer;
  std::vector<bool> available;
  size_t capacity;
} bucket_t;

class BucketPool {
  std::vector<bucket_t *> pool;
  std::vector<bool> modified;
  size_t bucketSize;

 public:
  using bucketId_t = size_t;

  // input(pool capacity,bucket record capacity,pool filename, buckets filename)
  BucketPool(size_t, size_t, std::string, std::string);
};