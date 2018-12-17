#define __CL_ENABLE_EXCEPTIONS

#include "../inc/Computing.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>

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

    _matrix1 = nullptr;
    _matrix2 = nullptr;
    _matrixResult = nullptr;
	_matrix1Up = nullptr;
	_matrix1Down = nullptr;
	_matrixResultUp = nullptr;
	_matrixResultDown = nullptr;
	_labelDevide = 0;
}

void Computing::setBarrier(int label)
{
	_labelDevide = label;
}

void Computing::getDevicesInfo()
{
    if(devices.size() < 0)
    {
        std::cout<<"System isn't has any OpenCL devices..\n";
        return;
    }
    std::cout<<"----------------------------- OpenCL Devices -----------------------------\n";

	int num = 0;
    for (cl::Device device : devices)
    {
        std::cout<<"["<<num<<"]"<<"Name: "<<device.getInfo<CL_DEVICE_NAME>()<<std::endl;
        std::cout<<"Vendor: "<<device.getInfo<CL_DEVICE_VENDOR>()<<std::endl;
        std::cout<<"Version: "<<device.getInfo<CL_DEVICE_VERSION>()<<std::endl;
        std::cout<<std::endl;
		num++;
    }
}

void Computing::compute(int numDevice)
{
    std::cout<<"----------------------------- Computing -----------------------------\n";
    if(_matrix1 == nullptr || _matrix2 == nullptr || _matrixResult == nullptr)
    {
        std::cout<<"Error: Data is null\n";
        return;
    }

    std::cout<<"Device: "<<devices[numDevice].getInfo<CL_DEVICE_NAME>()<<std::endl;
    std::vector<cl::Device> device = {devices[numDevice]};

    cl::Context context(device);
    cl::CommandQueue comqueque(context,device[0]);

    cl::Buffer vector1 = cl::Buffer(context, 
                        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                        (sizeM1.r*sizeM1.c) * sizeof(TYPE), _matrix1);
	cl::Buffer vector2 = cl::Buffer(context,
                        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                        (sizeM2.r*sizeM2.c) * sizeof(TYPE), _matrix2);
	cl::Buffer vectorOut = cl::Buffer(context, 
                        CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
                        (sizeRes.r*sizeRes.c) * sizeof(TYPE), _matrixResult);
    
	std::ifstream sourceFile("../src/kernel.cl");
	//std::string sourceCode(std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
	std::string sourceCode =
		" #define SIZEK        "+ std::to_string(sizeM1.c) +					   "\r\n"
		" #define NROWS        " + std::to_string(sizeRes.r) +						"\r\n"
		" #define NCOLS        " + std::to_string(sizeRes.c) +						"\r\n"
		" #define REALTYPE4 float4													\r\n"
		"	typedef float TYPE;														\r\n"
		"__kernel void compute(														\r\n"
		"			__global const TYPE *matrix1,									\r\n"
		"			__global const TYPE *matrix2,									\r\n"
		"			__global TYPE *matrixOut)										\r\n"
		"		{																	\r\n"
		"			int row = get_global_id(0);										\r\n"
		"			REALTYPE4 row_buf[SIZEK/4];											\r\n"
		"			for( int c = 0; c < SIZEK/4; c++)									\r\n"
		"			{																\r\n"
		"				row_buf[c] = (REALTYPE4)(matrix1[SIZEK*row + 4*c],			\r\n"
		"										matrix1[SIZEK*row + 4*c + 1],		\r\n"
		"										matrix1[SIZEK*row + 4*c + 2],		\r\n"
		"										matrix1[SIZEK*row + 4*c + 3]);		\r\n"
		"			}																\r\n"
		"			TYPE acc = 0;													\r\n"
		"			for (int col = 0; col < NCOLS; col++)							\r\n"
		"			{																\r\n"
		"				acc = 0;													\r\n"
		"				for (int k = 0; k < SIZEK/4; k++)								\r\n"
		"				{															\r\n"
		"				acc += dot(						row_buf[k],                      \r\n"
		"							 ( REALTYPE4 ) ( matrix2[col * SIZEK + 4*k     ],             \r\n"
		"											 matrix2[col * SIZEK + 4*k + 1 ],             \r\n"
		"											 matrix2[col * SIZEK + 4*k + 2 ],             \r\n"
		"											 matrix2[col * SIZEK + 4*k + 3 ] ) );         \r\n"
		"				}																		\r\n"
		"				matrixOut[NCOLS*row + col] = acc;										\r\n"
		"			}																			\r\n"
		"		}																				\r\n";



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
		system("pause");
        //throw err;
    }
    

    cl::Kernel kernel(program, "compute");

    kernel.setArg(0, vector1);
    kernel.setArg(1, vector2);
    kernel.setArg(2, vectorOut);

    std::cout<<"Computing..............";
	auto startTime = std::chrono::steady_clock::now();
    comqueque.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(sizeRes.r));
    comqueque.finish();
	auto endTime = std::chrono::steady_clock::now();

    std::cout<<"OK\n";
    
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout<<"Time: "<<time.count()<<" ms"<<std::endl;

    comqueque.enqueueReadBuffer(vectorOut, CL_TRUE, 0, sizeRes.r*sizeRes.c*sizeof(TYPE),_matrixResult);
}

