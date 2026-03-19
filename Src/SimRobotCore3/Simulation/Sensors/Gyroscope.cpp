/**
 * @file Simulation/Sensors/Gyroscope.cpp
 * Implementation of class Gyroscope
 * @author Colin Graf
 */

#include "Gyroscope.h"
#include "CoreModule.h"
#include "Platform/Assert.h"
#include "Simulation/Body.h"
#include "Simulation/Simulation.h"
#include <mujoco/mujoco.h>

Gyroscope::Gyroscope(const std::string& name)
  : Sensor(findAvailableName(name, "Gyroscope"))
{
  sensor.sensorType = SimRobotCore3::SensorPort::floatArraySensor;
  sensor.unit = QString::fromUtf8("°/s");
  sensor.descriptions.append("x");
  sensor.descriptions.append("y");
  sensor.descriptions.append("z");
  sensor.dimensions.append(3);
  sensor.data.floatArray = sensor.angularVel;
  sensor.name = this->name;
}

void Gyroscope::createPhysicsInternal()
{
  const char* siteName = name.c_str();

  mjsSite* site = mjs_addSite(sensor.body->body, nullptr);
  mjs_setName(site->element, siteName);
  mju_f2n(site->pos, relativeTransformation.getPosition().data(), 3);
  mju_f2n(site->quat, relativeTransformation.getRotation().data(), 4);
  mju_negQuat(site->quat, site->quat); // column major -> row major

  mjsSensor* sensor = mjs_addSensor(Simulation::simulation->spec);
  mjs_setName(sensor->element, name.c_str());
  sensor->type = mjSENS_GYRO;
  sensor->objtype = mjOBJ_SITE;
  mjs_setString(sensor->objname, siteName);
}

void Gyroscope::createIDs()
{
  sensor.createIDs();
}

void Gyroscope::addParent(Element& element)
{
  sensor.body = dynamic_cast<Body*>(&element);
  ASSERT(sensor.body);
  Sensor::addParent(element);
}

void Gyroscope::registerObjects()
{
  sensor.fullName = fullName + ".angularVelocities";
  CoreModule::application->registerObject(*CoreModule::module, sensor, this);

  Sensor::registerObjects();
}

void Gyroscope::GyroscopeSensor::updateValue()
{
  ASSERT(Simulation::simulation->model->sensor_dim[id] == 3);
  mju_n2f(angularVel, Simulation::simulation->data->sensordata + Simulation::simulation->model->sensor_adr[id], 3);
}
