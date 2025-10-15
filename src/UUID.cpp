#include "UUID.h"
#include <random>
#include "ResourceManager.h"

namespace MKEngine
{

  UUID::UUID()
  {
    //uint32 newUUID = 0;
    //uint32 storedUUID = 0;
    //bool found = false;

    //while (!found)
    //{
    //  newUUID = GenerateUUID();

    //  for (auto it = g_resourceManager().m_appUUID.begin(),
    //    itEnd = g_resourceManager().m_appUUID.end();
    //    it != itEnd;
    //    ++it)
    //  {
    //    storedUUID = it->first.getUUID();
    //    if (storedUUID != newUUID)
    //    {
    //      //g_resourceManager().m_appUUID.insert{""}
    //    }

    //    found = true;
    //    break;
    //  }
    //}

    m_uuid = GenerateUUID();

  }

  uint32 UUID::GenerateUUID()
  {

    std::random_device rd;
    std::mt19937 gen(rd());

    uint32 max = std::uniform_int_distribution<uint32>().max();
    uint32 min = std::uniform_int_distribution<uint32>().min();
    std::uniform_int_distribution<uint32> dist(min, max);

    
    return dist(rd);
  }

  

}

