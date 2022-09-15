#include <fstream>
#include <vector>

template <typename key_t>
struct recordMeta {
  size_t pos;
  size_t size;
  key_t key;
};

template <typename key_t>
struct bucket {
  bucket(size_t);
  bucket();
  recordMeta<key_t> get(size_t);
  bool add(recordMeta<key_t>);
  void remove(size_t);
  void readBucket(std::ifstream &);
  void writeBucket(std::ofstream &);

  size_t localDegree;

 private:
  std::vector<recordMeta<key_t>> buffer;
  std::vector<bool> available;
  size_t capacity;
  void checkPos(size_t);
};