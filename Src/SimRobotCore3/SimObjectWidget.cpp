/**
 * @file SimObjectWidget.cpp
 * Implementation of class SimObjectWidget
 * @author Colin Graf
 */

#include <System/Output.h>
#include <QActionGroup>
#include <QApplication>
#include <QSettings>
#include <QMenu>
#include <QClipboard>
#include <QFileDialog>
#include <QMainWindow>
#include <QTimer>

#include "SimObjectWidget.h"
#include "CoreModule.h"
#include "Platform/Assert.h"
#include "Simulation/Scene.h"

#include <gum-engine.h>
#include <Engine/PostProcessing/PostProcessing.h>
#include <gum-maths.h>
#include <Graphics/Graphics.h>
#include <Engine/3D/Lightning/ShadowMapping/ShadowMapping.h>
#include <mujoco/mujoco.h>
#include "Platform/System.h"

SimObjectWidget::SimObjectWidget(SimObject& simObject) : QOpenGLWidget(),
  simObject(simObject), object(dynamic_cast<SimRobot::Object&>(simObject)), oMouse(nullptr), oKeyboard(nullptr),
  defaultCameraPos(1.5f, 0.0f, 0.0f)
{
  Gum::Window::CurrentlyBoundWindow->overrideMouseIO(&oMouse);
  Gum::Window::CurrentlyBoundWindow->overrideKeyboardIO(&oKeyboard);

  //setMouseTracking(true);
  setFocusPolicy(Qt::StrongFocus);
  grabGesture(Qt::PinchGesture);

  isSceneWidget = object.getKind() == SimRobotCore3::Kind::scene;

  camera = new Camera3D(ivec2(0,0), nullptr);
  camera->setWorldUpDirection(vec3(0,0,1));
  camera->setMode(Camera3D::Modes::THIRDPERSON_WITH_DRAGGING);
  camera->setOffset(5.0f);
  camera->setZoomSpeed(0.3f);
  camera->makeActive();
  camera->setPositionBoundaries(Simulation::simulation->cameraBoundaries);

  if(isSceneWidget)
  {
    physicsRenderer = Simulation::simulation->scene->physicsRenderer;
  }
  else
  {
    switch(object.getKind())
    {
      case SimRobotCore3::Kind::geometry:
      case SimRobotCore3::Kind::body:
        physicsRenderer = new PhysicsRenderer(static_cast<const PhysicalObject*>(&simObject));
        break;
    }
  }

  // load layout settings
  QSettings* settings = &CoreModule::application->getLayoutSettings();
  settings->beginGroup(object.getFullName());
  appearanceShadeMode = ShadeMode(settings->value("SurfaceShadeMode", int(appearanceShadeMode)).toInt());
  if(Simulation::simulation->scene->controllerRenderer)
    Simulation::simulation->scene->controllerRenderer->setShadeMode(ShadeMode(settings->value("DrawingsShadeMode", int(Simulation::simulation->scene->controllerRenderer->getShadeMode())).toInt()));
  if(physicsRenderer)
    physicsRenderer->setShadeMode(ShadeMode(settings->value("PhysicsShadeMode", int(physicsRenderer->getShadeMode())).toInt()));
  setDragPlane(Tools::StringToVec<float, 3>(settings->value("DragPlane").toString().toStdString(), vec3(0,0,1)));
  setDragMode(DragAndDropMode(settings->value("DragMode", int(dragMode)).toInt()));
  renderFlags = settings->value("RenderFlags", renderFlags).toInt();
  camera->setFOV(settings->value("Fov", 40.0f).toFloat());
  //camera->setPosition(Tools::StringToVec<float, 3>(settings->value("cameraPos").toString().toStdString(), defaultCameraPos) * vec3(1.0f,1.0f,0.0f));
  camera->setPosition(defaultCameraPos);
  camera->lookAt(defaultCameraPos + vec3(1,0,0));
  camera->thirdPersonMotionUpdate();
  settings->endGroup();


  Simulation::simulation->originRenderer->enable(renderFlags & SimRobotCore3::Renderer::showCoordinateSystem);

  oKeyboard.onKeyPress([this](int key, [[maybe_unused]] int mods) {
    if(key == GUM_KEY_PAGE_UP || key == GUM_KEY_PLUS)
    {
      camera->increaseZoom(camera->getZoomSpeed() * -1.0f);
      updateZoomInNextFrame = camera->updateZoom();
    }
    else if(key == GUM_KEY_PAGE_DOWN || key == GUM_KEY_MINUS)
    {
      camera->increaseZoom(camera->getZoomSpeed());
      updateZoomInNextFrame = camera->updateZoom();
    }
  });

  oMouse.onPress([this](int btn, int mods) {
    resizeGL(width(), height());
    if(btn & (GUM_MOUSE_BUTTON_LEFT))
    {
      if(mods & GUM_KEYBOARD_MOD_ALT)
      {
        Object3DInstance* instance = pWorld->getObjectManager()->getInstanceByID(renderer->getIDUnderMouse());
        if(instance != nullptr && instance->getUserPtr() != nullptr)
        {
          SimObject* clickedobj = (SimObject*)instance->getUserPtr();
          SimRobot::Object* castobj = dynamic_cast<SimRobot::Object*>(clickedobj);
          CoreModule::application->activateObject(*castobj);
        }
      }
      else
      {
        clickedBody = selectObject();
        if(clickedBody != nullptr)
        {
          dragRotate = mods & GUM_KEYBOARD_MOD_SHIFT;
          if(!Physics::calcRayPlaneIntersection(camera->getPosition(), camera->calcScreenRayDirection(oMouse.getPosition()), clickedBody->getPosition(), dragPlane, dragStartPos))
          {
            clickedBody = nullptr;
          }
          else
          {
            static_cast<SimRobotCore3::Body*>(clickedBody)->enablePhysics(false);
            if(dragMode == resetDynamics)
              static_cast<SimRobotCore3::Body*>(clickedBody)->resetDynamics();

            if(dragMode == adoptDynamics)
              dragStartTime = System::getTime();

            clickedBodyRing = Simulation::simulation->scene->dragPlaneMesh->addInstance();
            clickedBodyRing->setPosition(clickedBody->getPosition());
          }
        }
        else
        {
          updateCamera = true;
        }
      }
      
      update();
    }
  });


  oMouse.onDouble([]([[maybe_unused]] int btn, [[maybe_unused]] int mod) {
    //if(btn & GUM_MOUSE_BUTTON_LEFT)
    //{
    //  SimRobot::Object* selectedObject = objectRenderer.getDragSelection();
    //  if(selectedObject)
    //    CoreModule::application->selectObject(*selectedObject);
    //}
  });

  oMouse.onMoved([this]([[maybe_unused]]ivec2 pos) {
    if(updateCamera)
    {
      camera->update();
      update();
      paintGL();
    }
    else if(clickedBody != nullptr)
    {
      ASSERT(clickedBody->rootBody == clickedBody);
      if(dragMode == applyDynamics)
        return;
      vec3 currentPos;
      if(Physics::calcRayPlaneIntersection(camera->getPosition(), camera->calcScreenRayDirection(pos), clickedBody->getPosition(), dragPlane, currentPos))
      {
        if(dragRotate)
        {
          vec3 oldV = dragStartPos - clickedBody->getPosition();
          vec3 newV = currentPos - clickedBody->getPosition();

          mat3 invRotation = mat3::transpose(Gum::Maths::rotateMatrix(clickedBody->getRotation()));
          oldV = invRotation * oldV;
          newV = invRotation * newV;

          float angle = 0.0f;
          if     (dragPlane == vec3(1,0,0)) angle = std::atan2(newV.z, newV.y) - std::atan2(oldV.z, oldV.y);
          else if(dragPlane == vec3(0,1,0)) angle = std::atan2(newV.x, newV.z) - std::atan2(oldV.x, oldV.z);
          else                              angle = std::atan2(newV.y, newV.x) - std::atan2(oldV.y, oldV.x);

          const vec3 offset = dragPlane * Gum::Maths::toDegree(angle);
          clickedBody->increaseRotation(offset);
          if(dragMode == adoptDynamics)
          {
            const unsigned int now = System::getTime();
            const float t = std::max(1U, now - dragStartTime) * 0.001f;
            vec3 velocity = offset / t;
            ASSERT(Simulation::simulation->model->body_jntnum[clickedBody->id] == 1);
            const int jointIndex = Simulation::simulation->model->body_jntadr[clickedBody->id];
            ASSERT(Simulation::simulation->model->jnt_type[jointIndex] == mjJNT_FREE);
            const int velocityIndex = Simulation::simulation->model->jnt_dofadr[jointIndex];
            mjtNum* mjcVel = Simulation::simulation->data->qvel + velocityIndex + 3;
            velocity = velocity * 0.3f + vec3(static_cast<float>(mjcVel[0]), static_cast<float>(mjcVel[1]), static_cast<float>(mjcVel[2])) * 0.7f;
            mju_f2n(mjcVel, velocity.data(), 3);
            dragStartTime = now;
          }
          dragStartPos = currentPos;
        }
        else
        {
          const vec3 offset = currentPos - dragStartPos;
          clickedBody->increasePosition(offset * (vec3(1,1,1) - dragPlane));
          if(dragMode == adoptDynamics)
          {
            const unsigned int now = System::getTime();
            const float t = std::max(1U, now - dragStartTime) * 0.001f;
            vec3 velocity = offset / t;
            ASSERT(Simulation::simulation->model->body_jntnum[clickedBody->id] == 1);
            const int jointIndex = Simulation::simulation->model->body_jntadr[clickedBody->id];
            ASSERT(Simulation::simulation->model->jnt_type[jointIndex] == mjJNT_FREE);
            const int velocityIndex = Simulation::simulation->model->jnt_dofadr[jointIndex];
            mjtNum* mjcVel = Simulation::simulation->data->qvel + velocityIndex;
            velocity = velocity * 0.3f + vec3(static_cast<float>(mjcVel[0]), static_cast<float>(mjcVel[1]), static_cast<float>(mjcVel[2])) * 0.7f;
            mju_f2n(mjcVel, velocity.data(), 3);
            dragStartTime = now;
          }
          dragStartPos = currentPos;
          clickedBodyRing->setPosition(clickedBody->getPosition());
        }
      }
    }
  });

  oMouse.onRelease([this]([[maybe_unused]] int btn, [[maybe_unused]] int mod) {
    if(clickedBody != nullptr)
      static_cast<SimRobotCore3::Body*>(clickedBody)->enablePhysics(true);
    updateCamera = false;
    clickedBody = nullptr;
    clickedBodyRing = nullptr;
    Simulation::simulation->scene->dragPlaneMesh->clearInstances();
  });

  std::function<void (vec2)> scrollFunc = [this]([[maybe_unused]] vec2 dir) {
    updateZoomInNextFrame = camera->updateZoom();
    oMouse.resetDelta();
    camera->thirdPersonMotionUpdate();
    update();
    paintGL();
  };

  oMouse.onScroll(scrollFunc);
  QMetaObject::Connection connection = connect(this, &QOpenGLWidget::frameSwapped, this, [scrollFunc, this](){
    if(updateZoomInNextFrame)
      scrollFunc(vec2());
  });


  if(isSceneWidget)
  {
    connect(this, &QOpenGLWidget::frameSwapped, this, [](){
      Time::update();
      Texture::updateBackgroundLoading();
    });
  }
}

