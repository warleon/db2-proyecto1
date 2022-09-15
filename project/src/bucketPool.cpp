
#include "bucketPool.hpp"
// bucketPool stuff

template <class bucket_t>
BucketPool<bucket_t>::BucketPool(size_t poolCap, size_t bucketCap,
                                 fs::path dirname)
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

template <class bucket_t>
BucketPool<bucket_t>::~BucketPool() {
  std::ofstream file(poolDirName / poolFileName, std::ios::binary);
  writePool(file);
}

template <class bucket_t>
size_t BucketPool<bucket_t>::tick() {
  lock_t lock(mutex);
  clockCount = clockCount++ % capacity;
  return clockCount;
}

template <class bucket_t>
BucketPool<bucket_t>::bucketId_t BucketPool<bucket_t>::create() {
  bucketId_t nId = lastId++;
  std::ofstream file(makeBucketPath(nId));
  bucket_t bucket(bucketSize);
  bucket.writeBucket(file);
  return nId;
}
template <class bucket_t>
size_t BucketPool<bucket_t>::evict() {
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
template <class bucket_t>
fs::path BucketPool<bucket_t>::makeBucketPath(
    BucketPool<bucket_t>::bucketId_t id) {
  return poolDirName / std::to_string(id);
}
template <class bucket_t>
bool BucketPool<bucket_t>::fetched(bucketId_t id) {
  return idToPos.find(id) != idToPos.end();
}
template <class bucket_t>
bool BucketPool<bucket_t>::isFull() {
  return idToPos.size() == capacity;
}

template <class bucket_t>
bucket_t *BucketPool<bucket_t>::fetch(bucketId_t id) {
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
template <class bucket_t>
void BucketPool<bucket_t>::writePool(std::ofstream &file) {
  size_t lid = lastId;
  file.write((char *)&lid, sizeof(size_t));
  file.write((char *)&capacity, sizeof(size_t));
  file.write((char *)&bucketSize, sizeof(size_t));
}
template <class bucket_t>
void BucketPool<bucket_t>::readPool(std::ifstream &file) {
  size_t lid;
  file.read((char *)&lid, sizeof(size_t));
  file.read((char *)&capacity, sizeof(size_t));
  file.read((char *)&bucketSize, sizeof(size_t));
  lastId = lid;
}