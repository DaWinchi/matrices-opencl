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
	void compute(int numDevice1, int numDevice2);
	void setData(int **pMatr1, int **pMatr2, int **pMatrResult, int nRows1, int nCols1, int nRows2, int nCols2);
	void printData();
	void printResult();
	int** getResult();
	void setBarrier(int b);



	Size sizeM1, sizeM2, sizeRes;
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;
private:
	void setSubData(int **pMatr1, int **pMatrResult, int nRows1, int nCols1, int nRows2, int nCols2);
	int _labelDevide;;
	int *_matrix1;
	int *_matrix2;
	int *_matrixResult;

	int *_matrixResultUp, *_matrixResultDown, *_matrix1Up, *_matrix1Down;
	Size _sizeM1Up, _sizeM1Down, _sizeResultUp, _sizeResultDown;


};