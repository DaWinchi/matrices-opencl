#include "../inc/Computing.h"
#include <iostream>

Computing::Computing()
{
    platforms.clear();
    devices.clear();

    /*Get platfroms*/
    cl::Platform::get(&platforms);

    /*Get all devices*/
    for (cl::Platform plat : platforms)
    {
        std::vector<cl::Device> device;
        plat.getDevices(CL_DEVICE_TYPE_ALL, &device);
        devices.insert(devices.end(), device.begin(), device.end());
    }

    matrix1 = nullptr;
    matrix2 = nullptr;
    matrixResult = nullptr;
}

void Computing::getDevicesInfo()
{
    if(devices.size() < 0)
    {
        std::cout<<"System isn't has any OpenCL devices..\n";
        return;
    }
    std::cout<<"----------------------------- Devices -----------------------------\n";
    for (cl::Device device : devices)
    {
        std::cout<<"Name: "<<device.getInfo<CL_DEVICE_NAME>()<<std::endl;
        std::cout<<"Vendor: "<<device.getInfo<CL_DEVICE_VENDOR>()<<std::endl;
        std::cout<<"Version: "<<device.getInfo<CL_DEVICE_VERSION>()<<std::endl;
        std::cout<<std::endl;
    }
}

void Computing::compute(int numDevice)
{
    std::cout<<"----------------------------- Computing -----------------------------\n";
    if(matrix1 == nullptr || matrix2 == nullptr || matrixResult == nullptr)
    {
        std::cout<<"Error: Data is null\n";
        return;
    }
    cl::Device device = devices[numDevice];

    cl::Context context(device);
    cl::CommandQueue comqueque(context, device);


}

void Computing::setData(int **pMatr1, int **pMatr2, int **pMatrResult, 
                        int nRows1, int nCols1, int nRows2, int nCols2)
{
    sizeM1.r = nRows1;
    sizeM1.c = nCols1;
    sizeM2.r = nRows2;
    sizeM2.c = nCols2;
    sizeRes.r = nRows1;
    sizeRes.c = nCols2;

    matrix1 = new int [sizeM1.r*sizeM1.c];
    matrix2 = new int [sizeM2.r*sizeM2.c];
    matrixResult = new int [sizeRes.r*sizeRes.c];

    for (int i = 0; i < sizeM1.r; i++)
    {
        for (int j = 0; j< sizeM1.c; j++)
        {
            matrix1[i*sizeM1.c + j] = pMatr1[i][j];
        }
    }

    for (int i = 0; i < sizeM2.r; i++)
    {
        for (int j = 0; j< sizeM2.c; j++)
        {
            matrix2[i*sizeM2.c + j] = pMatr2[i][j];
        }
    }

    for (int i = 0; i < sizeRes.r; i++)
    {
        for (int j = 0; j< sizeRes.c; j++)
        {
            matrixResult[i*sizeRes.c + j] = pMatrResult[i][j];
        }
    }

}

Computing::~Computing(){}