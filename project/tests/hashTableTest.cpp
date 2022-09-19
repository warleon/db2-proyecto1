#include <gtest/gtest.h>

#include <fstream>
#include <genRecord.hpp>
#include <hashTable.hpp>
#include <utility>

const std::string infoFilePath = "/data/info.info";
const std::string dataFilePath = "/data/data.data";
const fs::path hashHome("/data/index");

dtype types[2] = {dtype::int8, dtype::int32};
size_t sizes[2] = {5, 5};
// record values
char a[5] = "hola";
int b[5] = {1, 2, 3, 4, 5};
GenRecordInfo info(types, sizes, 2);

TEST(HashTable, constructorTest_0) { ExtendibleHash index(hashHome); }

TEST(HashTable, searchTest_0) {}

TEST(HashTable, rangeSearchTest_0) {}

TEST(HashTable, addTest_0) {}

TEST(HashTable, removeTest_0) {}