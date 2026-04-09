/**
 * @file SimObjectRenderer.h
 * Declaration of class SimObjectRenderer
 * @author Colin Graf
 */

#pragma once

#include "SimRobotCore3.h"
#include "Tools/Math/Eigen.h"
class SimObject;
class Body;

/**
 * @class SimObjectRenderer
 * An interface for rendering scene objects on an OpenGL context
 */
class SimObjectRenderer : public SimRobotCore3::Renderer
{
private:
  unsigned int width = 0;
  unsigned int height = 0;

  Vector3f cameraPos;
  Vector3f cameraTarget;
  Pose3f cameraTransformation;
  Matrix4f projection;

  ShadeMode surfaceShadeMode = smoothShading;
  ShadeMode physicsShadeMode = noShading;
  ShadeMode drawingsShadeMode = flatShading;

  bool initialized = false;
  bool dragging = false;
  Body* dragSelection;
  Vector3f dragStartPos;
  Vector3f interCameraPos;
  Vector3f dragPlaneVector;
  DragAndDropMode dragMode = keepDynamics;
  unsigned int dragStartTime;
  static constexpr int degreeSteps = 15;

  Body* selectObject(const Vector3f& projectedClick);
};
