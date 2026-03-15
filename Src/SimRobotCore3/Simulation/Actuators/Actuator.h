/**
 * @file Simulation/Actuators/Actuator.h
 * Declaration of class Actuator
 * @author Colin Graf
 */

#pragma once

#include "SimRobotCore3.h"
#include "Simulation/PhysicalObject.h"
#include <QString>
#include <Graphics/Object3D.h>

/**
 * @class Actuator
 * An abstract class for actuators
 */
class Actuator : public PhysicalObject, public SimRobotCore3::Actuator
{
public:
  Actuator(const std::string& name);

  class Port : public SimRobotCore3::ActuatorPort
  {
  public:
    QString fullName; /**< The path name to the object in the scene graph */
    QString unit; /**< The unit of the actuator's setpoint */

    /** Called before computing a simulation step to do something with the set-point of the actuator */
    virtual void act() = 0;

  private:
    // API
    const QString& getFullName() const override {return fullName;}
    const QIcon* getIcon() const override;
    SimRobot::Widget* createWidget() override;
    const QString& getUnit() const override {return unit;}
  };

private:
  // API
  const QString& getFullName() const override {return SimObject::getFullName();}
  SimRobot::Widget* createWidget() override {return SimObject::createWidget();}
  const QIcon* getIcon() const override {return SimObject::getIcon();}
  bool registerDrawing(SimRobotCore3::Controller3DDrawing& drawing) override {return ::PhysicalObject::registerDrawing(drawing);}
  bool unregisterDrawing(SimRobotCore3::Controller3DDrawing& drawing) override {return ::PhysicalObject::unregisterDrawing(drawing);}
  SimRobotCore3::Body* getParentBody() override {return ::PhysicalObject::getParentBody();}
};
