#pragma once
#include "HelperMacros.h"


class MKWindow
{
public:
  MKWindow() = default;
  virtual ~MKWindow() = default;

  virtual void Render() = 0;

protected:
  String m_name = "Default Window Name";
  unsigned int m_id = 0;
};

