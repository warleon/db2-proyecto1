#include <gtest/gtest.h>

#include <genRecord.hpp>
#include <utility>

dtype types[7] = {dtype::int8,    dtype::int16,   dtype::int32,  dtype::int64,
                  dtype::float32, dtype::float64, dtype::float96};
size_t sizes[7] = {1, 2, 3, 4, 5, 6, 7};

auto typevec = GenRecordInfo::dtypeSet_t(types, types + 7);
auto sizevec = GenRecordInfo::sizeSet_t(sizes, sizes + 7);
auto typevecref = std::move(GenRecordInfo::dtypeSet_t(types, types + 7));
auto sizevecref = std::move(GenRecordInfo::sizeSet_t(sizes, sizes + 7));

TEST(GenRecord, constructorTest_0) { GenRecordInfo(types, sizes, 7); }
TEST(GenRecord, constructorTest_1) { GenRecordInfo(typevec, sizevec); }
TEST(GenRecord, constructorTest_2) { GenRecordInfo(typevecref, sizevecref); }