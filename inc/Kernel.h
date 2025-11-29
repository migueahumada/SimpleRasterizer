#pragma once
#include "HelperMacros.h"

#include <CL/cl.h>

class Kernel
{
public:
  Kernel() = default;

  virtual ~Kernel()
  {
    if (m_kernel)
    {
      clReleaseKernel(m_kernel);
    }

    if (m_program)
    {
      clReleaseProgram(m_program);
    }
  }

  Vector<char> GetSource(const String& filePath);

  cl_kernel& getKernel()
  {
    return m_kernel;
  }

  cl_program& getProgram()
  {
    return m_program;
  }

  bool isEmpty()
  {
    return !m_kernel || !m_program ? true : false;
  }

protected:
  cl_kernel m_kernel = nullptr;
  cl_program m_program = nullptr;
};

class KernelBuffer
{
 public:
  KernelBuffer() = default;
  virtual ~KernelBuffer()
  {
    if (m_buffer)
    {
      clReleaseMemObject(m_buffer);
    }
  };

  cl_mem& getBuffer()
  {
    return m_buffer;
  }

 protected:
  cl_mem m_buffer = nullptr;
};