SimObjectWidget::~SimObjectWidget()
{  
  // save layout settings
  QSettings* settings = &CoreModule::application->getLayoutSettings();
  settings->beginGroup(object.getFullName());
  settings->setValue("SurfaceShadeMode", int(appearanceShadeMode));
  settings->setValue("PhysicsShadeMode", int(physicsRenderer->getShadeMode()));
  if(Simulation::simulation->scene->controllerRenderer)
    settings->setValue("DrawingsShadeMode", int(Simulation::simulation->scene->controllerRenderer->getShadeMode()));
  settings->setValue("DragPlane", dragPlane.toString("","",", ").c_str());
  settings->setValue("DragMode", int(dragMode));
  settings->setValue("RenderFlags", renderFlags);
  settings->setValue("Fov", camera->getFOV());
  settings->setValue("cameraPos", QString(((Transformable3D*)camera)->getPosition().toString("", "", ", ").c_str()));
  settings->endGroup();

  if(!isSceneWidget)
    Gum::_delete(pWorld);
  Gum::_delete(renderCanvas);
  Gum::_delete(camera);
  Gum::_delete(renderer);
}

void recursivelyAddObjects(World3D* world, const SimRobot::Object* object)
{
  if(object == nullptr || object->getKind() != SimRobotCore3::Kind::appearance)
    return;
    
  if(((Appearance*)object)->getMesh() != nullptr)
    world->getObjectManager()->addObject(((Appearance*)object));
  
  for(SimObject* obj : ((Appearance*)object)->children)
    recursivelyAddObjects(world, dynamic_cast<Appearance*>(obj));
}

