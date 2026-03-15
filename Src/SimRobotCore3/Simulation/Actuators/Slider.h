/**
 * @file Simulation/Actuators/Slider.h
 * Declaration of class Slider
 * @author Colin Graf
 * @author Thomas Röfer
 */

#pragma once

#include "Joint.h"

/**
 * @class Slider
 * A Slider joint that connects two bodies
 */
class Slider : public Joint
{
public:
  Slider(const std::string& name);

private:
  /**
   * Creates the physical objects used by the OpenDynamicsEngine (ODE).
   * These are a geometry object for collision detection and/or a body,
   * if the simulation object is movable.
   */
  void createPhysicsInternal() override;

  //API
  const QIcon* getIcon() const override;
};
