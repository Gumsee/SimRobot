/**
 * @file Simulation/Geometries/Geometry.h
 * Declaration of class Geometry
 * @author Colin Graf
 */

#pragma once

#include "SimRobotCore3.h"
#include "Graphics/bGraphicsContext.h"
#include "Simulation/PhysicalObject.h"
#include <mujoco/mjspec.h>
#include <mujoco/mujoco.h>
#include <list>
#include <string>
#include <unordered_map>
#include <Graphics/Object3D.h>

/**
 * @class Geometry
 * Abstract class for geometries of physical objects
 */
class Geometry : public PhysicalObject, public SimRobotCore3::Geometry
{
public:
  /**
   * @class Material
   * Describes friction properties of the material a geometry is made of
   */
  class Material : public ElementCore3
  {
  public:
    float friction = 0.f; /**< The friction of the material */
    float rollingFriction = 0.f; /**< The rolling friction of the material */

  private:
    /**
     * Registers an element as parent
     * @param element The element to register
     */
    void addParent(Element& element) override;
  };

  float innerRadius; /**< The radius of a sphere that is enclosed by the geometry. This radius is used for implementing a fuzzy but fast distance sensor */
  float innerRadiusSqr; /**< precomputed square of \c innerRadius */
  float outerRadius; /**< The radius of a sphere that covers the geometry. */

  mjsGeom* geom = nullptr;
  Object3D* obj = nullptr;
  ::color color; /**< A color for drawing the geometry */
  Material* material = nullptr; /**< The material the surface of the geometry is made of */
  std::list<SimRobotCore3::CollisionCallback*>* collisionCallbacks = nullptr; /**< Collision callback functions registered by another SimRobot module */
  inline static unsigned int contactPoints = 0U;
  inline static std::vector<Geometry*> registeredGeometries;

  Geometry(const std::string& name);

  /** Destructor */
  ~Geometry();

  void createPhysicsInternal() override;
  void createIDs() override;

  /**
   * Creates the geometry and adds it to a body at the given offset
   * @param body The body to which to attach the geometry
   * @param offset Offset of the geometry's frame relative to the body's frame.
   * @param collisionGroup The collision group to which the geometry belongs. Geometries within a group don't collide
   * @param immaterial Whether the geometry collides or just tests for collision
   */
  void createGeometry(mjsBody* body, int collisionGroup, bool immaterial = false) {}

  void updateTransformation() override;
  void drawPhysics() const override;

  static void checkCollisions();

  bGraphicsContext::Surface* surface = nullptr; /**< The surface of this geometry drawing */

protected:
  /**
   * Creates the geometry (not including \c translation and \c rotation)
   * @param body The body to which to attach the geometry
   * @return The created geometry
   */
  virtual mjsGeom* assembleGeometry([[maybe_unused]] mjsBody* body)
  {
    return nullptr;
  }

private:
  friend class CollisionSensor;

private:
  // API
  const QString& getFullName() const override {return SimObject::getFullName();}
  SimRobot::Widget* createWidget() override {return SimObject::createWidget();}
  const QIcon* getIcon() const override {return SimObject::getIcon();}
  bool registerDrawing(SimRobotCore3::Controller3DDrawing& drawing) override {return ::PhysicalObject::registerDrawing(drawing);}
  bool unregisterDrawing(SimRobotCore3::Controller3DDrawing& drawing) override {return ::PhysicalObject::unregisterDrawing(drawing);}
  SimRobotCore3::Body* getParentBody() override {return ::PhysicalObject::getParentBody();}
  bool registerCollisionCallback(SimRobotCore3::CollisionCallback& collisionCallback) override;
  bool unregisterCollisionCallback(SimRobotCore3::CollisionCallback& collisionCallback) override;
};
