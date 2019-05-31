#include <mpi.h>    //MPI_*
#include <vector>   //vector<int>
#include <string.h>
#include <cstdlib>  //rand()
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <fstream>

using namespace std;

/*Funcion que comprueba que el vector ingresado es un cubo mágico*/

bool comprobarCuboMagico(vector<vector<int>> cuboMagico){
        int sumaDiagPrinc = 0;  /*Variable que almacena la suma de la diagonal principal*/
        int sumaDiagSecun = 0;  /*Variable que almacena la suma de la diagonal secundaria*/
        int size = cuboMagico.size(); /*Variable que contiene el tamaño del vector*/
        vector<int> sumas;  /*Vector que contiene todas las sumas de la matriz*/
        for(int pivote = 0; pivote < cuboMagico.size(); pivote++) { /*Se utiliza un pivote para identificar en que fila y columna se debe iterar*/
                int sumaFila = 0;
                int sumaCol = 0;
                sumaDiagPrinc += cuboMagico.at(pivote).at(pivote);  /*Obtención de la diagonal principal*/
                sumaDiagSecun += cuboMagico.at(pivote).at(size - (pivote + 1)); /*Obtención de la diagonal secundaria*/

                for(int col = 0; col < cuboMagico.at(pivote).size(); col++) { /* iteración en fila y columna del pivote*/
                        sumaFila += cuboMagico.at(pivote).at(col);
                        sumaCol += cuboMagico.at(col).at(pivote);

                }

                sumas.push_back(sumaCol);
                sumas.push_back(sumaFila);
        }

        sumas.push_back(sumaDiagPrinc);
        sumas.push_back(sumaDiagSecun);

        int primer = sumas.at(0);
        for(int iterador : sumas) {
                if(primer != iterador) {  /*Si alguna suma es distinta de la primera quiere decir que no es un cubo mágico*/
                        return false;
                }
        }
        return true;
}

vector<string> split_iterator(const string& str, char limitador)
{
        vector<string> resultado;

        string::const_iterator itBegin = str.begin();
        string::const_iterator itEnd   = str.end();

        int numItems = 1;
        for( string::const_iterator it = itBegin; it!=itEnd; ++it )
                numItems += *it==limitador;

        resultado.reserve(numItems);

        for( string::const_iterator it = itBegin; it!=itEnd; ++it )
        {
                if( *it == limitador )
                {
                        resultado.push_back(string(itBegin,it));
                        itBegin = it+1;
                }
        }

        if( itBegin != itEnd )
                resultado.push_back(string(itBegin,itEnd));

        return resultado; /*Retorna un vector de string*/
}

vector<vector<int> > limpiaString(string sucio){
        string str = sucio;
        string limpio;
        for(int i = 1; i<str.length()-1; i++) {
                limpio += str[i];
        }

        vector<string> separado;

        separado = split_iterator(limpio, ';');
        vector<vector<int> > cuboMagico;
        for(int iterador = 0; iterador < separado.size(); iterador++) {
                vector<string> row = split_iterator(separado.at(iterador), ',');
                vector<int> rowInt;
                for(string num : row) {
                        rowInt.push_back(atoi( num.c_str() ));
                }
                cuboMagico.push_back(rowInt);
        }
        return cuboMagico;    /*Retorna un vector de tipo int */
}


int main(){

        int mi_rango;   /* rango del proceso    */
        int p;   /* numero de procesos   */
        int tag = 0;   /* etiqueta del mensaje */
        int* vec = NULL; /* vector que representa una fila del archivo*/

        string cadena_fila;
        int largo=0;

        vector<vector<int> > limpio;
        string vacio;
        char* cadena_recibida = "";
        vector<string> linea;
        ifstream archivo("prueba.txt");

        MPI_Init(NULL, NULL);
        MPI_Comm_size(MPI_COMM_WORLD, &p);
        MPI_Comm_rank(MPI_COMM_WORLD, &mi_rango);

        if (mi_rango != 0) {     /* -- Esclavos -- fuentes encargadas de realizar los calculos y verificar si los vectores recibidos cumplen con la matriz magica*/
                std::cout << "** mi rango es : "<<mi_rango << '\n';
                cout << "Estoy arriba del recv" << endl;

                MPI_Recv(&largo, 2, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE); /* Se recibe el largo del string a recibir*/
                cadena_recibida = new char[largo + 1]();  /*Se le asigna el espacio al nuevo char */

                MPI_Recv(cadena_recibida, largo +1 , MPI_CHAR, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE); /* Se recibe el string*/
                cout << "******** Estoy abajo del recv " << cadena_recibida << endl;

                //limpio = limpiaString(cadena_fila);
                // if(comprobarCuboMagico(limpio)) {
                //         MPI_Send(&cadena, 150, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
                // }else{
                //         MPI_Send(&vacio, 150, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
                // }

        } else {        /* -- MASTER -- fuente encargada de distribuir los sub vectores*/
                int fuente = 1;
                std::cout << "**mi rango es : "<<mi_rango << '\n';

                while(!archivo.eof()) {  /*itera hasta que no queden más filas en el archivo*/
                  archivo >> cadena_fila;   /*Obtiene linea del archivo*/
                  if (archivo) {  /*Valida que la cadena_fila contenga algo*/
                    //std::cout <<"ESTO ES CADENA"<< cadena_fila << '\n';
                    cout << "Holi, soy "<<fuente << endl;

                    largo=cadena_fila.size(); /*Obtiene el largo de la fila del archivo*/

                    MPI_Send(&largo, 2  , MPI_INT, fuente, tag, MPI_COMM_WORLD);         /* Envía el largo de la cadena que recibira un esclavo*/

                    MPI_Send(cadena_fila.c_str(), largo  , MPI_CHAR, fuente, tag, MPI_COMM_WORLD);         /* Envía la fila obtenida del archivo a un esclavo*/

                    fuente++;

                    if(fuente > p-1) {  /*Cuando se llega al ultimo procesador, se vuelve a trabajar con el primero, asi sucesivamente hasta que no queden lineas en el archivo*/
                      fuente = 1;
                    }
                  }
                }
                archivo.close();


                // for (int fuente = 1; fuente < p; fuente++) {
                //         MPI_Recv(&cadena, 150, MPI_CHAR, fuente, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE); /* Se recibe un sub vector por cada ESCLAVO, para luego calcular el promedio general*/
                //         cout <<"ESTA SE RECIBIO" <<cadena << endl;
                // }


        }



        MPI_Finalize();
        return 0;


}
