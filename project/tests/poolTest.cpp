#include <gtest/gtest.h>

#include <bucket.cpp>
#include <bucketPool.cpp>

using bucket_t = bucket<size_t>;
using pool_t = BucketPool<bucket_t>;

std::string poolDir = "/data/poolTest";

TEST(BucketPool, constructorTest_0) { pool_t pool(1000, 10, poolDir); }
TEST(BucketPool, createTest_0) {
  pool_t pool(1000, 10, poolDir);
  pool.create();
}