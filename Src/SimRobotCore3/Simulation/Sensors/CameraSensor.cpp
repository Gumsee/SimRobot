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

CameraSensor::CameraSensor(const std::string& name)
  : ::Sensor(findAvailableName(name, "CameraSensor"))
{
  sensor.camera = this;
  sensor.sensorType = SimRobotCore3::SensorPort::cameraSensor;
  sensor.imageBuffer = nullptr;
  sensor.imageBufferSize = 0;

  canvas = new Canvas(ivec2(imageWidth, imageHeight));
  renderer = new Renderer3D(canvas);
  renderer->setExposure(1.0f);

  camera3d = new Camera3D(ivec2(imageWidth, imageHeight), nullptr);
  camera3d->setWorldUpDirection(vec3(0,0,1));
  camera3d->setPosition(vec3(0,0,0));
}

CameraSensor::~CameraSensor()
{
  if(sensor.imageBuffer)
    delete[] sensor.imageBuffer;
}

void CameraSensor::createPhysicsInternal()
{
  sensor.dimensions.append(imageWidth);
  sensor.dimensions.append(imageHeight);
  sensor.dimensions.append(3);

  sensor.offset = relativeTransformation;

  float aspect = std::tan(angleX * 0.5f) / std::tan(angleY * 0.5f);
  OpenGLTools::computePerspective(angleY, aspect, 0.01f, 500.f, sensor.projection);

  ASSERT(!pyramid);
  pyramid = new Object3D(Mesh::generatePyramid(vec2(std::tan(angleX * 0.5f) * 2.f, std::tan(angleY * 0.5f) * 2.f), 1.f), "CameraSensor");

  //TODO
  //ASSERT(!surface);
  //static const float color[] = {0.f, 0.f, 0.5f, 1.f};
  //surface = graphicsContext.requestSurface(color, color);
}

void CameraSensor::addParent(Element& element)
{
  sensor.physicalObject = dynamic_cast< ::PhysicalObject*>(&element);
  ASSERT(sensor.physicalObject);
  ::Sensor::addParent(element);
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
  const unsigned int imageWidth = camera->imageWidth;
  const unsigned int imageHeight = camera->imageHeight;
  const unsigned int imageSize = imageWidth * imageHeight * 3;
  if(imageBufferSize < imageSize)
  {
    if(imageBuffer)
      delete[] imageBuffer;
    imageBuffer = new unsigned char[imageSize];
    imageBufferSize = imageSize;
  }

  //// setup camera position
  camera->camera3d->setMatrix(physicalObject->worldTransformation.getMatrix());

  //graphicsContext.startColorRendering(projection, transformation, 0, 0, imageWidth, imageHeight, true);


  // draw all objects
  Gum::Window::CurrentlyBoundWindow->getContext()->bind();
  camera->camera3d->makeActive();
  camera->renderer->setWorld(Simulation::simulation->scene->world);
  camera->renderer->render();

  std::cout << "Rendering camera sensor" << std::endl;

  // read frame buffer
  camera->renderer->getHighDynamicRange()->getFramebuffer()->readPixelData(imageBuffer, ivec2(0,0), ivec2(imageWidth, imageHeight), Gum::Graphics::Pixelformat::RGB);
  data.byteArray = imageBuffer;
}

bool CameraSensor::Sensor::renderCameraImages(SimRobotCore3::SensorPort** cameras, unsigned int count)
{
  if(lastSimulationStep == Simulation::simulation->simulationStep)
    return true;

  // allocate buffer
  /*const unsigned int imageWidth = camera->imageWidth;
  const unsigned int imageHeight = camera->imageHeight;
  const unsigned int imageSize = imageWidth * imageHeight * 3;
  int imagesOfCurrentSize = 0;
  for(unsigned int i = 0; i < count; ++i)
  {
    CameraSensor::Sensor* sensor = static_cast<CameraSensor::Sensor*>(cameras[i]);
    if(sensor && sensor->lastSimulationStep != Simulation::simulation->simulationStep &&
       sensor->camera->imageWidth == imageWidth && sensor->camera->imageHeight == imageHeight)
      ++imagesOfCurrentSize;
  }
  const unsigned int multiImageBufferSize = imageSize * imagesOfCurrentSize;

  if(imageBufferSize < multiImageBufferSize)
  {
    if(imageBuffer)
      delete[] imageBuffer;
    imageBuffer = new unsigned char[multiImageBufferSize];
    imageBufferSize = multiImageBufferSize;
  }

  bGraphicsContext& graphicsContext = Simulation::simulation->graphicsContext;
  graphicsContext.makeCurrent(imageWidth, imageHeight * count);
  graphicsContext.updateModelMatrices(bGraphicsContext::ModelMatrix::appearance, false);

  // render images
  int currentHorizontalPos = 0;
  unsigned char* currentBufferPos = imageBuffer;
  for(unsigned int i = 0; i < count; ++i)
  {
    CameraSensor::Sensor* sensor = static_cast<CameraSensor::Sensor*>(cameras[i]);
    if(sensor && sensor->lastSimulationStep != Simulation::simulation->simulationStep &&
       sensor->camera->imageWidth == imageWidth && sensor->camera->imageHeight == imageHeight)
    {
      // setup camera position
      Pose3f pose = sensor->physicalObject->poseInWorld;
      pose.conc(sensor->offset);
      static const RotationMatrix cameraRotation = (Matrix3f() << Vector3f(0.f, -1.f, 0.f), Vector3f(0.f, 0.f, 1.f), Vector3f(-1.f, 0.f, 0.f)).finished();
      pose.rotate(cameraRotation);
      Matrix4f transformation;
      OpenGLTools::convertTransformation(pose.invert(), transformation);

      graphicsContext.startColorRendering(sensor->projection, transformation, 0, currentHorizontalPos, imageWidth, imageHeight, !currentHorizontalPos);

      // draw all objects
      //TODO
      //Simulation::simulation->scene->drawAppearances(graphicsContext);

      graphicsContext.finishRendering();

      sensor->data.byteArray = currentBufferPos;
      sensor->lastSimulationStep = Simulation::simulation->simulationStep;

      currentHorizontalPos += imageHeight;
      currentBufferPos += imageSize;
    }
  }

  // read frame buffer
  graphicsContext.finishImageRendering(imageBuffer, imageWidth, currentHorizontalPos);*/
  return true;
}

void CameraSensor::drawPhysics() const
{
  pyramid->render();
  ::Sensor::drawPhysics();
}
