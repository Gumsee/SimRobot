/**
 * @file Simulation/Geometries/CapsuleGeometry.h
 * Declaration of class CapsuleGeometry
 * @author Colin Graf
 */

#pragma once

#include "Simulation/Geometries/Geometry.h"

/**
 * @class CapsuleGeometry
 * A capsule shaped geometry
 */
class CapsuleGeometry : public Geometry
{
public:
  CapsuleGeometry(const std::string& name)
    : Geometry(findAvailableName(name, "CapsuleGeometry"))
  {}

  float height; /**< The height of the capsule (including the spheres at the ends) */
  float radius; /**< The radius of the capsule */

private:
  /**
   * Creates the geometry (not including \c translation and \c rotation)
   * @param body The body to which to attach the geometry
   * @param The created geometry
   */
  mjsGeom* assembleGeometry(mjsBody* body) override
  {
    mjsGeom* geom = mjs_addGeom(body, nullptr);
    name = Simulation::simulation->getName(mjOBJ_GEOM, "CapsuleGeometry", nullptr, this);
    mjs_setName(geom->element, name.c_str());
    geom->type = mjGEOM_CAPSULE;
    geom->size[0] = radius;
    geom->size[1] = 0.5f * height - radius;
    innerRadius = radius;
    innerRadiusSqr = innerRadius * innerRadius;
    outerRadius = std::max(radius, height * 0.5f);

    obj = new Object3D(Mesh::generateCapsule(radius, height, 16, 17), "CapsuleGeometry");

    return geom;
  }
};
