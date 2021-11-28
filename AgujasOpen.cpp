#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <omp.h>
#include <math.h>

using namespace std;

int Hilos;

string Convert (float number){
    std::ostringstream buff;
    buff<<number;
    return buff.str();}
    
class Floor{
	public:
		float l;//Distancia entre las lineas verticales
};
class Needle{
	public:
		float x;//Distancia entre el origen y el centro de la aguja
		float t;//Angulo entre la linea de la x y la pos de la aguja
		float L;//Longitud de la aguja
		void Imprimir(){
			cout<< "x="<< x<< " t="<<t<<endl;
		}
};
Needle TirarAguja(float Longitud)
{
	double limite=1.0/RAND_MAX;
	Needle Aguja;
	srand((int)clock());
	Aguja.x=(double)rand()*limite;
	Aguja.t=(double)rand()*limite*180;
	Aguja.L=Longitud;
	return Aguja;
}
bool Cruzar_Linea(Needle Aguja, float Piso)
{
	float x_right_tip = Aguja.x + Aguja.L / 2 * sin(Aguja.t*M_PI/180);
    float x_left_tip  = Aguja.x - Aguja.L / 2 * sin(Aguja.t*M_PI/180);
    return x_right_tip >= Piso || x_left_tip <= 0.0;
}
void Estimar_Probabilidad_Aguja_Cruzar_Linea(int L, int NAgujas, int* nb_crosses)
{
	omp_set_num_threads(Hilos);
	int t;
	#pragma omp parallel shared(nb_crosses,NAgujas) private(t)
  	{
    for (t=0;t<NAgujas;t++)
        {Needle Aguja=TirarAguja(L);
        
		if (Cruzar_Linea(Aguja, 1))
			{
			#pragma omp critical
				{cout <<"Hilo: "<<omp_get_thread_num()<<" dice: ";
				Aguja.Imprimir();}
			nb_crosses[omp_get_thread_num()]+=1;}//Modifica solo la posicion que es el	
	}		
    }
}


int main(int argc, char const *argv[])
{
	float NAgujas=(float)atoi(argv[1]);
	int Intentos=atoi(argv[2]);
	Hilos=atoi(argv[3]);
	/*float NAgujas=10;
	int Intentos=1;
	Hilos=2;*/
	int CantAgujas;
	//float NAgujas=1000000;
	//int Intentos=2;
	//int Hilos=1;
	int nb_crosses[Hilos];
	ofstream file;
	struct timeval start;
	struct timeval end;
  	double milisecs;
  	long seconds, useconds;
  	file.open("ResultadosAgujasOpen.txt",ios::app);
	file << "Resultados para "<<NAgujas<<" agujas con "<<Hilos<<" hilos\n";
	file << "PI;Segundos;Aciertos;Error\n";
	for(int inte=0;inte<Intentos;inte++)
		{
			for(int i=0;i<Hilos;i++)
				nb_crosses[i]=0;
			CantAgujas=NAgujas/Hilos;
			gettimeofday(&start, 0);
			Estimar_Probabilidad_Aguja_Cruzar_Linea(1,CantAgujas,nb_crosses);
			gettimeofday(&end, 0);	
	  		seconds  = end.tv_sec  - start.tv_sec;
	  		useconds = end.tv_usec - start.tv_usec;
	  		milisecs = ((seconds) * 1000 + useconds/1000.0);		
			float Suma=0;
			for(int i=0;i<Hilos;i++)
				Suma+=nb_crosses[i];
		    float pi_calculated= 2.0*(NAgujas/Suma);
		    float error=((M_PI-pi_calculated)/pi_calculated)*100;
		    cout << "PI=: " <<pi_calculated<<endl;
		    cout << "Segundos= "<<milisecs/1000<<endl;
		    cout << "Numero de intentos= "<<NAgujas<< " Numero de aciertos= "<<Suma<<endl;
		    cout << "Error relativo=" << error <<"%\n"<< endl;
		    file << Convert(pi_calculated)<<";"<< Convert(milisecs/1000)<<";"<< Convert(Suma)<<";"<< Convert(error)<<";\n";
		}
	file.close();
}
