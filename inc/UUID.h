#pragma once

#include "HelperMacros.h"
namespace MKEngine
{
  class UUID
  {
   public:
    UUID();
    ~UUID() = default;

    const uint32 getUUID() const
    {
      return m_uuid;
    }

   private:

    uint32 GenerateUUID();
   
   private:
    uint32 m_uuid;



  };
}


