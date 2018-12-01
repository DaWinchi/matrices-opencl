#include <CL/cl.hpp>

struct Size
{
    int r;
    int c;
};

class Computing
{
    public:
    Computing();
    virtual ~Computing();
    void getDevicesInfo();
    void compute(int numDevice);
    void setData(int **pMatr1, int **pMatr2, int **pMatrResult, int nRows1, int nCols1, int nRows2, int nCols2);
    void printData();

    int *matrix1;
    int *matrix2;
    int *matrixResult;

    Size sizeM1, sizeM2, sizeRes;
    std::vector<cl::Platform> platforms;
    std::vector<cl::Device> devices;

};