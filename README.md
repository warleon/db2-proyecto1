<img src="https://upload.wikimedia.org/wikipedia/commons/7/7a/UTEC.jpg" width="200">

# db2-proyecto1
 proyecto1 de db2
## **Integrantes**
* Aguilar, Anthony
* Carhuancho, Eros
* Chahua, Luis
* Ángeles, Jean

### **Disclaimer**
- Devido a las limitacion de los teclados de algunos integrantes algunas de las palabras en el siguiente informe que deberian tener tilde no la poseen.

## **Objetivo del proyecto**
El objetivo principal del proyecto es diseñar técnicas de organización de archivos en la memoria secundaria mediante la implementación de las funciones principales de inserción, eliminacion y búsqueda y asimismo realizar una comparación experimental entre estas técnicas. Para nuestro proyecto hemos implementado las técnicas de Sequential File y Extendible Hash

## **Dominio de los datos**
Para la implementación del proyecto se han utilizado datasets en formato csv de la plataforma Kaggle, de los cuales seleccionaremos un key específico para cada csv en la implementación.

## **Resultados esperados**
Se espera comprobar la funcionalidad y la eficiencia de las técnicas utilizando el testeo en cada una de sus funciones.

## **Sequential File**
Esta técnica de organización de archivos refiere a que los registros se irán almacenando de manera ordenada en un archivo principal. Además, las inserciones de registros se colocaran en un archivo auxiliar pero se mantendrá el orden de los keys de los registros mediante los punteros. 

### Organización del index
La organizacion del sequential file que tuvimos en cuenta fue que apartir de un archivo .csv construimos un indice corespondiente a la técnica. El indice consiste en dos archivos con nombre datos.dat y auxiliar.dat, con referencia al primero este empieza con una cabecera, el cual tiene 4 parametros. El primero de estos es la posicion desde donde inicia el primer registro o el registro mayor. Para el caso del segundo parametro nos referimos a este con los valores 0 y 1 los cuales indican que el registro inicial se encuentra en datos.dat o axuliar.dat respectivamente. Los últimos dos parámetros hace referencia a los registro ya eliminados siendo el primero la posición y el segundo el archivo donde se encuentra que al igual que los anteriores parámetros tienen valores de 0 y 1. 
Después de la caberera inicia los registros los cuales tienen 5 parámetros que se explicara acontinuación. El primero de estos es la key por la cual sera ordenado los registros. El segundo parámetro hace refencia a la posicion física de este registro en el archivo csv. Los siguientes dos parámetros hacen referencia a el siguiente registro y al archivo donde este se encuentra. El último de estos elementos indica si este registro esta eliminado o no.

### Búsqueda 
Respecto a la búsqueda, se han implementado 2 tipos: búsqueda por valor y búsqueda por rango. Respecto a la búsqueda por valor, se implementó considerando principalmente 2 situaciones: Si el key existe, entonces debe de estar en alguno de los 2 archivos, ya sea el principal o el auxiliar. Si se encuentra en el principal, entonces, ya que los registros están posicionados de forma ordenada, se puede aplicar búsqueda binaria en el key para poder encontrar exactamente la posición lógica del registro. Si se encuentra ahí, verificamos que no esté eliminado, para ello se ha agregado un flag en cada uno de los archivos para verificar ello. Si en caso no se encuentra, se aplica una búsqueda secuencial en el archivo auxiliar y , de la misma forma que el caso anterior, verificamos que aún no esté eliminado. Finalmente, si se encuentra en alguno de los 2 archivos, entonces nos retornará la posición lógica del registro y el archivo en el que se encuentra.
Respecto a la búsqueda por rango, se procederá a utilizar dos funciones de búsqueda adicionales: Búsqueda del registro con el key más cercano y menor o igual al pedido y búsqueda del registro con el key más cercano y mayor o igual al pedido. Ambas funciones se implementaron utilizando la misma técnica de la búsqueda para un key específico. Entonces, para la búsqueda por rango, lo que haremos es obtener el key más cercano y menor al key mayor que tomamos de ejemplo, y luego realizar una búsqueda secuencial hasta llegar al último key que se encuentre en ese rango. Todos los registros que cumplen estas condiciones y que además no hayan sido eliminados se almacenarán y retornará como un vector de registros.
La complejidad es O(log(N) + M) para el caso de la búsqueda de un key específico y para la búsqueda por rango sería O(log(N) + N + M) puesto que primero se hace búsqueda binaria y luego una búsqueda secuencial que en el peor de los casos, puede recorrer todos los registros de ambos archivos. Note que N es la cantidad de datos en el archivo principal y M es la cantidad de datos para el archivo auxiliar.

