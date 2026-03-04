/**
 * @file Graphics/Primitives.h
 *
 * This file declares functions to create meshes for standard geometry objects.
 *
 * @author Arne Hasselbring
 */

#pragma once

#include "Graphics/bGraphicsContext.h"
#include "Tools/Math/Eigen.h"

class Primitives final
{
public:
  /**
   * Creates a line mesh.
   * @param bGraphicsContext The context in which to create the mesh.
   * @param start The start of the line.
   * @param end The end of the line.
   * @return A pointer to the new mesh (owned by the context).
   */
  static bGraphicsContext::Mesh* createLine(bGraphicsContext& bGraphicsContext, const Vector3f& start, const Vector3f& end);

  /**
   * Creates a disk mesh (centered at 0, 0, 0, in the xy plane).
   * @param bGraphicsContext The context in which to create the mesh.
   * @param inner Inner radius of the disk.
   * @param outer Outer radius of the disk.
   * @param slices The number of subdivisions in the plane.
   * @return A pointer to the new mesh (owned by the context).
   */
  static bGraphicsContext::Mesh* createDisk(bGraphicsContext& bGraphicsContext, float inner, float outer, unsigned int slices);

  /**
   * Creates a pyramid mesh (the top is at 0, 0, 0, the base in the yz plane).
   * @param bGraphicsContext The context in which to create the mesh.
   * @param width The width of the pyramid base (y axis).
   * @param height The height of the pyramid base (z axis).
   * @param depth The depth of the pyramid (x axis).
   * @return A pointer to the new mesh (owned by the context).
   */
  static bGraphicsContext::Mesh* createPyramid(bGraphicsContext& bGraphicsContext, float width, float height, float depth);
};
