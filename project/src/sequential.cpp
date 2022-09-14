#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

struct Record {
  string Name;
  double Earnings;
  int Year;
  string Sport;
};

struct DataRecord {
double Earnings;
int pos;
int next;
//int file; // 0->datos.dat  1->auxiliar.dat
};

void init(){
  ifstream file("forbesathletesv2.csv",ios::binary );
  ofstream file2;
  file2.open("datos.dat",ios::binary);
  string line;
  int pos = 0;
  int siguiente = 0;
  int archivo = 0;
  while(getline(file,line)){
    Record record;
    char *data = const_cast<char *>(line.c_str());
    record.Name = strtok(data, ",");
    record.Earnings = stod(strtok(NULL, ","));
    record.Year = stoi(strtok(NULL, ","));
    record.Sport = strtok(NULL, "\r");
    siguiente = file.tellg();
    file2.write((char*)&(record.Earnings), sizeof(double));
    file2.write((char*)&pos, sizeof(int));
    file2.write((char*)&siguiente, sizeof(int));
    //file2.write((char*)&archivo, sizeof(int));
    pos=siguiente;
  }
  file2.close();
}

void readFile(){
  ifstream file2("datos.dat",ios::binary);

while(file2.peek() != EOF){
  DataRecord record;
  file2.read((char*)&record,sizeof(DataRecord));
  cout<<record.Earnings<<" "<<record.pos<<" "<<record.next<<endl;
}
}
DataRecord readRecord(int pos){
  ifstream file2("datos.dat",ios::binary);
  file2.seekg(pos*sizeof(DataRecord),ios::beg);
  DataRecord record;
  file2.read((char*)&record,sizeof(DataRecord));
  return record;
}

class SequentialFile {
  ofstream outFile;
  ifstream inFile;
  string fileName;
  string fileAux;

public:
  SequentialFile(string fileName) : fileName(fileName) {}
  vector<DataRecord> search(double key) {
    vector<DataRecord> ans;
    fstream file;
    file.open(fileName, ios::binary|ios::ate|ios::app );
    int l = 0;
    int r = file.tellp()/sizeof(DataRecord) - 1;
    cout<<r<<endl;
    while (l <= r) {
      int mid = l + (r - l) / 2;
      DataRecord re = readRecord(mid);
      cout<<re.Earnings<<endl;
      if (re.Earnings == key) {
        ans.push_back(re);
        return ans;
      } else if (re.Earnings < key) {
        r = mid - 1;
      } else {
        l = mid + 1;
      }
    }
    return ans;
  }

  bool add(Record registro) {

  }
  bool remove(int key) {}

  //******Funciones auxiliares*****
  // Retorna el numero de registros del aux
  int sizeAux() { return 0; }

  // reconstruye los archivos cuando sizeAux llega a K
  void reconstruction() { return 0; }
};
int main() {
  init();
  SequentialFile sf("datos.dat");
  auto ans=sf.search(130);
  cout<<ans.size()<<endl;
  }
/*https://www.kaggle.com/datasets/darinhawley/forbes-high-paid-athletes-19902021
*/
