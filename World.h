#pragma once
#include "HelperMacros.h"

class Actor;

class World
{

public:
  World() = default;
  virtual ~World() = default;

  void AddActor(const SPtr<Actor>& actor);

  void RemoveActor(const SPtr<Actor>& actor);

  virtual void Init();
  virtual void Update(float deltaTime);
  virtual void Render();

protected:

 //Actors in thiiis world
  Vector<SPtr<Actor>> m_actors;

};

