#include "hashTable.hpp"

// macros taken from
// https://stackoverflow.com/questions/10090326/how-to-extract-specific-bits-from-a-number-in-c
#define LAST(k, n) ((k) & ((1 << (n)) - 1))
#define MID(k, m, n) LAST((k) >> (m), ((n) - (m)))

#include <iostream>

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
  return buc->buffer[key];
}

size_t ExtendibleHash::hashToIndex(hash_t h) {
  return MID(h, sizeof(hash_t) - globalDepth, sizeof(hash_t));
}

void ExtendibleHash::add(recordMeta meta, key_t key) {
  static size_t addcount = 0;
  std::cerr << "start adding val number " << addcount++ << std::endl;
  hash_t nk = keyToHash(key);
  std::cerr << "key hashed " << nk << std::endl;
  size_t index = hashToIndex(nk);
  std::cerr << "hash converted to index " << index << std::endl;
  auto oid = directory[index];
  std::cerr << "bucket id retrieved " << oid << std::endl;
  bucket* buc = pool.fetch(oid);
  std::cerr << "bucket fetched ptr = " << (void*)buc << std::endl;
  if (buc->add(key, meta)) {
    std::cerr << "val succesfully added " << addcount << std::endl;
    return;
  }
  std::cerr << "Bucket full need to split" << std::endl;
  buc->localDeph++;
  std::cerr << "increaced bucket local depth" << std::endl;
  pool_t::bucketId_t nid = pool.create();
  bucket* nbuc = pool.fetch(nid);
  nbuc->localDeph = buc->localDeph;
  std::cerr << "created new bucket" << std::endl;
  auto buff = std::move(buc->buffer);
  if (buc->localDeph > globalDepth) {
    std::cerr << "have to resize th edirectory" << std::endl;
    doubleCapacity();
    size_t nbindex =  // TODO fix index calculation
        directory[index << 1] == oid ? (index << 1) : (index << 1) + 1;
    directory[nbindex] = nid;
  } else {
    // TODO else handle insertion of new bucket
    std::cerr << "panic bucket split without resize not implemented"
              << std::endl;
  }

  add(meta, key);
  for (auto& it : buff) {
    add(it.second, it.first);
  }
  std::cerr << "end adding val number " << addcount << std::endl;
}

void ExtendibleHash::doubleCapacity() {
  globalDepth = globalDepth << 1;
  auto temp = std::move(directory);
  directory.resize(2 << globalDepth);
  for (auto& it : temp) {
    for (size_t i = 0; i < 2; i++) {
      directory.push_back(it);
    }
  }
}

bool ExtendibleHash::remove(key_t key) {
  hash_t nk = keyToHash(key);
  size_t index = hashToIndex(nk);
  auto oid = directory[index];
  bucket* buc = pool.fetch(oid);
  return buc->buffer.erase(key);
}

void ExtendibleHash::index(std::string infoFile, std::string dataFile,
                           size_t keyPos) {
  std::ifstream info(infoFile, std::ios::binary);
  std::ifstream data(dataFile, std::ios::binary);
  if (!(info.good() && data.good()))
    throw std::runtime_error("can't index unexisting files");
  GenRecordInfo tempInfo;
  size_t off = 0;
  while (info.good()) {
    info >> tempInfo;
    Record* rec = tempInfo.allocate(1);
    tempInfo.read(rec, 1, data);
    add({off, tempInfo}, getKey(tempInfo.at(rec, 0), tempInfo, keyPos));
    off += tempInfo.getSize();
    tempInfo.deallocate(rec);
  }
}

ExtendibleHash::key_t ExtendibleHash::getKey(Record rec, GenRecordInfo info,
                                             size_t keyPos) {
  char* field = info.field(rec, keyPos);

  return key_t(field, field + info.fieldSize(keyPos));
}