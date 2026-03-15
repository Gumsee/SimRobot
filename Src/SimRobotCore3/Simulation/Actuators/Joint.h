/**
 * @file Simulation/Actuators/Joint.h
 * Declaration of class Joint
 * @author Colin Graf
 * @author Thomas Röfer
 */

#pragma once

#include "Graphics/bGraphicsContext.h"
#include "Simulation/Actuators/Actuator.h"

class Axis;

/**
 * @class Joint
 * A Joint joint that connects two bodies
 */
class Joint : public Actuator
{
public:
  Joint(const std::string& name);

  Axis* axis = nullptr;
  const char* jointName = nullptr;
  int jointIndex = -1;

protected:
  /**
   * Creates the physical objects used by the OpenDynamicsEngine (ODE).
   * These are a geometry object for collision detection and/or a body,
   * if the simulation object is movable.
   */
  void createPhysicsInternal() override;

private:
  /**
   * Submits draw calls for physical primitives of the object (including children) in the given graphics context
   */
  void drawPhysics() const override;

  /** Registers this object with children, actuators and sensors at SimRobot's GUI */
  void registerObjects() override;

  Object3D* axisLine = nullptr;
  Object3D* sphere = nullptr;
  bGraphicsContext::Surface* surface = nullptr;
};
