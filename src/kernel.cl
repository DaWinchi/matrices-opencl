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

    TYPE acc = 0;
	for (int col = 0; col < nCols; col++)
	{ 
		acc = 0;
		for(int k = 0; k < sizeK; k++)
		{
		    acc+= matrix1[sizeK*row + k]* matrix2[sizeK*col + k];
		}    
		matrixOut[nCols*row + col] = acc;
	}
}