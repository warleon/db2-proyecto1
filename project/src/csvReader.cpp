#include "csvReader.hpp"

CSV::CSV(std::string fn, std::string sep)
    : record(""), header(nullptr), line(nullptr), file(fn) {
  if (!file.good()) throw std::runtime_error("filename does not exists");
  parseLine(sep.c_str());
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
bool CSV::parseLine(const char* sep) {
  if (line) {
    delete[] line;
    line = nullptr;
    lineInfo = {};
  }
  if (file.good())
    std::getline(file, record);
  else
    return false;

  std::vector<dtype> recType;
  std::vector<size_t> recSize;
  std::vector<char*> tokens;

  auto size = record.size();
  char* recBuff = strncpy(new char[size + 1]{}, record.c_str(), size);

  for (char* token = strtok(recBuff, sep); token;
       token = strtok(nullptr, sep)) {
    tokens.push_back(token);
    auto meta = parseToken(token);
    recType.push_back(meta.first);
    recSize.push_back(meta.second);
  }
  lineInfo = GenRecordInfo(recType, recSize);
  // build line
  line = (Record)lineInfo.allocate(1);
  for (size_t i = 0; i < tokens.size(); i++) {
    auto token = tokens[i];
    castToken(lineInfo.field(line, i), token, i);
  }

  delete[] recBuff;
  return true;
}
std::pair<dtype, size_t> CSV::parseToken(char* token) {
  dtype type = dtype::int64;
  size_t size = 0;
  for (; token[size]; ++size) {
    auto c = token[size];
    if (isalpha(c) || isblank(c) || type == dtype::int8) {
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

  return std::make_pair(type, type == dtype::int8 ? size + 1 : 1);
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