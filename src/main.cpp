#include <iostream>
#include <ctime>

const size_t NROWS1 = 2;
const size_t NCOLS1 = 10;
const size_t NROWS2 = 10;
const size_t NCOLS2 = 5;

int matrix1 [NROWS1][NCOLS1];
int matrix2 [NROWS2][NCOLS2];
int matrixResult [NROWS1][NCOLS2];

void InitializeData()
{   
    /*Initialize matrix 1*/
    for (size_t i = 0; i< NROWS1; i++ )
    {
        for (size_t j = 0; j< NCOLS1; j++ )
        { 
            matrix1[i][j] = rand() % 101;
        }  
    }

    /*Initialize matrix 2*/
    for (size_t i = 0; i< NROWS2; i++ )
    {
        for (size_t j = 0; j< NCOLS2; j++ )
        {
            matrix2[i][j] = rand() % 101;
        }  
    }
}

void ComputingOnHost()
{

}

int main (int argc, char **argv)
{
    srand(time(NULL));
    std::cout<<"Hello world\n";
    return 0;
}