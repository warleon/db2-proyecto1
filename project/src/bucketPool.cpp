
#include "bucketPool.hpp"
void bucket::checkPos(size_t pos) {
  if (pos >= capacity) throw std::runtime_error("position out of bounds");
}

bucket::bucket() {}

bucket::bucket(size_t cap)
    : buffer(cap, {0, 0}), available(cap, 1), capacity(cap) {}
recordPos_t bucket::get(size_t pos) {
  checkPos(pos);
  return buffer[pos];
}
bool bucket::add(recordPos_t info) {
  for (size_t i = 0; i < capacity; i++) {
    if (available[i]) {
      buffer[i] = info;
      available[i] = false;
      return true;
    }
  }
  return false;
}
bool bucket::remove(size_t pos) { checkPos(pos); }
void bucket::writeBucket(std::ofstream os) {
  os.write((char *)&capacity, sizeof(size_t));
  os.write((char *)available.data(), available.size() * sizeof(bool));
  os.write((char *)buffer.data(), buffer.size() * sizeof(recordPos_t));
}
void bucket::readBucket(std::ifstream is) {
  is.read((char *)&capacity, sizeof(size_t));
  auto availableBuffer = new bool[capacity];
  auto bufferBuffer = new recordPos_t[capacity];
  is.read((char *)availableBuffer, capacity * sizeof(bool));
  is.read((char *)bufferBuffer, capacity * sizeof(recordPos_t));
  available = std::vector<bool>(availableBuffer,
                                availableBuffer + capacity * sizeof(bool));
  buffer = std::vector<recordPos_t>(
      bufferBuffer, bufferBuffer + capacity * sizeof(recordPos_t));
}