void SimObjectWidget::initializeGL()
{ 
  GraphicsContext::MainContext->bind();

  renderCanvas = new Canvas(ivec2(width(), height()));

  Framebuffer::DefaultFramebufferID = this->defaultFramebufferObject();
  pContextFramebuffer = new Framebuffer(renderCanvas->getSize(), true, Framebuffer::DefaultFramebufferID);
  pContextFramebuffer->setClearColor(Simulation::simulation->scene->backgroundcolor);

  if(isSceneWidget)
  {
    pWorld = Simulation::simulation->scene->world;
    physicsRenderer = Simulation::simulation->scene->physicsRenderer;
  }
  else
  {
    pWorld = new World3D(Simulation::simulation->scene->world->getObjectManager()->getSkybox());

    if(physicsRenderer)
      pWorld->addRenderable(physicsRenderer);

    switch(object.getKind())
    {
      case SimRobotCore3::Kind::body:
      case SimRobotCore3::Kind::appearance:
        pWorld->getObjectManager()->selfManageObjects(true);
        recursivelyAddObjects(pWorld, &object);
        camera->setPosition(simObject.getPosition());
        camera->thirdPersonMotionUpdate();
        break;

      default:
        Gum::Output::error("Unknown object type");
        break;
    }
  }
  

  Settings::setSetting(Settings::SHADOW_SIZE, 6000);
  renderer = new Renderer3D(renderCanvas);
  if(pWorld != nullptr)
    renderer->setWorld(pWorld);
  renderer->setExposure(1.0f);
  renderer->renderSky(isSceneWidget);

  pShader = ShaderProgram::requestShaderProgram("CanvasShader", true);
  pShader->addShader(Gum::PostProcessing::VertexShader);
  pShader->addShader(Gum::PostProcessing::FragmentShader);
  pShader->build();
}

