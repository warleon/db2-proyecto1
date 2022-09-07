#include <gtest/gtest.h>

#include <fstream>
#include <genRecord.hpp>
#include <utility>

const std::string metaFilePath = "/data/info.meta";
const std::string infoFilePath = "/data/info.info";

dtype types[7] = {dtype::int8,    dtype::int16,   dtype::int32,  dtype::int64,
                  dtype::float32, dtype::float64, dtype::float96};
size_t sizes[7] = {1, 2, 3, 4, 5, 6, 7};

auto typevec = GenRecordInfo::dtypeSet_t(types, types + 7);
auto sizevec = GenRecordInfo::sizeSet_t(sizes, sizes + 7);
auto typevecref = std::move(GenRecordInfo::dtypeSet_t(types, types + 7));
auto sizevecref = std::move(GenRecordInfo::sizeSet_t(sizes, sizes + 7));

TEST(GenRecord, constructorTest_0) { GenRecordInfo(&types[0], &sizes[0], 7); }
TEST(GenRecord, constructorTest_1) { GenRecordInfo(typevec, sizevec); }
TEST(GenRecord, constructorTest_2) { GenRecordInfo(typevecref, sizevecref); }
TEST(GenRecord, writeReadInfoTest_0) {
  GenRecordInfo baseRecord(typevec, sizevec);
  std::ofstream metaOut(metaFilePath, std::ofstream::binary);
  std::ofstream infoOut(infoFilePath, std::ofstream::binary);
  baseRecord.writeInfo(infoOut, metaOut);
  metaOut.close();
  infoOut.close();
  std::ifstream metaIn(metaFilePath, std::ofstream::binary);
  std::ifstream infoIn(infoFilePath, std::ofstream::binary);
  GenRecordInfo::readInfo(infoIn, metaIn);
}