/**
 * @file Simulation/Sensors/DepthImageSensor.cpp
 * Implementation of class DepthImageSensor
 * @author Colin Graf
 */

#include "DepthImageSensor.h"
#include "CoreModule.h"
#include "Platform/Assert.h"
#include "Simulation/Body.h"
#include "Simulation/Scene.h"
#include "Tools/Math/Constants.h"
#include "Tools/OpenGLTools.h"
#include <algorithm>
#include <cmath>

DepthImageSensor::DepthImageSensor()
{
  sensor.depthImageSensor = this;
  sensor.sensorType = SimRobotCore3::SensorPort::floatArraySensor;
  sensor.unit = "m";
  sensor.imageBuffer = nullptr;
  sensor.renderBuffer = nullptr;
  sensor.lut = nullptr;
}

DepthImageSensor::~DepthImageSensor()
{
  if(sensor.imageBuffer)
    delete[] sensor.imageBuffer;
  if(projection == sphericalProjection && sensor.renderBuffer)
    delete[] sensor.renderBuffer;
  if(sensor.lut)
    delete[] sensor.lut;
}

void DepthImageSensor::createPhysics(bGraphicsContext& graphicsContext)
{
  Sensor::createPhysics(graphicsContext);

  sensor.imageBuffer = new float[imageWidth * imageHeight];
  sensor.renderHeight = imageHeight;

  if(projection == sphericalProjection)
  {
    ASSERT(imageHeight == 1);

    sensor.numOfBuffers = static_cast<unsigned int>(std::ceil(angleX / (pi * 2.0f / 3.0f)));
    sensor.bufferWidth = static_cast<unsigned int>(std::ceil(static_cast<float>(imageWidth) / static_cast<float>(sensor.numOfBuffers)));
    sensor.lut = new float*[sensor.bufferWidth];
    sensor.renderAngleX = angleX * sensor.bufferWidth / imageWidth;

    //Compute new resolution of rendering buffer
    float maxAngle(sensor.renderAngleX / 2.0f);
    float minPixelWidth(std::tan(maxAngle/(float(sensor.bufferWidth) / 2.0f)));
    float totalWidth(std::tan(maxAngle));
    float newXRes(totalWidth / minPixelWidth);
    sensor.renderWidth = static_cast<unsigned int>(ceil(newXRes)) * 2;
    sensor.renderBuffer = new float[sensor.renderWidth];

    //Compute values for LUT (sensor data -> rendering buffer)
    float firstAngle(-maxAngle);
    float step(maxAngle / (static_cast<float>(sensor.bufferWidth) / 2.0f));
    float currentAngle(firstAngle);
    float gToPixelFactor(newXRes / std::tan(maxAngle));
    for(unsigned int i = 0; i < sensor.bufferWidth; ++i)
    {
      float g(std::tan(currentAngle));
      g *= gToPixelFactor;
      int gPixel(static_cast<int>(g) + static_cast<int>(sensor.renderWidth) / 2);
      sensor.lut[i] = &sensor.renderBuffer[gPixel];
      currentAngle += step;
    }
  }
  else
  {
    sensor.numOfBuffers = 1;
    sensor.bufferWidth = imageWidth;
    sensor.renderWidth = imageWidth;
    sensor.renderAngleX = angleX;
    sensor.renderBuffer = sensor.imageBuffer;
  }

  sensor.dimensions.append(imageWidth);
  if(imageHeight > 1)
    sensor.dimensions.append(imageHeight);
  sensor.data.floatArray = sensor.imageBuffer;

  if(translation)
    sensor.offset.translation = *translation;
  if(rotation)
    sensor.offset.rotation = *rotation;

  sensor.min = min;
  sensor.max = max;

  const float zNear = std::max(min, 0.001f); // at least 1mm since zNear must not be zero
  float aspect = std::tan(sensor.renderAngleX * 0.5f) / std::tan(angleY * 0.5f);
  OpenGLTools::computePerspective(angleY, aspect, zNear, max, sensor.projection);

  Mesh* mesh = new Mesh();
  
  vec3 ml;
  if(projection == perspectiveProjection)
    ml = vec3(max, -std::tan(angleX * 0.5f) * max, 0);
  else
    ml = vec3(std::cos(angleX * 0.5f) * max, -std::sin(angleX * 0.5f) * max, 0);
  vec3 mt(ml.x, 0, std::tan(angleY * 0.5f) * max);
  vec3 tl(ml.x, ml.y, mt.z);
  vec3 tr(ml.x, -ml.y, mt.z);
  vec3 bl(ml.x, ml.y, -mt.z);
  vec3 br(ml.x, -ml.y, -mt.z);
  const unsigned segments = static_cast<int>(18.f * angleX / pi);
  
  mesh->addVertex(Vertex(vec3(0,0,0), vec2(0,0), vec3(0.f, 0.f, 1.f)));
  mesh->addVertex(Vertex(tl, vec2(0,0), vec3(0.f, 0.f, 1.f)));
  mesh->addVertex(Vertex(tr, vec2(0,0), vec3(0.f, 0.f, 1.f)));
  mesh->addVertex(Vertex(bl, vec2(0,0), vec3(0.f, 0.f, 1.f)));
  mesh->addVertex(Vertex(br, vec2(0,0), vec3(0.f, 0.f, 1.f)));
  if(projection == sphericalProjection && segments > 0)
  {
    const float rotX = std::cos(angleX / static_cast<float>(segments));
    const float rotY = std::sin(angleX / static_cast<float>(segments));
    float x = tl.x;
    float y = tl.y;
    for(unsigned int i = 0; i < segments; ++i)
    {
      mesh->addVertex(Vertex(vec3(x, y, tl.z), vec2(0,0), vec3(0.f, 0.f, 1.f)));
      const float x2 = x * rotX - y * rotY;
      y = y * rotX + x * rotY;
      x = x2;
    }
    for(unsigned int i = 0; i < segments; ++i)
    {
      mesh->addVertex(Vertex(vec3(x, y, br.z), vec2(0,0), vec3(0.f, 0.f, 1.f)));
      const float x2 = x * rotX + y * rotY;
      y = y * rotX - x * rotY;
      x = x2;
    }
  }

  if(projection == sphericalProjection && segments > 0)
  {
    mesh->addIndex(5);
    for(unsigned int i = 1; i < segments; ++i)
    {
      mesh->addIndex(5 + i);
      mesh->addIndex(5 + i);
    }
    mesh->addIndex(2);
    mesh->addIndex(2);
    for(unsigned int i = 0; i < segments; ++i)
    {
      mesh->addIndex(5 + segments + i);
      mesh->addIndex(5 + segments + i);
    }
    mesh->addIndex(3);
  }
  else
  {
    mesh->addIndex(1);
    mesh->addIndex(2);
    mesh->addIndex(2);
    mesh->addIndex(4);
    mesh->addIndex(4);
    mesh->addIndex(3);
    mesh->addIndex(3);
    mesh->addIndex(1);
  }
  mesh->addIndex(1);
  mesh->addIndex(0);
  mesh->addIndex(0);
  mesh->addIndex(2);
  mesh->addIndex(3);
  mesh->addIndex(0);
  mesh->addIndex(0);
  mesh->addIndex(4);

  pyramidChain = new Object3D(mesh, "pyramidChain");

  ASSERT(!surface);
  static const float color[] = {0.f, 0.f, 0.5f, 1.f};
  surface = graphicsContext.requestSurface(color, color);
}

