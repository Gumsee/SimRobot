#include "PhysicsRenderer.h"
#include "Simulation/Simulation.h"
#include <Engine/Rendering/Camera.h>
#include <Graphics/Graphics.h>

PhysicsRenderer::PhysicsRenderer(const PhysicalObject* physicalobject)
  : physicalObject(physicalobject)
{

}

void PhysicsRenderer::render()
{
  if(shademode == SimRobotCore3::Renderer::ShadeMode::noShading)
    return;
    
  Gum::Graphics::renderWireframe(shademode == SimRobotCore3::Renderer::ShadeMode::wireframeShading);
  Simulation::simulation->forwardRenderingShader->use();
  Simulation::simulation->forwardRenderingShader->loadUniform("projectionMatrix", Camera::getActiveCamera()->getProjectionMatrix());
  Simulation::simulation->forwardRenderingShader->loadUniform("viewMatrix", Camera::getActiveCamera()->getViewMatrix());
  Simulation::simulation->forwardRenderingShader->loadUniform("color", rgba(255,0,0,255));
  Simulation::simulation->forwardRenderingShader->loadUniform("hasTexture", 0);
  
  physicalObject->drawPhysics();
  Simulation::simulation->forwardRenderingShader->unuse();
  Gum::Graphics::renderWireframe(false);
}

void PhysicsRenderer::setShadeMode(SimRobotCore3::Renderer::ShadeMode shademode)
{
  this->shademode = shademode;
}

SimRobotCore3::Renderer::ShadeMode PhysicsRenderer::getShadeMode()
{
  return this->shademode;
}
