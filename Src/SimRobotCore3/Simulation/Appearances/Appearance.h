/**
 * @file Simulation/Appearances/Appearance.h
 * Declaration of class Appearance
 * @author Colin Graf
 */

#pragma once

#include "SimRobotCore3.h"
#include "Simulation/GraphicalObject.h"
#include "Simulation/SimObject.h"
#include <string>

/**
 * @class Appearance
 * Abstract class for the graphical representation of physical objects
 */
class Appearance : public SimObject, public GraphicalObject, public SimRobotCore3::Appearance
{
public:
  Appearance(const std::string& name);

  class Material        : public ElementCore3, public ::Material { public:
    void addParent(Element& element) override {
      Appearance* appearance = dynamic_cast<Appearance*>(&element);
      ASSERT(appearance);
      ASSERT(!appearance->getMaterial());
      appearance->setMaterial(this);
    } 
  };

  bool renderForward = false;
  void updateAppearances() override;
  
protected:
  /**
   * Creates resources to later draw the object in the given graphics context
   */
  void createGraphics() override;

  /**
   * Creates a mesh for this appearance in the given graphics context
   * @return The resulting mesh
   */
  virtual Mesh* createMesh()
  {
    return nullptr;
  }

private:
  /**
   * Registers an element as parent
   * @param element The element to register
   */
  void addParent(Element& element) override;

  //API
  const QString& getFullName() const override {return SimObject::getFullName();}
  SimRobot::Widget* createWidget() override {return SimObject::createWidget();}
  const QIcon* getIcon() const override;
  bool registerDrawing(SimRobotCore3::Controller3DDrawing& drawing) override {return ::GraphicalObject::registerDrawing(drawing);}
  bool unregisterDrawing(SimRobotCore3::Controller3DDrawing& drawing) override {return ::GraphicalObject::unregisterDrawing(drawing);}
};
