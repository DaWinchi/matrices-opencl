#include <iostream>
#include <ctime>

const int NROWS1 = 1000;
const int NCOLS1 = 1000;
const int NROWS2 = 1000;
const int NCOLS2 = 1000;

int matrix1 [NROWS1][NCOLS1];
int matrix2 [NROWS2][NCOLS2];
int matrixResultCPU [NROWS1][NCOLS2];
int matrixResultGPU [NROWS1][NCOLS2];

void InitializeData()
{   
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
    ComputingOnHost();
    return 0;
}