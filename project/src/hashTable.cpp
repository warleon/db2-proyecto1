#include "hashTable.hpp"

ExtendibleHash::ExtendibleHash(fs::path home, size_t bucketSize = 32,
                               size_t poolSize = 1024)
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
}

std::ostream& operator<<(std::ostream& os, ExtendibleHash& eh) {
  os.write((char*)&eh.globalDepth, sizeof(eh.globalDepth));
  os.write((char*)eh.directory.data(),
           sizeof(ExtendibleHash::pool_t::bucketId_t) * eh.directory.size());
}
std::istream& operator>>(std::istream& is, ExtendibleHash& eh) {
  is.read((char*)&eh.globalDepth, sizeof(eh.globalDepth));
  size_t size = 1 << eh.globalDepth;
  auto buffer = new ExtendibleHash::pool_t::bucketId_t[size];
  is.read((char*)eh.directory.data(),

          sizeof(ExtendibleHash::pool_t::bucketId_t) * size);
}

// ----------------------------------------------------------------
// ACCESS (given key Ko)
// 1. Calculate K< = h(K).

// 2. Read d, the depth of the directory.

// 3. Take the initial d bits of K<, interpret them as an integer base 2, and
// call this number.

// 4. ket u be the length in bytes of the region (one for each pointer in the
// directory) that tells the numbber of entries on that leaf page. If this
// information is not being stored in the directory then let v = 0.

// 5. Find the pointer that is located r(Z + u) bytes from the start of the
// nonheader portion of the directory, when 1 is the length of each pointer in
// bytes.

// 6. Follow this pointer to a leaf page P.

// 7. Use the trailing s bits of the pseudokey to hash onto leaft page P (where
// s is a fixed, system-determined parameter).

// 8. If necessary, follow the collision-resolution scheme within page P.
// ----------------------------------------------------------------

ExtendibleHash::queryResult_t ExtendibleHash::search(
    ExtendibleHash::key_t key) {
  hash_t nk = keyToHash(key);
  size_t index = hashToIndex(nk);
  bucket* buc = pool.fetch(directory[index]);
  return buc->buffer[key];
}

size_t ExtendibleHash::hashToIndex(hash_t h) {
  size_t mask = ~(1 << (sizeof(size_t) - globalDepth));
  size_t index = h & mask;
}

// ----------------------------------------------------------------
// INSERT (given (Ko, I(Ko)))
// 1. Apply the first seven steps of ACCESS, using key Ko.

// 2. If by inserting key KO on leaf page P, we would exceed our threshold, then
// go to Step 7.

// 3. If there is sufficient free space at the location calculated at the end of
// Step 1, then insert (Ko, I(Ko)) there.

// 4. Otherwise, follow the collision-resolution scheme to insert (Ko, I(Ko)) on
// leaf page P, if this is possible.

// 5. (Optional) For each directory pointer that points to page P, increment by
// one the entry that tells the number of entries on the leaf page.

// 6. If (Ko, I(Ko)) has been successfully inserted, then stop.

// 7. At this point, we know there is not sufficient free space on page P.
// Obtain a new page P* to use as a leaf page.

// 8. Obtain a temporary area Q to store all (K, I(K)) pairs that appeared on
// page P, along with the new (Ko, I(Ko)).

// 9. Set the local depth of each of P and P* to d’ + 1, where d’ is the old
// local depth of P.

// 10. Erase all nonheader information from page P.

// 11. If the new local depth of P is bigger than the current directory depth,
// then do the following.
// a. Increase the depth of the directory by one.
// b. Double the size of the directory, and update the pointers in the obvious
// manner.
// c. (Optional) Set to zero the entry giving the number of entries on
// the leaf pages P and P*.

// 12. INSERT all (K, I(K)) pairs one at a time from the temporary area Q.

// Note that the INSERT routine can (repeatedly) call itself recursively (in
// Step 12).

// ----------------------------------------------------------------

void ExtendibleHash::add(recordMeta meta, key_t key) {
  hash_t nk = keyToHash(key);
  size_t index = hashToIndex(nk);
  auto oid = directory[index];
  bucket* buc = pool.fetch(oid);
  if (buc->add(key, meta)) return;
  buc->localDeph++;
  pool_t::bucketId_t nid = pool.create();
  bucket* nbuc = pool.fetch(nid);
  nbuc->localDeph = buc->localDeph;
  auto buff = std::move(buc->buffer);
  if (buc->localDeph > globalDepth) doubleCapacity();
  size_t nbindex =
      directory[index << 1] == oid ? (index << 1) : (index << 1) + 1;
  directory[nbindex] = nid;

  add(meta, key);
  for (auto& it : buff) {
    add(it.second, it.first);
  }
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

// ----------------------------------------------------------------
// DELETE (given Ko)
// 1. ACCESS, using Ko.

// 2. If K. does not appear, then stop (and send the appropriate return code).

// 3. Delete by writing the deleted sign over the entry or by unchaining,
// depending on the collision-resolution strategy.

// 4. (Optional) If the sum of the number of entries on this page and its
// sibling page are below the threshold, then coalesce these two pages as
// follows:
//  a. Copy all (K, I(K)) entries from these two pages into a temporary region
//     Q.
//  b. Throw away (i.e. return to free space) one of the two pages. Make all
//     pointers that point to it point to the remaining page.
//  c. Decrement the depth on the remaining page P by one.
//  d. Erase all (K, I(K)) entries on page P.
//  e. Set to zero the “number of entries on page” values associated with all
//     pointers to P.
//  f. INSERT all (K, I(K)) pairs one at a time from the temporary area Q.

// 5. (Optional) If every pointer in the directory equals its sibling pointer,
// then do the following:
//   a. Decrease the depth of the directory by one.
//   b. Halve the size of the directory, and update the pointers in the obvious
//      manner.
// ----------------------------------------------------------------

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
  if (!(info.good() && data.good())) throw "can't index unexisting files";
  GenRecordInfo tempInfo;
  size_t off = 0;
  while (info >> tempInfo) {
    Record* rec = tempInfo.allocate(1);
    tempInfo.read(rec, 1, data);
    add({off, tempInfo}, getKey(tempInfo.at(rec, 0), tempInfo, keyPos));
    off += tempInfo.getSize();
    tempInfo.deallocate(rec);
  }
}

ExtendibleHash::key_t ExtendibleHash::getKey(Record rec, GenRecordInfo info,
                                             size_t keyPos) {
  char* field = nullptr;
  info.field(rec, keyPos, field);

  return key_t(field, field + info.fieldSize(keyPos));
}