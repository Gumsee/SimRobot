/**
 * @file Simulation/Masses/CylinderMass.h
 * Declaration of class CylinderMass
 * @author Arne Hasselbring
 */

#pragma once

#include "Mass.h"

/**
 * @class CylinderMass
 * The mass of a cylinder
 */
class CylinderMass : public Mass
{
public:
  CylinderMass(const std::string& name)
    : Mass(findAvailableName(name, "CylinderMass"))
  {}

  float value; /**< The total mass of the cylinder */
  float height; /**< The height of the cylinder */
  float radius; /**< The radius of the cylinder */

private:
  /** Creates the mass (not including children, \c translation or \c rotation) */
  void assembleMass() override
  {
    mass = value;
    inertia[0] = inertia[1] = value * (0.25f * radius * radius + height * height / 12.f);
    inertia[2] = 0.5f * value * radius * radius;
    inertia[3] = inertia[4] = inertia[5] = 0.f;
  }
};
