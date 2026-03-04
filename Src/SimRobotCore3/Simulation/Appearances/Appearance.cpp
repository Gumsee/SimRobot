/**
 * @file Simulation/Appearances/Appearance.cpp
 * Implementation of class Appearance
 * @author Colin Graf
 */

#include "Appearance.h"
#include "CoreModule.h"
#include "Platform/Assert.h"
#include "Tools/OpenGLTools.h"
#include <cstring>
#include <Engine/3D/Renderer3D.h>
#include <Engine/3D/World3D.h>
#include <Engine/Material/MaterialManager.h>
#include "Simulation/Scene.h"

void Appearance::createGraphics(GraphicsContext& graphicsContext)
{
  setName(this->name);
  pMesh = createMesh();
  if(pMesh != nullptr)
  {
    load();
    Object3DInstance *instance = addInstance();
    instance->setMatrix(getTransformationMatrix());
    applyTransformationMatrix(instance);

    if(!renderForward)
      Simulation::simulation->scene->world->getObjectManager()->addObject(this);
    else
      Simulation::simulation->scene->world->getObjectManager()->addObject(this, Simulation::simulation->forwardRenderingShader, false);
  }

  GraphicalObject::createGraphics(graphicsContext);
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
