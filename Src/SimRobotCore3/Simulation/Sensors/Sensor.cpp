/**
 * @file Simulation/Sensor.cpp
 * Implementation of class Sensor
 * @author Colin Graf
 */

#include "Sensor.h"
#include "CoreModule.h"
#include "Graphics/bGraphicsContext.h"
#include "SensorWidget.h"
#include "Simulation/Simulation.h"
#include <mujoco/mujoco.h>

Sensor::Sensor(const std::string& name)
  : ::PhysicalObject(mjOBJ_SENSOR, findAvailableName(name, "Sensor"))
{}

void Sensor::Port::createIDs()
{
  if(type != mjOBJ_UNKNOWN)
    id = mj_name2id(Simulation::simulation->model, type, name.c_str());
}

const QIcon* Sensor::Port::getIcon() const
{
  return &CoreModule::module->sensorIcon;
}

SimRobot::Widget* Sensor::Port::createWidget()
{
  return new SensorWidget(this);
}

SimRobotCore3::SensorPort::Data Sensor::Port::getValue()
{
  if(lastSimulationStep != Simulation::simulation->simulationStep)
  {
    updateValue();
    lastSimulationStep = Simulation::simulation->simulationStep;
  }
  return data;
}
