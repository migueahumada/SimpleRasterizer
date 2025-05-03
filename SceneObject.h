#pragma once
#include "Object.h"
#include "HelperMacros.h"

class SceneObject : public Object
{

public:
  SceneObject() = default;
  virtual ~SceneObject() = default;

  virtual void AddChild(const SPtr<SceneObject>& child);
  virtual void RemoveChild(const SPtr<SceneObject>& child);

protected:
  

  SPtr<SceneObject> SharedFromSelf()
  {
    return m_selfPtr.lock();
  }

  void SetSelfPtr(const SPtr<SceneObject>& self)
  {
    m_selfPtr = self;
  }

  WPtr<SceneObject> m_selfPtr;
  WPtr<SceneObject> m_parent;
  Vector<SPtr<SceneObject>> m_children;

public:
  template<typename T, typename ...Args>
  inline std::shared_ptr<T> CreateSceneObject(Args&& ...args);
  
};

template<typename T, typename ...Args>
inline std::shared_ptr<T> SceneObject::CreateSceneObject(Args&& ...args)
{
  static_assert(std::is_base_of<SceneObject, T>::value,
    "T must be derived from Scene Object");

    auto node = SPtr<T>(make_shared<T>(std::forward<Args>(args)...));
    node->SetSelfPtr(node);

  return node;
}

