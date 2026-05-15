#include "OriginRenderer.h"
#include "Simulation/Simulation.h"
#include <Engine/Rendering/Camera.h>
#include <Graphics/Graphics.h>
#include "SimObjectWidget.h"

OriginRenderer::OriginRenderer()
{
  xAxisMesh = new Object3D(Mesh::generateLine(vec3(0,0,0), vec3(1.f, 0.f, 0.f)), "xAxis");
  yAxisMesh = new Object3D(Mesh::generateLine(vec3(0,0,0), vec3(0.f, 1.f, 0.f)), "yAxis");
  zAxisMesh = new Object3D(Mesh::generateLine(vec3(0,0,0), vec3(0.f, 0.f, 1.f)), "zAxis");

  xAxisMesh->addInstance();
  xAxisMesh->getVertexArrayObject()->setPrimitiveType(VertexArrayObject::PrimitiveTypes::LINE_STRIP);
  xAxisMesh->getVertexArrayObject()->setVertexCount(2);

  yAxisMesh->addInstance();
  yAxisMesh->getVertexArrayObject()->setPrimitiveType(VertexArrayObject::PrimitiveTypes::LINE_STRIP);
  yAxisMesh->getVertexArrayObject()->setVertexCount(2);

  zAxisMesh->addInstance();
  zAxisMesh->getVertexArrayObject()->setPrimitiveType(VertexArrayObject::PrimitiveTypes::LINE_STRIP);
  zAxisMesh->getVertexArrayObject()->setVertexCount(2);
}

void OriginRenderer::render()
{
  if(!SimObjectWidget::RENDERING_IN_WIDGET || !enabled)
    return;

  Gum::Graphics::disableFeature(Gum::Graphics::Features::DEPTH_TESTING);
  Simulation::simulation->forwardRenderingShader->use();
  Simulation::simulation->forwardRenderingShader->loadUniform("projectionMatrix", Camera::getActiveCamera()->getProjectionMatrix());
  Simulation::simulation->forwardRenderingShader->loadUniform("viewMatrix", Camera::getActiveCamera()->getViewMatrix());
  Simulation::simulation->forwardRenderingShader->loadUniform("hasTexture", 0);

  Simulation::simulation->forwardRenderingShader->loadUniform("color", rgba(255,0,0,255));
  xAxisMesh->render();
  Simulation::simulation->forwardRenderingShader->loadUniform("color", rgba(0,255,0,255));
  yAxisMesh->render();
  Simulation::simulation->forwardRenderingShader->loadUniform("color", rgba(0,0,255,255));
  zAxisMesh->render();
  Simulation::simulation->forwardRenderingShader->unuse();
  Gum::Graphics::enableFeature(Gum::Graphics::Features::DEPTH_TESTING);
}

void OriginRenderer::enable(bool enabled)
{
  this->enabled = enabled;
}
