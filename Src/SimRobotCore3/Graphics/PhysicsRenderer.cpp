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
    Gum::Graphics::renderWireframe(true);
    Simulation::simulation->forwardRenderingShader->use();
    Simulation::simulation->forwardRenderingShader->loadUniform("projectionMatrix", Camera::getActiveCamera()->getProjectionMatrix());
    Simulation::simulation->forwardRenderingShader->loadUniform("viewMatrix", Camera::getActiveCamera()->getViewMatrix());
    Simulation::simulation->forwardRenderingShader->loadUniform("color", color(255,0,0,255).getGLColor());
    Simulation::simulation->forwardRenderingShader->loadUniform("hasTexture", 0);
    
    physicalObject->drawPhysics();
    Simulation::simulation->forwardRenderingShader->unuse();
    Gum::Graphics::renderWireframe(false);
}