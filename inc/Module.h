#pragma once
#include "HelperMacros.h"

/*
****************************************************
  Descripción:
  - Esta clase crea un módulo que es como una especie de singleton.
  - Elimina copy constuctor, move, copy assignment y el move assignment.
  - Crea tres variables estáticas con sus respectivos métodos:
    - _instance: regresa la dirección de memoria del módulo. (un puntero porque se crea en el heap)
    - isDestroyed: regresa si ha sido destruido el módulo.
    - isStartedUp: regresa si ha sido inicializado el módulo.
*****************************************************
*/
namespace MKEngine {
  template <class T>
  class Module
  {
  public:
  
    static T& instance() //regresa la dirección de memoria del módulo
    {
      if (!isStartedUp())
      {
        printf("Trying to access a module that hasn't been started yet.");
      }

      if (isDestroyed())
      {
        printf("Trying to access a module that's destroyed.");
      }

      return *_instance();
    }

  
    static T* instancePtr() //regresa un puntero a la instancia del módulo
    {
      if (!isStartedUp())
      {
        printf("Trying to access a module that hasn't been started yet.");
      }

      if (isDestroyed())
      {
        printf("Trying to access a module that's destroyed.");
      }

      return _instance();
    }

    template<class... Args>
    static void startUp(Args&& ... args)
    {
      if (isStartedUp())
      {
        printf("Trying to start a module already started.");
      }
 
      _instance() = new T(std::forward<Args>(args)...);
      isStartedUp() = true;

      static_cast<Module*>(_instance())->onStartUp();
    }

    static void shutDown()
    {
      if (isDestroyed())
      {
        printf("Trying to shutdown a module that is aldready destroyed");
      }
      if (!isStartedUp())
      {
        printf("Trying to shutdown a module hasn't been started up.");
      }

      static_cast<Module*>(_instance())->onShutDown();

      delete(_instance());
      isDestroyed() = true;
    }

    static bool isStarted()
    {
      return isStartedUp() && !isDestroyed();
    }



  protected: //porqué son protegidos?
    Module() = default;

    virtual ~Module() = default; 
  
    Module(Module&&) = delete; // no se puede mover

    Module(const Module&) = delete; // no se puede copiar

    Module& operator=(Module&&) = delete; // no se puede asignar por movimiento

    Module& operator=(const Module&) = delete; // no se puede asignar por copia

    virtual void onStartUp() {}
  
    virtual void onShutDown() {}

    static T*& _instance() // regresa la dirección del puntero donde se crea la clase 
    {
      static T* inst = nullptr;
      return inst;
    }

    static bool& isDestroyed()
    {
      static bool inst = false;
      return inst;
    }

    static bool& isStartedUp()
    {
      static bool inst = false;
      return inst;
    }

  };
}



