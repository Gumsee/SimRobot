/**
 * @file Simulation/Geometries/SphereGeometry.h
 * Declaration of class SphereGeometry
 * @author Colin Graf
 */

#pragma once

#include "Simulation/Geometries/Geometry.h"

/**
 * @class SphereGeometry
 * A sphere shaped geometry
 */
class SphereGeometry : public Geometry
{
public:
  SphereGeometry(const std::string& name)
    : Geometry(findAvailableName(name, "SphereGeometry"))
  {}
  float radius; /**< The radius of the sphere */

private:
  /**
   * Creates the geometry (not including \c translation and \c rotation)
   * @param body The body to which to attach the geometry
   * @param The created geometry
   */
  mjsGeom* assembleGeometry(mjsBody* body) override
  {
    mjsGeom* geom = mjs_addGeom(body, nullptr);
    geom->type = mjGEOM_SPHERE;
    geom->size[0] = radius;
    innerRadius = radius;
    innerRadiusSqr = innerRadius * innerRadius;
    outerRadius = radius;

    obj = new Object3D(Mesh::generateSphere(radius, 16, 16), "SphereGeometry");

    return geom;
  }
};
