<img src="https://upload.wikimedia.org/wikipedia/commons/7/7a/UTEC.jpg" width="200">

# db2-proyecto1
 proyecto1 de db2
## **Integrantes**
* Aguilar, Anthony
* Carhuancho, Eros
* Chahua, Luis
* Ángeles, Jean

## **Objetivo del proyecto**
El objetivo principal del proyecto es diseñar técnicas de organización de archivos en la memoria secundaria mediante la implementación de las funciones principales de inserción, deleción y búsqueda y asímismo realizar una comparación experimental entre estas técnicas.

## **Dominio de los datos**
Para la implementación del proyecto se han utilizado datasets en formato csv de la plataforma Kaggle, de los cuales seleccionaremos un key específico para el uso en la implementación.

## **Resultados esperados**
Se espera comprobar la funcionalidad y la eficiencia de las técnicas utilizando el testeo en cada una de sus funciones.

## **Sequential File**
Esta técnica de organización de archivos refiere a que los registros se irán almacenando de manera ordenada en un archivo principal. Además, las inserciones de registros se colocaran en un archivo auxiliar pero se mantendrá el orden de los keys de los registros mediante los punteros. 

### Búsqueda 
Respecto a la búsqueda, se han implementado 2 tipos: búsqueda por valor y búsqueda por rango. Respecto a la búsqueda por valor, se implementó considerando principalmente 2 situaciones: Si el valor existe, entonces debe de estar en alguno de los 2 archivos, ya sea el principal o el auxiliar. Si se encuentra en el principal, entonces, ya que los registros están posicionados de forma ordenada, se puede aplicar búsqueda binaria en el key para poder encontrar exactamente la posición lógica del registro. Si se encuentra ahí, verificamos que no esté eliminado, para ello se ha agregado un flag en el archivo principal para verificar ello. Si en caso no se encuentra, se aplica una búsqueda secuencial en el archivo auxilia y , de la misma forma que el caso anterior, verificamos que aún no esté eliminado. Finalmente, si se encuentra en alguno de los 2 archivos, entonces nos retornará la posición lógica del registro y el archivo en el que se encuentra.
Respecto a la búsqueda por rango, se procederá a utilizar dos funciones de búsqueda adicionales: Búsqueda del registro con el key más cercano y menor o igual al pedido y búsqueda del registro con el key más cercano y mayor o igual al pedido. Ambas funciones se implementaron utilizando la misma técnica de la búsqueda para un key específico. Luego, para la búsqueda por rango, lo que haremos es obtener el key más cercano y menor al key mayor que tomamos de ejemplo, y luego realizar una búsqueda secuencial hasta llegar al último key que se encuentre en ese rango. Todos los registros que cumplen estas condiciones se almacenarán en un vector y se retornará como un vector.

## **Extendible Hash**
# to run
create a build dir in the project folder
```
mkdir project/build
```
turn on docker and run docker-compose
```
sudo docker-compose up --build
```
