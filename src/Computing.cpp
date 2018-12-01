#define __CL_ENABLE_EXCEPTIONS

#include "../inc/Computing.h"
#include <iostream>
#include <fstream>
#include <ctime>

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
    std::vector<cl::Device> device = {devices[numDevice]};

    cl::Context context(device);
    cl::CommandQueue comqueque(context,device[0]);

    cl::Buffer vector1 = cl::Buffer(context, 
                        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                        (sizeM1.r*sizeM1.c) * sizeof(int), matrix1);
	cl::Buffer vector2 = cl::Buffer(context,
                        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                        (sizeM2.r*sizeM2.c) * sizeof(int), matrix2);
	cl::Buffer vectorOut = cl::Buffer(context, 
                        CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
                        (sizeRes.r*sizeRes.c) * sizeof(int), matrixResult);
    
	std::ifstream sourceFile("../src/kernel.cl");
	std::string sourceCode(std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
	cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));
	cl::Program program = cl::Program(context, source);

    try
    {
        program.build(device);
    }
    catch(const cl::Error &err) 
    {
     // Выводим ошибки компиляции.
        std::cerr
        << "OpenCL compilation error" << std::endl
        << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device[0])
        << std::endl;
        throw err;
    }
    

    cl::Kernel kernel(program, "compute");

    kernel.setArg(0, sizeM1.c);
    kernel.setArg(1, vector1);
    kernel.setArg(2, vector2);
    kernel.setArg(3, vectorOut);

    clock_t startTime = 0, endTime = 0;
    std::cout<<"Computing..............";

    startTime = clock();
    comqueque.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(sizeRes.r, sizeRes.c));
    endTime = clock();

    std::cout<<"OK\n";
    double time = (double)(endTime - startTime)/CLOCKS_PER_SEC*1000;
    std::cout<<"Time: "<<time<<" ms"<<std::endl;
    
    comqueque.enqueueReadBuffer(vectorOut, CL_TRUE, 0, sizeRes.r*sizeRes.c*sizeof(int),matrixResult);
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

void Computing::printData()
{
    std::cout<<std::endl<<"Matrix 1:\n";
    for (int i = 0; i < sizeM1.r*sizeM1.c; i++ )
    {
        printf("%3i", matrix1[i]);
        if(i % sizeM1.c == (sizeM1.c-1))
        {
            std::cout<<std::endl;
        }
    }

    std::cout<<std::endl<<"Matrix 2:\n";
    for (int i = 0; i < sizeM2.r*sizeM2.c; i++ )
    {
        printf("%3i", matrix2[i]);
        if(i % sizeM2.c == (sizeM2.c-1))
        {
            std::cout<<std::endl;
        }
    }

}

void Computing::printResult()
{
    std::cout<<std::endl<<"Result:\n";
    for (int i = 0; i < sizeRes.r*sizeRes.c; i++ )
    {
        printf("%6i", matrixResult[i]);
        if(i % sizeM1.c == (sizeRes.c-1))
        {
            std::cout<<std::endl;
        }
    }
}

Computing::~Computing(){}