/**
 * @file Simulation/Sensors/ObjectSegmentedImageSensor.h
 * Declaration of class ObjectSegmentedImageSensor
 *
 * @author Jesse Richter-Klug
 *  includes a lot of duplicated code origin in Simulation/Sensors/CameraSensor.h authored by Colin Graf
 */

#pragma once

#include "Graphics/bGraphicsContext.h"
#include "Simulation/Sensors/Sensor.h"
#include "Tools/Math/Eigen.h"
#include "Tools/Math/Pose3f.h"

/**
 * @class ObjectSegmentedImageSensor
 * A simulated camera that takes pictures where each pixel that belongs to a different object gets a
 *   different color value. In fact, each pixel that belongs to the same object gets the same color value
 */
class ObjectSegmentedImageSensor : public Sensor
{
public:
  unsigned int imageWidth; /**< The width of a camera image */
  unsigned int imageHeight; /**< The height of a camera image */
  float angleX;
  float angleY;

  /** Default constructor */
  ObjectSegmentedImageSensor(const std::string& name);

private:
  /**
   * @class ObjectSegmentedImageSensorPort
   * The camera sensor interface
   */
  class ObjectSegmentedImageSensorPort : public Sensor::Port
  {
  public:
    ::PhysicalObject* physicalObject; /**< The physical object were the camera is mounted on */
    ObjectSegmentedImageSensor* camera;
    unsigned char* imageBuffer; /**< A buffer for rendered image data */
    unsigned int imageBufferSize;
    Transformable3D offset; /**< Offset of the camera relative to the body it mounted on */
    Matrix4f projection; /**< The perspective projection matrix */

    /** Update the sensor value. Is called when required. */
    void updateValue() override;

    //API
    bool getMinAndMax(float& min, float& max) const override {min = 0; max = 0xff; return true;}
    bool renderCameraImages(SimRobotCore3::SensorPort** cameras, unsigned int count) override;
  } sensor;

  /** Destructor */
  ~ObjectSegmentedImageSensor();

  /** Initializes the camera after all attributes have been set */
  void createPhysicsInternal() override;

  /**
   * Registers an element as parent
   * @param element The element to register
   */
  void addParent(Element& element) override;

  /** Registers this object with children, actuators and sensors at SimRobot's GUI */
  void registerObjects() override;

  /**
   * Submits draw calls for physical primitives of the object (including children) in the given graphics context
   */
  void drawPhysics() const override;

  Object3D* pyramid = nullptr; /**< The pyramid mesh for the sensor drawing. */
  bGraphicsContext::Surface* surface = nullptr; /**< The surface for the sensor drawing. */

  std::vector<bGraphicsContext::Surface*>& surfaces; /**< The special surfaces for each body. */
};
