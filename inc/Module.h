#pragma once
#include "HelperMacros.h"


/*
****************************************************
  Descripción:
  - Esta clase crea un módulo que es como una especie de singleton.
  - Elimina copy constuctor, move, copy assignment y el move assignment.
  - Crea tres variables estáticas con sus respectivos métodos:
    - instance: regresa la dirección de memoria del módulo. (un puntero porque se crea en el heap)
    - isDestroyed: regresa si ha sido destruido el módulo.
    - isStartedUp: regresa si ha sido inicializado el módulo.
*****************************************************
*/

template <class T>
class Module
{
public:
  
  static T& GetInstance() //regresa la dirección de memoria del módulo
  {
    assert(!isStartedUp() && "The instance is not started yet.");

    assert(!isDestroyed() && "The instance is not destroyed.");

    return *instance();
  }

  
  static T* GetInstancePtr() //regresa un puntero a la instancia del módulo
  {
    assert(!isStartedUp() && "The instance is already started.");

    assert(!isDestroyed() && "The instance is already destroyed.");

    return instance();
  }

  template<class... Args>
  static void StartUp(Args&& ... args)
  {
    assert(!isStartedUp() && "The instance is already started.");
    assert(!isDestroyed() && "The instance is already destroyed.");
 
    instance() = new T(std::forward<Args>(args)...);
    isStartedUp() = true;

    static_cast<Module*>(instance())->onStartUp();
  }

  static void Shutdown()
  {
    assert(isStartedUp() && "Cannot destroy something that hasn't been started up.");
    assert(!isDestroyed() && "Cannot destroy something that's already been destroyed.");

    static_cast<Module*>(instance())->onShutdown();

    delete(instance());
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
  
  virtual void onShutdown() {}

  static T*& instance() // regresa la dirección del puntero donde se crea la clase 
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






