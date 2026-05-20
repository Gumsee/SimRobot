#include "ControllerRenderer.h"
#include "Simulation/Simulation.h"
#include <Graphics/Graphics.h>
#include <Engine/Rendering/Camera.h>
#include "SimObjectWidget.h"

ControllerRenderer::ControllerRenderer(SimRobotCore3::Controller3DDrawingManager* drawingmanager)
  : drawingManager(drawingmanager)
{
  ASSERT(drawingManager);
  GraphicsContext::MainContext->bind();
  drawingManager->registerContext();
}

ControllerRenderer::~ControllerRenderer()
{
  GraphicsContext::MainContext->bind();
  drawingManager->unregisterContext();
}

void ControllerRenderer::render()
{
  if(!SimObjectWidget::RENDERING_IN_WIDGET || shadeMode == SimRobotCore3::Renderer::ShadeMode::noShading)
    return;

  GraphicsContext::MainContext->bind();
  Gum::Graphics::renderWireframe(shadeMode == SimRobotCore3::Renderer::ShadeMode::wireframeShading);

  //glEnable(GL_BLEND);
  //glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
  //glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);

  drawingManager->beforeFrame();

  PhysicalObject* physicalObject = dynamic_cast<PhysicalObject*>(simObject);
  GraphicalObject* graphicalObject = dynamic_cast<GraphicalObject*>(simObject);

  if(physicalObject)
    physicalObject->beforeControllerDrawings(&Camera::getActiveCamera()->getProjectionMatrix()[0][0], &Camera::getActiveCamera()->getViewMatrix()[0][0]);
  if(graphicalObject)
    graphicalObject->beforeControllerDrawings(&Camera::getActiveCamera()->getProjectionMatrix()[0][0], &Camera::getActiveCamera()->getViewMatrix()[0][0]);

  drawingManager->uploadData();

  //if(renderFlags & enableDrawingsTransparentOcclusion)
  //{
  //  drawingManager->beforeDraw();
  //
  //  if(physicalObject)
  //    physicalObject->drawControllerDrawings();
  //  if(graphicalObject)
  //    graphicalObject->drawControllerDrawings();
  //}

  //if((renderFlags & enableDrawingsTransparentOcclusion) || !(renderFlags & enableDrawingsOcclusion))
  //  pContextFramebuffer->clear(Framebuffer::ClearFlags::DEPTH);
//
  //if(renderFlags & enableDrawingsTransparentOcclusion)
  //  glBlendColor(0.5f, 0.5f, 0.5f, 0.5f);

  drawingManager->beforeDraw();

  if(physicalObject)
    physicalObject->drawControllerDrawings();
  if(graphicalObject)
    graphicalObject->drawControllerDrawings();

  if(physicalObject)
    physicalObject->afterControllerDrawings();
  if(graphicalObject)
    graphicalObject->afterControllerDrawings();

  drawingManager->afterFrame();

  Gum::Graphics::renderWireframe(false);
}


void ControllerRenderer::setSimObject(SimObject* simobject)
{
  this->simObject = simobject;
}

void ControllerRenderer::setShadeMode(SimRobotCore3::Renderer::ShadeMode shademode)
{
  this->shadeMode = shademode;
}

SimRobotCore3::Renderer::ShadeMode ControllerRenderer::getShadeMode()
{
  return this->shadeMode;
}
