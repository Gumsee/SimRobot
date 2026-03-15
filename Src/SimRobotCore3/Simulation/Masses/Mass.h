/**
 * @file Simulation/Masses/Mass.h
 * Declaration of class Mass
 * @author Colin Graf
 */

#pragma once

#include "SimRobotCore3.h"
#include "Simulation/SimObject.h"
#include "Simulation/PhysicalObject.h"
#include "Tools/Math/Eigen.h"

/**
 * @class Mass
 * Abstract class for masses of physical objects
 */
class Mass : public SimObject, public SimRobotCore3::Mass
{
public:
  Mass(const std::string& name);
  /**
   * Creates the mass of a physical object (including children and not including \c translation and \c rotation)
   * @return The mass
   */
  float createMass(vec3& com, float* inertia);

protected:
  float mass;
  vec3 com;
  float inertia[6];
  bool created = false;

  /** Creates the mass (not including children, \c translation or \c rotation) */
  virtual void assembleMass();

private:
  // API
  const QString& getFullName() const override {return SimObject::getFullName();}
  SimRobot::Widget* createWidget() override {return SimObject::createWidget();}
  const QIcon* getIcon() const override {return SimObject::getIcon();}
};
