/**
 * @file Simulation/Geometries/CylinderGeometry.h
 * Declaration of class CylinderGeometry
 * @author Colin Graf
 */

#pragma once

#include "Simulation/Geometries/Geometry.h"

/**
 * @class CylinderGeometry
 * A cylinder shaped geometry
 */
class CylinderGeometry : public Geometry
{
public:
  CylinderGeometry(const std::string& name)
    : Geometry(findAvailableName(name, "Geometry"))
  {}
  float height; /**< The height of the cylinder */
  float radius; /**< The radius of the cylinder */

private:
  /**
   * Creates the geometry (not including \c translation and \c rotation)
   * @param body The body to which to attach the geometry
   * @param The created geometry
   */
  mjsGeom* assembleGeometry(mjsBody* body) override
  {
    mjsGeom* geom = mjs_addGeom(body, nullptr);
    geom->type = mjGEOM_CYLINDER;
    geom->size[0] = radius;
    geom->size[1] = 0.5f * height;
    innerRadius = radius;
    innerRadiusSqr = innerRadius * innerRadius;
    outerRadius = std::sqrt(height * height * 0.25f + radius * radius);

    obj = new Object3D(Mesh::generateCylinder(radius, height, 16), "CylinderGeometry");

    return geom;
  }

};
