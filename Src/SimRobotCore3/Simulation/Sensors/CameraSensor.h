/**
 * @file Simulation/Sensors/CameraSensor.h
 * Declaration of class CameraSensor
 * @author Colin Graf
 */

#pragma once

#include "Simulation/Sensors/Sensor.h"
#include "Graphics/SimObject3D.h"
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
  CameraSensor(const std::string& name, const ivec2& imageSize, const vec2& angle);

private:
  ivec2 imageSize = 0; /**< The size of a camera image */
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
    vec3 rot;

    /** Update the sensor value. Is called when required. */
    void updateValue() override;

    //API
    bool getMinAndMax(float& min, float& max) const override {min = 0; max = 0xff; return true;}
  } sensor;

  /** Destructor */
  ~CameraSensor();

  /** Initializes the camera after all attributes have been set */
  void createPhysicsInternal() override;

  /** Registers this object with children, actuators and sensors at SimRobot's GUI */
  void registerObjects() override;

  /**
   * Submits draw calls for physical primitives of the object (including children) in the given graphics context
   */
  void drawPhysics() const override;

  void updateTransformation() override;

  SimObject3D* pyramid = nullptr; /**< The pyramid mesh for the sensor drawing. */
};
