/**
 * @file Simulation/Simulation.h
 * Declaration of class Simulation
 * @author Colin Graf
 */

#pragma once

#include "Graphics/OriginRenderer.h"
#include "Simulation/Appearances/ComplexAppearance.h"
#include <mujoco/mjdata.h>
#include <mujoco/mjmodel.h>
#include <mujoco/mjspec.h>
#include <string>
#include <list>
#include <unordered_map>

class Body;
class Geometry;
class Scene;
class ElementCore3;

/**
 * @class Simulation
 * A class for managing the simulation
 */
class Simulation
{
public:
  static Simulation* simulation;

  Scene* scene = nullptr; /**< The root of the scene graph */
  std::list<ElementCore3*> elements; /**< All scene graph elements */

  int nextCollisionGroup = 1; /**< The collision group that the next root body is assigned. */
  mjSpec* spec = nullptr; /**< The model specification that is built from the scene description. Only valid during \c createPhysics. */
  mjsBody* worldBody = nullptr; /**< The world (root) body in the model specification. Only valid during \c createPhysics. */
  mjModel* model = nullptr; /**< The MuJoCo model that is compiled from the model specification. Only valid after \c createPhysics. */
  mjData* data = nullptr; /**< The MuJoCo simulation state. Only valid after \c createPhysics. */
  OriginRenderer* originRenderer;
  Object3D* bodyComSphereMesh = nullptr; /**< The mesh for the physical CoM drawing of bodies. */
  //bGraphicsContext::ModelMatrix* dragPlaneModelMatrix = nullptr; /**< The model matrix for the drag plane in object renderers. */
  Pose3f originPose; /**< Pose of the origin (assuming that renderers are sequential. */
  Pose3f dragPlanePose; /**< Pose of the drag plane (assuming it is not possible to drag simultaneously in multiple renderers). */
  ShaderProgram* forwardRenderingShader;
  bbox3 cameraBoundaries;

  unsigned int currentFrameRate = 0U; /**< The current frame rate of the simulation */
  unsigned int collisions = 0U;

  /** Default Constructor. */
  Simulation();

  /** Destructor. */
  virtual ~Simulation();

  /**
   * Loads a file and initializes the simulation
   * @param filename The name of the file
   * @param errors The errors that occurred during parsing.
   */
  bool loadFile(const std::string& filename, std::list<std::string>& errors);

  /** Executes one simulation step */
  void doSimulationStep();
  unsigned int simulationStep = 0;
  double simulatedTime = 0;

  /** Registers all objects of the simulation (including children, actuators and sensors) at SimRobot's GUI */
  void registerObjects();

private:
  /** Computes the frame rate of simulation */
  void updateFrameRate();
  unsigned int lastFrameRateComputationTime = 0;
  unsigned int lastFrameRateComputationStep = 0;

  static void mjError(const char*);
  static void mjWarning(const char*);
};
