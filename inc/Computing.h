#include <CL/cl.hpp>

typedef float TYPE;

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
	void compute(int numDevice1, int numDevice2);
	void setData(TYPE **pMatr1, TYPE **pMatr2, TYPE **pMatrResult, int nRows1, int nCols1, int nRows2, int nCols2);
	void printData();
	void printResult();
	TYPE** getResult();
	void setBarrier(int b);



	Size sizeM1, sizeM2, sizeRes;
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;
private:
	void setSubData(TYPE **pMatr1, TYPE **pMatrResult, int nRows1, int nCols1, int nRows2, int nCols2);
	void setResultFromSubResult();
	int _labelDevide;;
	TYPE *_matrix1;
	TYPE *_matrix2;
	TYPE *_matrixResult;

	TYPE *_matrixResultUp, *_matrixResultDown, *_matrix1Up, *_matrix1Down;
	Size _sizeM1Up, _sizeM1Down, _sizeResultUp, _sizeResultDown;


};