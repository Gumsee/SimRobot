/**
 * @file Simulation/PhysicalObject.h
 * Declaration of class PhysicalObject
 * @author Colin Graf
 */

#pragma once

#include "SimRobotCore3.h"
#include "Graphics/bGraphicsContext.h"
#include "Simulation/SimObject.h"
#include "Tools/Math/Pose3f.h"
#include <list>
#include <Desktop/GraphicsContext.h>
#include <Primitives/Transformable.h>

class Body;
class bGraphicsContext;

/**
 * @class PhysicalObject
 * Abstract class for scene graph objects with physical representation
 */
class PhysicalObject : public SimObject
{
public:
  Body* parentBody = nullptr; /**< The superior body object (might be 0) */
  int id = -1; /**< The index of the element in MuJoCo's data. */
  bool isinitialized = false;
  int type;

  
  std::list<PhysicalObject*> physicalChildren; /**< List of subordinate physical scene graph objects */
  std::list<PhysicalObject*> physicalDrawings; /**< List of subordinate physical objects that will be drawn relative to this one */

  PhysicalObject(const int& type, const std::string& name);

  /**
   * Creates the physical objects used by the OpenDynamicsEngine (ODE).
   * These are a geometry object for collision detection and/or a body,
   * if the simulation object is movable.
   */
  void createPhysics();
  virtual void createPhysicsInternal() {}
  virtual void createIDs();

  /**
   * Submits draw calls for physical primitives of the object (including children) in the given graphics context
   */
  virtual void drawPhysics() const;

  /** Draws controller drawings of this physical object (and children) */
  void drawControllerDrawings() const;

  /**
   * Prepare controller drawings of this physical object (and children) for a frame
   * @param projection Pointer to a column-major 4x4 projection matrix
   * @param view Pointer to a column-major 4x4 view matrix
   */
  void beforeControllerDrawings(const float* projection, const float* view) const;

  /** Finish a frame of controller drawings for this physical object (and children) */
  void afterControllerDrawings() const;

protected:
  /**
   * Visits controller drawings of physical children
   * @param accept The functor to apply to every child
   */
  virtual void visitPhysicalControllerDrawings(const std::function<void(PhysicalObject&)>& accept);

  /**
   * Registers an element as parent
   * @param element The element to register
   */
  void addParent(Element& element) override;

private:
  std::list<SimRobotCore3::Controller3DDrawing*> controllerDrawings; /**< Drawings registered by another SimRobot module */

protected:
  // API
  virtual bool registerDrawing(SimRobotCore3::Controller3DDrawing& drawing);
  virtual bool unregisterDrawing(SimRobotCore3::Controller3DDrawing& drawing);
  virtual SimRobotCore3::Body* getParentBody();
};