void Computing::compute(int numDevice1, int numDevice2)
{
	std::cout << "----------------------------- Computing -----------------------------\n";
	if (_matrix1Up == nullptr || _matrix1Down == nullptr || _matrix2 == nullptr
		|| _matrixResultUp == nullptr || _matrixResultDown == nullptr)
	{
		std::cout << "Error: Data is null\n";
		return;
	}

	std::cout << "Device 1: " << devices[numDevice1].getInfo<CL_DEVICE_NAME>() << std::endl;
	std::cout << "Device 2: " << devices[numDevice2].getInfo<CL_DEVICE_NAME>() << std::endl;
	std::vector<cl::Device> device1 = { devices[numDevice1] };
	std::vector<cl::Device> device2 = { devices[numDevice2] };

	cl::Context context1(device1);
	cl::Context context2(device2);

	cl::CommandQueue comqueque1(context1, device1[0]);
	cl::CommandQueue comqueque2(context2, device2[0]);

	cl::Buffer vector1Up = cl::Buffer(context1,
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		(_sizeM1Up.r*_sizeM1Up.c) * sizeof(TYPE), _matrix1Up);
	cl::Buffer vector2Up = cl::Buffer(context1,
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		(sizeM2.r*sizeM2.c) * sizeof(TYPE), _matrix2);
	cl::Buffer vectorOutUp = cl::Buffer(context1,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		(_sizeResultUp.r*_sizeResultUp.c) * sizeof(TYPE), _matrixResultUp);

	cl::Buffer vector1Down = cl::Buffer(context2,
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		(_sizeM1Down.r*_sizeM1Down.c) * sizeof(TYPE), _matrix1Down);
	cl::Buffer vector2Down = cl::Buffer(context2,
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		(sizeM2.r*sizeM2.c) * sizeof(TYPE), _matrix2);
	cl::Buffer vectorOutDown = cl::Buffer(context2,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		(_sizeResultDown.r*_sizeResultDown.c) * sizeof(TYPE), _matrixResultDown);

	std::ifstream sourceFile("../src/kernel.cl");
	//std::string sourceCode(std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
	std::string sourceCode1 =
		" #define SIZEK        " + std::to_string(_sizeM1Up.c) + "\r\n"
		" #define NROWS        " + std::to_string(_sizeResultUp.r) + "\r\n"
		" #define NCOLS        " + std::to_string(_sizeResultUp.c) + "\r\n"
		" #define REALTYPE4 float4													\r\n"
		"	typedef float TYPE;														\r\n"
		"__kernel void compute(														\r\n"
		"			__global const TYPE *matrix1,									\r\n"
		"			__global const TYPE *matrix2,									\r\n"
		"			__global TYPE *matrixOut)										\r\n"
		"		{																	\r\n"
		"			int row = get_global_id(0);										\r\n"
		"			REALTYPE4 row_buf[SIZEK/4];											\r\n"
		"			for( int c = 0; c < SIZEK/4; c++)									\r\n"
		"			{																\r\n"
		"				row_buf[c] = (REALTYPE4)(matrix1[SIZEK*row + 4*c],			\r\n"
		"										matrix1[SIZEK*row + 4*c + 1],		\r\n"
		"										matrix1[SIZEK*row + 4*c + 2],		\r\n"
		"										matrix1[SIZEK*row + 4*c + 3]);		\r\n"
		"			}																\r\n"
		"			TYPE acc = 0;													\r\n"
		"			for (int col = 0; col < NCOLS; col++)							\r\n"
		"			{																\r\n"
		"				acc = 0;													\r\n"
		"				for (int k = 0; k < SIZEK/4; k++)								\r\n"
		"				{															\r\n"
		"				acc += dot(						row_buf[k],                      \r\n"
		"							 ( REALTYPE4 ) ( matrix2[col * SIZEK + 4*k     ],             \r\n"
		"											 matrix2[col * SIZEK + 4*k + 1 ],             \r\n"
		"											 matrix2[col * SIZEK + 4*k + 2 ],             \r\n"
		"											 matrix2[col * SIZEK + 4*k + 3 ] ) );         \r\n"
		"				}																		\r\n"
		"				matrixOut[NCOLS*row + col] = acc;										\r\n"
		"			}																			\r\n"
		"		}																				\r\n";

	std::string sourceCode2 =
		" #define SIZEK        " + std::to_string(_sizeM1Down.c) + "\r\n"
		" #define NROWS        " + std::to_string(_sizeResultDown.r) + "\r\n"
		" #define NCOLS        " + std::to_string(_sizeResultDown.c) + "\r\n"
		" #define REALTYPE4 float4													\r\n"
		"	typedef float TYPE;														\r\n"
		"__kernel void compute(														\r\n"
		"			__global const TYPE *matrix1,									\r\n"
		"			__global const TYPE *matrix2,									\r\n"
		"			__global TYPE *matrixOut)										\r\n"
		"		{																	\r\n"
		"			int row = get_global_id(0);										\r\n"
		"			REALTYPE4 row_buf[SIZEK/4];											\r\n"
		"			for( int c = 0; c < SIZEK/4; c++)									\r\n"
		"			{																\r\n"
		"				row_buf[c] = (REALTYPE4)(matrix1[SIZEK*row + 4*c],			\r\n"
		"										matrix1[SIZEK*row + 4*c + 1],		\r\n"
		"										matrix1[SIZEK*row + 4*c + 2],		\r\n"
		"										matrix1[SIZEK*row + 4*c + 3]);		\r\n"
		"			}																\r\n"
		"			TYPE acc = 0;													\r\n"
		"			for (int col = 0; col < NCOLS; col++)							\r\n"
		"			{																\r\n"
		"				acc = 0;													\r\n"
		"				for (int k = 0; k < SIZEK/4; k++)								\r\n"
		"				{															\r\n"
		"				acc += dot(						row_buf[k],                      \r\n"
		"							 ( REALTYPE4 ) ( matrix2[col * SIZEK + 4*k     ],             \r\n"
		"											 matrix2[col * SIZEK + 4*k + 1 ],             \r\n"
		"											 matrix2[col * SIZEK + 4*k + 2 ],             \r\n"
		"											 matrix2[col * SIZEK + 4*k + 3 ] ) );         \r\n"
		"				}																		\r\n"
		"				matrixOut[NCOLS*row + col] = acc;										\r\n"
		"			}																			\r\n"
		"		}																				\r\n";

	cl::Program::Sources source1(1, std::make_pair(sourceCode1.c_str(), sourceCode1.length() + 1));
	cl::Program::Sources source2(1, std::make_pair(sourceCode2.c_str(), sourceCode2.length() + 1));
	cl::Program program1 = cl::Program(context1, source1);
	cl::Program program2 = cl::Program(context2, source2);

	try
	{
		program1.build(device1);
	}
	catch (const cl::Error &err)
	{
		// Выводим ошибки компиляции.
		std::cerr
			<< "OpenCL compilation error" << std::endl
			<< program1.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device1[0])
			<< std::endl;
		system("pause");
		//throw err;
	}

	try
	{
		program2.build(device2);
	}
	catch (const cl::Error &err)
	{
		// Выводим ошибки компиляции.
		std::cerr
			<< "OpenCL compilation error" << std::endl
			<< program2.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device2[0])
			<< std::endl;
		system("pause");
	}


	cl::Kernel kernel1(program1, "compute");
	cl::Kernel kernel2(program2, "compute");

	kernel1.setArg(0, vector1Up);
	kernel1.setArg(1, vector2Up);
	kernel1.setArg(2, vectorOutUp);

	kernel2.setArg(0, vector1Down);
	kernel2.setArg(1, vector2Down);
	kernel2.setArg(2, vectorOutDown);

	std::cout << "Computing..............";
	auto startTime = std::chrono::steady_clock::now();
	comqueque1.enqueueNDRangeKernel(kernel1, cl::NullRange, cl::NDRange(_sizeResultUp.r));
	comqueque2.enqueueNDRangeKernel(kernel2, cl::NullRange, cl::NDRange(_sizeResultDown.r));
	comqueque1.finish();
	comqueque2.finish();
	auto endTime = std::chrono::steady_clock::now();

	std::cout << "OK\n";
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
	std::cout << "Time: " << time.count() << " ms" << std::endl;

	comqueque1.enqueueReadBuffer(vectorOutUp, CL_TRUE, 0, _sizeResultUp.r*_sizeResultUp.c * sizeof(TYPE), _matrixResultUp);
	comqueque2.enqueueReadBuffer(vectorOutDown, CL_TRUE, 0, _sizeResultDown.r*_sizeResultDown.c * sizeof(TYPE), _matrixResultDown);
	setResultFromSubResult();
}

