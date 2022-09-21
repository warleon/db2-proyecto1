#include "hashTable.hpp"

// macros taken from
// https://stackoverflow.com/questions/10090326/how-to-extract-specific-bits-from-a-number-in-c
#define LAST(k, n) ((k) & ((1 << (n)) - 1))
#define MID(k, m, n) LAST((k) >> (m), ((n) - (m)))

ExtendibleHash::ExtendibleHash(fs::path home, size_t bucketSize,
                               size_t poolSize)
    : dirName(home),
      poolDirName(home / "buckets"),
      pool(poolSize, bucketSize, poolDirName),
      globalDepth(1),
      directory(2) {
  // check for previus instance
  std::ifstream file(home / filename, std::ios::binary);
  if (file.good()) {
    file >> *this;
    return;
  }
  // initialize with new buckets
  for (auto& it : directory) {
    it = pool.create();
  }
}
ExtendibleHash::~ExtendibleHash() {
  std::ofstream file(dirName / filename, std::ios::binary);
  if (file.good()) file << *this;
}

std::ostream& operator<<(std::ostream& os, ExtendibleHash& eh) {
  os.write((char*)&eh.globalDepth, sizeof(eh.globalDepth));
  os.write((char*)eh.directory.data(),
           sizeof(ExtendibleHash::pool_t::bucketId_t) * eh.directory.size());
  return os;
}
std::istream& operator>>(std::istream& is, ExtendibleHash& eh) {
  is.read((char*)&eh.globalDepth, sizeof(eh.globalDepth));
  size_t size = 1 << eh.globalDepth;
  auto buffer = new ExtendibleHash::pool_t::bucketId_t[size];
  is.read((char*)buffer, sizeof(ExtendibleHash::pool_t::bucketId_t) * size);
  eh.directory =
      std::vector<ExtendibleHash::pool_t::bucketId_t>(buffer, buffer + size);
  delete[] buffer;
  return is;
}

ExtendibleHash::queryResult_t ExtendibleHash::search(
    ExtendibleHash::key_t key) {
  hash_t nk = keyToHash(key);
  size_t index = hashToIndex(nk);
  bucket* buc = pool.fetch(directory[index]);
  buc->checkKey(key);
  return buc->buffer[key];
}

size_t ExtendibleHash::hashToIndex(hash_t h) {
  return MID(h, sizeof(hash_t) - globalDepth, sizeof(hash_t));
}

void ExtendibleHash::add(recordMeta meta, key_t key) {
  hash_t nk = keyToHash(key);
  size_t index = hashToIndex(nk);
  auto oid = directory[index];
  bucket* buc = pool.fetch(oid);
  if (buc->add(key, meta)) {
    pool.setDirty(oid);
    return;
  }
  bucket::buffer_t buff;
  do {
    pool_t::bucketId_t nid = pool.create();
    bucket* nbuc = pool.fetch(nid);
    nbuc->localDepth = ++(buc->localDepth);
    buff.insert(buc->buffer.begin(), buc->buffer.end());
    // just in case
    buc->buffer.clear();
    nbuc->buffer.clear();

    buff[key] = meta;

    size_t nbindex;
    if (buc->localDepth > globalDepth) {
      doubleCapacity();
      nbindex = siblingAfterDoubling(index);
    } else {
      nbindex = index;
    }
    directory[nbindex] = nid;

    auto it = distribute(buff, buc, nbuc);

    pool.setDirty(oid);
    pool.setDirty(nid);
    // if all inserted
    if (it == buff.end()) {
      break;
    }
  } while (1);
}
bucket::buffer_t::iterator ExtendibleHash::distribute(bucket::buffer_t& buff,
                                                      bucket* b1, bucket* b2) {
  bool inserted = false;
  auto it = buff.begin();
  for (; it != buff.end(); it++) {
    // hash again
    auto nk = keyToHash(it->first);
    auto index =
        MID(nk, sizeof(nk) - globalDepth - 1, sizeof(nk) - globalDepth);
    if (index)
      inserted = b1->add(it->first, it->second);
    else
      inserted = b2->add(it->first, it->second);

    if (!inserted) {
      break;
    }
  }
  it = buff.erase(buff.begin(), it);

  return it;
}

void ExtendibleHash::doubleCapacity() {
  ++globalDepth;
  // auto temp = std::move(directory);
  directory.resize(1 << globalDepth);
  for (size_t i = directory.size() - 1; i >= 0; --i) {
    directory[i] = directory[i >> 1];
    if (!i) break;
  }
}

void ExtendibleHash::remove(key_t key) {
  hash_t nk = keyToHash(key);
  size_t index = hashToIndex(nk);
  auto oid = directory[index];
  bucket* buc = pool.fetch(oid);
  buc->remove(key);
  pool.setDirty(oid);
}

void ExtendibleHash::index(std::string infoFile, std::string dataFile,
                           size_t keyPos) {
  std::ifstream info(infoFile, std::ios::binary);
  std::ifstream data(dataFile, std::ios::binary);
  if (!(info.good() && data.good()))
    throw std::runtime_error("can't index unexisting files");
  GenRecordInfo tempInfo;
  size_t off = 0;
  Record rec = nullptr;
  while (info >> tempInfo) {
    if (!info.good()) break;
    rec = (Record)tempInfo.allocate(1);
    data.read(rec, tempInfo.getSize());
    add({off, tempInfo}, getKey(rec, tempInfo, keyPos));
    off += tempInfo.getSize();
    delete[] rec;
    rec = nullptr;
  }
}

ExtendibleHash::key_t ExtendibleHash::getKey(Record rec, GenRecordInfo info,
                                             size_t keyPos) {
  char* field = info.field(rec, keyPos);

  return key_t(field, field + info.fieldSize(keyPos));
}
size_t ExtendibleHash::siblingAfterDoubling(size_t pos) { return (pos << 1); }