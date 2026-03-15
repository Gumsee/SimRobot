/**
 * @file Simulation/Appearances/CapsuleAppearance.h
 * Declaration of class CapsuleAppearance
 * @author Colin Graf
 */

#pragma once

#include "Graphics/bGraphicsContext.h"
#include "Simulation/Appearances/Appearance.h"

/**
 * @class CapsuleAppearance
 * The graphical representation of a capsule
 */
class CapsuleAppearance : public Appearance
{
public:
  CapsuleAppearance(const std::string& name)
    : Appearance(findAvailableName(name, "CapsuleAppearance"))
  {}

  float height; /**< The height of the capsule */
  float radius; /**< The radius */

private:
  /**
   * Creates a mesh for this appearance in the given graphics context
   * @return The resulting mesh
   */
  Mesh* createMesh() override
  {
    return Mesh::generateCapsule(radius, height, 16, 17);
  }
};
