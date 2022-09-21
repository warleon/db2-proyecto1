#include <gtest/gtest.h>

#include <cstring>
#include <csvReader.hpp>
#include <fstream>
#include <genRecord.hpp>
#include <hashTable.hpp>
#include <iostream>
#include <random>
#include <utility>

const fs::path hashHome("/data/tests/hashTest/index");
const fs::path infoFilePath = hashHome / "info.info";
const fs::path dataFilePath = hashHome / "data.dat";
const fs::path csvPath = "/data/forbesathletesv2.csv";

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

TEST(HashTable, removeTest_0) {
  ExtendibleHash index(hashHome / "test1");
  auto recs = info.allocate(1);
  setData(info.at(recs, 0), info);
  auto key = index.getKey(info.at(recs, 0), info, 1);
  info.deallocate(recs);
  EXPECT_NO_THROW(index.remove(key));
}
TEST(HashTable, removeTest_1) {
  ExtendibleHash index(hashHome / "test1");
  auto recs = info.allocate(1);
  setData(info.at(recs, 0), info);
  auto key = index.getKey(info.at(recs, 0), info, 1);
  info.deallocate(recs);
  EXPECT_ANY_THROW(index.remove(key));
}
TEST(HashTable, resizeTest_0) {
  CSV csv(csvPath);
  std::ofstream info(infoFilePath, std::ios::binary);
  std::ofstream data(dataFilePath, std::ios::binary);
  while (csv.parseLine(",")) {
    info << csv.lineInfo;
    data.write(csv.line, csv.lineInfo.getSize());
  }

  info.close();
  data.close();
  ExtendibleHash index(hashHome / "resize");
  index.index(infoFilePath, dataFilePath, 0);
}
TEST(HashTable, searchTest_1) {
  CSV csv(csvPath);
  ExtendibleHash index(hashHome / "resize");

  // copied from google
  std::random_device rd;   // obtain a random number from hardware
  std::mt19937 gen(rd());  // seed the generator
  std::uniform_int_distribution<size_t> distr(10, 100);  // define the range
  size_t recordIndex = distr(gen);
  for (size_t i = 0; i < recordIndex; i++) {
    csv.parseLine(",");
  }
  auto key = index.getKey(csv.line, csv.lineInfo, 0);
  std::cerr << "search for record " << recordIndex << std::endl;
  std::cerr << "record key = " << key << std::endl;
  auto meta = index.search(key);
  std::cerr << "found size " << meta.info.getSize() << std::endl;
  std::cerr << "expected size " << csv.lineInfo.getSize() << std::endl;
  EXPECT_TRUE(meta.info.getSize() == csv.lineInfo.getSize());
  std::ifstream data(dataFilePath, std::ios::binary);
  EXPECT_TRUE(data.good()) << data.rdstate();
  data.seekg(meta.pos);
  Record rec = (Record)csv.lineInfo.allocate(1);
  data.read(rec, csv.lineInfo.getSize());
  (std::cerr << "found record [|").write(rec, meta.info.getSize())
      << "|]" << std::endl;
  (std::cerr << "expected record [|").write(csv.line, csv.lineInfo.getSize())
      << "|]" << std::endl;
  EXPECT_EQ(strncmp(rec, csv.line, csv.lineInfo.getSize()), 0);
  delete[] rec;
}