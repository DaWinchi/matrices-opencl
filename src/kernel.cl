typedef float TYPE;
__kernel void compute(
                int sizeK,
                int nRows,
                int nCols,
                __global const TYPE *matrix1,
                __global const TYPE *matrix2,
                __global TYPE *matrixOut)
{
    int row = get_global_id(0);
    int col = get_global_id(1);

    TYPE acc = 0;
    for(int k = 0; k < sizeK; k++)
    {
        acc+= matrix1[sizeK*row + k]* matrix2[k*nCols + col];
    }    
    matrixOut[nCols*row + col] = acc;
}