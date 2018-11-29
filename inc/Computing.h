#include <CL/cl.hpp>

class Computing
{
    public:
    Computing();
    virtual ~Computing();
    void getDevicesInfo();
    std::vector<cl::Platform> platforms;
    std::vector<cl::Device> devices;

};