void SimObjectWidget::paintGL()
{
  if(appearanceShadeMode == SimRobotCore3::Renderer::ShadeMode::noShading)
    return;

  RENDERING_IN_WIDGET = true;

  GraphicsContext::MainContext->bind();
  bindFramebuffer();


  if(Simulation::simulation->scene->controllerRenderer)
    Simulation::simulation->scene->controllerRenderer->setSimObject(&simObject);

  if(updateZoomInNextFrame)
    updateZoomInNextFrame = camera->updateZoom();
  
  pContextFramebuffer->clear(Framebuffer::ClearFlags::COLOR | Framebuffer::ClearFlags::DEPTH);
  switch(appearanceShadeMode)
  {
    case SimRobotCore3::Renderer::ShadeMode::wireframeShading:
    case SimRobotCore3::Renderer::ShadeMode::flatShading:
      Gum::Graphics::renderWireframe(appearanceShadeMode == SimRobotCore3::Renderer::ShadeMode::wireframeShading);
      Simulation::simulation->forwardRenderingShader->use();
      Simulation::simulation->forwardRenderingShader->loadUniform("projectionMatrix", Camera::getActiveCamera()->getProjectionMatrix());
      Simulation::simulation->forwardRenderingShader->loadUniform("canvassize", renderCanvas->getSize());
      pWorld->getObjectManager()->renderEverything();
      Simulation::simulation->forwardRenderingShader->unuse();
      Gum::Graphics::renderWireframe(false);
      break;
  
    default:
      renderer->render();

      pContextFramebuffer->bind();
      pShader->use();
      renderCanvas->getTexture()->bind(0);
      //renderer->getIDRenderer()->getResultTexture()->bind(0);
      renderCanvas->render();
      renderCanvas->getTexture()->unbind(0);
      pShader->unuse();
      break;
  }

  renderer->renderIDs();

  RENDERING_IN_WIDGET = false;
}

