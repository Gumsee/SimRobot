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
#include <Simulation/Simulation.h>

Joint::Joint(const std::string& name)
  : Actuator(findAvailableName(name, "Joint"))
{}

void Joint::createPhysicsInternal()
{
  ASSERT(!axisLine);
  axisLine = new Object3D(Mesh::generateLine(vec3(axis->x, axis->y, axis->z) * -0.05f, vec3(axis->x, axis->y, axis->z) * 0.05f), "JointLine");
  axisLine->getVertexArrayObject()->setPrimitiveType(VertexArrayObject::PrimitiveTypes::LINE_STRIP);
  axisLine->getVertexArrayObject()->setVertexCount(2);
  Object3DInstance* instance = axisLine->addInstance();
  instance->setMatrix(getMatrix());
  axisLine->applyTransformationMatrix(instance);

  ASSERT(!sphere);
  sphere = new Object3D(Mesh::generateSphere(0.002f, 10, 10), "JointSphere");
  instance = sphere->addInstance();
  instance->setMatrix(getMatrix());
  sphere->applyTransformationMatrix(instance);

  color = rgba(std::abs(axis->x) * 255.0f, std::abs(axis->y) * 255.0f, std::abs(axis->z) * 255.0f, 255.0f);
}

void Joint::drawPhysics() const
{
  Simulation::simulation->forwardRenderingShader->loadUniform("color", color);
  axisLine->render();
  sphere->render();

  Actuator::drawPhysics();
}

void Joint::createIDs()
{
  if(axis->motor != nullptr)
    axis->motor->createIDs();

  ::PhysicalObject::createIDs();
}
void Joint::registerObjects()
{
  // add sensors and actuators
  if(axis->motor)
    axis->motor->registerObjects();

  // add children
  ::PhysicalObject::registerObjects();
}

void Joint::updateTransformation()
{
  calcTransformationMatrix();
  axisLine->getInstance()->setMatrix(getMatrix());
  axisLine->applyTransformationMatrix(axisLine->getInstance());

  sphere->getInstance()->setMatrix(getMatrix());
  sphere->applyTransformationMatrix(sphere->getInstance());

  ::PhysicalObject::updateTransformation();
}