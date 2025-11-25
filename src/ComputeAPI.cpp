#include "ComputeAPI.h"


ComputeAPI& g_ComputeAPI()
{
  return ComputeAPI::GetInstance();
}

void ComputeAPI::OnStartUp()
{
  cl_int err;
  cl_uint numPlatforms;

  err = clGetPlatformIDs(0, nullptr, &numPlatforms);
  THROW_ON_ERROR(err, "No compute num platforms found");

  Vector<cl_platform_id> platforms(numPlatforms);
  err = clGetPlatformIDs(numPlatforms, platforms.data(), nullptr);
  THROW_ON_ERROR(err, "No compute platforms found");

  for (cl_uint i = 0; i < numPlatforms; ++i)
  {
    cl_platform_id& curPlatform = platforms[i];

    cl_uint numDevices = 0;
    err = clGetDeviceIDs(curPlatform, CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices);
    THROW_ON_ERROR(err, "No compute devices found");

    Vector<cl_device_id> devices(numDevices);
    clGetDeviceIDs(curPlatform, CL_DEVICE_TYPE_ALL, numDevices, devices.data(), nullptr);

    for (cl_uint j = 0; j < numDevices; ++j)
    {
      cl_device_id& currDevice = devices[j];

      size_t infoSize = 0;
      clGetDeviceInfo(currDevice, CL_DEVICE_NAME, 0, nullptr, &infoSize);

      Vector<char> deviceName(infoSize);
      clGetDeviceInfo(currDevice, CL_DEVICE_NAME, infoSize, deviceName.data(), nullptr);

      std::cout << "Device" << j << ".-" << deviceName.data() << std::endl;
    }

    m_device = devices[0];

  }

  m_context = clCreateContext(nullptr, 1, &m_device, nullptr, nullptr, &err);
  THROW_ON_ERROR(err, "Failed to create compute context");

  m_cmdQueue = clCreateCommandQueueWithProperties(m_context, m_device, nullptr, &err);
  THROW_ON_ERROR(err, "Failed to create command queue");
}

void ComputeAPI::OnShutdown()
{
  
}
