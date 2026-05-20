/**
 * @file Simulation/Sensors/CameraSensor.cpp
 * Implementation of class CameraSensor
 * @author Colin Graf
 */

#include "CameraSensor.h"
#include "CoreModule.h"
#include "Platform/Assert.h"
#include "Simulation/Body.h"
#include "Simulation/Scene.h"
#include "Tools/OpenGLTools.h"
#include <cmath>
#include <Desktop/Window.h>
#include <Graphics/Graphics.h>
#include <Engine/PostProcessing/Effects/MotionBlur/MotionBlur.h>

CameraSensor::CameraSensor(const std::string& name, const ivec2& imageSize, const vec2& angle)
  : ::Sensor(findAvailableName(name, "CameraSensor")),
  openingAngle(angle),
  imageSize(imageSize)
{

  sensor.camera = this;
  sensor.sensorType = SimRobotCore3::SensorPort::cameraSensor;
  sensor.imageBuffer = nullptr;
  sensor.imageBufferSize = 0;
  sensor.name = this->name;
  sensor.mujocoName = this->mujocoName;
  sensor.physicalObject = this;

  canvas = new Canvas(this->imageSize);
  Settings::setSetting(Settings::SHADOW_SIZE, 6000);
  Settings::setSetting(Settings::SHADOW_DISTANCE_FACTOR, 0.5f);
  renderer = new Renderer3D(canvas);
  renderer->setExposure(1.0f);

  renderer->addPostProcessingEffect(new MotionBlur(canvas, static_cast<TextureDepth2D*>(renderer->getFramebuffer()->getDepthTextureAttachment())));

  camera3d = new Camera3D(canvas->getSize(), nullptr);
  camera3d->setWorldUpDirection(vec3(0,0,1));
  camera3d->setPosition(vec3(0,0,0));
  camera3d->setMode(Camera3D::CONTROLLED);
  camera3d->setProjectionMode(Camera3D::PERSPECTIVE);
  camera3d->setAspectRatio(std::tan(openingAngle.x * 0.5f) / std::tan(openingAngle.y * 0.5f));
  camera3d->setFOV(Gum::Maths::toDegree(openingAngle.y));
}

CameraSensor::~CameraSensor()
{
  if(sensor.imageBuffer)
    delete[] sensor.imageBuffer;
}

void CameraSensor::createPhysicsInternal()
{
  sensor.dimensions.append(imageSize.x);
  sensor.dimensions.append(imageSize.y);
  sensor.dimensions.append(3);

  sensor.offset = relativeTransformation;

  ASSERT(!pyramid);
  pyramid = new SimObject3D(Mesh::generatePyramid(vec2(std::tan(openingAngle.x * 0.5f) * 2.f, std::tan(openingAngle.y * 0.5f) * 2.f), 1.f), "CameraSensor");
  pyramid->getVertexArrayObject()->setPrimitiveType(VertexArrayObject::PrimitiveTypes::LINES);
  pyramid->getMaterial()->setColor(rgba(0.0f, 0.0f, 128.0f, 255.0f));
  Object3DInstance *instance = pyramid->addInstance();
  instance->setMatrix(getMatrix());
  pyramid->applyTransformationMatrix(instance);

  renderer->setWorld(Simulation::simulation->scene->world);
}

void CameraSensor::registerObjects()
{
  sensor.fullName = fullName + ".image";
  CoreModule::application->registerObject(*CoreModule::module, sensor, this);

  ::Sensor::registerObjects();
}

void CameraSensor::Sensor::updateValue()
{
  // allocate buffer
  const unsigned int imageWidth = camera->imageSize.x;
  const unsigned int imageHeight = camera->imageSize.y;
  const unsigned int imageSize = imageWidth * imageHeight * 3;
  if(imageBufferSize < imageSize)
  {
    if(imageBuffer)
      delete[] imageBuffer;
    imageBuffer = new unsigned char[imageSize];
    imageBufferSize = imageSize;
  }
  mat4 fixedOffset(
    0.0f, -0.0f, -1.0f, 0.0f, 
    -1.0f, 0.0f, -0.0f, 0.0f, 
    0.0f, 1.0f, -0.0f, 0.0f, 
    0.0f, 0.0f, 0.0f, 1.0f
  );
  
  camera->renderer->makeActive();
  camera->camera3d->makeActive();
  camera->camera3d->overrideViewMatrix(Gum::Maths::inverseTransformationMatrix(physicalObject->getMatrix() * fixedOffset * offset.getMatrix()));

  // draw all objects
  GraphicsContext::MainContext->bind();
  Framebuffer::WindowFramebuffer = camera->renderer->getFramebuffer();
  SimRobotCore3::Renderer::ShadeMode currPhysicsShadeMode = Simulation::simulation->scene->physicsRenderer->getShadeMode();
  Simulation::simulation->scene->physicsRenderer->setShadeMode(SimRobotCore3::Renderer::ShadeMode::noShading);
  camera->renderer->render();
  Simulation::simulation->scene->physicsRenderer->setShadeMode(currPhysicsShadeMode);

  // read frame buffer
  camera->renderer->getHighDynamicRange()->getFramebuffer()->readPixelData(imageBuffer, ivec2(0,0), ivec2(imageWidth, imageHeight), Gum::Graphics::Pixelformat::RGB);
  data.byteArray = imageBuffer;
  lastSimulationStep = Simulation::simulation->simulationStep;
}

void CameraSensor::drawPhysics() const
{
  pyramid->render();
  ::Sensor::drawPhysics();
}


void CameraSensor::updateTransformation()
{
  calcTransformationMatrix();
  pyramid->getInstance()->setMatrix(getMatrix());
  pyramid->applyTransformationMatrix(pyramid->getInstance());

  ::PhysicalObject::updateTransformation();
}
