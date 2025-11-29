#pragma once
#include "HelperMacros.h"
#include "Module.h"
#include <CL/cl.h>
#include <iostream>
#include "Kernel.h"
/*
  Macro for doing something when the message
  from when the compute API shows an error.
*/
#define THROW_ON_ERROR(x, msg)                     \
{                                                  \
  if(err != CL_SUCCESS)                            \
  {                                                \
    std::cerr << msg << std::endl;                 \
    return;                                        \
  }                                                \
}                                                  \

using KernelMap = Map<String, UPtr<Kernel>>;

class ComputeAPI : public Module<ComputeAPI>
{
 public:
  ComputeAPI() = default;
  virtual ~ComputeAPI() = default;
  
  void OnStartUp() override;

  void OnShutdown() override;

  /*
    Creates a Kernel and puts the Kernel in the Kernel Map.
  */
  String CreateKernel(const String& kernelName,
                      const char* filePath,
                      const String& entryFunction);
  
  UPtr<KernelBuffer> CreateKernelBuffer(cl_mem_flags flags,
                                        size_t size, 
                                        void* data);
  bool ConfigureKernel(const UPtr<Kernel>& kernel,
                       cl_uint argIndex,
                       size_t size,
                       const void* kernelBuffer);

 private:
  cl_device_id m_device {0};
  cl_context m_context {0};
  cl_command_queue m_cmdQueue {0};
  cl_uint m_numDevices {0};
  
  /*
    Holds all the Kernel Objects
  */
  KernelMap m_kernelMap;
};

ComputeAPI& g_ComputeAPI();

