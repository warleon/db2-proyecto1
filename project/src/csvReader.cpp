#include "csvReader.hpp"

CSV::CSV(std::string fn)
    : record(""), header(nullptr), line(nullptr), file(fn) {
  if (!file.good()) throw std::runtime_error("filename does not exists");
  readPage();
  parseLine();
  header = line;
  line = nullptr;
  headerInfo = lineInfo;
  lineInfo = {};
}
CSV::~CSV() {
  if (header) delete[] header;
  header = nullptr;
  if (line) delete[] line;
  line = nullptr;
}
void CSV::readPage() {
  memset(buffer, 0, pageSize);
  file.read(buffer, pageSize);
}
void CSV::parseLine(const char* sep) {
  if (line) {
    delete[] line;
    line = nullptr;
  }
  char* ptr = strtok(buffer, "\n");
  if (!ptr) {
    record += buffer;
    readPage();
    return parseLine(sep);
  } else {
    record = ptr;
  }
  // here a complete line has been readed into record

  std::vector<dtype> recType;
  std::vector<size_t> recSize;
  std::vector<char*> tokens;

  auto size = record.size();
  char* recBuff = strncpy(new char[size], record.c_str(), size);

  for (char* token = strtok(recBuff, sep); token;
       token = strtok(nullptr, sep)) {
    tokens.push_back(token);
    auto meta = parseToken(token);
    recType.push_back(meta.first);
    recSize.push_back(meta.second);
  }
  // build info
  lineInfo = GenRecordInfo(recType, recSize);
  // build line
  line = (Record)lineInfo.allocate(1);
  for (size_t i = 0; i < tokens.size(); i++) {
    auto token = tokens[i];
    castToken(lineInfo.field(line, i), token, i);
  }

  // reset record;
  record = "";
  delete[] recBuff;
}
std::pair<dtype, size_t> CSV::parseToken(char* token) {
  dtype type = dtype::int64;
  size_t size = 0;
  for (; token[size]; size++) {
    auto c = token[size];
    if (isalpha(c) || type == dtype::int8) {
      type = dtype::int8;
    } else if (isdigit(c) && (type == dtype::int64 || type == dtype::float64)) {
      ;
    } else if (c == '.') {
      if (type == dtype::int64)
        type = dtype::float64;
      else
        type = dtype::int8;
    }
  }

  return std::make_pair(type, type == dtype::int8 ? size : 1);
}
void CSV::castToken(char* field, char* token, size_t i) {
  auto size = lineInfo.fieldItemsCount[i];
  auto type = lineInfo.fieldType[i];
  long int dec;
  double dou;

  switch (type) {
    case dtype::int8:
      strncpy(field, token, size);
      break;
    case dtype::int64:
      dec = atol(token);
      strncpy(field, (char*)&dec, sizeof(dec));
      break;
    case dtype::float64:
      dou = atof(token);
      strncpy(field, (char*)&dou, sizeof(dou));
      break;

    default:
      break;
  }
}