void SimObjectWidget::resizeGL(int width, int height)
{
  bindFramebuffer();
  
  renderCanvas->setSize(ivec2(width*this->devicePixelRatio(), height*this->devicePixelRatio()));

  pContextFramebuffer->setSize(renderCanvas->getSize());
  pContextFramebuffer->resetViewport();

  renderer->updateFramebufferSize();
  camera->updateProjection(renderCanvas->getSize());
}

void SimObjectWidget::update()
{
  QOpenGLWidget::update();
  renderer->update();
  oMouse.reset();
  oKeyboard.reset();
}

void SimObjectWidget::bindFramebuffer()
{
  Framebuffer::WindowFramebuffer = pContextFramebuffer;
  Framebuffer::DefaultFramebufferID = this->defaultFramebufferObject();
  pContextFramebuffer->overrideID(Framebuffer::DefaultFramebufferID);
  pContextFramebuffer->bind();
  renderer->makeActive();
  camera->makeActive();
  Gum::Window::CurrentlyBoundWindow->overrideMouseIO(&oMouse);
  Gum::Window::CurrentlyBoundWindow->overrideKeyboardIO(&oKeyboard);
}

QMenu* SimObjectWidget::createEditMenu() const
{
  QMenu* menu = new QMenu(tr("&Edit"));
  QIcon icon(":/Icons/icons8-copy-to-clipboard-50.png");
  icon.setIsMask(true);
  QAction* action = menu->addAction(icon, tr("&Copy"));
  action->setShortcut(QKeySequence(QKeySequence::Copy));
  action->setStatusTip(tr("Copy the rendered object to the clipboard"));
  connect(action, &QAction::triggered, this, &SimObjectWidget::copy);

  return menu;
}

