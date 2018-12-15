#include "../inc/Computing.h"
#include <ctime>
#include <iostream>
#include <chrono>

const int NROWS1 = 6000;
const int NCOLS1 = 3000;
const int NROWS2 = 3000;
const int NCOLS2 = 3000;

int **matrix1;
int **matrix2;
int **matrixResultCPU;
int **matrixResultGPU;

void InitializeData()
{
	matrix1 = new int*[NROWS1];
	for (int i = 0; i < NROWS1; i++)
	{
		matrix1[i] = new int[NCOLS1];
	}

	matrix2 = new int*[NROWS2];
	for (int i = 0; i < NROWS2; i++)
	{
		matrix2[i] = new int[NCOLS2];
	}

	matrixResultCPU = new int*[NROWS1];
	for (int i = 0; i < NROWS1; i++)
	{
		matrixResultCPU[i] = new int[NCOLS2];
	}

	matrixResultGPU = new int*[NROWS1];
	for (int i = 0; i < NROWS1; i++)
	{
		matrixResultGPU[i] = new int[NCOLS2];
	}


	/*Initialize matrix 1*/
	for (int i = 0; i < NROWS1; i++)
	{
		for (int j = 0; j < NCOLS1; j++)
		{
			matrix1[i][j] = rand() % 10;
		}
	}

	/*Initialize matrix 2*/
	for (int i = 0; i < NROWS2; i++)
	{
		for (int j = 0; j < NCOLS2; j++)
		{
			matrix2[i][j] = rand() % 10;
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
			printf("%6i", matrixResultCPU[i][j]);
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
			int sum = 0;
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
		computer.setBarrier(1500);
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