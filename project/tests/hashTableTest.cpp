#include <gtest/gtest.h>

#include <fstream>
#include <genRecord.hpp>
#include <hashTable.hpp>
#include <utility>

const std::string infoFilePath = "/data/tests/info.info";
const std::string dataFilePath = "/data/tests/data.data";
const fs::path hashHome("/data/tests/index");

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
  std::ofstream infoOut(dataFilePath, std::ios::binary);
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
  std::ofstream infoOut(dataFilePath, std::ios::binary);
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