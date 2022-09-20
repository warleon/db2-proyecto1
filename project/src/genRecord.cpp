#include "genRecord.hpp"

#include <exception>
#include <utility>

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

void GenRecordInfo::constructorCheckConditions() {
  if (fieldItemsCount.size() != fieldType.size()) {
    throw std::runtime_error(
        "the size of field type and field items count must be the same");
  }
  if (!fieldItemsCount.size()) {
    throw std::runtime_error("Can't accept a 0 fields record.");
  }
}

GenRecordInfo::GenRecordInfo() {}
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
GenRecordInfo::GenRecordInfo(const dtype* dtypeBuffer, const size_t* sizeBuffer,
                             size_t fieldCount_)
    : fieldType(dtypeBuffer, dtypeBuffer + fieldCount_),
      fieldItemsCount(sizeBuffer, sizeBuffer + fieldCount_) {
  constructorCheckConditions();
  fieldCount = fieldItemsCount.size();
  computeSizeAndOffsets();
}

void GenRecordInfo::computeSizeAndOffsets() {
  fieldOffset.resize(fieldCount);
  fieldOffset[0] = 0;
  for (size_t i = 1; i < fieldCount; i++) {
    fieldOffset[i] = fieldOffset[i - 1] +
                     dtypeSize[fieldType[i - 1]] * fieldItemsCount[i - 1];
  }
  size = fieldOffset[fieldCount - 1] +
         dtypeSize[fieldType[fieldCount - 1]] * fieldItemsCount[fieldCount - 1];
}

void GenRecordInfo::read(Record* records, size_t n, std::ifstream& in) {
  in.read((char*)records, size * n);
}
void GenRecordInfo::writeInfo(std::ofstream& infoFile,
                              std::ofstream& metaFile) {
  metaFile.write((char*)&fieldCount, sizeof(size_t));
  infoFile.write((char*)fieldType.data(), sizeof(dtype) * fieldType.size());
  infoFile.write((char*)fieldItemsCount.data(),
                 sizeof(size_t) * fieldItemsCount.size());
}
GenRecordInfo GenRecordInfo::readInfo(std::ifstream& infoFile,
                                      std::ifstream& metaFile) {
  size_t fieldCount = 0;
  metaFile.read((char*)&fieldCount, sizeof(size_t));
  dtype* dtypeBuffer = new dtype[fieldCount];
  size_t* sizeBuffer = new size_t[fieldCount];
  infoFile.read((char*)dtypeBuffer, sizeof(dtype) * fieldCount);
  infoFile.read((char*)sizeBuffer, sizeof(size_t) * fieldCount);
  GenRecordInfo res = GenRecordInfo(dtypeBuffer, sizeBuffer, fieldCount);
  delete[] dtypeBuffer;
  delete[] sizeBuffer;
  return res;
}

size_t GenRecordInfo::getSize() { return size; }

Record* GenRecordInfo::allocate(size_t n) {
  return (Record*)std::memset(new char[n * size], 0, size* n);
}
void GenRecordInfo::write(Record* records, size_t n, std::ofstream& out) {
  out.write((char*)records, n * size);
}
void GenRecordInfo::deallocate(Record* records) { delete[] records; }
void GenRecordInfo::field(Record record, size_t field, char*& fieldStart) {
  fieldStart = record + fieldOffset[field];
}
Record GenRecordInfo::at(Record* records, size_t i) {
  return ((Record)records) + i * size;
}
size_t GenRecordInfo::fieldSize(size_t j) {
  return dtypeSize[fieldType[j]] * fieldItemsCount[j];
}

std::ostream& operator<<(std::ostream& os, GenRecordInfo& info) {
  os.write((char*)&info.fieldCount, sizeof(size_t));
  os.write((char*)info.fieldType.data(), sizeof(dtype) * info.fieldType.size());
  os.write((char*)info.fieldItemsCount.data(),
           sizeof(size_t) * info.fieldItemsCount.size());
  return os;
}
std::istream& operator>>(std::istream& is, GenRecordInfo& info) {
  size_t fieldCount = 0;
  is.read((char*)&fieldCount, sizeof(size_t));
  dtype* dtypeBuffer = new dtype[fieldCount];
  size_t* sizeBuffer = new size_t[fieldCount];
  is.read((char*)dtypeBuffer, sizeof(dtype) * fieldCount);
  is.read((char*)sizeBuffer, sizeof(size_t) * fieldCount);
  info.fieldCount = fieldCount;
  info.fieldType = std::vector<dtype>(dtypeBuffer, dtypeBuffer + fieldCount);
  info.fieldItemsCount =
      std::vector<size_t>(sizeBuffer, sizeBuffer + fieldCount);

  delete[] dtypeBuffer;
  delete[] sizeBuffer;
  return is;
}