void Computing::setData(TYPE **pMatr1, TYPE **pMatr2, TYPE **pMatrResult,
                        int nRows1, int nCols1, int nRows2, int nCols2)
{
    sizeM1.r = nRows1;
    sizeM1.c = nCols1;
    sizeM2.r = nRows2;
    sizeM2.c = nCols2;
    sizeRes.r = nRows1;
    sizeRes.c = nCols2;

	delete[] _matrix1;
	delete[] _matrix2;
	delete[] _matrixResult;

    _matrix1 = new TYPE[sizeM1.r*sizeM1.c];
    _matrix2 = new TYPE[sizeM2.r*sizeM2.c];
    _matrixResult = new TYPE[sizeRes.r*sizeRes.c];

    for (int i = 0; i < sizeM1.r; i++)
    {
        for (int j = 0; j< sizeM1.c; j++)
        {
            _matrix1[i*sizeM1.c + j] = pMatr1[i][j];
        }
    }

    for (int i = 0; i < sizeM2.r; i++)
    {
        for (int j = 0; j< sizeM2.c; j++)
        {
            _matrix2[j*sizeM2.r + i] = pMatr2[i][j];
        }
    }

    for (int i = 0; i < sizeRes.r; i++)
    {
        for (int j = 0; j< sizeRes.c; j++)
        {
            _matrixResult[i*sizeRes.c + j] = pMatrResult[i][j];
        }
    }

	setSubData(pMatr1, pMatrResult, nRows1, nCols1, nRows2, nCols2);

}


