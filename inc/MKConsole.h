#pragma once
#include "MKWindow.h"

template<typename T, typename ... Args>
class MKConsole : public MKWindow
{
public:
  MKConsole() = default;
  ~MKConsole() = default;

  //void Render();

  //
  //void AddReference(const String& name, const WPtr<T>& ref, Args&& ... args);
private:
  
  
  std::map<String, WPtr<T>> m_references;

};


/*
template<typename T, typename ... Args>
inline SPtr<T> SceneObject::CreateSceneObject(Args&& ...args)
{
  static_assert(std::is_base_of<SceneObject, T>::value,
    "T must be derived from Scene Object");

  auto node = make_shared<T>(std::forward<Args>(args)...);
  node->SetSelfPtr(node);

  return node;
}
*/
