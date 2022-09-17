#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <string>
using namespace std;
#define AuxMax 5

struct Record{
  string Name;
  float Earnings;
  int Year;
  string Sport;
  Record(string n, float e, int y, string s){
    Name = n;
    Earnings = e;
    Year = y;
    Sport = s;
  }
};

struct DataRecord{
  float Earnings; // key  
  int pos;  // posicion fisica en el archivo csv
  int next; //posicion fisica del siguiente en el datos.dat o auxiliar.dat y no en el csv
  int file; // 0-> datos.dat  1-> auxiliar.dat del next
  int eliminado; // 0-> False  1-> True
  DataRecord (){}
  DataRecord(float earnings, int p =0, int n=0, int f=0, int e=0){
    Earnings = earnings;
    pos = p;
    next = n;
    file = f;
    eliminado = e;
  }

  void print(){
    cout<<Earnings<<" "<<pos<<" "<<next<<" "<<file<<" "<<eliminado<<endl;  
  }
};


class SequentialFile{
public:
  string fileName;
  string fileAux;
  string fileCsv;

  int eliminados;   // Cantidad de eliminados 
  int sizeAux;      // Cantidad de elementos en el fileAux
  int sizeFile;     // Cantidad de elementos en el fileName
  int header;       // Primer registro
  int file_header;  // Archivo donde esta el primer registro 0-> datos.dat  1-> auxiliar.dat


public:
  //******Funciones principales*****
  SequentialFile();
  bool add(DataRecord& registro);

  //******Funciones auxiliares*****

  // Actualiza los punteros de los registros
  void actualizarPunteros(){
    cout<<"\n Falta actualizar los punteros";
  }

  // reconstruye los archivos cuando sizeAux llega a K
  void reconstruction() { 
    sizeAux = 0;
    cout<<"\n Falta Reconstruccion";
  }
  // Retorna el numero de registros en el Aux
  int records(string file_n){
    fstream file(file_n, ios::binary | ios::in | ios::out);
    file.seekg(0,ios_base::end);
    int num_registros = file.tellg();
    if(file_n==fileName){
      num_registros -= 8;
    }
    num_registros = num_registros / sizeof(DataRecord);
    file.close();
    return num_registros;
  }


  // Devuelve el numero de eliminados que hay en el file
  int num_eliminados(string File)  {
    int elim = 0;
    ifstream file2(File,ios::binary);
    file2.seekg(8,ios_base::beg); 
    DataRecord record;  
    while(file2.peek() != EOF){  
      record = DataRecord();  
      file2.read((char*)&record,sizeof(DataRecord));  
      if(record.eliminado == 1) 
        elim++;  
    }         
    return elim;  
  }

  // Actualizar los headers ante posibles cambios
  void actualizar_header(int h_nuevo ,int f_nuevo){
    fstream file(fileName,ios::binary | ios::in | ios::out);
    file.seekg(0,ios_base::beg);
    file.write((char*)&h_nuevo,sizeof(int));
    file.write((char*)&f_nuevo,sizeof(int));
    header = h_nuevo;
    file_header = f_nuevo;
  }
};


// constructor SequentialFile
SequentialFile::SequentialFile(){
  fileName = "datos.dat";
  fileAux = "auxiliar.dat";
  fileCsv = "forbesathletesv2.csv";
  // En el constructor tener inicializado el número de eliminados que hay
  eliminados = num_eliminados("datos.dat");
  sizeAux = records(fileAux);
  sizeFile = records(fileName);
  ifstream file(fileName,ios::binary);
  file.read((char*)&header,sizeof(int));
  file.read((char*)&file_header,sizeof(int));
};


