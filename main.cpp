#include <mpi.h>    //MPI_*
#include <vector>   //vector<int>
#include <string>
#include <cstdlib>  //rand()
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <fstream>

using namespace std;

bool comprobarCuboMagico(vector<vector<int> > cuboMagico){
        int sumaDiagPrinc = 0;
        int sumaDiagSecun = 0;
        int size = cuboMagico.size();
        vector<int> sumas;
        for(int pivote = 0; pivote < cuboMagico.size(); pivote++) {
                int sumaFila = 0;
                int sumaCol = 0;
                sumaDiagPrinc += cuboMagico.at(pivote).at(pivote);
                sumaDiagSecun += cuboMagico.at(pivote).at(size - (pivote + 1));

                for(int col = 0; col < cuboMagico.at(pivote).size(); col++) {
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
                if(primer != iterador) {
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

        return resultado;
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
        return cuboMagico;
}


int main(){
        vector<string> archivo;
        char cadena[128];
        ifstream fe("prueba.txt");

        while(!fe.eof()) {
                fe >> cadena;
                archivo.push_back(cadena);
                // cout << cadena << endl;
        }
        fe.close();
        // cout << archivo.size() -1 << endl;


        int mi_rango;   /* rango del proceso    */
        int p;   /* numero de procesos   */
        int tag = 0;   /* etiqueta del mensaje */
        int* vec = NULL; /* vector que representa una fila del archivo*/
        vector<int> limpio;
        string vacio;
        int fuente = 1;

        MPI_Init(NULL, NULL);
        MPI_Comm_size(MPI_COMM_WORLD, &p);
        MPI_Comm_rank(MPI_COMM_WORLD, &mi_rango);

        if (mi_rango != 0) {     /* -- Esclavos -- fuentes encargadas de realizar los calculos y verificar si los vectores recibidos cumplen con la matriz magica*/
                string cadena;
                MPI_Recv(&cadena, cadena.length(), MPI_STRING, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE); /* Se recibe un sub vector para luego calcular el sub promedio*/

                limpio = limpiaString(cadena);
                if(comprobarCuboMagico(limpio)) {
                        MPI_Send(cadena, cadena.length(), MPI_STRING, 0, tag, MPI_COMM_WORLD);
                }else{
                        MPI_Send(vacio, vacio.length(), MPI_STRING, 0, tag, MPI_COMM_WORLD);
                }

        } else {        /* -- MASTER -- fuente encargada de distribuir los sub vectores*/
                while(!fe.eof()) {
                        fe >> cadena;
                        MPI_Send(&cadena, cadena.length(), MPI_STRING, fuente, tag, MPI_COMM_WORLD);         /* Envía sub vectores a todas las fuentes (exceptuando MASTER)*/
                        fuente++;
                        if(fuente == p-1) {
                                fuente = 1;
                        }

                }

                for (int fuente = 1; fuente < p; fuente++) {
                        MPI_Recv(&cadena, cadena.length(), MPI_STRING, fuente, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE); /* Se recibe un sub vector por cada ESCLAVO, para luego calcular el promedio general*/
                        cout << cadena << endl;
                }

                // sub_prom=obtener_sub_prom(div_conjunto,sub_vec);  /* Sub promedio del vector */
                // prom=prom+sub_prom;   /* Se comienza la sumatoria para el promedio general */
                // printf("Sub promedio MASTER %d : %.4f\n",mi_rango, sub_prom );
                //
                // printf("#PROMEDIO GENERAL (MASTER %d) : %.4f\n",mi_rango, prom );
                //
                // for (int fuente = 1; fuente < p; fuente++) {
                //         MPI_Send(&prom, 2, MPI_FLOAT, fuente, tag, MPI_COMM_WORLD);; /* Envía el promedio general a todas las fuentes (exceptuando MASTER)*/
                // }
                //
                // for (int fuente = 1; fuente < p; fuente++) {
                //         MPI_Recv(&sumatoria_diferencias, 2, MPI_FLOAT, fuente, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE); /* Se recibe una sub sumatoria por cada ESCLAVO, para luego calcular la varianza*/
                //         sumatoria_gral=sumatoria_gral+sumatoria_diferencias; /* Se agregan los promedios que van llegando*/
                // }
                //
                // sumatoria_diferencias=obtener_dif_cuadrada(prom,div_conjunto,sub_vec); /* Se obtiene la sub diferencia al cuadrado de MASTER*/
                // sumatoria_gral=sumatoria_gral+sumatoria_diferencias;
                // printf("Sub sumatoria de diferencias al cuadrado MASTER %d : %.4f\n",mi_rango, sumatoria_diferencias );
                //
                // printf("Sumatoria General de diferencias al cuadrado (MASTER %d) : %.4f\n",mi_rango, sumatoria_gral );
                //
                // varianza=sumatoria_gral/100000.0;
                //
                // printf("#VARIANZA (MASTER %d) : %.4f\n",mi_rango, varianza );
                //
                // desviacion= desv(varianza);   /* Se calcula la desviacion estandar en base a la varianza */
                //
                // printf("#DESVIACION ESTANDAR (MASTER %d) : %.4f\n",mi_rango, desviacion );

                //print(mi_rango, p, div_conjunto, sub_vec);
        }



        MPI_Finalize();
        return 0;



        //vector<vector<>>

        // MPI_Init(NULL, NULL);
        //
        // int cantProcesadores = 0;
        // MPI_Comm_size(MPI_COMM_WORLD, &cantProcesadores);
        //
        // int procesador = 0;
        // MPI_Comm_rank(MPI_COMM_WORLD, &procesador);
        //
        // MPI_Status status;
        //
        //
        //
        // if (procesador == 0) {
        //
        //
        // } else {
        //
        //
        // }
        //
        // MPI_Finalize();

        // vector<vector<int> > cuboMagico;
        // vector<int> a = {5, 0, 1};
        // vector<int> b = {-2, 0, 6};
        // vector<int> c = {3, 4, -1};
        //
        // cuboMagico.push_back(a);
        // cuboMagico.push_back(b);
        // cuboMagico.push_back(c);
        return 0;

}
