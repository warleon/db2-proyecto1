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

template <typename key_t>
void bucket<key_t>::checkKey(key_t key) {
  if (buffer.find(key) == buffer.end())
    throw std::runtime_error("key does not exist in this bucket");
}

template <typename key_t>
bucket<key_t>::bucket() {}

template <typename key_t>
bucket<key_t>::bucket(size_t cap) : buffer(cap), capacity(cap) {}

template <typename key_t>
recordMeta bucket<key_t>::get(key_t key) {
  checkKey(key);
  return buffer[key];
}
template <typename key_t>
bool bucket<key_t>::add(key_t key, recordMeta info) {
  if (buffer.size() == capacity) return false;
  buffer[key] = info;
  return true;
}
template <typename key_t>
void bucket<key_t>::remove(key_t key) {
  checkKey(key);
  buffer.erase(key);
}

template <typename key_t>
void bucket<key_t>::writeBucket(std::ofstream &os) {
  os << capacity;
  for (auto &it : buffer) {
    os << it.first << it.second;
  }
}

template <typename key_t>
void bucket<key_t>::readBucket(std::ifstream &is) {
  is >> capacity;
  key_t k;
  recordMeta m;
  buffer.clear();
  for (size_t i = 0; i < capacity; i++) {
    is >> k >> m;
    buffer[k] = m;
  }
}

template <typename key_t>
std::ostream &operator<<(std::ostream &os, bucket<key_t> &bucket) {
  os.write((char *)&bucket.capacity, sizeof(bucket.capacity));
  for (auto &it : bucket.buffer) {
    os << it.first << it.second;
  }
  return os;
}

template <typename key_t>
std::istream &operator>>(std::istream &is, bucket<key_t> &bucket) {
  is >> bucket.capacity;
  key_t k;
  recordMeta m;
  bucket.buffer.clear();
  for (size_t i = 0; i < bucket.capacity; i++) {
    is >> k >> m;
    bucket.buffer[k] = m;
  }
  return is;
}