void Computing::setSubData(TYPE **pMatr1, TYPE **pMatrResult,  int nRows1, int nCols1, int nRows2, int nCols2)
{
	/**Initialize sizes up and down left submatrix*/
	_sizeM1Up.r = _labelDevide;
	_sizeM1Down.r = nRows1 - _labelDevide;
	_sizeM1Up.c = nCols1;
	_sizeM1Down.c = nCols1;

	/**Initialize sizes up and down result submatrix*/
	_sizeResultUp.r = _sizeM1Up.r;
	_sizeResultUp.c = nCols2;
	_sizeResultDown.r = _sizeM1Down.r;
	_sizeResultDown.c = nCols2;


	delete[] _matrix1Up;
	delete[] _matrix1Down;
	delete[] _matrixResultUp;
	delete[] _matrixResultDown;

	_matrix1Up = new TYPE[_sizeM1Up.r*_sizeM1Up.c];
	_matrix1Down = new TYPE[_sizeM1Down.r*_sizeM1Down.c];

	_matrixResultUp = new TYPE[_sizeResultUp.c*_sizeResultUp.r];
	_matrixResultDown = new TYPE[_sizeResultDown.c*_sizeResultDown.r];


	/**Initialize up submatrix matrix1*/
	for (int i = 0; i < _sizeM1Up.r; i++)
	{
		for (int j = 0; j < _sizeM1Up.c; j++)
		{
			_matrix1Up[i*_sizeM1Up.c + j] = pMatr1[i][j];
		}
	}

	/**Initialize down submatrix matrix2*/
	for (int i = 0; i < _sizeM1Down.r; i++)
	{
		for (int j = 0; j < _sizeM1Down.c; j++)
		{
			_matrix1Down[i*_sizeM1Down.c + j] = pMatr1[i + _labelDevide][j];
		}
	}

	/**Initialize up submatrix result*/
	for (int i = 0; i < _sizeResultUp.r; i++)
	{
		for (int j = 0; j < _sizeResultUp.c; j++)
		{
			_matrixResultUp[i*_sizeResultUp.c + j] = pMatrResult[i][j];
		}
	}

	/**Initialize down submatrix result*/
	for (int i = 0; i < _sizeResultDown.r; i++)
	{
		for (int j = 0; j < _sizeResultDown.c; j++)
		{
			_matrixResultDown[i*_sizeResultDown.c + j] = pMatrResult[i + _labelDevide][j];
		}
	}
}

