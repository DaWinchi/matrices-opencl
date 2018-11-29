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

Computing::~Computing(){}