/**
 * @file Simulation/Sensors/CollisionSensor.cpp
 * Implementation of class CollisionSensor
 * @author Colin Graf
 */

#include "CollisionSensor.h"
#include "CoreModule.h"
#include "Simulation/Body.h"
#include "Simulation/Geometries/Geometry.h"

CollisionSensor::CollisionSensor(const std::string& name)
  : Sensor(findAvailableName(name, "CollisionSensor"))
{
  sensor.sensorType = SimRobotCore3::SensorPort::boolSensor;
}

void CollisionSensor::createPhysicsInternal()
{
  // add geometries
  for(::PhysicalObject* child : physicalDrawings)
  {
    if(dynamic_cast<Geometry*>(child) != nullptr)
    {
      hasGeometries = true;
      break;
    }
  }

  // register collision callback function
  if(hasGeometries)
    registerCollisionCallback(physicalDrawings);
  else // in case the sensor has no geometries use the geometries of the body to which the sensor is attached
    registerCollisionCallback(parentBody->physicalDrawings);
}

void CollisionSensor::registerCollisionCallback(std::list<::PhysicalObject*>& geometries)
{
  for(::PhysicalObject* i : geometries)
  {
    Geometry* geometry = dynamic_cast<Geometry*>(i);
    if(geometry)
    {
      geometry->registerCollisionCallback(sensor);
      registerCollisionCallback(geometry->physicalDrawings);
    }
  }
}

void CollisionSensor::registerObjects()
{
  sensor.fullName = fullName + ".contact";
  CoreModule::application->registerObject(*CoreModule::module, sensor, this);

  Sensor::registerObjects();
}

void CollisionSensor::CollisionSensorPort::updateValue()
{
  data.boolValue = lastCollisionStep == Simulation::simulation->simulationStep;
}

void CollisionSensor::CollisionSensorPort::collided(SimRobotCore3::Geometry&, SimRobotCore3::Geometry&)
{
  lastCollisionStep = Simulation::simulation->simulationStep;
}

void CollisionSensor::drawPhysics() const
{
  for(const ::PhysicalObject* drawing : (hasGeometries ? physicalDrawings : parentBody->physicalDrawings))
  {
    if(drawing != this) //TODO
      drawing->drawPhysics();
  }

  Sensor::drawPhysics();
}
