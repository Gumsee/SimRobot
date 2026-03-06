/**
 * @file Simulation/Geometries/SphereGeometry.cpp
 * Implementation of class SphereGeometry
 * @author Colin Graf
 */

#include "SphereGeometry.h"
#include "Platform/Assert.h"
#include <mujoco/mujoco.h>
#include "Simulation/Simulation.h"

mjsGeom* SphereGeometry::assembleGeometry(mjsBody* body)
{
  mjsGeom* geom = mjs_addGeom(body, nullptr);
  mjs_setName(geom->element, Simulation::simulation->getName(mjOBJ_GEOM, "SphereGeometry", nullptr, this));
  geom->type = mjGEOM_SPHERE;
  geom->size[0] = radius;
  innerRadius = radius;
  innerRadiusSqr = innerRadius * innerRadius;
  outerRadius = radius;
  return geom;
}

void SphereGeometry::createPhysics(bGraphicsContext& graphicsContext)
{
  Geometry::createPhysics(graphicsContext);

  ASSERT(!sphere);
  sphere = new Object3D(Mesh::generateSphere(radius, 16, 16), "SphereGeometry");
  Object3DInstance* instance = sphere->addInstance();
  instance->setMatrix(getTransformationMatrix());
}

void SphereGeometry::drawPhysics() const
{
  sphere->render();
  Geometry::drawPhysics();
}
