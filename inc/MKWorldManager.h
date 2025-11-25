#pragma once
#include "Module.h"
#include "Camera.h"
#include "World.h"

namespace MKEngine{

  class MKWorldManager : public Module<MKWorldManager>
  {
  public:
    MKWorldManager();
    virtual ~MKWorldManager();

    

  private:
    
  protected:
    SPtr<Camera> m_pCamera;
    SPtr<World> m_pWorld; 
    
  };

}


