/**
 * @file Simulation/Sensors/CameraSensor.h
 * Declaration of class CameraSensor
 * @author Colin Graf
 */

#pragma once

#include "Graphics/bGraphicsContext.h"
#include "Simulation/Sensors/Sensor.h"
#include "Tools/Math/Eigen.h"
#include "Tools/Math/Pose3f.h"
#include <Engine/3D/Renderer3D.h>

/**
 * @class CameraSensor
 * A simulated camera
 */
class CameraSensor : public Sensor
{
public:
  vec2 openingAngle;
  Renderer3D* renderer;
  Canvas* canvas;
  Camera3D* camera3d;

  /** Default constructor */
  CameraSensor(const std::string& name);

  void setImageDimensions(ivec2 dimensions, vec2 angle)
  {
    this->openingAngle = angle;
    this->imageWidth = dimensions.x;
    this->imageHeight = dimensions.y;
    canvas->setSize(dimensions);
    renderer->updateFramebufferSize();
    float aspect = std::tan(openingAngle.x * 0.5f) / std::tan(openingAngle.y * 0.5f);
    camera3d->setAspectRatio(aspect);
    camera3d->setFOV(Gum::Maths::toDegree(openingAngle.y));
    camera3d->updateProjection(dimensions);
  }

private:
  unsigned int imageWidth; /**< The width of a camera image */
  unsigned int imageHeight; /**< The height of a camera image */
  /**
   * @class CameraSensor
   * The camera sensor interface
   */
  class Sensor : public ::Sensor::Port
  {
  public:
    ::PhysicalObject* physicalObject; /**< The physical object were the camera is mounted on */
    CameraSensor* camera;
    unsigned char* imageBuffer; /**< A buffer for rendered image data */
    unsigned int imageBufferSize;
    Transformable3D offset; /**< Offset of the camera relative to the body it mounted on */

    /** Update the sensor value. Is called when required. */
    void updateValue() override;

    //API
    bool getMinAndMax(float& min, float& max) const override {min = 0; max = 0xff; return true;}
    bool renderCameraImages(SimRobotCore3::SensorPort** cameras, unsigned int count) override;
  } sensor;

  /** Destructor */
  ~CameraSensor();

  /** Initializes the camera after all attributes have been set */
  void createPhysicsInternal() override;

  /**
   * Registers an element as parent
   * @param element The element to register
   */
  void addParent(Element& element) override;

  /** Registers this object with children, actuators and sensors at SimRobot's GUI */
  void registerObjects(int level) override;

  /**
   * Submits draw calls for physical primitives of the object (including children) in the given graphics context
   */
  void drawPhysics() const override;

  void updateTransformation() override;

  Object3D* pyramid = nullptr; /**< The pyramid mesh for the sensor drawing. */
  bGraphicsContext::Surface* surface = nullptr; /**< The surface for the sensor drawing. */
};
