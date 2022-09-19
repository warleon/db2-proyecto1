#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <string>
#include <stdio.h>
using namespace std;
#define AuxMax 2
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
  int search_aux(int key);
  Record search(int key);
  Record range_search(int key1,int key2);
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

    /*
      //  Eliminar datos.dat, limpiar axiliar.dat y renombrar datos2.dat 
      std::remove("datos.dat");
      std::rename("datos2.dat","datos.dat");
      ofstream ofs;
      ofs.open("auxiliar.dat", ios::out | ios::trunc);
      ofs.close();
      */
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
  //  Con busqueda sequencial nomas :(
  fstream file(fileName, ios::binary | ios::in | ios::out);
  fstream file2(fileName, ios::binary | ios::in | ios::out);
  int pos_buscar = header;
  int file_buscar = file_header;
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
    if(file_header==0){
      file.seekg(pos_buscar,ios_base::beg);
      file.read((char*)&temp,sizeof(temp));
    }
    else{
      file2.seekg(pos_buscar,ios_base::beg);
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
   if(file.is_open()){
     DataRecord dr;
     int left=0;
     int right=sizeFile- 1;
     int mid=-1;
     while(left<=right){
       mid=left + (right-left)/2;
       file.seekg(mid*sizeof(DataRecord)+16,ios::beg);
       file.read((char*)&dr,sizeof(DataRecord));
       if(dr.Earnings ==key){
        if(dr.eliminado==1)
          return -2;
        return mid;
       }
       if(dr.Earnings > key){
         left=mid+1;
       }
       else{
        right=mid-1;
       }
     }
     throw std::runtime_error("error por search_data\n");
     return -1;
    }
    else return -1;

}

// devuelve la posicion del menor mas cercano para postriormente modificar sus punteros
pair<int,int> SequentialFile::search_data_prev(int key){  // retorna (pos_indice, flag de archivo)
   fstream file(fileName, ios::binary | ios::in | ios::out);
   if(file.is_open()){

     DataRecord dr;
     int left=0;
     int right=sizeFile- 1;
     int mid=-1;
     int ans=-1;
     bool p=0;
     file.seekg(-sizeof(DataRecord),ios::end);
     file.read((char*)&dr,sizeof(DataRecord));
     if(dr.Earnings > key){
      p=1;
     }
     // buscar ese valor en el datos.dat
     while(left<=right){
       mid=left + (right-left)/2;
       file.seekg(mid*sizeof(DataRecord)+16,ios::beg);
       file.read((char*)&dr,sizeof(DataRecord));
       if(dr.Earnings <= key){
         ans=mid;
         right=mid-1;
       }
       else{
        left=mid+1;
       }
     }
      //cout<<"despues del bsprev: "<<ans<<'\n';
      file.seekg(ans*sizeof(DataRecord)+16,ios::beg);
      file.read((char*)&dr,sizeof(DataRecord));
      while(dr.eliminado==1 && ans<=sizeFile-1){
      ans++;
      file.seekg(ans*sizeof(DataRecord)+16,ios::beg);
      file.read((char*)&dr,sizeof(DataRecord));
     }
    file.seekg(ans*sizeof(DataRecord)+16,ios::beg);
    file.read((char*)&dr,sizeof(DataRecord));
    if(dr.eliminado==1){
      p=1;
    }

    
     // buscar ese valor en el aux.dat
     fstream file(fileAux, ios::binary | ios::in | ios::out);
     DataRecord temp;
     int pos=-1;
     int val=-1;
     for(int i=0;i<sizeAux;i++){
       file.seekg(i*sizeof(DataRecord),ios::beg);
       file.read((char*)&temp,sizeof(DataRecord));
       if(temp.Earnings<=key){
         if(temp.eliminado==0){
            if(val<temp.Earnings){
              val=temp.Earnings;
              pos=i;
            }
         }
           
       }
     }

     if(p==1){
      if(pos==-1){
        return make_pair(-2,-2);
      }
      else return make_pair(pos,1);
     }
     else{
      if(pos==-1){
        return make_pair(ans,0);
      }
      else {
        if(dr.Earnings > val){
          return make_pair(ans,0);
        }
        else return make_pair(pos,1);
      }
     }

      

     return make_pair(-2,-2);
    }
    else return make_pair(-2,-2);
}

