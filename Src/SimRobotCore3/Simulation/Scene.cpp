/**
 * @file Simulation/Scene.h
 * Implementation of class Scene
 * @author Colin Graf
 */

#include "Scene.h"
#include "CoreModule.h"
#include "Platform/Assert.h"
#include "Simulation/Actuators/Actuator.h"
#include "Simulation/Body.h"
#include "Simulation/Simulation.h"
#include <iostream>
#include <Engine/3D/Renderer3D.h>
#include "Graphics/PhysicsRenderer.h"

Scene::Scene(const std::string& name)
  : ::PhysicalObject(mjOBJ_UNKNOWN, findAvailableName(name, "Scene"))
{
}

void Scene::updateTransformations()
{
  for(Body* body : bodies)
    body->updateTransformation();
}

void Scene::updateActuators()
{
  for(Actuator::Port* actuator : actuators)
    actuator->act();
}

void Scene::createGraphics()
{
  // The model matrix is needed for controller drawings.
  // Physical object and graphical object share it because it really is just at the origin.
  //TODO
  //const float color[4] = {0.2f, 0.2f, 0.2f, 1.f};
  //graphicsContext.setGlobalAmbientLight(color);
  //for(Light* light : lights)
  //  graphicsContext.addLight(light);
//
  calcTransformationMatrix();

  world = new World3D();
  world->getObjectManager()->getSkybox()->renderSky(true);
  world->addRenderable(new PhysicsRenderer(this));

  for(Body* body : bodies)
    body->createGraphics();
  GraphicalObject::createGraphics();
}

void Scene::updateAppearances()
{
  //std::cout << "Drawing scene with " << bodies.size() << " children" << std::endl;
  for(Body* body : bodies)
    body->updateAppearances();
  GraphicalObject::updateAppearances();
}

void Scene::drawPhysics() const
{
  for(const Body* body : bodies)
    body->drawPhysics();
  ::PhysicalObject::drawPhysics();
}

void Scene::visitGraphicalControllerDrawings(const std::function<void(GraphicalObject&)>& accept)
{
  for(Body* body : bodies)
    accept(*body);
  GraphicalObject::visitGraphicalControllerDrawings(accept);
}

void Scene::visitPhysicalControllerDrawings(const std::function<void(::PhysicalObject&)>& accept)
{
  for(Body* body : bodies)
    accept(*body);
  ::PhysicalObject::visitPhysicalControllerDrawings(accept);
}

const QIcon* Scene::getIcon() const
{
  return &CoreModule::module->sceneIcon;
}

unsigned int Scene::getStep() const
{
  return Simulation::simulation->simulationStep;
}

double Scene::getTime() const
{
  return Simulation::simulation->simulatedTime;
}

unsigned int Scene::getFrameRate() const
{
  return Simulation::simulation->currentFrameRate;
}

bool Scene::registerDrawingManager(SimRobotCore3::Controller3DDrawingManager& manager)
{
  if(drawingManager)
    return false;
  drawingManager = &manager;
  return true;
}
