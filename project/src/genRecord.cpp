#include "genRecord.hpp"

#include <exception>
#include <utility>
void GenRecordInfo::constructorCheckConditions() {
  if (fieldItemsCount.size() != fieldType.size()) {
    throw std::runtime_error(
        "the size of field type and field items count must be the same");
  }
  if (!fieldItemsCount.size()) {
    throw std::runtime_error("Can't accept a 0 fields record.");
  }
}

GenRecordInfo::GenRecordInfo(dtypeSet_t fixLenTypes_, sizeSet_t fixLenCount_)
    : fieldType(fixLenTypes_), fieldItemsCount(fixLenCount_) {
  constructorCheckConditions();
  fieldCount = fieldItemsCount.size();
  computeSizeAndOffsets();
}

GenRecordInfo::GenRecordInfo(dtypeSet_t&& fixLenTypes_,
                             sizeSet_t&& fixLenCount_)
    : fieldType(fixLenTypes_), fieldItemsCount(fixLenCount_) {
  constructorCheckConditions();
  fieldCount = fieldItemsCount.size();
  computeSizeAndOffsets();
}
GenRecordInfo::GenRecordInfo(dtype* dtypeBuffer, size_t* sizeBuffer,
                             size_t fieldCount_)
    : fieldCount(fieldCount_) {
  fieldType = dtypeSet_t(dtypeBuffer, dtypeBuffer + fieldCount);
  fieldItemsCount = sizeSet_t(sizeBuffer, sizeBuffer + fieldCount);
  computeSizeAndOffsets()
}

void GenRecordInfo::computeSizeAndOffsets() {
  fieldOffset.resize(fieldCount);
  fieldOffset[0] = 0;
  for (size_t i = 1; i < fieldCount; i++) {
    fieldOffset[i] += dtypeSize[fieldType[i - 1]] * fieldItemsCount[i - 1];
  }
  size = fieldOffset[fieldCount - 1] +
         dtypeSize[fieldType[fieldCount - 1]] * fieldItemsCount[fieldCount - 1];
}

void GenRecordInfo::fetchOne(Record record, std::ifstream& in) {
  in.read(record, size);
}
void GenRecordInfo::fetchMany(Record* records, size_t n, std::ifstream& in) {
  in.read((char*)records, size * n);
}
void GenRecordInfo::writeInfo(std::ofstream& infoFile,
                              std::ofstream& metaFile) {
  metaFile.write((char*)&size, sizeof(size_t));
  infoFile.write((char*)fieldType.data(), sizeof(dtype) * fieldType.size());
  infoFile.write((char*)fieldItemsCount.data(),
                 sizeof(size_t) * fieldItemsCount.size());
}
GenRecordInfo GenRecordInfo::readInfo(std::ifstream& infoFile,
                                      std::ifstream& metaFile) {
  size_t fieldCount;
  metaFile.read((char*)&fieldCount, sizeof(size_t));
  dtype* dtypeBuffer = new dtype[fieldCount];
  size_t* sizeBuffer = new size_t[fieldCount];
  infoFile.read((char*)dtypeBuffer, sizeof(dtype) * fieldCount);
  infoFile.read((char*)sizeBuffer, sizeof(size_t) * fieldCount);
  return GenRecordInfo(dtypeBuffer, sizeBuffer, fieldCount);
}

size_t GenRecordInfo::getSize() { return size; }

void bytesToInt8(const char* from, char* to, size_t n) {
  memcpy(to, from, n * sizeof(char));
}
void bytesToInt16(const char* from, short int* to, size_t n) {
  memcpy(to, from, n * sizeof(short int));
}
void bytesToInt32(const char* from, int* to, size_t n) {
  memcpy(to, from, n * sizeof(int));
}
void bytesToInt64(const char* from, long int* to, size_t n) {
  memcpy(to, from, n * sizeof(long int));
}
void bytesToFloat32(const char* from, float* to, size_t n) {
  memcpy(to, from, n * sizeof(float));
}
void bytesToFloat64(const char* from, double* to, size_t n) {
  memcpy(to, from, n * sizeof(double));
}
void bytesToFloat96(const char* from, long double* to, size_t n) {
  memcpy(to, from, n * sizeof(long double));
}