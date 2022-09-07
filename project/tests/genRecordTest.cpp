#include <gtest/gtest.h>

#include <fstream>
#include <genRecord.hpp>
#include <utility>

const std::string metaFilePath = "/data/info.meta";
const std::string infoFilePath = "/data/info.info";
const std::string dataFilePath = "/data/data.data";

dtype types[7] = {dtype::int8,    dtype::int16,   dtype::int32,  dtype::int64,
                  dtype::float32, dtype::float64, dtype::float96};
size_t sizes[7] = {5, 1, 1, 1, 1, 1, 1};
// record values
char a[5] = "hola";
short int b = 3;
int c = 42;
long int d = 9999999;
float e = 0.69;
double f = 0.123123;
long double g = 0.123123123123123;

auto typevec = GenRecordInfo::dtypeSet_t(types, types + 7);
auto sizevec = GenRecordInfo::sizeSet_t(sizes, sizes + 7);
auto typevecref = std::move(GenRecordInfo::dtypeSet_t(types, types + 7));
auto sizevecref = std::move(GenRecordInfo::sizeSet_t(sizes, sizes + 7));

TEST(GenRecord, constructorTest_0) { GenRecordInfo(&types[0], &sizes[0], 7); }
TEST(GenRecord, constructorTest_1) { GenRecordInfo(typevec, sizevec); }
TEST(GenRecord, constructorTest_2) { GenRecordInfo(typevecref, sizevecref); }
TEST(GenRecord, recordInfoOperationsTest_0) {
  GenRecordInfo baseRecord(typevec, sizevec);
  std::ofstream metaOut(metaFilePath, std::ofstream::binary);
  std::ofstream infoOut(infoFilePath, std::ofstream::binary);
  baseRecord.writeInfo(infoOut, metaOut);
  metaOut.close();
  infoOut.close();
  std::ifstream metaIn(metaFilePath, std::ofstream::binary);
  std::ifstream infoIn(infoFilePath, std::ofstream::binary);
  GenRecordInfo::readInfo(infoIn, metaIn);
  metaIn.close();
  infoIn.close();
}
TEST(GenRecord, allocFreeRecordTest_0) {
  GenRecordInfo base(typevec, sizevec);
  auto recs = base.allocate(100);
  base.deallocate(recs);
}
TEST(GenRecord, recordOperationsTest_0) {
  GenRecordInfo base(typevec, sizevec);
  std::ofstream dataOut(dataFilePath, std::ofstream::binary);
  std::ofstream metaOut(metaFilePath, std::ofstream::binary);
  std::ofstream infoOut(infoFilePath, std::ofstream::binary);
  base.writeInfo(infoOut, metaOut);
  Record* recs = base.allocate(2);
  auto rec0 = base.at(recs, 0);
  auto ptr = rec0;
  base.field(rec0, 0, ptr);
  memcpy(ptr, &a, base.fieldSize(0));
  base.field(rec0, 1, ptr);
  memcpy(ptr, &b, base.fieldSize(1));
  base.field(rec0, 2, ptr);
  memcpy(ptr, &c, base.fieldSize(2));
  base.field(rec0, 3, ptr);
  memcpy(ptr, &d, base.fieldSize(3));
  base.field(rec0, 4, ptr);
  memcpy(ptr, &e, base.fieldSize(4));
  base.field(rec0, 5, ptr);
  memcpy(ptr, &f, base.fieldSize(5));
  base.field(rec0, 6, ptr);
  memcpy(ptr, &g, base.fieldSize(6));
  base.write((Record*)rec0, 1, dataOut);
  dataOut.close();
  metaOut.close();
  infoOut.close();
  std::ifstream dataIn(dataFilePath, std::ofstream::binary);
  std::ifstream metaIn(metaFilePath, std::ofstream::binary);
  std::ifstream infoIn(infoFilePath, std::ofstream::binary);
  auto rec1 = base.at(recs, 1);
  dataIn.seekg(0, std::ios::beg);
  base.read((Record*)rec1, 1, dataIn);
  dataIn.close();
  metaIn.close();
  infoIn.close();
  EXPECT_EQ(strncmp(rec0, rec1, base.getSize()), 0);
  base.deallocate(recs);
}