#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <string>
#include <stdio.h>
#include <time.h>
using namespace std;
#define AuxMax 10
#define Header 16

struct Record{
  string Name;
  float Earnings;
  int Year;
  string Sport;
  Record (){}
  Record(string n, float e, int y, string s){
    Name = n;
    Earnings = e;
    Year = y;
    Sport = s;
  }

  void print(){
    cout<<Name<<" "<<Earnings<<" "<<Year<<" "<<Sport<<endl;  
  }
};

struct DataRecord{
  float Earnings; // key  
  int pos;  // posicion fisica en el archivo csv
  int next; //posicion fisica del siguiente en el datos.dat o auxiliar.dat y no en el csv
  int file; // 0-> datos.dat  1-> auxiliar.dat del next
  int eliminado; // 0->False 1->True 

  DataRecord (){}

  DataRecord(float earnings, int p =0, int n=0, int f=0, int e=0){
    Earnings = earnings;
    pos = p;
    next = n;
    file = f;
    eliminado = e;
  }

  DataRecord(Record& reg){
    fstream file("forbesathletesv2.csv", ios::binary | ios::in | ios::out);
    file.seekg(0,ios_base::end);
    int position = file.tellg();
    char delimitador = ',';
    char salto = '\n';
    //  Name
    char name_str[reg.Name.length()]; 
    strcpy(name_str, reg.Name.c_str());
    file.write(name_str, sizeof(name_str));
    file.write((char*)&delimitador, sizeof(delimitador));
    //  Earnings
    char earnings_str[to_string(reg.Earnings).length()]; 
    strcpy(earnings_str, to_string(reg.Earnings).c_str());
    file.write(earnings_str, sizeof(earnings_str));
    file.write((char*)&delimitador, sizeof(delimitador));
    //  Year
    char year_str[to_string(reg.Year).length()]; 
    strcpy(year_str, to_string(reg.Year).c_str());
    file.write(year_str, sizeof(year_str));
    file.write((char*)&delimitador, sizeof(delimitador));
    //  Sport
    char sport_str[reg.Sport.length()]; 
    strcpy(sport_str, reg.Sport.c_str());
    file.write(sport_str, sizeof(sport_str));
    file.write((char*)&salto, sizeof(salto));    
    Earnings = reg.Earnings;
    pos = position;
    next = 0;
    file = 0;
    eliminado = 0;
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

  int eliminados;     //  Cantidad de eliminados 
  int sizeAux;        //  Cantidad de elementos en el fileAux
  int sizeFile;       //  Cantidad de elementos en el fileName
  int header;         //  Primer registro
  int file_header;    //  Archivo donde esta el primer registro 0-> datos.dat  1-> auxiliar.dat
  int eliminado;      //  Primer Datarecord eliminado eliminado -1-> No hay
  int file_eliminado; //  El file del primer eliminado -1-> No hay 0-> datos.dat  1-> auxiliar.dat
public:
  //******Funciones principales*****
  SequentialFile();
  bool add(Record& reg);
  bool remove_register(float key);
  int search_data(int key);
  vector<Record> range_search(int key1, int key2);
  bool remove_register_binary(float key);

  int search_aux(int key);
  Record search(int key);
  pair<int,int> search_data_prev(int key);
  pair<int,int>search_data_next(int key);

  //******Funciones auxiliares*****

  // Actualiza los punteros de los registros
  void actualizarPunteros(){
    cout<<"\n Falta actualizar los punteros";
  }

  // reconstruye los archivos cuando sizeAux llega a K
  void reconstruction() { 
    ifstream file_name(fileName, ios::binary);
    ifstream file_aux(fileAux, ios::binary);
    ofstream file_new("datos2.dat", ios::binary);
    int pos_read = header;
    int file_read = file_header;
    DataRecord record;
    //Escribir cabecera del nuevo datos2.dat
    int temp;
    temp=16;
    file_new.seekp(0,ios::end);
    file_new.write((char*)&temp,sizeof(temp));
    temp=0;
    file_new.seekp(0,ios::end);
    file_new.write((char*)&temp,sizeof(temp));
    temp=-1;
    file_new.seekp(0,ios::end);
    file_new.write((char*)&temp,sizeof(temp));
    temp=-1;
    file_new.seekp(0,ios::end);
    file_new.write((char*)&temp,sizeof(temp));
    //Contador para el next
    int contador_next=36;
    while(pos_read!=-1 && file_read!=-1){
      //  Buscar en el datos.dat
      if(file_read==0){
        file_name.seekg(pos_read,ios::beg);
        file_name.read((char*)&record,sizeof(DataRecord));
        pos_read=record.next;
        file_read=record.file;
        record.file=0;
        record.next=contador_next;
        file_new.seekp(0,ios::end);
        file_new.write((char*)&record,sizeof(DataRecord));
      }
      //  Buscar en el auxiliar.dat
      else{
        file_aux.seekg(pos_read,ios::beg);
        file_aux.read((char*)&record,sizeof(DataRecord));
        pos_read=record.next;
        file_read=record.file;
        record.file=0;
        record.next=contador_next;
        file_new.seekp(0,ios::end);
        file_new.write((char*)&record,sizeof(DataRecord));
      }
      contador_next+=20;
      }
      // Fijar el ultimos next en -1
      int ultimo = -1;
      file_new.seekp(-12,ios_base::end); // sizeof(next) + sizeof(file) + sizeof(eliminado)
      file_new.write((char*)&ultimo,sizeof(int));

      //  Eliminar datos.dat, limpiar axiliar.dat y renombrar datos2.dat 
      ofstream ofs;
      ofs.open("auxiliar.dat", ios::out | ios::trunc);
      ofs.close();
      
      sizeAux=0;
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
  file.read((char*)&eliminado,sizeof(int));
  file.read((char*)&file_eliminado,sizeof(int));
};

//  Eliminar un registro del datos.dat segun la key
bool SequentialFile::remove_register(float key){
  fstream file(fileName, ios::binary | ios::in | ios::out);
  fstream file2(fileAux, ios::binary | ios::in | ios::out);
  int pos_buscar;
  file.seekg(0,ios::beg);
  file.read((char*)&pos_buscar,sizeof(int));
  int file_buscar;
  file.read((char*)&file_buscar,sizeof(int));
  int pos_auxiliar;
  int file_auxiliar;
  DataRecord temp;
  bool cambiar_anterior;
  DataRecord temp2;
  int pos_temp2;
  do{
    temp2 = temp;
    pos_temp2 = pos_auxiliar;
    pos_auxiliar = pos_buscar;
    file_auxiliar = file_buscar;
    if(file_auxiliar==0){
      file.seekg(pos_auxiliar,ios_base::beg);
      file.read((char*)&temp,sizeof(temp));
    }
    else{
      file2.seekg(pos_auxiliar,ios_base::beg);
      file2.read((char*)&temp,sizeof(temp));
    }
    pos_buscar = temp.next;
    file_buscar = temp.file;
  }while(key<temp.Earnings);
  temp2.next = temp.next;
  temp2.file = temp.file;
  if(key == temp.Earnings){
    if(pos_auxiliar == header){
      cambiar_anterior = true;
      actualizar_header(temp.next,temp.file);
    }
      
    int swap;
    swap = eliminado;
    eliminado = pos_auxiliar;
    temp.next = swap;
    swap = file_eliminado;
    file_eliminado = file_auxiliar;
    temp.file = swap;
    temp.eliminado = 1;
    
    if(file_buscar==0){
      file.seekp(pos_auxiliar,ios_base::beg);
      file.write((char*)&temp,sizeof(temp));
    }
    else{
      file2.seekp(pos_auxiliar,ios_base::beg);
      file2.write((char*)&temp,sizeof(temp));
    }
    // Actualizar los header de eliminar
    file.seekp(8,ios_base::beg);
    file.write((char*)&eliminado,sizeof(eliminado));
    file.write((char*)&file_eliminado,sizeof(file_eliminado));
    if(!cambiar_anterior){
      if(file_buscar==0){
        file.seekp(pos_temp2,ios_base::beg);
        file.write((char*)&temp2,sizeof(temp));
      }
      else{
        file2.seekp(pos_temp2,ios_base::beg);
        file2.write((char*)&temp2,sizeof(temp));
      }
    }
    return true;
  
  }
  else{
    cout<<"La key no existe\n";
    return false;
  }
}


int SequentialFile::search_data(int key){
  
   fstream file(fileName, ios::binary | ios::in | ios::out);
     DataRecord dr;
     int left=0;
     int right=sizeFile- 1;
     int mid=-1;
     while(left<=right){
       mid=left + (right-left)/2;
       file.seekg(mid*sizeof(DataRecord)+16,ios::beg);
       file.read((char*)&dr,sizeof(DataRecord));
       if(dr.Earnings==key){
        if(dr.eliminado==1) return -1;
        return mid;
       }
       else if(dr.Earnings > key){
         left=mid+1;
       }
       else{
        right=mid-1;
       }
     }
     return -1;

}

// devuelve la posicion del menor mas cercano para postriormente modificar sus punteros
pair<int,int> SequentialFile::search_data_prev(int key){  // retorna (pos_indice, flag de archivo)
   fstream file_name(fileName, ios::binary | ios::in | ios::out);
      
    // buscar ese valor en el aux.dat
     fstream file_aux(fileAux, ios::binary | ios::in | ios::out);
     DataRecord temp;
     int pos=-1;
     int val=-1e9;
     for(int i=0;i<sizeAux;i++){
       file_aux.seekg(i*sizeof(DataRecord),ios::beg);
       file_aux.read((char*)&temp,sizeof(DataRecord));
       if(temp.eliminado==1) continue;
       if(temp.Earnings<=key){
            if(val<temp.Earnings){
              val=temp.Earnings;
              pos=i;
            }
       }
      }
     
     
     DataRecord dr;
     int left=0;
     int right=sizeFile- 1;
     int mid=-1;
     int ans=sizeFile-1;
     // buscar ese valor en el datos.dat
     while(left<=right){
       mid=left + (right-left)/2;
       file_name.seekg(mid*sizeof(DataRecord)+16,ios::beg);
       file_name.read((char*)&dr,sizeof(DataRecord));

       if(dr.Earnings==key){
         if(dr.eliminado==1) ans=mid+1;
         else ans=mid;
         break;
       }

       else if(dr.Earnings < key){
         ans=mid;
         right=mid-1;
       }
       else{
        left=mid+1;
       }
     }
      file_name.seekg(ans*sizeof(DataRecord)+16,ios::beg);
      file_name.read((char*)&dr,sizeof(DataRecord));
      if(dr.Earnings > key) ans++;


      while(dr.eliminado==1 && ans<=sizeFile-1){
      file_name.seekg(ans*sizeof(DataRecord)+16,ios::beg);
      file_name.read((char*)&dr,sizeof(DataRecord));
      ans++;
     }
    file_name.seekg(ans*sizeof(DataRecord)+16,ios::beg);
    file_name.read((char*)&dr,sizeof(DataRecord));
    

    // calculo de resultados
    if(ans==sizeFile){
      return make_pair(pos,1);
    }
    if(pos==-1){
      return make_pair(ans,0); // por verse
    }

    DataRecord regis;
    int key_datos;
    file_name.seekg(ans*sizeof(DataRecord)+16,ios::beg);
    file_name.read((char*)&regis,sizeof(DataRecord));
    key_datos=regis.Earnings;
    int key_aux;
    file_aux.seekg(pos*sizeof(DataRecord),ios::beg);
    file_aux.read((char*)&regis,sizeof(DataRecord));
    key_aux=regis.Earnings;
    if(key_datos<key_aux)
    return make_pair(pos,1);
    else
    return make_pair(ans,0);
}


pair<int,int> SequentialFile::search_data_next(int key){  // retorna (pos_indice, flag de archivo)
   fstream file_name(fileName, ios::binary | ios::in | ios::out);
     DataRecord dr;
     int left=0;
     int right=sizeFile- 1;
     int mid=-1;
     int ans=sizeFile-1;

     // buscar ese valor en el aux.dat
     fstream file_aux(fileAux, ios::binary | ios::in | ios::out);
     DataRecord temp;
     int pos=-1;
     int ans_aux=1e9;



     // for para la busqueda en el aux.dat
     for(int i=0;i<sizeAux;i++){
      
       file_aux.seekg(i*sizeof(DataRecord),ios::beg);
       file_aux.read((char*)&temp,sizeof(DataRecord));
       // si el registro ya fue eliminado
       if(temp.eliminado==1) continue;
       
       // calculo del primer elemento mayor o igual a key y valido
       if(temp.Earnings>=key){
            if(ans_aux>temp.Earnings){
              ans_aux=temp.Earnings;
              pos=i;
            }
           
       }
     }

     

     // buscar ese valor en el datos.dat
     while(left<=right){
       mid=left + (right-left)/2;
       file_name.seekg(mid*sizeof(DataRecord)+16,ios::beg);
       file_name.read((char*)&dr,sizeof(DataRecord));

       if(dr.Earnings==key){
         if(dr.eliminado==1) ans=mid-1;
         else ans=mid;
         break;
       }

       else if(dr.Earnings < key){
         ans=mid;
         right=mid-1;
       }
       else{
        left=mid+1;
       }
     }
     
    file_name.seekg(ans*sizeof(DataRecord)+16,ios::beg);
    file_name.read((char*)&dr,sizeof(DataRecord));
    if(dr.Earnings < key) ans--;

    while(dr.eliminado==1 && ans>=0){
      file_name.seekg(ans*sizeof(DataRecord)+16,ios::beg);
      file_name.read((char*)&dr,sizeof(DataRecord));
      ans--;
     }
    if(ans==-1){
      return make_pair(pos,1);
    }
    if(pos==-1){
      return make_pair(ans,0);
    }
    DataRecord regis;
    int key_datos;
    file_name.seekg(ans*sizeof(DataRecord)+16,ios::beg);
    file_name.read((char*)&regis,sizeof(DataRecord));
    key_datos=regis.Earnings;
    int key_aux;
    file_aux.seekg(pos*sizeof(DataRecord),ios::beg);
    file_aux.read((char*)&regis,sizeof(DataRecord));
    key_aux=regis.Earnings;
    if(key_datos>key_aux)
    return make_pair(pos,1);
    else
    return make_pair(ans,0);


}


int SequentialFile::search_aux(int key){
  fstream file(fileAux, ios::binary | ios::in | ios::out);
  if(file.is_open()){
    DataRecord dr;
    for(int i=0;i<sizeAux;i++){
      file.seekg(i*sizeof(DataRecord),ios::beg);
      file.read((char*)&dr,sizeof(DataRecord));
      if(dr.Earnings==key){
        if(dr.eliminado==1)
          return -2;
        return i;
      }
    }
    return -1;
  } 
  return -1;

  
}


vector<Record> SequentialFile::range_search(int key1, int key2){
      // key2 < key1
      fstream file1(fileName, ios::binary | ios::in | ios::out);
      fstream file2(fileAux, ios::binary | ios::in | ios::out);
      fstream file3(fileCsv, ios::binary | ios::in | ios::out);
      vector<Record> vr;
      DataRecord dr;
      Record elemento;
      string line;
        pair<int,int> p1 = search_data_prev(key1);
        if(p1.second==0){
            p1.first = p1.first*sizeof(DataRecord)+16;
            file1.seekg(p1.first,ios::beg);
            file1.read((char*)&dr,sizeof(DataRecord));
          }
          else{
            p1.first = p1.first*sizeof(DataRecord);
            file2.seekg(p1.first,ios::beg);
            file2.read((char*)&dr,sizeof(DataRecord));
          }
        while(dr.Earnings>=key2 ){
          
          elemento = Record();
          if(p1.second==0){
            //  Leer directamente en el csv
            file3.seekg(dr.pos,ios::beg);
            getline(file3,line);
            char *data = const_cast<char *>(line.c_str());
            elemento.Name = strtok(data, ",");
            elemento.Earnings = stod(strtok(NULL, ","));
            elemento.Year = stoi(strtok(NULL, ","));
            elemento.Sport = strtok(NULL, "\r");
            //  Actualizar punteros
            p1.first = dr.next;
            p1.second = dr.file;
            //  Obtener posicion para leer en el csv
            if(p1.second==0){
              file1.seekg(p1.first,ios::beg);
            file1.read((char*)&dr,sizeof(DataRecord));
            }
            else {
              file2.seekg(p1.first,ios::beg);
            file2.read((char*)&dr,sizeof(DataRecord));
            }

            vr.push_back(elemento);
            if(p1.first==-1) break;
          }
          else{
            //  Leer directamente en el csv
            file3.seekg(dr.pos,ios::beg);
            getline(file3,line);
            char *data = const_cast<char *>(line.c_str());
            elemento.Name = strtok(data, ",");
            elemento.Earnings = stod(strtok(NULL, ","));
            elemento.Year = stoi(strtok(NULL, ","));
            elemento.Sport = strtok(NULL, "\r");
            //  Actualizar punteros
            p1.first = dr.next;
            p1.second = dr.file;
            //  Obtener posicion para leer en el csv
            if(p1.second==0){
              file1.seekg(p1.first,ios::beg);

            file1.read((char*)&dr,sizeof(DataRecord));
            }
            else {
              file2.seekg(p1.first,ios::beg);

            file2.read((char*)&dr,sizeof(DataRecord));
            }
            if(dr.eliminado==0)
            vr.push_back(elemento);
            if(p1.first==-1) break;
          }
        }
        
        return vr;
}

Record SequentialFile::search(int key){
    int pos = search_data(key);
    DataRecord temp;
    Record temp2;
    string line;
    if(pos>=0){
      //Leer en el csv y construir el Record
      ifstream file(fileName, ios::binary);
      ifstream file2(fileCsv, ios::binary);
      file.seekg(pos*sizeof(DataRecord)+16,ios_base::beg);
      file.read((char*)&temp,sizeof(DataRecord));
      file2.seekg(temp.pos,ios_base::beg);
      getline(file2,line);
      char *data = const_cast<char *>(line.c_str());
      temp2.Name = strtok(data, ",");
      temp2.Earnings = stod(strtok(NULL, ","));
      temp2.Year = stoi(strtok(NULL, ","));
      temp2.Sport = strtok(NULL, "\r");
      return temp2;
    }
    else {
      pos = search_aux(key);
      if(pos>=0){
      ifstream file(fileAux, ios::binary);
      ifstream file2(fileCsv, ios::binary);
      file.seekg(pos*sizeof(DataRecord),ios_base::beg);
      file.read((char*)&temp,sizeof(DataRecord));
      file2.seekg(temp.pos,ios_base::beg);
      getline(file2,line);
      char *data = const_cast<char *>(line.c_str());
      temp2.Name = strtok(data, ",");
      temp2.Earnings = stod(strtok(NULL, ","));
      temp2.Year = stoi(strtok(NULL, ","));
      temp2.Sport = strtok(NULL, "\r");
      return temp2;
      }
      throw std::runtime_error("error elemento no encontrado\n");
    }
}





bool SequentialFile::add(Record& reg){
  fstream file_name(fileName, ios::binary | ios::in | ios::out);
  fstream file_aux(fileAux, ios::binary | ios::in | ios::out);
  // Insertar en el auxiliar
  if(sizeAux == AuxMax){
    sizeFile=sizeFile+sizeAux;
    reconstruction();
    file_name.close();
    remove("datos.dat");
    rename("datos2.dat","datos.dat");
    add(reg);
    return true;
  }
  else{
    DataRecord registro(reg);
    pair<int,int> p1 = search_data_prev(registro.Earnings);
    pair<int,int> p2 = search_data_next(registro.Earnings);

    file_aux.seekp(0,ios::end);
    int pos_aux = file_aux.tellp();
    int fa =1;
    /*******************     Corregir           *****************/
    //Insertamos uno mayor a todos
    if(p2.first==-1){
      file_aux.seekg(0,ios::end);
      int h=file_aux.tellg();
      file_name.seekg(0,ios::beg);
      file_name.write((char*)&h,sizeof(int));
      h=1;
      file_name.write((char*)&h,sizeof(int));
    }
    else{
    //Actualizar el mayor que apunta al nuevo
      if(p2.second==0){
        file_name.seekp(p2.first*sizeof(DataRecord)+24,ios::beg);
        file_name.write((char*)&pos_aux,sizeof(pos_aux));
        file_name.write((char*)&fa,sizeof(fa));
      }
      else{
        file_aux.seekp(p2.first*sizeof(DataRecord)+8,ios::beg);
        file_aux.write((char*)&pos_aux,sizeof(pos_aux));
        file_aux.write((char*)&fa,sizeof(fa));
      }
    }
    //  Caso en el que es menor a todos
    if(p1.first==-1){
      registro.next=-1;
      registro.file=0;
    }
    else{
      //actualizar punteros hacia el prev
     if(p1.second==0)
      registro.next=p1.first*sizeof(DataRecord)+16;
     else 
      registro.next=p1.first*sizeof(DataRecord);
    registro.file=p1.second;
    }
    file_aux.seekp(0,ios::end);
    file_aux.write((char*)&registro,sizeof(DataRecord));
    sizeAux++;
    return true;
    }
  }

void init(){
  ifstream file("forbesathletesv2.csv",ios::binary );
  ofstream file2("datos.dat",ios::binary);
  string line;
  int pos = 0;
  int pos_siguiente = 1;                                //  Para la posicion fisica siguiente en el .dat y no en el csv
  int siguiente;                                        //  Posicion del siguiente
  int archivo = 0;
  int eliminado = 0;
  int inicial = Header ;                                     //  Para colocar en la cabecera el primer registro
  int file_inicial = 0;                                 //  Para colocar en que file comienza 0->datos.dat o 1->auxiliar.dat
  file2.write((char*)&inicial,sizeof(int));             //  Header
  file2.write((char*)&file_inicial,sizeof(int));        //  Header
  int eli = -1;
  int file_eli = -1;
  file2.write((char*)&eli,sizeof(int));
  file2.write((char*)&file_eli,sizeof(int));
  while(getline(file,line)){ 
    Record record;
    char *data = const_cast<char *>(line.c_str());
    record.Name = strtok(data, ",");
    record.Earnings = stod(strtok(NULL, ","));
    record.Year = stoi(strtok(NULL, ","));
    record.Sport = strtok(NULL, "\r");
    siguiente = (pos_siguiente*sizeof(DataRecord))+Header;  // Sumandole el header
    file2.write((char*)&record.Earnings, sizeof(float));
    file2.write((char*)&pos, sizeof(int));
    file2.write((char*)&siguiente, sizeof(int)); //El siguiente en la posicion fisica en el datos.dat o auxiliar.dat
    file2.write((char*)&archivo, sizeof(int));
    file2.write((char*)&eliminado, sizeof(eliminado));
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
  if(file=="datos.dat" || file=="datos2.dat")
  {  
    // Leemos la cabecera
    int header;
    int file_header;
    int eliminado;
    int file_eliminado;
    file2.read((char*)&header,sizeof(header));
    file2.read((char*)&file_header,sizeof(file_header));
    file2.read((char*)&eliminado,sizeof(eliminado));
    file2.read((char*)&file_eliminado,sizeof(file_eliminado));
    cout<<header<<" "<<file_header<<" "<<eliminado<<" "<<file_eliminado<<endl;
  }
  while(file2.peek() != EOF){
    record = DataRecord();
    file2.read((char*)&record,sizeof(DataRecord));
    record.print();
  }
}




void test(){
SequentialFile seq;
/*
//  Mostrar el index
  show_date("datos.dat");
  cout<<"\n\n";
  show_date("auxiliar.dat");
  cout<<"\n\n";
*/

/*
//  Agregar
  Record reg("Eros",109,2021,"Computacion");
  seq.add(reg);
  Record reg1("Luis",100,2021,"Computacion");
  seq.add(reg1);
  Record reg2("Jose",98,2021,"Computacion");
  seq.add(reg2);
  Record reg3("Alberto",60,2021,"Computacion");
  seq.add(reg3);
  Record reg4("Juan",67,2021,"Computacion");
  seq.add(reg4);
  Record reg5("Heider",58,2021,"Computacion");
  seq.add(reg5);
  Record reg6("Bellido",54,2021,"Computacion");
  seq.add(reg6);
//  Mostrar el index
  show_date("datos.dat");
  cout<<"\n\n";
  show_date("auxiliar.dat");
  cout<<"\n\n";
*/

/*
//  Search
  Record record_search = seq.search(60);
  record_search.print();
  record_search = seq.search(180);
  record_search.print();
*/
/*
  Record record_search = seq.search(35);
  record_search.print();
*/

/*
//  RangeSearch
  vector<Record> range_record = seq.range_search(110,55);
  for (auto a:range_record){
    a.print();
  }
  cout<<"\n\n\n";
  vector<Record> range_record2 = seq.range_search(300,20);
  for (auto a:range_record2){
    a.print();
  }
*/


//  Remove
  /*
  seq.remove_register(95);
  show_date("datos.dat");
  cout<<"\n\n";
  show_date("auxiliar.dat");
  cout<<"\n\n";
  */
  /*
  seq.remove_register(82);
  show_date("datos.dat");
  cout<<"\n\n";
  show_date("auxiliar.dat");
  cout<<"\n\n";
  */
  
}

void time_search(){
  SequentialFile seq;
  clock_t t;
  t = clock();
  Record record_search = seq.search(78);
  record_search.print();
  t = clock() - t;
  double time_taken = ((double)t) / CLOCKS_PER_SEC;
  printf("El programa tomo %.8f segundos en buscar el rango de registros", time_taken);
}

void time_insert(){
  SequentialFile seq;
  clock_t t;
  t = clock();
  Record reg("Eros",109,2021,"Computacion");
  seq.add(reg);
  t = clock() - t;
  double time_taken = ((double)t) / CLOCKS_PER_SEC;
  printf("El programa tomo %.8f segundos en agregar el registro", time_taken);
}



int main(){
  //time_search();
  //time_insert();
}