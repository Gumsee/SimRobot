/**
 * @file Simulation/Actuators/Joint.cpp
 * Implementation of class Joint
 * @author <A href="mailto:tlaue@uni-bremen.de">Tim Laue</A>
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 * @author Colin Graf
 * @author Thomas Röfer
 */

#include "Joint.h"
#include "Platform/Assert.h"
#include "SimRobotCore3.h"
#include "Simulation/Axis.h"
#include "Simulation/Motors/Motor.h"
#include <cmath>

void Joint::createPhysics(bGraphicsContext& graphicsContext)
{
  Actuator::createPhysics(graphicsContext);

  ASSERT(!axisLine);
  axisLine = new Object3D(Mesh::generateLine(vec3(axis->x, axis->y, axis->z) * -0.05f, vec3(axis->x, axis->y, axis->z) * 0.05f), "JointLine");

  ASSERT(!sphere);
  sphere = new Object3D(Mesh::generateSphere(0.002f, 10, 10), "JointSphere");

  ASSERT(!surface);
  const float color[] = {std::abs(axis->x), std::abs(axis->y), std::abs(axis->z), 1.f};
  surface = graphicsContext.requestSurface(color, color);
}

void Joint::drawPhysics(bGraphicsContext& graphicsContext, unsigned int flags) const
{
  axisLine->render();
  sphere->render();

  Actuator::drawPhysics(graphicsContext, flags);
}

void Joint::registerObjects()
{
  // add sensors and actuators
  if(axis->motor)
    axis->motor->registerObjects();

  // add children
  ::PhysicalObject::registerObjects();
}
