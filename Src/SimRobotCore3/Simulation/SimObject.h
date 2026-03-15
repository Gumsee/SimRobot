/**
 * @file Simulation/SimObject.h
 * Declaration of class SimObject
 * @author Colin Graf
 */

#pragma once

#include "SimRobotCore3.h"
#include "Parser/ElementCore3.h"
#include "Tools/Math/Eigen.h"
#include "Tools/Math/Pose3f.h"
#include "Tools/Math/RotationMatrix.h"
#include <gum-maths.h>
#include <QString>
#include <list>
#include <string>
#include <Primitives/Transformable.h>

/**
 * @class SimObject
 * Abstract class for scene graph objects with a name and a transformation
 */
class SimObject : public ElementCore3
{
public:
  Transformable3D relativeTransformation; /**< The initial offset relative transformation to the origin of the parent object */
  Transformable3D worldTransformation; /**< The absolute pose of the object */
  QString fullName; /**< The path name to the object in the scene graph */
  std::string name; /**< The name of the scene graph object (without path) */
  std::list<SimObject*> children; /**< List of subordinate scene graph objects */
  SimObject* parent = nullptr;
  inline static std::unordered_map<std::string, SimObject*> loadedObjects;

  SimObject(const std::string& name);
  /** Destructor */
  ~SimObject();

  /** Registers this object with children, actuators and sensors at SimRobot's GUI */
  virtual void registerObjects();

  void calcTransformationMatrix();

  /** Updates the world transformation */
  virtual void updateTransformation();

  std::string findAvailableName(std::string name, const std::string& defaultvalue);

protected:
  /**
   * Registers an element as parent
   * @param element The element to register
   */
  virtual void addParent(Element& element);

protected:
  // API
  virtual const QString& getFullName() const {return fullName;}
  virtual SimRobot::Widget* createWidget();
  virtual const QIcon* getIcon() const;
};
