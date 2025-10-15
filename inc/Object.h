#pragma once
#include "HelperMacros.h"
#include "UUID.h"

class Object
{
public:
  Object() = default;
  virtual ~Object() = default;

  void SetName(const String& name){m_name = name;}

  String GetName() const {return m_name; }

  const MKEngine::UUID GetUUID() const { return m_objectUUID; }


protected:
  String m_name = "Default Name";
  MKEngine::UUID m_objectUUID;
};

