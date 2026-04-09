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
  renderer = new Renderer3D(canvas);
  renderer->setExposure(1.0f);

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
  pyramid = new Object3D(Mesh::generatePyramid(vec2(std::tan(openingAngle.x * 0.5f) * 2.f, std::tan(openingAngle.y * 0.5f) * 2.f), 1.f), "CameraSensor");
  pyramid->getVertexArrayObject()->setPrimitiveType(VertexArrayObject::PrimitiveTypes::LINES);
  Object3DInstance *instance = pyramid->addInstance();
  instance->setMatrix(getMatrix());
  pyramid->applyTransformationMatrix(instance);

  //TODO
  //ASSERT(!surface);
  //static const float color[] = {0.f, 0.f, 0.5f, 1.f};
  //surface = graphicsContext.requestSurface(color, color);
}

void CameraSensor::addParent(Element& element)
{
  //sensor.physicalObject = dynamic_cast< ::PhysicalObject*>(&element);
  //ASSERT(sensor.physicalObject);
  ::Sensor::addParent(element);
}

void CameraSensor::registerObjects(int level)
{
  sensor.fullName = fullName + ".image";
  CoreModule::application->registerObject(*CoreModule::module, sensor, this);

  ::Sensor::registerObjects(level);
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

  //// setup camera position
  camera->camera3d->setPosition(physicalObject->getPosition());
  vec3 eulerrot = fquat::toEuler(physicalObject->getRotation());
  float p = eulerrot.x * GUM_PI_F / 180.0f;
  float y = eulerrot.z * GUM_PI_F / 180.0f;
  vec3 direction(
    cos(y)*cos(p),
    sin(y)*cos(p),
    -sin(p)
  );
  //std::cout << direction.toString() << std::endl;
  camera->camera3d->lookAt(direction);
  camera->camera3d->update();

  //// setup camera position
  //Pose3f pose = sensor->physicalObject->poseInWorld;
  //pose.conc(sensor->offset);
  //static const RotationMatrix cameraRotation = (Matrix3f() << Vector3f(0.f, -1.f, 0.f), Vector3f(0.f, 0.f, 1.f), Vector3f(-1.f, 0.f, 0.f)).finished();
  //pose.rotate(cameraRotation);
  //Matrix4f transformation;
  //OpenGLTools::convertTransformation(pose.invert(), transformation);

  // draw all objects
  Gum::Window::CurrentlyBoundWindow->getContext()->bind();
  camera->camera3d->makeActive();
  SimRobotCore3::Renderer::ShadeMode currPhysicsShadeMode = Simulation::simulation->scene->physicsRenderer->getShadeMode();
  camera->renderer->setWorld(Simulation::simulation->scene->world);
  Simulation::simulation->scene->physicsRenderer->setShadeMode(SimRobotCore3::Renderer::ShadeMode::noShading);
  camera->renderer->render();
  Simulation::simulation->scene->physicsRenderer->setShadeMode(currPhysicsShadeMode);

  // read frame buffer
  camera->renderer->getHighDynamicRange()->getFramebuffer()->readPixelData(imageBuffer, ivec2(0,0), ivec2(imageWidth, imageHeight), Gum::Graphics::Pixelformat::RGB);
  data.byteArray = imageBuffer;
  lastSimulationStep = Simulation::simulation->simulationStep;
}

bool CameraSensor::Sensor::renderCameraImages(SimRobotCore3::SensorPort** cameras, unsigned int count)
{
  return true;
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
