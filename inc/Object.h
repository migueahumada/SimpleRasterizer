#pragma once
#include "HelperMacros.h"

class Object
{
public:
  Object() = default;
  virtual ~Object() = default;

  void SetName(const String& name){m_name = name;}
  void SetId(unsigned int id) { m_id= id; }

  String GetName() const {return m_name; }
  unsigned int GetId() const { return m_id; }


protected:
  String m_name = "Default Name";
  unsigned int m_id;
};

