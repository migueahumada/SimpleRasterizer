#include "Actor.h"

void Actor::Init()
{
}

void Actor::FixedUpdate()
{
}

void Actor::Update(float deltaTime)
{
  UpdateTransform();
}

void Actor::Render()
{
}

void Actor::UpdateTransform()
{
  if (!m_parent.expired())
  {
    SPtr<Actor> parent = std::static_pointer_cast<Actor>(m_parent.lock());

    Vector3 parPos = parent->getWorldTransform().getPosition();
    Vector3 parRot = parent->getWorldTransform().getRotation();
    Vector3 parScl = parent->getWorldTransform().getScale();

    m_worldTransform.setPosition(parPos + m_localTransform.getPosition());
    m_worldTransform.setRotation(parRot + m_localTransform.getRotation());
    m_worldTransform.setScale(parScl + m_localTransform.getScale());

  }
  else
  {
    m_worldTransform.setPosition(m_localTransform.getPosition());
    m_worldTransform.setRotation(m_localTransform.getRotation());
    m_worldTransform.setScale(m_localTransform.getScale());
  }
}
