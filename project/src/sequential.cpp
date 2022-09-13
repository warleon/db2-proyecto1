#include "sequential.hpp"


#include <climits>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

// Numero maximo de registros en el aux
#define K 10;

struct Record {
  string Name;
  double Earnings;
  int Year;
  string Sport;
  int nextDel;
};

ostream &operator<<(ostream &stream, Record &record) {
  stream.write(record.nombre, sizeof(record.nombre));
  stream.write(record.codigo, sizeof(record.codigo));
  stream.write(record.apellidos, sizeof(record.apellidos));
  stream.write(record.carrera, sizeof(record.carrera));
  stream << "\n";
  stream << flush;
  return stream;
}
istream &operator>>(istream &stream, Record &record) {
  stream.read(record.nombre, sizeof(record.nombre));
  stream.read(record.codigo, sizeof(record.codigo));
  stream.read(record.apellidos, sizeof(record.apellidos));
  stream.read(record.carrera, sizeof(record.carrera));
  stream.get();
  record.codigo[4] = '\0';
  record.nombre[10] = '\0';
  record.apellidos[19] = '\0';
  record.carrera[14] = '\0';
  return stream;
}

class SequentialFile {
  ofstream outFile;
  ifstream inFile;
  string fileName;
  string fileAux;

public:
  SequentialFile(string fileName) : fileName(fileName) {}
  vector<Record> search(int key) {
    fstream file;
    file.open(fileName, ios::out);
    int l = 0;
    int u = file.tellp();
    while (l <= u) {
      int mid = l + (u - l) / 2;
      Record re = readRecord(mid);
      if (re.Year == key) {
        return mid;
      } else if (re.year < key) {
        l = mid + 1;
      } else {
        r = mid - 1;
      }
      return mid;
    }
  }

  Record readRecord(int pos) {
    ifstream File(this->fileName, ios::in);
    if (!File.is_open())
      cout << "No se pudo abrir el archivo. \n";
    string line;
    getline(File, line);
    for (int i = 0; i < pos; ++i)
      getline(File, line);
    Record record;
    char *data = const_cast<char *>(line.c_str());
    record.Name = strtok(data, ",");
    record.Earnings = strtok(NULL, ",");
    record.Year = strtok(NULL, ",");
    record.Sport = atoi(strtok(NULL, ","));
    return record;
  }

  bool add(Record registro) {
    
  }
  bool remove(T key) {}

  //******Funciones auxiliares*****
  // Retorna el numero de registros del aux
  int sizeAux() { return 0; }

  // reconstruye los archivos cuando sizeAux llega a K
  void reconstruction() { return 0; }
};

int main() { std::cout << "Hello World!\n"; }

// https://www.kaggle.com/datasets/darinhawley/forbes-high-paid-athletes-19902021
*/
