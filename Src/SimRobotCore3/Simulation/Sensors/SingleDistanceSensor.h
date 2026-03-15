/**
 * @file Simulation/Sensors/SingleDistanceSensor.h
 * Declaration of class SingleDistanceSensor
 * @author Colin Graf
 */

#pragma once

#include "Graphics/bGraphicsContext.h"
#include "Simulation/Sensors/Sensor.h"
#include "Tools/Math/Pose3f.h"

/**
 * @class SingleDistanceSensor
 * A distance sensor that uses a ray to detect distances to other objects
 */
class SingleDistanceSensor : public Sensor
{
public:
  float min; /**< The minimum distance the distance sensor can measure */
  float max; /**< The maximum distance the distance sensor can measure */

  /** Default constructor */
  SingleDistanceSensor(const std::string& name);

private:
  /**
   * @class DistanceSensor
   * The distance sensor interface
   */
  class DistanceSensor : public Sensor::Port
  {
  public:
    ::PhysicalObject* physicalObject;
    float min;
    float max;

  private:
    /** Update the sensor value. Is called when required. */
    void updateValue() override;

    //API
    bool getMinAndMax(float& min, float& max) const override;
  } sensor;

  /**
   * Creates the physical objects used by the OpenDynamicsEngine (ODE).
   * These are a geometry object for collision detection and/or a body,
   * if the simulation object is movable.
   */
  void createPhysicsInternal() override;

  /** Registers this object with children, actuators and sensors at SimRobot's GUI */
  void registerObjects() override;

  /**
   * Registers an element as parent
   * @param element The element to register
   */
  void addParent(Element& element) override;

  /**
   * Submits draw calls for physical primitives of the object (including children) in the given graphics context
   */
  void drawPhysics() const override;

  void updateTransformation() override;

  Object3D* ray = nullptr; /**< The ray mesh for the sensor drawing. */
  bGraphicsContext::Surface* surface = nullptr; /**< The surface for the sensor drawing. */
};
