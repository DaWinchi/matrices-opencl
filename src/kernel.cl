__kernel void compute(
                int sizeRow,
                __global const int *matrix1,
                __global const int *matrix2,
                __global int *matrixOut)
{
    int row = get_global_id(0);
    int col = get_global_id(1);

    for (int k = 0; k < sizeRow; k++ )
    {
        matrixOut[row*sizeRow + col] += matrix1[row*sizeRow + k]*matrix2[k*sizeRow + col];
    }

}