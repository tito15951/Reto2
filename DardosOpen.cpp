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
int N;
int Hilos;
const double factor=1.0/RAND_MAX;

string Convert (float number){
    ostringstream buff;
    buff<<number;
    return buff.str();}
    

void Calcular(int* hits)
{
	omp_set_num_threads(Hilos);
	int Limite=N/Hilos;
	long k;
	double x,y;
	#pragma omp parallel shared(hits,Limite) private(k,x,y)
	{
	srand((int)clock());
		for(k=0;k<Limite;++k)
		{
			x = rand()*factor;
			y = rand()*factor;
			if(x*x+y*y<1)
				{hits[omp_get_thread_num()]+=1;}
		}
	}
}
    
int main(int argc, char const *argv[])
{	N=atoi(argv[1]);
	int Intentos=atoi(argv[2]);
	Hilos=atoi(argv[3]);
	/*Hilos=2;
	N=10;
	int Intentos=1;*/
	int hits[Hilos];
	ofstream file;
	file.open("ResultadosDardosOpen.txt",ios::app);
	struct timeval start;
  	struct timeval end;
  	double milisecs;
  	long seconds, useconds;
	file << "Resultados para "<<N<<" dardos con "<<Hilos<< " hilos\n";
	file << "PI;Segundos;Error\n";
	for(int inte=0;inte<Intentos;inte++)
		{
		for(int i=0;i<Hilos;i++)
			hits[i]=0;
		gettimeofday(&start, 0);
		Calcular(hits);
		gettimeofday(&end, 0);	
	  	seconds  = end.tv_sec  - start.tv_sec;
	  	useconds = end.tv_usec - start.tv_usec;
	  	milisecs = ((seconds) * 1000 + useconds/1000.0);
		float Suma=0;
		for(int i=0;i<Hilos;i++)
			Suma+=hits[i];
		float pi_approx=4.0*Suma/N;
	    float error=((M_PI-pi_approx)/pi_approx)*100;
	    cout << "PI=: " <<pi_approx<<endl;
	    cout << "Segundos= "<<milisecs/1000<<endl;
	    cout << "Error relativo=" << error <<"%\n"<< endl; 
	    file << Convert(pi_approx)<<";"<< Convert(milisecs/1000)<<";"<< Convert(error)<<";\n";
	}
return 0;
}
