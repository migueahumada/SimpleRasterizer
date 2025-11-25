#pragma once
#include "Module.h"
#include <CL/cl.h>
#include <iostream>

#define THROW_ON_ERROR(x, msg)                     \
{                                                  \
  if(err != CL_SUCCESS)                            \
  {                                                \
    std::cerr << msg << std::endl;                 \
    return;                                        \
  }                                                \
}                                                  \


class ComputeAPI : public Module<ComputeAPI>
{
 public:
  void OnStartUp() override;

  void OnShutdown() override;

 private:
  

private:
  cl_device_id m_device {0};
  cl_context m_context {0};
  cl_command_queue m_cmdQueue {0};
};

ComputeAPI& g_ComputeAPI();