bool SequentialFile::add(DataRecord& registro){
  //Localizar la posición en donde será insertado el nuevo registro.
      //Si el espacio está libre, insertar.
      //Sino, insertar el registro en un espacio auxiliar.
      //En este caso, los punteros deberían ser actualizados.
  //Se requiere reorganizar el archivo original cada cierto tiempo mezclando ordenadamente con el espacio auxiliar.
  
  
  if(sizeAux == AuxMax){
    cout<<"Reconstruccion\n";
    reconstruction(); // Reconstruimos el datos.dat y el auxiliar.dat con una busqueda sequencial
    ofstream file(fileAux, ios::binary);
    file.seekp(0,ios_base::end);
    file.write((char*)&registro,sizeof(registro));
    actualizarPunteros();
    sizeAux++;
    return true;
  }
  else{
    if(sizeAux==0){  // Insertamos en el auxiliar.dat y luego actualizamos los punteros con ayuda de la busqueda binaria
      cout<<"Cero eliminados\n";
      ifstream file1(fileName, ios::binary);
      ofstream file2(fileAux, ios::binary | ios::app);
      file1.seekg(0,ios_base::end);
      DataRecord temp;        
      int inicial_register = 0;                                             //  Posicion del registro inicial (en n. de registros)
      int final_register = sizeFile-1;                                      //  Posicion del registro final (en n. de registros)
      int inicial_pos = 8;                                                  //  Posicion inicial en bytes tomando el cuenta el header
      int final_pos = (file1.tellg());                                      //  Posicion final en bytes
      int pos;                                                              //  Iterados de registros en bytes
      float key;                                                            //  La key a comparar
      int actualizar_registro;                                              //  Registro final actualizar los punteros (posicion en bytes)
      pos = (((inicial_register+final_register)/2)*sizeof(DataRecord))+8;   //  La posicion del registro medio en bytes     
      file1.seekg(pos,ios_base::beg);
      file1.read((char*)&temp,sizeof(DataRecord));
      key = registro.Earnings;
      // Si la key es menor el final pos disminuye en 1
      if(key>temp.Earnings){
        do{
          actualizar_registro=pos;
          final_register = (inicial_register+final_register)/2;
          pos = (((inicial_register+final_register)/2)*sizeof(DataRecord))+8;
          file1.seekg(pos,ios_base::beg);
          file1.read((char*)&temp,sizeof(DataRecord));
          if(actualizar_registro==8)
            break;
        }while(key>temp.Earnings);
        if(actualizar_registro==8){ 
          actualizar_header(0,1);
          registro.file = 0;
          registro.next = 8;
          registro.print();
          file2.write((char*)&registro,sizeof(registro));
        }
      }
      /*
      // si la key es mayor o igual 
      else{

      }
      */
      sizeAux++;
      return true;
    }
    else{ // Hay eliminados y pada actualizar los punteros tenemos que usar la buqueda sequencial
      cout<<"Hay eliminados\n";
      sizeAux++;
      return true;
    }
  }


}

void init(){
  ifstream file("forbesathletesv2.csv",ios::binary );
  ofstream file2("datos.dat",ios::binary);
  string line;
  int pos = 0;
  int pos_siguiente = 1; //Para la posicion fisica siguiente en el .dat y no en el csv
  int siguiente; // Posicion del siguiente
  int archivo = 0;
  int eliminado = 0;
  int inicial = 8 ;     // Para colocar en la cabecera el primer registro
  int file_inicial = 0; // Para colocar en que file comienza 0->datos.dat o 1->auxiliar.dat
  file2.write((char*)&inicial,sizeof(int));
  file2.write((char*)&file_inicial,sizeof(int));
  while(getline(file,line)){ 
    Record record;
    char *data = const_cast<char *>(line.c_str());
    record.Name = strtok(data, ",");
    record.Earnings = stod(strtok(NULL, ","));
    record.Year = stoi(strtok(NULL, ","));
    record.Sport = strtok(NULL, "\r");
    siguiente = (pos_siguiente*sizeof(DataRecord))+8;  // Sumandole el header
    file2.write((char*)&record.Earnings, sizeof(float));
    file2.write((char*)&pos, sizeof(int));
    file2.write((char*)&siguiente, sizeof(int)); //El siguiente en la posicion fisica en el datos.dat o auxiliar.dat
    file2.write((char*)&archivo, sizeof(int));
    file2.write((char*)&eliminado, sizeof(eliminado));
    cout<<record.Earnings<<" "<<pos<<" "<<siguiente<<" "<<archivo<<" "<<eliminado<<endl;
    pos=file.tellg();
    pos_siguiente++;
  }
  // Fijar el ultimos next en -1
  int ultimo = -1;
  file2.seekp(-12,ios_base::end); // sizeof(next) + sizeof(file) + sizeof(eliminado)
  file2.write((char*)&ultimo,sizeof(int));
  file.close();
  file2.close();
}

void show_date(string file){
  //Recorrer el datos.dat
  ifstream file2(file,ios::binary);
  DataRecord record;
  if(file=="datos.dat")
  {  
    // Leemos la cabecera
    int header;
    int file_header;
    file2.read((char*)&header,sizeof(header));
    file2.read((char*)&file_header,sizeof(file_header));
    cout<<header<<" "<<file_header<<endl;
  }
  while(file2.peek() != EOF){
    record = DataRecord();
    file2.read((char*)&record,sizeof(DataRecord));
    record.print();
  }
}


int main(){ 
 
 /*
 init();
 cout<<endl<<endl<<endl;
 show_date("datos.dat");
 cout<<endl<<endl<<endl;
 show_date("auxiliar.dat");
 */

//cout<<seq.eliminados<<endl<<seq.sizeAux<<endl<<seq.header<<endl<<seq.file_header<<endl;
//  cout<<seq.header<<" "<<seq.file_header;

  //init();

  Record registro1("Eros",190,2021,"Computacion");
  /*
  show_date("datos.dat");
  SequentialFile seq;
  DataRecord record(190);
  seq.add(record);
  show_date("datos.dat");
  show_date("auxiliar.dat");
  */
}