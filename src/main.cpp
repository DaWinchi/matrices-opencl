#include "../inc/Computing.h"
#include <ctime>
#include <iostream>
#include <chrono>

typedef float TYPE;
const int NROWS1 = 5000;
const int NCOLS1 = 5000;
const int NROWS2 = 5000;
const int NCOLS2 = 5000;

TYPE **matrix1;
TYPE **matrix2;
TYPE **matrixResultCPU;
TYPE **matrixResultGPU;

void InitializeData()
{
	matrix1 = new TYPE*[NROWS1];
	for (int i = 0; i < NROWS1; i++)
	{
		matrix1[i] = new TYPE[NCOLS1];
	}

	matrix2 = new TYPE*[NROWS2];
	for (int i = 0; i < NROWS2; i++)
	{
		matrix2[i] = new TYPE[NCOLS2];
	}

	matrixResultCPU = new TYPE*[NROWS1];
	for (int i = 0; i < NROWS1; i++)
	{
		matrixResultCPU[i] = new TYPE[NCOLS2];
	}

	matrixResultGPU = new TYPE*[NROWS1];
	for (int i = 0; i < NROWS1; i++)
	{
		matrixResultGPU[i] = new TYPE[NCOLS2];
	}


	/*Initialize matrix 1*/
	for (int i = 0; i < NROWS1; i++)
	{
		for (int j = 0; j < NCOLS1; j++)
		{
			matrix1[i][j] = (TYPE)(rand())/RAND_MAX*10.0;
		}
	}

	/*Initialize matrix 2*/
	for (int i = 0; i < NROWS2; i++)
	{
		for (int j = 0; j < NCOLS2; j++)
		{
			matrix2[i][j] = (TYPE)(rand()) / RAND_MAX * 10.0;
		}
	}

	/*Initialize matrix GPU CPU*/
	for (int i = 0; i < NROWS1; i++)
	{
		for (int j = 0; j < NCOLS2; j++)
		{
			matrixResultGPU[i][j] = 0;
			matrixResultCPU[i][j] = 0;
		}
	}
}

void printResult()
{
	std::cout << std::endl << "Result:\n";
	for (int i = 0; i < NROWS1; i++)
	{
		for (int j = 0; j < NCOLS2; j++)
		{
			printf("%7.2f", matrixResultCPU[i][j]);
		}
		std::cout << std::endl;
	}
}

void ComputingOnHost()
{
	std::cout << "-----------------Host-----------------\n";

	std::cout << "Computing..............";

	auto startTime = std::chrono::steady_clock::now();
	int i = 0, j = 0, k = 0;
#pragma omp parallel for shared(matrix1,matrix2,matrixResultCPU) private(i,j,k)
	for (i = 0; i < NROWS1; i++)
	{
		for (j = 0; j < NCOLS2; j++)
		{
			TYPE sum = 0;
			for (k = 0; k < NCOLS1; k++)
			{
				sum += matrix1[i][k] * matrix2[k][j];
			}
			matrixResultCPU[i][j] = sum;
		}
	}
	auto endTime = std::chrono::steady_clock::now();

	std::cout << "OK\n";
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
	std::cout << "Time: " << time.count() << " ms" << std::endl;

}

void CheckDifference()
{
	int diff = 0;
	for (int r = 0; r < NROWS1; r++)
	{
		for (int c = 0; c < NCOLS2; c++)
		{
			diff += std::abs(matrixResultCPU[r][c] - matrixResultGPU[r][c]);
		}
	}
	std::cout << std::endl << "Matrix difference: " << diff << std::endl;
}

int main(int argc, char **argv)
{
	srand(time(NULL));

	std::cout << "Matrix multiplication\n";
	std::cout << "Matrix 1: " << NROWS1 << "x" << NCOLS1 << std::endl;
	std::cout << "Matrix 2: " << NROWS2 << "x" << NCOLS2 << std::endl;

	InitializeData();
	ComputingOnHost();

	Computing computer;
	computer.getDevicesInfo();
	
	while (true)
	{
		computer.setBarrier(NROWS1/2);
		computer.setData(matrix1, matrix2, matrixResultGPU, NROWS1, NCOLS1, NROWS2, NCOLS2);
		int selectedDevice = 0;
		std::cout << "----------------------------- Single device computing -----------------------------\n";
		std::cout << "Select device: ";
		std::cin >> selectedDevice;
		computer.compute(selectedDevice);
		delete[] matrixResultGPU;
		matrixResultGPU = computer.getResult();
		CheckDifference();

		std::cout << "\n----------------------------- Duplex device computing -----------------------------\n";
		int selectedDevice1 = 0, selectedDevice2 = 0;
		std::cout << "Select device1: ";
		std::cin >> selectedDevice1;
		std::cout << "Select device2: ";
		std::cin >> selectedDevice2;
		computer.compute(selectedDevice1, selectedDevice2);
		delete[] matrixResultGPU;
		matrixResultGPU = computer.getResult();
		CheckDifference();
	}
	system("pause");
	return 0;
}