### Add
La función de agregar un registro consta en dos casos el primero de estos se da cuando aún hay espacio en el auxiliar.dat y se inserta al final de este archivo. Posteriormente se actualizan los punteros del registro anterior hacia este y los punteros del registro actual hacia uno menor. El segundo caso se da cuando no hay espacio en el auxiliar.dat y se tiene que realizar una reconstrucción. Esta reconstrucción consiste en combinar los registros de los dos archivos en uno solo insertandolo en un nuevo archivo pero de manera ordenada y al mismo tiempo actualizando sus punteros. Finalmente el último registro sera insertado en el auxiliar.
La complejidad de este algoritmo es de O(N+M) cuando se realiza una reconstrución si N y M los tamaños de datos.dat y auxiliar.dat respectivamente. En el caso de tener espacio en el auxiliar la complejidad algoritmica es de O(lgN), ya que actuliza los punteros del registro y los punteros que a señalan a este registro.

### Eliminar
La funcion eliminar consiste en buscar un registro apartir de su key y marcarlo como eliminado. Posteriormente se actulizan los punteros del anterior que apuntaban a este y ahora puntaran al siguiente al siguiente que el registro eliminado apuntaba. Se concluye que este registro estara ailado de los demas y que solo apuntaran a registros que estan eliminados para posteriormente ignorarlos para la reconstrucción.
La complejidad de este algoritmo es log(N), ya que marca como eliminado este registro y actualiza los punteros correspondientes. 

