/**
 * @file Simulation/Appearances/BoxAppearance.h
 * Declaration of class BoxAppearance
 * @author Colin Graf
 */

#pragma once

#include "Graphics/bGraphicsContext.h"
#include "Simulation/Appearances/Appearance.h"

/**
 * @class BoxAppearance
 * The graphical representation of a simple box
 */
class BoxAppearance : public Appearance
{
public:
  BoxAppearance(const std::string& name)
    : Appearance(findAvailableName(name, "BoxAppearance"))
  {}
  float width; /**< The width of the box (cy) */
  float height; /**< The height of the box (cz) */
  float depth; /**< The depth of the box (cx) */

private:
  /**
   * Creates a mesh for this appearance in the given graphics context
   * @return The resulting mesh
   */
  Mesh* createMesh() override
  {
    return Mesh::generateCube(vec3(depth, width, height));
  }
};