QMenu* SimObjectWidget::createUserMenu() const
{
  QMenu* menu = new QMenu(tr(&object == Simulation::simulation->scene ? "&Scene" : "&Object")); // cspell:disable-line
  {
    QMenu* subMenu = menu->addMenu(tr("&Drag and Drop"));
    QAction* action = subMenu->menuAction();
    QIcon icon(":/Icons/icons8-drag-and-drop-50.png");
    icon.setIsMask(true);
    action->setIcon(icon);
    action->setStatusTip(tr("Select the drag and drop dynamics mode and plane along which operations are performed"));
    QActionGroup* actionGroup = new QActionGroup(subMenu);
    auto addPlaneAction = [this, subMenu, actionGroup](const char* label, Qt::Key key, vec3 plane)
    {
      auto* action = subMenu->addAction(tr(label));
      actionGroup->addAction(action);
      action->setShortcut(QKeySequence(key));
      action->setCheckable(true);
      action->setChecked(dragPlane == plane);
      connect(action, &QAction::triggered, this, [this, plane]{ const_cast<SimObjectWidget*>(this)->setDragPlane(plane); });
    };
    addPlaneAction("X/Y Plane", Qt::Key_Z, vec3(0,0,1));
    addPlaneAction("X/Z Plane", Qt::Key_Y, vec3(0,1,0));
    addPlaneAction("Y/Z Plane", Qt::Key_X, vec3(1,0,0));
    subMenu->addSeparator();
    actionGroup = new QActionGroup(subMenu);
    auto addModeAction = [this, subMenu, actionGroup](const char* label, Qt::Key key, SimRobotCore3::Renderer::DragAndDropMode mode)
    {
      auto* action = subMenu->addAction(tr(label));
      actionGroup->addAction(action);
      action->setShortcut(QKeySequence(key));
      action->setCheckable(true);
      action->setChecked(dragMode == mode);
      connect(action, &QAction::triggered, this, [this, mode]{ const_cast<SimObjectWidget*>(this)->setDragMode(mode); });
    };
    addModeAction("&Keep Dynamics", Qt::Key_7, SimRobotCore3::Renderer::keepDynamics);
    addModeAction("&Reset Dynamics", Qt::Key_8, SimRobotCore3::Renderer::resetDynamics);
    addModeAction("A&dopt Dynamics", Qt::Key_9, SimRobotCore3::Renderer::adoptDynamics);
    addModeAction("&Apply Dynamics", Qt::Key_0, SimRobotCore3::Renderer::applyDynamics);
  }

  menu->addSeparator();

  {
    QAction* action = menu->addAction(tr("&Reset Camera"));
    QIcon icon(":/Icons/icons8-camera-50.png");
    icon.setIsMask(true);
    action->setIcon(icon);
    action->setShortcut(QKeySequence(Qt::Key_R));
    connect(action, &QAction::triggered, this, &SimObjectWidget::resetCamera);
  }

  {
    QMenu* subMenu = menu->addMenu(tr("&Vertical Opening Angle"));
    QAction* action = subMenu->menuAction();
    QIcon icon(":/Icons/icons8-focal-length-50.png");
    icon.setIsMask(true);
    action->setIcon(icon);
    QActionGroup* actionGroup = new QActionGroup(subMenu);
    auto addFovYAction = [this, subMenu, actionGroup](const char* label, Qt::Key key, float fov)
    {
      auto* action = subMenu->addAction(tr(label));
      actionGroup->addAction(action);
      action->setShortcut(QKeySequence(key));
      action->setCheckable(true);
      action->setChecked(this->camera->getFOV() == fov);
      connect(action, &QAction::triggered, this, [this, fov]{ const_cast<SimObjectWidget*>(this)->camera->setFOV(fov); });
    };
    addFovYAction("&20°", Qt::Key_1, 20.0f);
    addFovYAction("&40°", Qt::Key_2, 40.0f);
    addFovYAction("&60°", Qt::Key_3, 60.0f);
    addFovYAction("&80°", Qt::Key_4, 80.0f);
    addFovYAction("100°", Qt::Key_5, 100.0f);
    addFovYAction("120°", Qt::Key_6, 120.0f);
  }

  menu->addSeparator();

  {
    QMenu* subMenu = menu->addMenu(tr("&Appearances Rendering"));
    QActionGroup* actionGroup = new QActionGroup(subMenu);
    QAction* action = subMenu->menuAction();
    QIcon icon(":/Icons/icons8-layers-50.png");
    icon.setIsMask(true);
    action->setIcon(icon);
    action->setStatusTip(tr("Select different shading techniques for displaying the scene"));
    auto addShadingAction = [this, subMenu, actionGroup](const char* label, Qt::Key key, SimRobotCore3::Renderer::ShadeMode shading)
    {
      auto* action = subMenu->addAction(tr(label));
      actionGroup->addAction(action);
      if(key)
        action->setShortcut(QKeySequence(static_cast<int>(Qt::CTRL) + static_cast<int>(key)));
      action->setCheckable(true);
      action->setChecked(appearanceShadeMode == shading);
      connect(action, &QAction::triggered, this, [this, shading]{ const_cast<SimObjectWidget*>(this)->appearanceShadeMode = shading; });
    };
    addShadingAction("&Off", Qt::Key(0), SimRobotCore3::Renderer::noShading);
    addShadingAction("&Wire Frame", Qt::Key_W, SimRobotCore3::Renderer::wireframeShading);
    addShadingAction("&Deferred Shading", Qt::Key_M, SimRobotCore3::Renderer::smoothShading);
    addShadingAction("&Simple Shading", Qt::Key_F, SimRobotCore3::Renderer::flatShading);
  }

  if(physicsRenderer != nullptr)
  {
    QMenu* subMenu = menu->addMenu(tr("&Physics Rendering"));
    QActionGroup* actionGroup = new QActionGroup(subMenu);
    QAction* action = subMenu->menuAction();
    QIcon icon(":/Icons/icons8-orthogonal-view-50.png");
    icon.setIsMask(true);
    action->setIcon(icon);
    action->setStatusTip(tr("Select different shading techniques for displaying the physical representation of objects"));
    auto addShadingAction = [this, subMenu, actionGroup](const char* label, SimRobotCore3::Renderer::ShadeMode shading)
    {
      auto* action = subMenu->addAction(tr(label));
      actionGroup->addAction(action);
      action->setCheckable(true);
      action->setChecked(physicsRenderer->getShadeMode() == shading);
      connect(action, &QAction::triggered, this, [this, shading]{ physicsRenderer->setShadeMode(shading); });
    };
    addShadingAction("&Off", SimRobotCore3::Renderer::noShading);
    addShadingAction("&Wire Frame", SimRobotCore3::Renderer::wireframeShading);
    addShadingAction("&Smooth Shading", SimRobotCore3::Renderer::smoothShading);
  }

  {
    QMenu* subMenu = menu->addMenu(tr("&Controller drawings Rendering"));
    QActionGroup* actionGroup = new QActionGroup(subMenu);
    QAction* action = subMenu->menuAction();
    QIcon icon(":/Icons/icons8-line-chart-50.png");
    icon.setIsMask(true);
    action->setIcon(icon);
    action->setStatusTip(tr("Select different shading techniques for displaying controller drawings"));
    auto addShadingAction = [this, subMenu, actionGroup](const char* label, SimRobotCore3::Renderer::ShadeMode shading)
    {
      auto* action = subMenu->addAction(tr(label));
      actionGroup->addAction(action);
      action->setCheckable(true);
      action->setChecked(Simulation::simulation->scene->controllerRenderer->getShadeMode() == shading);
      connect(action, &QAction::triggered, this, [shading]{ Simulation::simulation->scene->controllerRenderer->setShadeMode(shading); });
    };
    addShadingAction("&Off", SimRobotCore3::Renderer::noShading);
    addShadingAction("&Wire Frame", SimRobotCore3::Renderer::wireframeShading);
    addShadingAction("&Filled", SimRobotCore3::Renderer::flatShading);

    subMenu->addSeparator();

    subMenu = subMenu->addMenu(tr("&Occlusion"));
    actionGroup = new QActionGroup(subMenu);
    action = subMenu->menuAction();
    action->setStatusTip(tr("Select different drawings occlusion modes"));

    auto addOcclusionAction = [this, subMenu, actionGroup](const char* label, SimRobotCore3::Renderer::RenderFlags flag)
    {
      auto* action = subMenu->addAction(tr(label));
      actionGroup->addAction(action);
      action->setCheckable(true);
      action->setChecked((renderFlags & (SimRobotCore3::Renderer::enableDrawingsOcclusion | SimRobotCore3::Renderer::enableDrawingsTransparentOcclusion)) == flag);
      connect(action, &QAction::triggered, this, [this, flag]{ const_cast<SimObjectWidget*>(this)->setDrawingsOcclusion(flag); });
    };

    addOcclusionAction("&On", SimRobotCore3::Renderer::enableDrawingsOcclusion);
    addOcclusionAction("&Off", SimRobotCore3::Renderer::RenderFlags(0));
    addOcclusionAction("&Transparent", SimRobotCore3::Renderer::enableDrawingsTransparentOcclusion);
  }

  menu->addSeparator();

  auto addRenderFlagAction = [this, menu](const char* label, const char* tip, SimRobotCore3::Renderer::RenderFlags flag, const char* icon = nullptr)
  {

    QAction* action;
    if(icon)
    {
      QIcon qIcon(icon);
      qIcon.setIsMask(true);
      action = menu->addAction(qIcon, tr(label));
    }
    else
      action = menu->addAction(tr(label));
    action->setStatusTip(tr(tip));
    action->setCheckable(true);
    action->setChecked(renderFlags & flag);
    connect(action, &QAction::triggered, this, [this, flag]{ 
      const_cast<SimObjectWidget*>(this)->toggleRenderFlag(flag); 

      Simulation::simulation->originRenderer->enable(renderFlags & SimRobotCore3::Renderer::showCoordinateSystem);
    });
  };

  addRenderFlagAction("Enable &Lights", "Enable lighting", SimRobotCore3::Renderer::enableLights);
  addRenderFlagAction("Enable &Textures", "Enable textures", SimRobotCore3::Renderer::enableTextures);
  addRenderFlagAction("Enable &Multisample", "Enable multisampling", SimRobotCore3::Renderer::enableMultisample);

  menu->addSeparator();

  addRenderFlagAction("Show &Coordinate System", "Show the coordinate system of the displayed object", SimRobotCore3::Renderer::showCoordinateSystem, ":/Icons/icons8-coordinate-system-50.png");
  addRenderFlagAction("Show &Sensors", "Show the values of the sensors in the scene view", SimRobotCore3::Renderer::showSensors, ":/Icons/icons8-speed-50.png");

  menu->addSeparator();

  {
    QMenu* subMenu = menu->addMenu(tr("Export as Image..."));
    auto* action = subMenu->addAction(tr("3840x2160"));
    connect(action, &QAction::triggered, this, [this]{ const_cast<SimObjectWidget*>(this)->exportAsImage(3840, 2160); });
    action = subMenu->addAction(tr("2880x1620"));
    connect(action, &QAction::triggered, this, [this]{ const_cast<SimObjectWidget*>(this)->exportAsImage(2880, 1620); });
    action = subMenu->addAction(tr("1920x1080"));
    connect(action, &QAction::triggered, this, [this]{ const_cast<SimObjectWidget*>(this)->exportAsImage(1920, 1080); });
    action = subMenu->addAction(tr("1280x1024"));
    connect(action, &QAction::triggered, this, [this]{ const_cast<SimObjectWidget*>(this)->exportAsImage(1280, 1024); });
  }

  return menu;
}

