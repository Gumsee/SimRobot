/**
 * @file Simulation/Geometries/CapsuleGeometry.cpp
 * Implementation of class CapsuleGeometry
 * @author Colin Graf
 */

#include "CapsuleGeometry.h"
#include "Platform/Assert.h"
#include <mujoco/mujoco.h>
#include <algorithm>
#include "Simulation/Simulation.h"

mjsGeom* CapsuleGeometry::assembleGeometry(mjsBody* body)
{
  mjsGeom* geom = mjs_addGeom(body, nullptr);
  mjs_setName(geom->element, Simulation::simulation->getName(mjOBJ_GEOM, "CapsuleGeometry", nullptr, this));
  geom->type = mjGEOM_CAPSULE;
  geom->size[0] = radius;
  geom->size[1] = 0.5f * height - radius;
  innerRadius = radius;
  innerRadiusSqr = innerRadius * innerRadius;
  outerRadius = std::max(radius, height * 0.5f);
  return geom;
}

void CapsuleGeometry::createPhysics(bGraphicsContext& graphicsContext)
{
  Geometry::createPhysics(graphicsContext);

  ASSERT(!capsule);
  capsule = new Object3D(Mesh::generateCapsule(radius, height, 16, 17), "CapsuleGeometry");
}

void CapsuleGeometry::drawPhysics(bGraphicsContext& graphicsContext, unsigned int flags) const
{
  capsule->render();

  Geometry::drawPhysics(graphicsContext, flags);
}
