#include "bucket.hpp"

std::ostream &operator<<(std::ostream &os, recordMeta &m) {
  os << m.pos;
  os << m.info;
  return os;
}
std::istream &operator>>(std::istream &is, recordMeta &m) {
  is >> m.pos;
  is >> m.info;

  return is;
}

// bucket stuff

void bucket::checkKey(key_t key) {
  if (buffer.find(key) == buffer.end())
    throw std::runtime_error("key does not exist in this bucket");
}

bucket::bucket() {}

bucket::bucket(size_t cap) : buffer(cap), capacity(cap) {}

recordMeta bucket::get(key_t key) {
  checkKey(key);
  return buffer[key];
}
bool bucket::add(key_t key, recordMeta info) {
  if (buffer.size() == capacity) return false;
  buffer[key] = info;
  return true;
}
void bucket::remove(key_t key) {
  checkKey(key);
  buffer.erase(key);
}

void bucket::writeBucket(std::ofstream &os) {
  os << capacity;
  for (auto &it : buffer) {
    os << it.first << it.second;
  }
}

void bucket::readBucket(std::ifstream &is) {
  is >> capacity;
  key_t k;
  recordMeta m;
  buffer.clear();
  for (size_t i = 0; i < capacity; i++) {
    is >> k >> m;
    buffer[k] = m;
  }
}

std::ostream &operator<<(std::ostream &os, bucket &bucket) {
  os.write((char *)&bucket.capacity, sizeof(bucket.capacity));
  for (auto &it : bucket.buffer) {
    os << it.first << it.second;
  }
  return os;
}

std::istream &operator>>(std::istream &is, bucket &buc) {
  is >> buc.capacity;
  bucket::key_t k;
  recordMeta m;
  buc.buffer.clear();
  for (size_t i = 0; i < buc.capacity; i++) {
    is >> k >> m;
    buc.buffer[k] = m;
  }
  return is;
}