/**
 * @file Simulation/Geometries/BoxGeometry.h
 * Declaration of class BoxGeometry
 * @author Colin Graf
 */

#pragma once

#include "Simulation/Geometries/Geometry.h"

/**
 * @class BoxGeometry
 * A box shaped geometry
 */
class BoxGeometry : public Geometry
{
public:
  BoxGeometry(const std::string& name)
    : Geometry(findAvailableName(name, "BoxGeometry"))
  {}

  float width; /**< The width of the box (cy) */
  float height; /**< The height of the box (cz) */
  float depth; /**< The depth of the box (cx) */

private:
  /**
   * Creates the geometry (not including \c translation and \c rotation)
   * @param body The body to which to attach the geometry
   * @param The created geometry
   */
  mjsGeom* assembleGeometry(mjsBody* body) override
  {
    mjsGeom* geom = mjs_addGeom(body, nullptr);
    geom->type = mjGEOM_BOX;
    geom->size[0] = 0.5f * depth;
    geom->size[1] = 0.5f * width;
    geom->size[2] = 0.5f * height;
    innerRadius = std::min(std::min(depth, width), height) * 0.5f;
    innerRadiusSqr = innerRadius * innerRadius;
    outerRadius = std::sqrt(depth * depth * 0.25f + width * width * 0.25f + height * height * 0.25f);

    obj = new Object3D(Mesh::generateCube(vec3(depth, width, height)), "BoxGeometry");

    return geom;
  }
};
