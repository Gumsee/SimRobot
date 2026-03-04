/**
 * @file Simulation/GraphicalObject.h
 * Declaration of class GraphicalObject
 * @author Colin Graf
 */

#pragma once

#include "SimRobotCore3.h"
#include "Simulation/SimObject.h"
#include <list>
#include <Engine/3D/Object/SceneObject.h>
#include <Desktop/GraphicsContext.h>

/**
 * @class GraphicalObject
 * Abstract class for scene graph objects with graphical representation or subordinate graphical representation
 */
class GraphicalObject : public SceneObject
{
public:
  GraphicalObject() : SceneObject("") {}
  std::list<GraphicalObject*> graphicalDrawings; /**< List of subordinate graphical scene graph objects */

  /**
   * Creates resources to later draw the object in the given graphics context
   * @param graphicsContext The graphics context to create resources in
   */
  virtual void createGraphics(GraphicsContext& graphicsContext);

  /**
   * Submits draw calls for appearance primitives of the object (including children) in the given graphics context
   * @param graphicsContext The graphics context to draw the object to
   */
  virtual void drawAppearances(GraphicsContext& graphicsContext) const;

  /** Draws controller drawings of this graphical object (and children) */
  void drawControllerDrawings() const;

  /**
   * Prepare controller drawings of this graphical object (and children) for a frame
   * @param projection Pointer to a column-major 4x4 projection matrix
   * @param view Pointer to a column-major 4x4 view matrix
   */
  void beforeControllerDrawings(const float* projection, const float* view) const;

  /** Finish a frame of controller drawings for this graphical object (and children) */
  void afterControllerDrawings() const;

protected:
  /**
   * Visits controller drawings of graphical children
   * @param accept The functor to apply to every child
   */
  virtual void visitGraphicalControllerDrawings(const std::function<void(GraphicalObject&)>& accept);

  /**
   * Registers an element as parent
   * @param element The element to register
   */
  virtual void addParent(Element& element);

private:
  std::list<SimRobotCore3::Controller3DDrawing*> controllerDrawings; /**< Drawings registered by another SimRobot module */

protected:
  // API
  virtual bool registerDrawing(SimRobotCore3::Controller3DDrawing& drawing);
  virtual bool unregisterDrawing(SimRobotCore3::Controller3DDrawing& drawing);
};
