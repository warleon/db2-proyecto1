#include <gtest/gtest.h>

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
  std::cerr << "recs at test0 " << (void*)recs << std::endl;
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
  std::cerr << "recs at test1 " << (void*)recs << std::endl;
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

TEST(HashTable, searchTest_0) {}

TEST(HashTable, rangeSearchTest_0) {}

TEST(HashTable, removeTest_0) {}