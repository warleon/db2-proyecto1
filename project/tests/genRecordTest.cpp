#include <gtest/gtest.h>

#include <genRecord.hpp>

TEST(GenRecord, Test_0) {
  dtype types[7] = {dtype::int8,    dtype::int16,   dtype::int32,  dtype::int64,
                    dtype::float32, dtype::float64, dtype::float96};
  size_t sizes[7] = {1, 2, 3, 4, 5, 6, 7};
  GenRecordInfo(types, sizes, 7);
}