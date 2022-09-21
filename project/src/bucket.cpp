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

bucket::bucket(size_t cap) : localDepth(0), buffer(cap), capacity(cap) {}

recordMeta bucket::get(key_t key) {
  checkKey(key);
  return buffer[key];
}
bool bucket::add(key_t key, recordMeta info) {
  if (buffer.size() >= capacity) return false;
  buffer.insert_or_assign(key, info);
  return true;
}
void bucket::remove(key_t key) {
  if (!buffer.erase(key))
    throw std::runtime_error("key not found at bucket::remove");
}

std::ostream &operator<<(std::ostream &os, bucket &bucket) {
  os.write((char *)&bucket.capacity, sizeof(bucket.capacity));
  size_t s = bucket.buffer.size();
  os.write((char *)&s, sizeof(s));
  for (auto &it : bucket.buffer) {
    auto ks = it.first.size();
    os.write((char *)&ks, sizeof(ks));
    os << it.first << it.second;
  }
  return os;
}

std::istream &operator>>(std::istream &is, bucket &buc) {
  is.read((char *)&buc.capacity, sizeof(buc.capacity));
  size_t s;
  is.read((char *)&s, sizeof(s));
  recordMeta m;
  buc.buffer.clear();
  for (size_t i = 0; i < s; i++) {
    size_t ks;
    is.read((char *)&ks, sizeof(ks));
    char *key = new char[ks];
    is.read(key, ks);
    is >> m;
    buc.buffer[bucket::key_t(key, key + ks)] = m;
    delete[] key;
  }
  return is;
}