void Computing::printData()
{
    std::cout<<std::endl<<"Matrix 1:\n";
    for (int i = 0; i < sizeM1.r*sizeM1.c; i++ )
    {
        printf("%5.2f", _matrix1[i]);
        if(i % sizeM1.c == (sizeM1.c-1))
        {
            std::cout<<std::endl;
        }
    }

	std::cout << std::endl << "Matrix 1 Up:\n";
	for (int i = 0; i < _sizeM1Up.r*_sizeM1Up.c; i++)
	{
		printf("%5.2f", _matrix1Up[i]);
		if (i % _sizeM1Up.c == (_sizeM1Up.c - 1))
		{
			std::cout << std::endl;
		}
	}

	std::cout << std::endl << "Matrix 1 Down:\n";
	for (int i = 0; i < _sizeM1Down.r*_sizeM1Down.c; i++)
	{
		printf("%5.2f", _matrix1Down[i]);
		if (i % _sizeM1Down.c == (_sizeM1Down.c - 1))
		{
			std::cout << std::endl;
		}
	}

    std::cout<<std::endl<<"Matrix 2:\n";
    for (int i = 0; i < sizeM2.r*sizeM2.c; i++ )
    {
        printf("%5.2f", _matrix2[i]);
        if(i % sizeM2.c == (sizeM2.c-1))
        {
            std::cout<<std::endl;
        }
    }

}

void Computing::setResultFromSubResult()
{	
	int i = 0;
	for (i = 0; i < _sizeResultUp.c*_sizeResultUp.r; i++)
	{
		_matrixResult[i] = _matrixResultUp[i];
	}

	for (int j = 0; j < _sizeResultDown.c*_sizeResultDown.r; j++)
	{
		_matrixResult[i] = _matrixResultDown[j];
		i++;
	}

}

void Computing::printResult()
{
    std::cout<<std::endl<<"Result:\n";
    for (int i = 0; i < sizeRes.r*sizeRes.c; i++ )
    {
        printf("%8.2f", _matrixResult[i]);
        if(i % sizeRes.c == (sizeRes.c-1))
        {
            std::cout<<std::endl;
        }
    }

	std::cout << std::endl << "Result Up:\n";
	for (int i = 0; i < _sizeResultUp.r*_sizeResultUp.c; i++)
	{
		printf("%8.2f", _matrixResultUp[i]);
		if (i % _sizeResultUp.c == (_sizeResultUp.c - 1))
		{
			std::cout << std::endl;
		}
	}

	std::cout << std::endl << "Result Down:\n";
	for (int i = 0; i < _sizeResultDown.r*_sizeResultDown.c; i++)
	{
		printf("%8.2f", _matrixResultDown[i]);
		if (i % _sizeResultDown.c == (_sizeResultDown.c - 1))
		{
			std::cout << std::endl;
		}
	}

}

TYPE ** Computing::getResult()
{
	TYPE **result = new TYPE*[sizeRes.r];
    for (int r = 0; r < sizeRes.r; r++)
    {
        result[r] = new TYPE[sizeRes.c];
    }

    for (int r = 0; r < sizeRes.r; r++)
    {
        for (int c = 0; c < sizeRes.c; c++)
        {
            result[r][c] = _matrixResult[r*sizeRes.c + c];
        }
    }
    return result;
}

Computing::~Computing()
{
	delete[] _matrix1;
	delete[] _matrix2;
	delete[] _matrixResult;
	delete[] _matrix1Up;
	delete[] _matrix1Down;
	delete[] _matrixResultUp;
	delete[] _matrixResultDown;
}