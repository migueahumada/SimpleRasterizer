#include "ComputeAPI.h"
#include <fstream>



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

    m_numDevices = 0;
    err = clGetDeviceIDs(curPlatform, CL_DEVICE_TYPE_ALL, 0, nullptr, &m_numDevices);
    THROW_ON_ERROR(err, "No OpenCL devices found");

    Vector<cl_device_id> devices(m_numDevices);
    clGetDeviceIDs(curPlatform, CL_DEVICE_TYPE_ALL, m_numDevices, devices.data(), nullptr);
    for (cl_uint j = 0; j < m_numDevices; ++j)
    {

      cl_device_id& currDevice = devices[j];

      size_t infoSize = 0;
      clGetDeviceInfo(currDevice, CL_DEVICE_NAME, 0, nullptr, &infoSize);

      Vector<char> deviceName(infoSize);
      clGetDeviceInfo(currDevice, CL_DEVICE_NAME, infoSize, deviceName.data(), nullptr);

      std::cout << "Device " << j << ".- " << deviceName.data() << std::endl;
    }

    m_device = devices[0];
  }

  m_context = clCreateContext(nullptr, 1, &m_device, nullptr, nullptr, &err);
  THROW_ON_ERROR(err, "Failed to create compute context");

  m_cmdQueue = clCreateCommandQueueWithProperties(m_context, m_device, nullptr, &err);
  THROW_ON_ERROR(err, "Failed to create command queue");

  CreateKernel("New Kernel", "compute/default.cl", "vectorAdd");

  const unsigned int numElements = 1024;
  Vector<float> h_A(numElements, 1.0f);
  Vector<float> h_B(numElements, 2.0f);
  Vector<float> h_C(numElements, 0.0f);

  UPtr<KernelBuffer> aKernelBuffer = CreateKernelBuffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                     numElements * sizeof(float),
                     h_A.data());
  

  UPtr<KernelBuffer> bKernelBuffer = CreateKernelBuffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                     numElements * sizeof(float),
                     h_B.data());



  UPtr<KernelBuffer> cKernelBuffer = CreateKernelBuffer(CL_MEM_WRITE_ONLY,
                     numElements * sizeof(float),
                     nullptr);

  auto myKernel = m_kernelMap.find("New Kernel");

  size_t globalWorkSize = numElements;

  

  bool value1 = ConfigureKernel(myKernel->second, 0, sizeof(cl_mem),  &aKernelBuffer->getBuffer());
  
  bool value2 = ConfigureKernel(myKernel->second, 1, sizeof(cl_mem), &bKernelBuffer->getBuffer());
  
  bool value3 = ConfigureKernel(myKernel->second, 2, sizeof(cl_mem), &cKernelBuffer->getBuffer());
  
  bool value4 = ConfigureKernel(myKernel->second, 3, sizeof(unsigned int), &numElements);

  
  err = clEnqueueNDRangeKernel(m_cmdQueue, myKernel->second->getKernel(), 
                               1, nullptr, &globalWorkSize, nullptr, 0,
                               nullptr, nullptr);

  clFinish(m_cmdQueue);

  err = clEnqueueReadBuffer(m_cmdQueue, cKernelBuffer->getBuffer(),
                      CL_TRUE, 0 , numElements * sizeof(float), h_C.data(), 0,nullptr,nullptr);

  

  bool success = true;
  for (unsigned int i = 0; i < numElements; ++i)
  {
    if (h_C[i] != h_A[i] + h_B[i])
    {
      success = false;

      break;
    }
  }

  if (success)
  {
    std::cout << "VectorAdd successful!" << std::endl;
  }
  else
  {
    std::cout << "VectorAdd failed!" << std::endl;
  }

}

void ComputeAPI::OnShutdown()
{
   
  clReleaseDevice(m_device);
  clReleaseContext(m_context);
  clReleaseCommandQueue(m_cmdQueue);
  
}



String ComputeAPI::CreateKernel(const String& kernelName,
                                const char* filePath,
                                const String& entryFunction)
{
  cl_int err;

  UPtr<Kernel> kernel = make_unique<Kernel>();

  Vector<char> source = kernel->GetSource(filePath);
  source.push_back('\0');
  const char* pSource = &source[0];

  kernel->getProgram() = clCreateProgramWithSource(m_context, 1, &pSource, nullptr, &err);
  
  if (err != CL_SUCCESS) return " ";

  err = clBuildProgram( kernel->getProgram(),
                  1, &m_device, 
                 nullptr, nullptr, nullptr);
  
  if (err != CL_SUCCESS) return " ";

  kernel->getKernel() = clCreateKernel(kernel->getProgram(), 
                                       entryFunction.c_str(), 
                                       &err);

  if (kernel->isEmpty()) return " ";

  m_kernelMap.insert({kernelName,std::move(kernel)});

  return kernelName;

}

UPtr<KernelBuffer> ComputeAPI::CreateKernelBuffer(cl_mem_flags flags, 
                                                  size_t size, 
                                                  void* data)
{
  cl_int err;
  
  UPtr<KernelBuffer> pKernelBuffer = make_unique<KernelBuffer>();

  pKernelBuffer->getBuffer() = clCreateBuffer(m_context, flags, size, data, &err);

  return pKernelBuffer;
}

bool ComputeAPI::ConfigureKernel(const UPtr<Kernel>& kernel, 
                                 cl_uint argIndex,
                                 size_t size,
                                 const void* kernelBuffer)
{
  cl_int err;

  err = clSetKernelArg(kernel->getKernel(),
                       argIndex,
                       size,
                       kernelBuffer);

  if (err!= CL_SUCCESS)
  {
    return false;
  }

  return true;
  
}


