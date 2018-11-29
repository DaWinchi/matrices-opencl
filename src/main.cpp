#include "../inc/Computing.h"
#include <ctime>
#include <iostream>

const int NROWS1 = 1000;
const int NCOLS1 = 1000;
const int NROWS2 = 1000;
const int NCOLS2 = 1000;

int **matrix1;
int **matrix2;
int **matrixResultCPU;
int **matrixResultGPU;

void InitializeData()
{   
    matrix1 = new int* [NROWS1];
    for (int i = 0; i< NROWS1; i++ )
    { 
        matrix1[i] = new int [NCOLS1];
    }  

    matrix2 = new int* [NROWS2];
    for (int i = 0; i< NROWS2; i++ )
    { 
        matrix2[i] = new int [NCOLS2];
    } 

    matrixResultCPU = new int* [NROWS1];
    for (int i = 0; i< NROWS1; i++ )
    { 
        matrixResultCPU[i] = new int [NCOLS2];
    }  

    matrixResultGPU = new int* [NROWS1];
    for (int i = 0; i< NROWS1; i++ )
    { 
        matrixResultGPU[i] = new int [NCOLS2];
    }    
    

    /*Initialize matrix 1*/
    for (int i = 0; i< NROWS1; i++ )
    {
        for (int j = 0; j< NCOLS1; j++ )
        { 
            matrix1[i][j] = rand() % 101;
        }  
    }

    /*Initialize matrix 2*/
    for (int i = 0; i< NROWS2; i++ )
    {
        for (int j = 0; j< NCOLS2; j++ )
        {
            matrix2[i][j] = rand() % 101;
        }  
    }

    /*Initialize matrix GPU CPU*/
    for (int i = 0; i< NROWS1; i++ )
    {
        for (int j = 0; j< NCOLS2; j++ )
        {
            matrixResultGPU[i][j] = 0;
            matrixResultCPU[i][j] = 0;
        }  
    }
}

void ComputingOnHost()
{
    std::cout<<"-----------------Host-----------------\n";
    clock_t startTime, endTime;

    std::cout<<"Computing..............";

    startTime = clock();  
    for (int i = 0 ; i< NROWS1; i++)
    {        
        for (int j = 0 ; j< NCOLS2; j++)
        {
            int sum = 0;
            for (int k = 0; k < NCOLS1; k++)
            {
                sum += matrix1[i][k]*matrix2[k][j];
            }
            matrixResultCPU[i][j] = sum;
        }    
    }
    endTime = clock();

    std::cout<<"OK\n";
    double time = (double)(endTime - startTime)/CLOCKS_PER_SEC*1000;
    std::cout<<"Time: "<<time<<" ms"<<std::endl;

}

int main (int argc, char **argv)
{
    srand(time(NULL));
    InitializeData();
    ComputingOnHost();

    Computing computer;
    computer.getDevicesInfo();

    computer.setData(matrix1, matrix2, matrixResultGPU, NROWS1, NCOLS1, NROWS2, NCOLS2);
    //computer.matrix1 = &matrix1;
    computer.compute(0);    


    return 0;
}