/**
 * @file Simulation/Compound.h
 * Declaration of class Compound
 * @author Colin Graf
 */

#pragma once

#include "SimRobotCore3.h"
#include "Simulation/GraphicalObject.h"
#include "Simulation/PhysicalObject.h"

class Geometry;

/**
 * @class Compound
 * A non-movable physical object
 */
class Compound : public PhysicalObject, public GraphicalObject, public SimRobotCore3::Compound
{
public:
  Compound(const std::string& name);

private:
  /**
   * Registers an element as parent
   * @param element The element to register
   */
  void addParent(Element& element) override;

  // API
  const QString& getFullName() const override {return SimObject::getFullName();}
  SimRobot::Widget* createWidget() override {return SimObject::createWidget();}
  const QIcon* getIcon() const override {return SimObject::getIcon();}
  bool registerDrawing(SimRobotCore3::Controller3DDrawing& drawing) override {return ::PhysicalObject::registerDrawing(drawing);}
  bool unregisterDrawing(SimRobotCore3::Controller3DDrawing& drawing) override {return ::PhysicalObject::unregisterDrawing(drawing);}
  SimRobotCore3::Body* getParentBody() override {return ::PhysicalObject::getParentBody();}
};
