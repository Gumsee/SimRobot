/**
 * @file Simulation/Body.h
 * Declaration of class Body
 * @author Colin Graf
 */

#pragma once

#include "SimRobotCore3.h"
#include "Simulation/PhysicalObject.h"
#include "Simulation/GraphicalObject.h"
#include <mujoco/mjspec.h>
#include <list>

class Geometry;
class Mass;

/**
 * @class Body
 * A movable rigid body
 */
class Body : public PhysicalObject, public GraphicalObject, public SimRobotCore3::Body
{
public:
  mjsBody* body = nullptr; /**< The MuJoCo specification of this body. Only valid during \c createPhysics. */
  Body* rootBody = nullptr; /**< The first movable body in a chain of bodies (might point to itself) */
  int collisionGroup = 0; /**< The collision group of this body (one per root body). Compounds belong to collision group 0. */
  inline static std::vector<Body*> registeredBodies;

  Body(const std::string& name);

  /**
   * Creates resources to later draw the object in the given graphics context
   */
  void createGraphics() override;

  /**
   * Submits draw calls for physical primitives of the object (including children) in the given graphics context
   */
  void drawPhysics() const override;

  void createIDs() override;

  void updateTransformation() override;

  /**
   * Moves the object and its children relative to its current position
   * @param offset The distance to move
   */
  void move(const Vector3f& offset);

  /**
   * Rotate the object and its children around a point
   * @param rotation The rotation offset
   * @param point The point to turn around
   */
  void rotate(const RotationMatrix& rotation, const Vector3f& point);

private:
  Vector3f centerOfMass = Vector3f::Zero(); /**< The position of the center of mass relative to the pose of the body */
  Vector3f velocityInWorld; /**< A buffer used by \c getVelocity */


  std::list<Body*> bodyChildren; /**< List of first-degree child bodies that are connected to this body over a joint */

  /**
   * Creates the physical objects used by the OpenDynamicsEngine (ODE).
   * These are a geometry object for collision detection and/or a body,
   * if the simulation object is movable.
   */
  void createPhysicsInternal() override;

  /**
   * Adds a mass to the mass of the body
   * @param mass A mass description of the mass to add
   */
  void addMass(Mass& mass);

  /**
   * Registers an element as parent
   * @param element The element to register
   */
  void addParent(Element& element) override;

  /**
   * Visits controller drawings of physical children
   * @param accept The functor to apply to every child
   */
  void visitPhysicalControllerDrawings(const std::function<void(::PhysicalObject&)>& accept) override;

  /**
   * Visits controller drawings of graphical children
   * @param accept The functor to apply to every child
   */
  void visitGraphicalControllerDrawings(const std::function<void(GraphicalObject&)>& accept) override;

  friend class CollisionSensor;

private:
  // API
  const QString& getFullName() const override {return SimObject::getFullName();}
  SimRobot::Widget* createWidget() override {return SimObject::createWidget();}
  const QIcon* getIcon() const override {return SimObject::getIcon();}
  bool registerDrawing(SimRobotCore3::Controller3DDrawing& drawing) override {return ::PhysicalObject::registerDrawing(drawing);}
  bool unregisterDrawing(SimRobotCore3::Controller3DDrawing& drawing) override {return ::PhysicalObject::unregisterDrawing(drawing);}
  SimRobotCore3::Body* getParentBody() override {return parentBody;}
  const float* getPosition() const override;
  const float* getVelocity() const override;
  void setVelocity(const float* velocity) override;
  bool getPose(float* position, float (*rotation)[3]) const override;
  void move(const float* pos) override;
  void move(const float* pos, const float (*rot)[3]) override;
  void resetDynamics() override;
  SimRobotCore3::Body* getRootBody() override {return rootBody;}
  void enablePhysics(bool enable) override;
  void enableGravity(bool enable) override;
};
