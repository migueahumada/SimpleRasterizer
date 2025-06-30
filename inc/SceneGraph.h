#pragma once
#include "MKWindow.h"
#include "imgui.h"
class SceneGraph : public MKWindow
{
public:
  SceneGraph() = default;
  ~SceneGraph() = default;

  virtual void Render() override;
};