## **Extendible Hash**
Propuesta por RONALD FAGIN, JURG NIEVERGELT,  NICHOLAS PIPPENGER y H. RAYMOND STRONG en el paper [Extendible Hashing A Fast Access
Method for Dynamic Files](https://dl.acm.org/doi/pdf/10.1145/320083.320092).
Es una estructura de datos para disco que almacena pares llave-valor en disco y garantiza el acceso a estos en no mas de 2 accesos a disco. Esto es posible gracias a la respuesta a dos preguntas:
- ¿Como hacer un TRIE valanceado?
- ¿Como hacer una TABLA HASH que se pueda redimencionar sin rehasear todos los elementos?  

En el paper indagan en la solucion a estas preguntas asi que aqui simplemente nos limitaremos a explicar nuestra implementacion a continuacion.

### **Estructura**
#### **Registro General**
Dado que queriamos realizar una estructura capaz de soportar cualquier tipo de registro o tabla (csv) nuestro registro general no es mas que una secuencia de bytes.
#### **Informacion de Registro**
Para poder interpretar un Registro necesitamos saber que tipo de dato tiene y de que tamaño es cada campo.
Y es justamente eso lo que almacena esta estrutura.
#### **Metadata de Registro**
Una ves hallamos escrito el registro a un archivo este estara en una poscion o desplazamiento especifico y necesitamos esta informacion para poder leer el Registro. Es por eso que esta estructura guarda la posicion absoluta del registro y la Informacion requerida para poder leerlo e interpretarlo.
#### **Llave**
Dada la simplesa de nuestro Registro General las llaves usadas en esta implementacion son de tipo STRING. Esto nos permite tambien concatenar campos de un registro para de forma trivial soportar llaves compuestas.
#### **Balde o Contenedor**
Un conjunto de pares Llave-Metadata con un tamaño maximo fijo. Asi como tambien un numero que indica la PROFUNDIDAD_LOCAL
#### **Piscina de Baldes**
Interfaz que se encarga de leer o escribir baldes de disco segun se requiera. Actua tambien como cache en memoria de los baldes cargados previamente. Y facilita la implentacion del Extendible Hash
#### **Funcion Hash**
Dado que nuestro tipo de llave es un STRING simplemente usamos el HASH estandar que provee c++.
#### **Directorio**
Un VECTOR de IDs de Baldes generados por la Piscina. Es el resultado de colapsar un TRIE de cadenas binarias y replicar las hojas que no hallan estado en la maxima profundidad.
#### **Extendible Hash**
La estructura principal consta de un DIRECTORIO y una Piscina de Baldes e implementa las siguientes operaciones. Asi como tambien un numero que indica la PROFUNDIDAD_GLOBAL. El DIRECTORIO completo permanece en memoria durante todo el tiempo de vida del Extendible Hash.
### **Operaciones**
A continuacion un pseudocodigo de las operaciones
#### **Busqueda**
0. Dada una LLAVE  
1. Aplicar la funcion hash a la LLAVE para obtener el HASH  
1. Extraer los PROFUNDIDAD_GLOBAL BITS mas significativos del HASH  
1. Interpretar los BITS como como un entero. Este sera el INDICE  
1. Obtener el ID del balde que se encuentra en DIRECTORIO\[INDICE\]  
1. Leer de disco el BALDE al que le corresponde la ID  
1. Buscar en el BALDE la METADATA correspondiente al la LLAVE y retornarla
#### **Insercion**
0. Dada una LLAVE y una METADATA
1. Aplicar los pasos de la Busqueda del 1 al 5 inclusivo para obtener el BALDE y su INDICE
1. Si el BALDE no esta lleno insertar el par LLAVE-METADATA y retornar
1. Si ya esta lleno crear un NUEVO_BALDE y asignar la PROFUNDIDAD_LOCAL de ambos baldes a la PROFUNDIDAD_LOCAL + 1 de BALDE
1. **Mover** el contenido del BALDE a un BUFFER auxiliar junto con el par LLAVE-METADATA
1. Si la nueva PROFUNDIDAD_LOCAL supera la PROFUNDIDAD_GLOBAL ejecutar Doblamiento y asignar el id de NUEVO_BALDE a DIRECTORIO\[INDICE*2\]
1. Si no asignar el id de NUEVO_BALDE a DIRECTORIO\[INDICE\]
1. Reinsertar de forma recursiva el contenido de BUFFER
##### **Doblamiento**
1. Incrementa la PROFUNDIDAD_GLOBAL en 1
1. Extender el DIRECTORIO a un tamaño de $2^{PROFUNDIDAD\_GLOBAL}$
1. Iterar desde el ultimo INDICE del direcorio hasta 0 y asignar a DIRECTORIO\[INDICE\] el ID en DIRECTORIO\[INDICE/2\]
#### **Eliminacion**
0. Dada una LLAVE 
1. Aplicar los pasos de la Busqueda del 1 al 5 inclusivo para obtener el BALDE
1. Eliminar el par LLAVE-METADATA en BALDE y retornar
#### **Indexacion**
0. Dado un **archivo con Informacion de Registros** , un **archivo con Registros** y un **conjunto de enteros que indican los campos forman la llave**
1. Mientras no se terminen de leer los archivos
1. Obtener la POSICION actual de lectura
1. Leer la INFORMACION de Registro y su REGISTRO respectivo
1. Construir una METADATA con la POSICION y INFORMACION obtenida
2. Usar la INFORMACION y el REGISTRO para generar una LLAVE concatenando los campos indicados
1. Ejecutar Insertar LLAVE METADATA
### **Analisis de Complejidad** (En accesos a disco)
#### **Busqueda**
Sabiendo que el DIRECTORIO en su totalidad permanece en memoria ram. La unica interacion con el disco que se realiza es la lectura y escritura del balde.  
Si bien el Balde tien una cantidad maxima fija de pares llave-metadata estos pares son de tamaño variable.
Dando como resultado una complejidad de O(n) donde n es la cantidad de paginas que ocupa el Balde. Aunque de imponerse un tamaño fijo en los campos del Registro y por ende en las llaves esta complejidad se volveria O(1).
#### **Eliminacion**
Mismo argumento y complejidad que la Busqueda
#### **Insercion**

#### **Indexacion**


# **to run**
create a build dir in the project folder
```
mkdir project/build
```
turn on docker and run docker-compose
```
sudo docker-compose up --build
```
