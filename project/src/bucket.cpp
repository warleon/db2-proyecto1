#include "bucket.hpp"

// bucket stuff

template <typename key_t>
void bucket<key_t>::checkPos(size_t pos) {
  if (pos >= capacity) throw std::runtime_error("position out of bounds");
}

template <typename key_t>
bucket<key_t>::bucket() {}

template <typename key_t>
bucket<key_t>::bucket(size_t cap)
    : buffer(cap, {0, 0}), available(cap, 1), capacity(cap) {}

template <typename key_t>
recordMeta<key_t> bucket<key_t>::get(size_t pos) {
  checkPos(pos);
  return buffer[pos];
}
template <typename key_t>
bool bucket<key_t>::add(recordMeta<key_t> info) {
  for (size_t i = 0; i < capacity; i++) {
    if (available[i]) {
      buffer[i] = info;
      available[i] = false;
      return true;
    }
  }
  return false;
}
template <typename key_t>
void bucket<key_t>::remove(size_t pos) {
  checkPos(pos);
  available[pos] = false;
}

template <typename key_t>
void bucket<key_t>::writeBucket(std::ofstream &os) {
  os.write((char *)&capacity, sizeof(size_t));
  os.write((char *)&localDegree, sizeof(size_t));
  os.write((char *)available.data(), available.size() * sizeof(bool));
  os.write((char *)buffer.data(), buffer.size() * sizeof(recordMeta<key_t>));
}

template <typename key_t>
void bucket<key_t>::readBucket(std::ifstream &is) {
  is.read((char *)&capacity, sizeof(size_t));
  is.read((char *)&localDegree, sizeof(size_t));
  auto availableBuffer = new bool[capacity];
  auto bufferBuffer = new recordMeta<key_t>[capacity];
  is.read((char *)availableBuffer, capacity * sizeof(bool));
  is.read((char *)bufferBuffer, capacity * sizeof(recordMeta<key_t>));
  available = std::vector<bool>(availableBuffer,
                                availableBuffer + capacity * sizeof(bool));
  buffer = std::vector<recordMeta<key_t>>(
      bufferBuffer, bufferBuffer + capacity * sizeof(recordMeta<key_t>));
}
