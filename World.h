#pragma once
#include "HelperMacros.h"
#include "Actor.h"

class World
{

public:
  World() = default;
  virtual ~World() = default;

  template<typename T, typename... Args>
  SPtr<Actor> SpawnActor(const SPtr<SceneObject>& parent, Args&&... args);

  virtual void Init();
  virtual void Update(float deltaTime);
  virtual void Render();

  Vector<SPtr<Actor>>& getActors(){
    return m_actors;
  }

protected:

 //Actors in thiiis world
  Vector<SPtr<Actor>> m_actors;

  SPtr<Actor> m_root;

};

template<typename T, typename... Args>
inline SPtr<Actor> World::SpawnActor(const SPtr<SceneObject>& parent, Args&&... args)
{
  static_assert(std::is_base_of<Actor, T>::value, "T must be derived from Actor");

  SPtr<T> pActor = SceneObject::CreateSceneObject<T>(std::forward<Args>(args)...);
  pActor->Init();
 
  m_actors.push_back(pActor);

  if (parent)
  {
    parent->AddChild(pActor);
  }
  else
  {
    m_root->AddChild(pActor);
  }

  return pActor;
}

