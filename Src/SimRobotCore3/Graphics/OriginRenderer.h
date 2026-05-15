#pragma once
#include <Graphics/Object3D.h>
#include "Simulation/PhysicalObject.h"

class OriginRenderer : public Renderable, public Transformable3D
{
private:
  Object3D* xAxisMesh = nullptr;
  Object3D* yAxisMesh = nullptr;
  Object3D* zAxisMesh = nullptr;

  bool enabled;

public:
  OriginRenderer();

  void render() override;
  void enable(bool enabled);
};