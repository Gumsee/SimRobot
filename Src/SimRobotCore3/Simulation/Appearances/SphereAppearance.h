/**
 * @file Simulation/Appearances/SphereAppearance.h
 * Declaration of class SphereAppearance
 * @author Colin Graf
 */

#pragma once

#include "Graphics/bGraphicsContext.h"
#include "Simulation/Appearances/Appearance.h"

/**
 * @class SphereAppearance
 * The graphical representation of a sphere
 */
class SphereAppearance : public Appearance
{
public:
  SphereAppearance(const std::string& name)
    : Appearance(findAvailableName(name, "SphereAppearance"))
  {}

  float radius; /**< The radius of the sphere */

private:
  /**
   * Creates a mesh for this appearance in the given graphics context
   * @return The resulting mesh
   */
  Mesh* createMesh() override
  {
    return Mesh::generateSphere(radius, 16, 16);
  }
};
