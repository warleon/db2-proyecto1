#pragma once

#include <cstring>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

enum dtype : size_t {
  int8 = 0,
  int16 = 1,
  int32 = 2,
  int64 = 3,
  float32 = 4,
  float64 = 5,
  float96 = 6
};

const size_t dtypeSize[7] = {
    sizeof(char),  sizeof(short int), sizeof(int),        sizeof(long int),
    sizeof(float), sizeof(double),    sizeof(long double)};

typedef char* Record;

// reads a char[N] from a buffer
void bytesToInt8(const char* from, char* to, size_t n);
// reads a short int[N] from a buffer
void bytesToInt16(const char* from, short int* to, size_t n);
// reads a int[N] from a buffer
void bytesToInt32(const char* from, int* to, size_t n);
// reads a long int[N] from a buffer
void bytesToInt64(const char* from, long int* to, size_t n);
// reads a float[N] from a buffer
void bytesToFloat32(const char* from, float* to, size_t n);
// reads a double[N] from a buffer
void bytesToFloat64(const char* from, double* to, size_t n);
// reads a long double[N] from a buffer
void bytesToFloat96(const char* from, long double* to, size_t n);

//[General Record Information] Information about a record to know how many bytes
// to read and write  to de file
class GenRecordInfo {
 public:
  using dtypeSet_t = std::vector<dtype>;
  using sizeSet_t = std::vector<size_t>;

  GenRecordInfo(dtypeSet_t, sizeSet_t);
  GenRecordInfo(dtypeSet_t&&, sizeSet_t&&);
  GenRecordInfo(const dtype*, const size_t*, size_t);
  GenRecordInfo();
  ~GenRecordInfo() {}

  // record operations

  // allocates space for many contiguous Records with 0 as fields values
  Record* allocate(size_t);

  // frees a previus allocated space
  void deallocate(Record*);

  // writes n records to the specified file; ensure that the file is valid
  // and its cursor is at the correct position
  void write(Record*, size_t, std::ofstream&);

  // reads n contiguous records of the same type; ensure that the file is valid
  // and its cursor is at the correct position
  void read(Record*, size_t, std::ifstream&);

  // gets a reference to the n field of the record
  void field(Record, size_t, char*&);

  // gets the size of the field in bytes
  size_t fieldSize(size_t);

  // gets the ith record in a record array
  Record at(Record*, size_t);

  // records info operations

  // reads the information needed to interpret the record data needs a
  // metadata file and the file where previus information were wrote
  static GenRecordInfo readInfo(std::ifstream&, std::ifstream&);

  // writes the information needed to interpret the record data
  void writeInfo(std::ofstream&, std::ofstream&);

  size_t getSize();

  friend std::ostream& operator<<(std::ostream&, GenRecordInfo&);
  friend std::istream& operator>>(std::istream&, GenRecordInfo&);

  // list of types of every field
  dtypeSet_t fieldType;

  // number of items in the field
  sizeSet_t fieldItemsCount;

 private:
  size_t size;
  size_t fieldCount;
  sizeSet_t fieldOffset;
  void computeSizeAndOffsets();
  void constructorCheckConditions();
};