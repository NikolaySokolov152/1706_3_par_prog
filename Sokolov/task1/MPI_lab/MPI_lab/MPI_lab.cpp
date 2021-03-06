// MPI_lab.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"

#include "mpi.h"
#include <iostream>
#include <windows.h>
#include <fstream>
#include <ctime>

using namespace std;

int main(int argc, char **argv)
{
	srand(time(0));
	int ProcNum, ProcRank;
	MPI_Init(&argc, &argv);
	MPI_Status status;

	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	
	//Начало
	if (ProcRank == 0)
	{
		//Чтение данных
		int n = 0;
		if (argc > 1)
			n = atoi(argv[1]);
		
		int min = 0;
		int max = 10;

		if (argc > 2)
		min	= atoi(argv[2]);
		if (argc > 3)
		max = atoi(argv[3]);

		float* Arr = new float[n];
		for (int i = 0; i < n; i++)
		{
			 Arr[i] = rand()%(max-min+1)+min;
		}

		cout << "size vektor " << n << endl;
		for (int i = 0; i < n; i++)
		{
			cout << Arr[i] << " ";
		}
		cout << endl;


		//расчёт нагрузки на процессы
		//cколько послать данных
		int nd = n / ProcNum;
			
		//рассылка кол-ва данных
		MPI_Bcast(&nd, 1, MPI_INT, 0, MPI_COMM_WORLD);

		//рассылка данных
		float* Arr0 = new float[nd];

		MPI_Scatter(Arr, nd, MPI_FLOAT,
			        Arr0, nd, MPI_FLOAT,
			       0, MPI_COMM_WORLD);


		//работа 0 ранга
		float Max = numeric_limits<float>::lowest();
		if (nd != 0)
		{
			Max = Arr[0];
			for (int i = 1; i < nd; i++)
				if (Max < Arr[i])
					Max = Arr[i];
		}
		if ((n%ProcNum) != 0)
		{
			for (int i = 0; i < n % ProcNum; i++)
				if (Max < Arr[nd*ProcNum + i])
				Max = Arr[nd*ProcNum + i];
		}
		
			cout << "Process " << ProcRank << "  size data " << nd + n % ProcNum << "   Max " << Max << endl;

			float ResultMax;

			MPI_Reduce(&Max, &ResultMax, 1, MPI_FLOAT,MPI_MAX ,0, MPI_COMM_WORLD);

			if (ResultMax != numeric_limits<float>::lowest())
			cout << "\n Rezult " << ResultMax << endl;
			else 
				cout << "Max not is" << endl;
		
		delete Arr;
		delete Arr0;
	}
	//работа других процессов
	else
	{
		int np;
		MPI_Bcast(&np, 1, MPI_INT, 0, MPI_COMM_WORLD);
		
		float* pArr = new float[np];

		MPI_Scatter(NULL, np, MPI_FLOAT,
		        	pArr, np, MPI_FLOAT,
			        0, MPI_COMM_WORLD);

		float Max = numeric_limits<float>::lowest();
		if (np != 0)
		{
			Max = pArr[0];
			for (int i = 1; i < np; i++)
				if (Max < pArr[i])
					Max = pArr[i];

			cout << "Process " << ProcRank << "  size data " << np << "   Max " << Max << endl;
		}

		MPI_Reduce(&Max, NULL, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);

		delete pArr;

	}
	MPI_Finalize();

}

//залить файл проекта и с++