#pragma once
#include "HelperMacros.h"

class GraphicsAPI;

class MK3DGizmo
{
public:
  explicit MK3DGizmo(const WPtr<GraphicsAPI>& pGraphicsAPI);
  ~MK3DGizmo() = default;

  void Init();
  void Update(float deltaTime);
  void Render();

  inline bool IsVisible() const { return m_bIsVisible; }

private:
  bool m_bIsVisible = false;
  WPtr<GraphicsAPI> m_pGraphicsAPI;

};

