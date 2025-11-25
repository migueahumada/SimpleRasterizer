#include "World.h"
#include "Actor.h"





void World::Init()
{
	m_root = SceneObject::CreateSceneObject<Actor>();
	m_root->Init();

}

void World::FixedUpdate()
{
	for (const auto& actor : m_actors)
	{
		actor->FixedUpdate();
	}
}

void World::Update(float deltaTime)
{
	for (const auto& actor : m_actors)
	{
		actor->Update(deltaTime);
	}
}

void World::Render()
{
  for (const auto& actor : m_actors)
  {
    actor->Render();
  }
}