pair<int,int> SequentialFile::search_data_next(int key){  // retorna (pos_indice, flag de archivo)
   fstream file(fileName, ios::binary | ios::in | ios::out);
   if(file.is_open()){

     DataRecord dr;
     int left=0;
     int right=sizeFile- 1;
     int mid=-1;
     int ans=-1;
     bool p=0;
     file.seekg(-sizeof(DataRecord),ios::end);
     file.read((char*)&dr,sizeof(DataRecord));
     if(dr.Earnings > key){
      p=1;
     }
     // buscar ese valor en el datos.dat
     while(left<=right){
       mid=left + (right-left)/2;
       file.seekg(mid*sizeof(DataRecord)+16,ios::beg);
       file.read((char*)&dr,sizeof(DataRecord));
       if(dr.Earnings <= key){
         ans=mid;
         right=mid-1;
       }
       else{
        left=mid+1;
       }
     }
      ans--;
      file.seekg(ans*sizeof(DataRecord)+16,ios::beg);
      file.read((char*)&dr,sizeof(DataRecord));
     while(dr.eliminado==1 && ans>=0){
      ans--;
      file.seekg(ans*sizeof(DataRecord)+16,ios::beg);
      file.read((char*)&dr,sizeof(DataRecord));
     }
    file.seekg(ans*sizeof(DataRecord)+16,ios::beg);
    file.read((char*)&dr,sizeof(DataRecord));
    if(dr.eliminado==1){
      p=1;
    }
    
     // buscar ese valor en el aux.dat
     fstream file(fileAux, ios::binary | ios::in | ios::out);
     DataRecord temp;
     int pos=-1;
     int val=-1;
     for(int i=0;i<sizeAux;i++){
       file.seekg(i*sizeof(DataRecord),ios::beg);
       file.read((char*)&temp,sizeof(DataRecord));
       if(temp.Earnings>=key){
         if(temp.eliminado==0){
            if(val>temp.Earnings){
              val=temp.Earnings;
              pos=i;
            }
         }
           
       }
     }

     if(p==1){
      if(pos==-1){
        return make_pair(-2,-2);
      }
      else return make_pair(pos,1);
     }
     else{
      if(pos==-1){
        return make_pair(ans,0);
      }
      else {
        if(dr.Earnings > val){
          return make_pair(ans,0);
        }
        else return make_pair(pos,1);
      }
     }

      

     return make_pair(-2,-2);
    }
    else return make_pair(-2,-2);
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

/*
Record SequentialFile::range_search(int key1, int key2){

}
*/

Record SequentialFile::search(int key){
    int pos = search_data(key);
    DataRecord temp;
    Record temp2;
    string line;
    if(pos>=0){
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
      throw std::runtime_error("error archivo no encontrado\n");
    }
}





bool SequentialFile::add(Record& reg){
  //Localizar la posición en donde será insertado el nuevo registro.
      //Si el espacio está libre, insertar.
      //Sino, insertar el registro en un espacio auxiliar.
      //En este caso, los punteros deberían ser actualizados.
  //Se requiere reorganizar el archivo original cada cierto tiempo mezclando ordenadamente con el espacio auxiliar.
  ofstream file_name(fileName, ios::binary | ios::in | ios::out);
  ofstream file_aux(fileAux, ios::binary | ios::in | ios::out);
  DataRecord registro(reg);
       //cout<<"registro.earnings es: "<<registro.Earnings<<'\n';
  pair<int,int> p1 = search_data_prev(registro.Earnings);
  pair<int,int> p2 = search_data_next(registro.Earnings);
       // Para agregarlo en el datos.dat
       //cout<<"p1.first es: "<<p1.first<<'\n';
       //cout<<"p2.first es: "<<p2.first<<'\n';
  if((p1.first-p2.first)>1 && p1.second==0 && p2.second==0){
        //cout<<"sasd"<<'\n';
        int pos_nuevo;
        pos_nuevo = (p2.first+1)*sizeof(DataRecord)+16;
        //cout<<"pos_nuevo "<<pos_nuevo<<endl;
        
        //actualizar punteros hacia el nuevo registro
        file_name.seekp(p2.first*sizeof(DataRecord)+24,ios::beg);
        file_name.write((char*)&pos_nuevo,sizeof(pos_nuevo));
        //actualizar punteros hacia el prev
        registro.next=p1.first*sizeof(DataRecord)+16;
        registro.file=p1.second;

        file_name.seekp(pos_nuevo,ios::beg);
        file_name.write((char*)&registro,sizeof(DataRecord));
      
       //
       
       
       //actualizar la cabecera
       //if(header==(p1.first*sizeof(DataRecord)+16) && file_header == p1.second){
       // actualizar_header(p1.first,p1.second);
      // }
       sizeFile++;
       return true;
  }
  // Insertar en el auxiliar
  else{
    cout<<"a\n";
    if(sizeAux == AuxMax){
      reconstruction();
      //falta actualizar punteros
      return true;
    }
    else{
      cout<<"b\n";
      file_aux.seekp(0,ios::end);
      int pos_aux = file_aux.tellp();
      int fa =1;
      //Actualizar el next
      if(p2.second==0){
        file_name.seekp(p2.first*sizeof(DataRecord)+24,ios::beg);
        file_name.write((char*)&pos_aux,sizeof(pos_aux));
        file_name.write((char*)&fa,sizeof(fa));
        cout<<"c\n";
      }
      else{
        file_aux.seekp(p2.first*sizeof(DataRecord)+8,ios::beg);
        file_aux.write((char*)&pos_aux,sizeof(pos_aux));
        file_aux.write((char*)&fa,sizeof(fa));
        cout<<"d\n";
      }

      //actualizar punteros hacia el prev
      registro.next=p1.first*sizeof(DataRecord)+16;
      registro.file=p1.second;

      file_aux.seekp(0,ios::end);
      file_aux.write((char*)&registro,sizeof(DataRecord));
      return true;
    }
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
    //cout<<record.Earnings<<" "<<pos<<" "<<siguiente<<" "<<archivo<<" "<<eliminado<<endl;
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
  //show_date("datos.dat");
  //Record registro1("Eros",190.35,2021,"Computacion");
  


  //init();
  
  //show_date("datos.dat");
  //cout<<"\n";
  /*
  SequentialFile seq;
  cout<<endl<<endl<<endl;
  seq.remove(180);
  show_date("datos.dat");
  */

 /*
  Record registro1("Joaquin222",115,2021,"Computacion");
  SequentialFile seq;
  seq.add(registro1);
  show_date("datos.dat");
  cout<<"\n";
  show_date("auxiliar.dat");
  cout<<"\n";
  show_date("datos2.dat");
*/


  //cout<<"\n";
  //show_date("datos2.dat");
  //seq.remove(90);
  //show_date("datos.dat");
  //cout<<seq.search_data_prev(74).first<<" "<<seq.search_data_prev(74).second<<'\n';
  //cout<<seq.search_data_next(74).first<<" "<<seq.search_data_next(74).second<<endl;
  /*
  Record record=seq.search(180);
  record.print();
  */
  /*
  show_date("datos.dat");
  SequentialFile seq;
  DataRecord record(190);
  seq.add(record);
  show_date("datos.dat");
  show_date("auxiliar.dat");
  */
}