void DepthImageSensor::addParent(Element& element)
{
  sensor.physicalObject = dynamic_cast< ::PhysicalObject*>(&element);
  ASSERT(sensor.physicalObject);
  Sensor::addParent(element);
}

void DepthImageSensor::registerObjects()
{
  sensor.fullName = fullName + ".image";
  CoreModule::application->registerObject(*CoreModule::module, sensor, this);

  Sensor::registerObjects();
}

void DepthImageSensor::DistanceSensor::updateValue()
{
  // make sure the poses of all movable objects are up to date
  Simulation::simulation->scene->updateTransformations();

  bGraphicsContext& graphicsContext = Simulation::simulation->graphicsContext;
  graphicsContext.makeCurrent(renderWidth, renderHeight, false);
  graphicsContext.updateModelMatrices(bGraphicsContext::ModelMatrix::appearance, false);

  // setup camera position
  /*Pose3f pose = physicalObject->poseInWorld;
  pose.conc(offset);
  static const RotationMatrix cameraRotation = (Matrix3f() << Vector3f(0.f, -1.f, 0.f), Vector3f(0.f, 0.f, 1.f), Vector3f(-1.f, 0.f, 0.f)).finished();
  pose.rotate(cameraRotation);
  pose.rotate(RotationMatrix::aroundY((depthImageSensor->angleX - renderAngleX) / 2.0f));

  float* val = imageBuffer;
  unsigned int widthLeft = depthImageSensor->imageWidth;
  for(unsigned int i = 0; i < numOfBuffers; ++i)
  {
    Matrix4f transformation;
    OpenGLTools::convertTransformation(pose.inverse(), transformation);

    graphicsContext.startDepthOnlyRendering(projection, transformation, 0, 0, renderWidth, renderHeight, true);

    // draw all objects
    //TODO
    //Simulation::simulation->scene->drawAppearances(graphicsContext);

    graphicsContext.finishRendering();

    // read frame buffer
    graphicsContext.finishDepthRendering(renderBuffer, renderWidth, renderHeight);

    if(depthImageSensor->projection == perspectiveProjection)
    {
      // convert pixels to points in world and compute the depth (renderBuffer == imageBuffer)
      const float halfP34 = projection(2, 3) * 0.5f;
      const float halfP33m1 = projection(2, 2) * 0.5f - 0.5f;
      for(float* end = val + renderWidth * renderHeight; val < end; ++val)
        *val = halfP34 / (*val + halfP33m1);
    }
    else
    {
      // convert pixels to points in world and compute the distances (renderBuffer != imageBuffer)
      const float fInvSqr = 1.f / (projection(0, 0) * projection(0, 0));
      const float halfP34 = projection(2, 3) * 0.5f;
      const float halfP33m1 = projection(2, 2) * 0.5f - 0.5f;
      float* const mid = lut[bufferWidth / 2];
      const float factor = 2.0f / float(renderWidth);
      const unsigned int end = std::min(bufferWidth, widthLeft);
      for(unsigned int i = 0; i < end; ++i)
      {
        const float vx = (lut[i] - mid) * factor;
        *val++ = std::min<float>(halfP34 / (*lut[i] + halfP33m1) * std::sqrt(1.f + vx * vx * fInvSqr), max);
      }
      widthLeft -= end;
      pose.rotate(RotationMatrix::aroundY(-renderAngleX));
    }
  }*/
}

bool DepthImageSensor::DistanceSensor::getMinAndMax(float& min, float& max) const
{
  min = this->min;
  max = this->max;
  return true;
}

void DepthImageSensor::drawPhysics() const
{
  pyramidChain->render();
  Sensor::drawPhysics();
}