void SimObjectWidget::copy()
{
  QApplication::clipboard()->setImage(grabFramebuffer());
}

void SimObjectWidget::exportAsImage(int width, int height)
{
  QSettings& settings = CoreModule::application->getSettings();
  QString fileName = QFileDialog::getSaveFileName(
    this, tr("Export as Image"), settings.value("ExportDirectory", "").toString(), tr("Portable Network Graphic (*.png)")
#if defined LINUX && defined QT_VERSION && QT_VERSION < QT_VERSION_CHECK(6, 6, 0)
    , nullptr, QFileDialog::DontUseNativeDialog
#endif
  );
  if(fileName.isEmpty())
    return;
  settings.setValue("ExportDirectory", QFileInfo(fileName).dir().path());

  // allocate buffer
  const unsigned int imageSize = width * height * 3;
  unsigned char* imageBuffer = new unsigned char[imageSize];

  ivec2 oldSize = renderCanvas->getSize();
  resizeGL(width, height);
  bindFramebuffer();

  renderer->render();
  renderer->getHighDynamicRange()->getFramebuffer()->readPixelData(imageBuffer, ivec2(0,0), ivec2(width, height), Gum::Graphics::Pixelformat::RGB);

  QImage image(&imageBuffer[0], width, height, QImage::Format_RGB888);
  //image.mirror();
  if(!image.save(fileName))
    Gum::Output::error("Failed to save image to file " + fileName.toStdString());

  resizeGL(oldSize.x, oldSize.y);
  bindFramebuffer();

  Gum::_delete(imageBuffer);
}

