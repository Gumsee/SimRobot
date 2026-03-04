/**
 * @file Simulation/Appearances/CylinderAppearance.h
 * Declaration of class CylinderAppearance
 * @author Colin Graf
 */

#pragma once

#include "Graphics/bGraphicsContext.h"
#include "Simulation/Appearances/Appearance.h"

/**
 * @class CylinderAppearance
 * The graphical representation of a cylinder
 */
class CylinderAppearance : public Appearance
{
public:
  float height; /**< The height of the cylinder */
  float radius; /**< The radius */

private:
  /**
   * Creates a mesh for this appearance in the given graphics context
   * @param graphicsContext The graphics context to create the mesh in
   * @return The resulting mesh
   */
  Mesh* createMesh() override
  {
    return Mesh::generateCylinder(radius, height, 16);
  }
};
