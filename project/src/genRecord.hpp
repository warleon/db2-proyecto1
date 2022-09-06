#include <cstring>
#include <memory>
#include <string>
//#include <unordered_map>
#include <fstream>
#include <vector>

enum dtype : char { int8 = 0, int16, int32, int64, float32, float64, float96 };

const size_t dtypeSize[] = {
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
  ~GenRecordInfo() {}

  // use when reading records of different sizes; ensure that the file is valid
  // and its cursor is in the correct position
  void fetchOne(Record, std::ifstream&);

  // use when multiple record is of the same size; ensure that the file is valid
  // and its cursor is in the correct position
  void fetchMany(Record*, size_t, std::ifstream&);

  // reads the information needed to interpret the record data needs a metadata
  // file and the file where previus information were wrote
  static GenRecordInfo readInfo(std::ifstream&, std::ifstream&);
  // writes the information needed to interpret the record data
  void writeInfo(std::ofstream&, std::ofstream&);

  // list of types of every field
  const dtypeSet_t fieldType;
  // number of items in the field
  const sizeSet_t fieldItemsCount;

  size_t getSize();

 private:
  size_t size;
  size_t fieldCount;
  sizeSet_t fieldOffset;
  void computeSizeAndOffsets();
  void constructorCheckConditions();
};