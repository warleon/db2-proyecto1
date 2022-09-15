
#include "bucketPool.hpp"
// bucket stuff

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
void bucket::writeBucket(std::ofstream &os) {
  os.write((char *)&capacity, sizeof(size_t));
  os.write((char *)available.data(), available.size() * sizeof(bool));
  os.write((char *)buffer.data(), buffer.size() * sizeof(recordPos_t));
}

void bucket::readBucket(std::ifstream &is) {
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

// bucketPool stuff

BucketPool::BucketPool(size_t poolCap, size_t bucketCap, fs::path dirname)
    : capacity(poolCap),
      bucketSize(bucketCap),
      pool(poolCap),
      dirty(poolCap),
      clock(poolCap),
      poolDirName(dirname) {
  for (auto &it : pool) {
    it = new bucket_t;
  }
  // check for previus runs
  fs::path meta(poolDirName / poolFileName);
  std::ifstream file(meta, std::ios::binary);
  if (file.is_open()) readPool(file);
}

BucketPool::~BucketPool() {
  std::ofstream file(poolDirName / poolFileName, std::ios::binary);
  writePool(file);
}

size_t BucketPool::tick() {
  lock_t lock(mutex);
  clockCount = clockCount++ % capacity;
  return clockCount;
}

BucketPool::bucketId_t BucketPool::create() {
  bucketId_t nId = lastId++;
  std::ofstream file(makeBucketPath(nId));
  bucket_t bucket(bucketSize);
  bucket.writeBucket(file);
  return nId;
}
size_t BucketPool::evict() {
  size_t t;
  while (true) {
    t = tick();
    if (clock[t])
      clock[t] = false;
    else
      break;
  }
  lock_t lock(mutex);
  bucketId_t id = posToId[t];
  idToPos.erase(id);
  std::ofstream file(makeBucketPath(id), std::ios::binary);
  if (dirty[t]) pool[t]->writeBucket(file);
  return t;
}
fs::path BucketPool::makeBucketPath(bucketId_t id) {
  return poolDirName / std::to_string(id);
}
bool BucketPool::fetched(bucketId_t id) {
  return idToPos.find(id) != idToPos.end();
}
bool BucketPool::isFull() { return idToPos.size() == capacity; }

bucket_t *BucketPool::fetch(bucketId_t id) {
  lock_t lock(mutex);
  if (fetched(id)) {
    clock[idToPos[id]] = true;
    return pool[idToPos[id]];
  }

  std::ifstream file(makeBucketPath(id), std::ios::binary);
  if (!file.good())
    throw std::runtime_error(
        "can't open file to fetch bucket at BucketPool::fetch");

  size_t pos;
  if (isFull())
    pos = evict();
  else
    pos = tick();  // as the pool is not full and this thread holds the lock the
                   // next position will be empty

  pool[pos]->readBucket(file);
  posToId[pos] = id;
  idToPos[id] = pos;
  return pool[pos];
}
void BucketPool::writePool(std::ofstream &file) {
  size_t lid = lastId;
  file.write((char *)&lid, sizeof(size_t));
  file.write((char *)&capacity, sizeof(size_t));
  file.write((char *)&bucketSize, sizeof(size_t));
}
void BucketPool::readPool(std::ifstream &file) {
  size_t lid;
  file.read((char *)&lid, sizeof(size_t));
  file.read((char *)&capacity, sizeof(size_t));
  file.read((char *)&bucketSize, sizeof(size_t));
  lastId = lid;
}