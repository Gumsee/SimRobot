/**
 * @file Simulation/Actuators/Actuator.cpp
 * Implementation of class Actuator
 * @author Colin Graf
 */

#include "Actuator.h"
#include "ActuatorsWidget.h"
#include "CoreModule.h"
#include "Graphics/bGraphicsContext.h"

Actuator::Actuator(const std::string& name)
  : ::PhysicalObject(mjOBJ_JOINT, findAvailableName(name, "Actuator"))
{}

const QIcon* Actuator::Port::getIcon() const
{
  return &CoreModule::module->actuatorIcon;
}

SimRobot::Widget* Actuator::Port::createWidget()
{
  CoreModule::module->application->openObject(CoreModule::module->actuatorsObject);
  if(ActuatorsWidget::actuatorsWidget)
    ActuatorsWidget::actuatorsWidget->openActuator(fullName);
  return nullptr;
}
