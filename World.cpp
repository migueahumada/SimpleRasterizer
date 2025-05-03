#include "World.h"
#include "Actor.h"

void World::AddActor(const SPtr<Actor>& actor)
{
	if (!actor)
	{
		return;
	}
  
	m_actors.push_back(actor);
}

void World::RemoveActor(const SPtr<Actor>& actor)
{
	if (!actor)
	{
		return;
	}
	
	auto it = std::find(m_actors.begin(),m_actors.end(),actor);
	
	if (it != m_actors.end())
	{
		m_actors.erase(it,m_actors.end());
	}
	
}

void World::Init()
{
	for (const auto& actor : m_actors)
	{
		actor->Init();
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
