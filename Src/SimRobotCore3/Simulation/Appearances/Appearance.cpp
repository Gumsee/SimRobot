/**
 * @file Simulation/Appearances/Appearance.cpp
 * Implementation of class Appearance
 * @author Colin Graf
 */

#include "Appearance.h"
#include "CoreModule.h"
#include "Platform/Assert.h"
#include <cstring>
#include <Engine/3D/Renderer3D.h>
#include <Engine/3D/World3D.h>
#include <Engine/Material/MaterialManager.h>
#include "Simulation/Scene.h"

Appearance::Appearance(const std::string& name)
  : SimObject(findAvailableName(name, "Appearance"))
{}

void Appearance::createGraphics()
{
  setName(this->name);
  pMesh = createMesh();

  calcTransformationMatrix();
  
  if(pMesh != nullptr)
  {
    load();
    Object3DInstance *instance = addInstance();
    instance->setMatrix(worldTransformation.getMatrix());
    applyTransformationMatrix(instance);

    if(!renderForward)
      Simulation::simulation->scene->world->getObjectManager()->addObject(this);
    else
      Simulation::simulation->scene->world->getObjectManager()->addObject(this, Simulation::simulation->forwardRenderingShader, false);
  }

  GraphicalObject::createGraphics();
}

const QIcon* Appearance::getIcon() const
{
  return &CoreModule::module->appearanceIcon;
}

void Appearance::addParent(Element& element)
{
  SimObject::addParent(element);
  GraphicalObject::addParent(element);
}

void Appearance::updateAppearances()
{
  calcTransformationMatrix();
  if(pMesh != nullptr)
  {
    Object3DInstance *instance = getInstance();
    instance->setMatrix(worldTransformation.getMatrix());
    applyTransformationMatrix(instance);
  }
}
