/**
 * @file Simulation/Geometries/CylinderGeometry.cpp
 * Implementation of class CylinderGeometry
 * @author Colin Graf
 */

#include "CylinderGeometry.h"
#include "Platform/Assert.h"
#include <mujoco/mujoco.h>
#include <cmath>
#include "Simulation/Simulation.h"

mjsGeom* CylinderGeometry::assembleGeometry(mjsBody* body)
{
  mjsGeom* geom = mjs_addGeom(body, nullptr);
  mjs_setName(geom->element, Simulation::simulation->getName(mjOBJ_GEOM, "CylinderGeometry", nullptr, this));
  geom->type = mjGEOM_CYLINDER;
  geom->size[0] = radius;
  geom->size[1] = 0.5f * height;
  innerRadius = radius;
  innerRadiusSqr = innerRadius * innerRadius;
  outerRadius = std::sqrt(height * height * 0.25f + radius * radius);
  return geom;
}

void CylinderGeometry::createPhysics(bGraphicsContext& graphicsContext)
{
  Geometry::createPhysics(graphicsContext);

  ASSERT(!cylinder);
  cylinder = new Object3D(Mesh::generateCylinder(radius, height, 16), "CylinderGeometry");
  cylinder->addInstance()->setMatrix(getTransformationMatrix());
}

void CylinderGeometry::drawPhysics() const
{
  cylinder->render();
  Geometry::drawPhysics();
}
