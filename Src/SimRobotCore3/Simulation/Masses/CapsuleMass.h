/**
 * @file Simulation/Masses/CapsuleMass.h
 * Declaration of class CapsuleMass
 * @author Arne Hasselbring
 */

#pragma once

#include "Mass.h"

/**
 * @class CapsuleMass
 * The mass of a capsule
 */
class CapsuleMass : public Mass
{
public:
  CapsuleMass(const std::string& name)
    : Mass(findAvailableName(name, "CapsuleMass"))
  {}

  float value; /**< The total mass of the capsule */
  float height; /**< The height of the capsule */
  float radius; /**< The radius of the capsule */

private:
  /** Creates the mass (not including children, \c translation or \c rotation) */
  void assembleMass() override
  {
    // dMassSetCapsuleTotal(&mass, value, 3, radius, height - radius - radius);
  }
};
