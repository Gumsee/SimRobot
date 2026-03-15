/**
 * @file Simulation/Masses/BoxMass.h
 * Declaration of class BoxMass
 * @author Colin Graf
 */

#pragma once

#include "Mass.h"

/**
 * @class BoxMass
 * The mass of a simple box
 */
class BoxMass : public Mass
{
public:
  BoxMass(const std::string& name)
    : Mass(findAvailableName(name, "BoxMass"))
  {}

  float value; /**< The total mass of the box */
  float width; /**< The width of the box (cy) */
  float height; /**< The height of the box (cz) */
  float depth; /**< The depth of the box (cx) */

private:
  /** Creates the mass (not including children, \c translation or \c rotation) */
  void assembleMass() override
  {
    mass = value;
    inertia[0] = value * (width * width + height * height) / 12.f;
    inertia[1] = value * (depth * depth + height * height) / 12.f;
    inertia[2] = value * (depth * depth + width * width) / 12.f;
    inertia[3] = inertia[4] = inertia[5] = 0.f;
  }
};
