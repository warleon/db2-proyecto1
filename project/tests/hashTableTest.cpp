#include <gtest/gtest.h>

#include <cstring>
#include <fstream>
#include <genRecord.hpp>
#include <hashTable.hpp>
#include <iostream>
#include <utility>

const fs::path hashHome("/data/tests/hashTest/index");
const fs::path infoFilePath = hashHome / "info.info";
const fs::path dataFilePath = hashHome / "data.dat";

dtype types[2] = {dtype::int8, dtype::int32};
size_t sizes[2] = {5, 5};
// record values
char a[5] = "hola";
int b[5] = {1, 2, 3, 4, 5};
GenRecordInfo info(types, sizes, 2);

void setData(Record rec, GenRecordInfo info) {
  memcpy(rec, a, 5);
  char* uu = nullptr;
  info.field(rec, 1, uu);
  memcpy(uu, b, 20);
}
TEST(HashTable, constructorTest_0) {
  ExtendibleHash index(hashHome / "constTest");
}

TEST(HashTable, indexTest_0) {
  ExtendibleHash index(hashHome / "test0");
  auto recs = info.allocate(2);
  setData(info.at(recs, 0), info);
  setData(info.at(recs, 1), info);
  std::ofstream data(dataFilePath, std::ios::binary);
  EXPECT_TRUE(data.good()) << data.rdstate();
  std::ofstream infoOut(infoFilePath, std::ios::binary);
  EXPECT_TRUE(infoOut.good()) << infoOut.rdstate();
  info.write(recs, 2, data);
  infoOut << info << info;
  infoOut.close();
  data.close();
  index.index(infoFilePath, dataFilePath, 0);
  info.deallocate(recs);
}
TEST(HashTable, indexTest_1) {
  ExtendibleHash index(hashHome / "test1");
  auto recs = info.allocate(2);
  setData(info.at(recs, 0), info);
  setData(info.at(recs, 1), info);
  std::ofstream data(dataFilePath, std::ios::binary);
  EXPECT_TRUE(data.good()) << data.rdstate();
  std::ofstream infoOut(infoFilePath, std::ios::binary);
  EXPECT_TRUE(infoOut.good()) << infoOut.rdstate();
  info.write(recs, 2, data);
  infoOut << info << info;
  infoOut.close();
  data.close();
  index.index(infoFilePath, dataFilePath, 1);
  info.deallocate(recs);
}

TEST(HashTable, searchTest_0) {
  ExtendibleHash index(hashHome / "test1");
  auto recs = info.allocate(2);
  setData(info.at(recs, 0), info);
  auto key = index.getKey(info.at(recs, 0), info, 1);
  auto meta = index.search(key);
  EXPECT_TRUE(meta.info.getSize() == info.getSize());
  std::ifstream data(dataFilePath, std::ios::binary);
  EXPECT_TRUE(data.good()) << data.rdstate();
  data.seekg(meta.pos);
  info.read((Record*)info.at(recs, 1), 1, data);
  EXPECT_EQ(strncmp(info.at(recs, 0), info.at(recs, 1), info.getSize()), 0);
  info.deallocate(recs);
}

TEST(HashTable, rangeSearchTest_0) {}

TEST(HashTable, removeTest_0) {}