void SimObjectWidget::setDrawingsOcclusion(int flag)
{
  renderFlags &= ~(SimRobotCore3::Renderer::enableDrawingsOcclusion | SimRobotCore3::Renderer::enableDrawingsTransparentOcclusion);
  renderFlags |= flag;
}

void SimObjectWidget::setDragPlane(vec3 plane)
{
  dragPlane = plane;
}

void SimObjectWidget::setDragMode(DragAndDropMode mode)
{
  dragMode = mode;
}

void SimObjectWidget::resetCamera()
{
  camera->setPosition(defaultCameraPos);
}

void SimObjectWidget::toggleRenderFlag(int flag)
{
  if(renderFlags & flag) renderFlags &= ~flag;
  else                   renderFlags |= flag;
}

Body* SimObjectWidget::selectObject()
{
  if(!isSceneWidget)
    return nullptr;

  Object3DInstance* instance = pWorld->getObjectManager()->getInstanceByID(renderer->getIDUnderMouse());
  if(instance != nullptr && instance->getUserPtr() != nullptr)
  {
    SimObject* clickedobj = (SimObject*)instance->getUserPtr();
    Body* castbody = dynamic_cast<Body*>(clickedobj);
    if(castbody)
      return castbody->rootBody;
  }

  return nullptr;
}
