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
#include <Graphics/SimLight.h>

Scene::Scene(const std::string& name)
  : ::PhysicalObject(mjOBJ_UNKNOWN, findAvailableName(name, "Scene"))
{
  world = new World3D();
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
  for(SimLight* light : lights)
  {
    if(dynamic_cast<SimPointLight*>(light))
      world->getLightManager()->addPointLight(dynamic_cast<SimPointLight*>(light));
    else if(dynamic_cast<SimSpotLight*>(light))
      world->getLightManager()->addSpotLight(dynamic_cast<SimSpotLight*>(light));
    else if(dynamic_cast<SimDirLight*>(light))
      world->getLightManager()->getSun()->setDirection(*dynamic_cast<SimDirLight*>(light)->getDirection());
  }

  calcTransformationMatrix();

  //world->getObjectManager()->getSkybox()->renderSky(true);
  world->addRenderable(physicsRenderer = new PhysicsRenderer(this));
  world->addRenderable(Simulation::simulation->originRenderer);

  dragPlaneMesh = new SimObject3D(Mesh::generateDisk(0.003f, 0.5f, 30), "dragPlane");
  dragPlaneMesh->getMaterial()->setColor(rgba(128, 128, 128, 128));
  dragPlaneMesh->omitShadow(true);
  world->getObjectManager()->addObject(dragPlaneMesh, Simulation::simulation->forwardRenderingShader, false);

  for(Body* body : bodies)
    body->createGraphics();
  GraphicalObject::createGraphics();
}

void Scene::updateAppearances()
{
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
  if(controllerRenderer)
    return false;
  world->addRenderable(controllerRenderer = new ControllerRenderer(&manager));
  return true;
}
