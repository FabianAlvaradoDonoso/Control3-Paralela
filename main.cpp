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
        char cadenaArch[128];
        ifstream fe("prueba.txt");

        int mi_rango;   /* rango del proceso    */
        int p;   /* numero de procesos   */
        int tag = 0;   /* etiqueta del mensaje */
        int* vec = NULL; /* vector que representa una fila del archivo*/
        vector<vector<int> > limpio;
        string vacio;
        char* cadena = NULL;

        MPI_Init(NULL, NULL);
        MPI_Comm_size(MPI_COMM_WORLD, &p);
        MPI_Comm_rank(MPI_COMM_WORLD, &mi_rango);

        if (mi_rango != 0) {     /* -- Esclavos -- fuentes encargadas de realizar los calculos y verificar si los vectores recibidos cumplen con la matriz magica*/
                cadena = (char*) malloc(150 * sizeof(char));
                cout << "Estoy arriba del recv" << endl;
                MPI_Recv(&cadena, 150, MPI_CHAR, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE); /* Se recibe un sub vector para luego calcular el sub promedio*/
                cout << "Estoy abajo del recv " << cadena << endl;


                limpio = limpiaString(cadena);
                if(comprobarCuboMagico(limpio)) {
                        MPI_Send(&cadena, 150, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
                }else{
                        MPI_Send(&vacio, 150, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
                }

        } else {        /* -- MASTER -- fuente encargada de distribuir los sub vectores*/
                int fuente = 1;
                std::cout << p << '\n';

                while(!fe.eof()) {
                        cout << "Distribuyendo lineas" << endl;
                        fe >> cadenaArch;
                        MPI_Send(&cadenaArch, 150, MPI_CHAR, fuente, tag, MPI_COMM_WORLD);         /* Envía sub vectores a todas las fuentes (exceptuando MASTER)*/
                        fuente++;
                        cout << "Holi" << endl;
                        if(fuente >= p-1) {
                                fuente = 1;
                        }

                }
                fe.close();


                for (int fuente = 1; fuente < p; fuente++) {
                        MPI_Recv(&cadena, 150, MPI_CHAR, fuente, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE); /* Se recibe un sub vector por cada ESCLAVO, para luego calcular el promedio general*/
                        cout << cadena << endl;
                }


        }



        MPI_Finalize();
        return 0;


}
