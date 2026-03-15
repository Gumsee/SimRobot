/**
 * @file Simulation/Sensors/SingleDistanceSensor.cpp
 * Implementation of class SingleDistanceSensor
 * @author Colin Graf
 */

#include "SingleDistanceSensor.h"
#include "CoreModule.h"
#include "Platform/Assert.h"
#include <mujoco/mujoco.h>

SingleDistanceSensor::SingleDistanceSensor(const std::string& name)
  : Sensor(findAvailableName(name, "SingleDistanceSensor"))
{
  sensor.sensorType = SimRobotCore3::SensorPort::floatSensor;
  sensor.unit = "m";
}

void SingleDistanceSensor::createPhysicsInternal()
{
  sensor.min = min;
  sensor.max = max;

  ASSERT(!ray);
  ray = new Object3D(Mesh::generateLine(vec3(0,0,0), vec3(max, 0.f, 0.f)), "SingleDistanceSensor");
  ray->getVertexArrayObject()->setPrimitiveType(VertexArrayObject::PrimitiveTypes::LINE_STRIP);
  ray->getVertexArrayObject()->setVertexCount(2);
  Object3DInstance* instance = ray->addInstance();
  instance->setMatrix(worldTransformation.getMatrix());
  ray->applyTransformationMatrix(instance);

  //TODO
  //ASSERT(!surface);
  //static const float color[] = {0.5f, 0.f, 0.f, 1.f};
  //surface = graphicsContext.requestSurface(color, color);
}

void SingleDistanceSensor::registerObjects()
{
  sensor.fullName = fullName + ".distance";
  CoreModule::application->registerObject(*CoreModule::module, sensor, this);

  Sensor::registerObjects();
}

void SingleDistanceSensor::addParent(Element& element)
{
  sensor.physicalObject = this;
  ASSERT(sensor.physicalObject);
  Sensor::addParent(element);
}

void SingleDistanceSensor::DistanceSensor::updateValue()
{
  vec3 eulerrot = fquat::toEuler(physicalObject->worldTransformation.getRotation());
  vec3 direction(
    Gum::Maths::cosdeg(eulerrot.z)*Gum::Maths::cosdeg(eulerrot.y),
    Gum::Maths::sindeg(eulerrot.z)*Gum::Maths::cosdeg(eulerrot.y),
    Gum::Maths::sindeg(eulerrot.y)
  );

  mjtNum origin[3], dir[3];
  mju_f2n(origin, physicalObject->worldTransformation.getPosition().data(), 3);
  mju_f2n(dir, direction.data(), 3);

  const float dist = static_cast<float>(mj_ray(Simulation::simulation->model, Simulation::simulation->data, origin, dir, nullptr, 1, -1, nullptr));
  
  if(dist < 0.f)
    data.floatValue = max;
  else if(dist < min)
    data.floatValue = min;
  else if(dist > max)
    data.floatValue = max;
  else
    data.floatValue = dist;
}

bool SingleDistanceSensor::DistanceSensor::getMinAndMax(float& min, float& max) const
{
  min = this->min;
  max = this->max;
  return true;
}

void SingleDistanceSensor::drawPhysics() const
{
  ray->render();
  Sensor::drawPhysics();
}


void SingleDistanceSensor::updateTransformation()
{
  calcTransformationMatrix();
  ray->getInstance()->setMatrix(worldTransformation.getMatrix());
  ray->applyTransformationMatrix(ray->getInstance());

  ::PhysicalObject::updateTransformation();
}
