#pragma once
#include <Graphics/Renderable.h>
#include "Simulation/Simulation.h"

class ControllerRenderer : public Renderable
{
private:
  SimObject* simObject;
  SimRobotCore3::Controller3DDrawingManager* drawingManager;
  SimRobotCore3::Renderer::ShadeMode shadeMode = SimRobotCore3::Renderer::ShadeMode::smoothShading;

public:
  ControllerRenderer(SimRobotCore3::Controller3DDrawingManager* drawingmanager);
  ~ControllerRenderer();

  void render() override;
  void setShadeMode(SimRobotCore3::Renderer::ShadeMode shademode);
  void setSimObject(SimObject* simobject);
  SimRobotCore3::Renderer::ShadeMode getShadeMode();
};