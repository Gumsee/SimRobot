/**
 * @file Simulation/Masses/SphereMass.h
 * Declaration of class SphereMass
 * @author Colin Graf
 */

#pragma once

#include "Mass.h"

/**
 * @class SphereMass
 * The mass of a sphere
 */
class SphereMass : public Mass
{
public:
  SphereMass(const std::string& name)
    : Mass(findAvailableName(name, "SphereMass"))
  {}
  float value; /**< The total mass of the sphere */
  float radius; /**< The radius of the sphere */

private:
  /** Creates the mass (not including children, \c translation or \c rotation) */
  void assembleMass() override
  {
    mass = value;
    inertia[0] = inertia[1] = inertia[2] = 0.4f * value * radius * radius;
    inertia[3] = inertia[4] = inertia[5] = 0.f;
  }
};
