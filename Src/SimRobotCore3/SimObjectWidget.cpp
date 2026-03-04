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

//#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <gum-engine.h>
#include <Engine/PostProcessing/PostProcessing.h>
#include <Engine/Material/MaterialManager.h>
#include <gum-maths.h>
#include <GL/glew.h>
#include <QOpenGLContext>
#include <QSurface>

SimObjectWidget::SimObjectWidget(SimObject& simObject) : QOpenGLWidget(),
  object(dynamic_cast<SimRobot::Object&>(simObject)), objectRenderer(simObject), oMouse(nullptr),
  wKey(false), aKey(false), sKey(false), dKey(false)
{
  pGLContext = new GraphicsContext(this->context(), Gum::Window::CurrentlyBoundWindow->getContext()->getNativeHandle(), nullptr, Gum::DefaultContextConfig);

  setFocusPolicy(Qt::StrongFocus);
  grabGesture(Qt::PinchGesture);

  // load layout settings
  QSettings* settings = &CoreModule::application->getLayoutSettings();
  settings->beginGroup(object.getFullName());

  objectRenderer.setSurfaceShadeMode(SimRobotCore3::Renderer::ShadeMode(settings->value("SurfaceShadeMode", int(objectRenderer.getSurfaceShadeMode())).toInt()));
  objectRenderer.setPhysicsShadeMode(SimRobotCore3::Renderer::ShadeMode(settings->value("PhysicsShadeMode", int(objectRenderer.getPhysicsShadeMode())).toInt()));
  objectRenderer.setDrawingsShadeMode(SimRobotCore3::Renderer::ShadeMode(settings->value("DrawingsShadeMode", int(objectRenderer.getDrawingsShadeMode())).toInt()));
  objectRenderer.setCameraMode(SimRobotCore3::Renderer::CameraMode(settings->value("CameraMode", int(objectRenderer.getCameraMode())).toInt()));
  fovY = settings->value("FovY", objectRenderer.getFovY()).toInt();
  objectRenderer.setDragPlane(SimRobotCore3::Renderer::DragAndDropPlane(settings->value("DragPlane", int(objectRenderer.getDragPlane())).toInt()));
  objectRenderer.setDragMode(SimRobotCore3::Renderer::DragAndDropMode(settings->value("DragMode", int(objectRenderer.getDragMode())).toInt()));
  objectRenderer.setRenderFlags(settings->value("RenderFlags", objectRenderer.getRenderFlags()).toInt());

  float pos[3];
  float target[3];
  objectRenderer.getCamera(pos, target);
  pos[0] = settings->value("cameraPosX", pos[0]).toFloat();
  pos[1] = settings->value("cameraPosY", pos[1]).toFloat();
  pos[2] = settings->value("cameraPosZ", pos[2]).toFloat();
  target[0] = settings->value("cameraTargetX", target[0]).toFloat();
  target[1] = settings->value("cameraTargetY", target[1]).toFloat();
  target[2] = settings->value("cameraTargetZ", target[2]).toFloat();
  objectRenderer.setCamera(pos, target);

  settings->endGroup();


  oMouse.onPress([this](int btn, int mod) {
    oMouse.reset();
    if(btn & (GUM_MOUSE_BUTTON_LEFT | GUM_MOUSE_BUTTON_MIDDLE))
    {
      const Qt::KeyboardModifiers m = QApplication::keyboardModifiers();
      if(objectRenderer.startDrag(oMouse.getPosition().x, oMouse.getPosition().y, m & Qt::ShiftModifier ? (m & Qt::ControlModifier ? SimObjectRenderer::dragRotateWorld : SimObjectRenderer::dragRotate) : (m & Qt::ControlModifier ? SimObjectRenderer::dragNormalObject : SimObjectRenderer::dragNormal)))
      {
        update();
      }
    }
  });


  oMouse.onDouble([this](int btn, int mod) {
    if(btn & GUM_MOUSE_BUTTON_LEFT)
    {
      SimRobotCore3::Object* selectedObject = objectRenderer.getDragSelection();
      if(selectedObject)
        CoreModule::application->selectObject(*selectedObject);
    }
  });

  oMouse.onMoved([this](ivec2 pos) {
    //Gum::Output::print("Moved: " + pos.toString() + " in fb: " + std::to_string(Framebuffer::CurrentlyBoundFramebuffer->getID()));
    const Qt::KeyboardModifiers m = QApplication::keyboardModifiers();
    if(objectRenderer.moveDrag(pos.x, pos.y,
                              m & Qt::ShiftModifier
                              ? (m & Qt::ControlModifier
                                  ? SimObjectRenderer::dragRotateWorld
                                  : SimObjectRenderer::dragRotate)
                              : (m & Qt::ControlModifier
                                  ? SimObjectRenderer::dragNormalObject
                                  : SimObjectRenderer::dragNormal)))
    {
      update();
    }
  });

  oMouse.onRelease([this](int btn, int mod) {
    if(objectRenderer.releaseDrag(oMouse.getPosition().x, oMouse.getPosition().y))
      update();
  });

  oMouse.onScroll([this](vec2 dir) {
    //if(dir.y != 0.f)
    //{
    //  objectRenderer.zoom(dir.y, oMouse.getPosition().x, oMouse.getPosition().y);
    //  update();
    //}
    update();
  });

  //QTimer *timer = new QTimer(this);
  //connect(timer, &QTimer::timeout, this, QOverload<>::of(&SimObjectWidget::update));
  //timer->start(16);
  connect(this, &QOpenGLWidget::frameSwapped, this, QOverload<>::of(&SimObjectWidget::update));
}

SimObjectWidget::~SimObjectWidget()
{
  // save layout settings
  QSettings* settings = &CoreModule::application->getLayoutSettings();
  settings->beginGroup(object.getFullName());

  settings->setValue("SurfaceShadeMode", int(objectRenderer.getSurfaceShadeMode()));
  settings->setValue("PhysicsShadeMode", int(objectRenderer.getPhysicsShadeMode()));
  settings->setValue("DrawingsShadeMode", int(objectRenderer.getDrawingsShadeMode()));
  settings->setValue("CameraMode", int(objectRenderer.getCameraMode()));
  settings->setValue("FovY", objectRenderer.getFovY());
  settings->setValue("DragPlane", int(objectRenderer.getDragPlane()));
  settings->setValue("DragMode", int(objectRenderer.getDragMode()));
  settings->setValue("RenderFlags", objectRenderer.getRenderFlags());

  float pos[3];
  float target[3];
  objectRenderer.getCamera(pos, target);

  settings->setValue("cameraPosX", pos[0]);
  settings->setValue("cameraPosY", pos[1]);
  settings->setValue("cameraPosZ", pos[2]);
  settings->setValue("cameraTargetX", target[0]);
  settings->setValue("cameraTargetY", target[1]);
  settings->setValue("cameraTargetZ", target[2]);

  settings->endGroup();

  makeCurrent();
  objectRenderer.destroy();

  if(object.getKind() == SimRobotCore3::Kind::appearance)
  {
    Gum::_delete(pWorld);
  }
  Gum::_delete(pMainCamera);
  Gum::_delete(pMainRenderer);
}

void recursivelyAddObjects(World3D* world, const SimRobot::Object* object)
{
  if(object == nullptr || object->getKind() != SimRobotCore3::Kind::appearance)
    return;
    
  if(((Appearance*)object)->getMesh() != nullptr)
    world->getObjectManager()->addObject(((Appearance*)object));
  
  for(SimObject* obj : ((Appearance*)object)->children)
  {
    recursivelyAddObjects(world, dynamic_cast<Appearance*>(obj));
  }
}

void SimObjectWidget::initializeGL()
{ 
  Gum::Window::CurrentlyBoundWindow->getContext()->bind();
  pGLContext->initOpenGL();
  pGLContext->setDefaults();

  pRenderCanvas = new Canvas(ivec2(width(), height()));

  Framebuffer::DefaultFramebufferID = this->defaultFramebufferObject();
  pContextFramebuffer = new Framebuffer(pRenderCanvas->getSize(), true, Framebuffer::DefaultFramebufferID);
  isSceneWidget = object.getKind() == SimRobotCore3::Kind::scene;

  if(isSceneWidget)
  {
    pWorld = Simulation::simulation->scene->world;
  }
  else if(object.getKind() == SimRobotCore3::Kind::appearance)
  {

    pWorld = new World3D(Simulation::simulation->scene->world->getObjectManager()->getSkybox());
    pWorld->getObjectManager()->selfManageObjects(true);
    recursivelyAddObjects(pWorld, &object);

  }
  else
  {
    Gum::Output::error("Unknown object type");
  }
  

  pMainRenderer = new Renderer3D(pRenderCanvas);
  pMainRenderer->setWorld(pWorld);
  pMainRenderer->setExposure(1.0f);
  if(object.getKind() == SimRobotCore3::Kind::appearance)
    pMainRenderer->renderSky(false);
  

  pMainCamera = new Camera3D(pRenderCanvas->getSize(), pWorld);
  pMainCamera->setWorldUpDirection(vec3(0,0,1));
  pMainCamera->setPosition(vec3(0,0,0));
  pMainCamera->setMode(Camera3D::Modes::THIRDPERSON);
  pMainCamera->setOffset(5.0f);
  pMainCamera->setZoomSpeed(0.3f);
  pMainCamera->setFOV(80);
  pMainCamera->makeActive();

  PointLight* testLight = new PointLight(vec3(0, 0, 2), vec3(1), "light");
  pWorld->getLightManager()->addPointLight(testLight);

  
  if(pShader == nullptr)
  {
    pShader = new ShaderProgram("CanvasShader", true);
    pShader->addShader(Gum::PostProcessing::VertexShader);
    pShader->addShader(Gum::PostProcessing::FragmentShader);
    pShader->build();
  }
}

void SimObjectWidget::paintGL()
{
  Gum::Window::CurrentlyBoundWindow->getContext()->bind();
  bindFramebuffer();
  
  pContextFramebuffer->clear(Framebuffer::ClearFlags::COLOR);
  pMainRenderer->render();
  pMainRenderer->renderIDs();

  pContextFramebuffer->bind();
  pShader->use();
  pRenderCanvas->getTexture()->bind(0);
  pRenderCanvas->render();
  pRenderCanvas->getTexture()->unbind(0);
  pShader->unuse();

  oMouse.reset();
  if(isSceneWidget)
  {
    Time::update();
    Texture::updateBackgroundLoading();
  }
}

void SimObjectWidget::resizeGL(int width, int height)
{
  bindFramebuffer();
  
  pRenderCanvas->setSize(ivec2(width*this->devicePixelRatio(), height*this->devicePixelRatio()));

  pContextFramebuffer->setSize(pRenderCanvas->getSize());
  pContextFramebuffer->resetViewport();

  pMainRenderer->updateFramebufferSize();
  pMainCamera->updateProjection(pRenderCanvas->getSize());
}

void SimObjectWidget::update()
{
  QOpenGLWidget::update();
  pMainCamera->update();
  pMainRenderer->update();
}

void SimObjectWidget::bindFramebuffer()
{
  Framebuffer::WindowFramebuffer = pContextFramebuffer;
  Framebuffer::DefaultFramebufferID = this->defaultFramebufferObject();
  pContextFramebuffer->overrideID(Framebuffer::DefaultFramebufferID);
  pContextFramebuffer->bind();
  pMainRenderer->makeActive();
  pMainCamera->makeActive();
  Gum::Window::CurrentlyBoundWindow->overrideMouseIO(&oMouse);
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
    auto addPlaneAction = [this, subMenu, actionGroup](const char* label, Qt::Key key, SimRobotCore3::Renderer::DragAndDropPlane plane)
    {
      auto* action = subMenu->addAction(tr(label));
      actionGroup->addAction(action);
      action->setShortcut(QKeySequence(key));
      action->setCheckable(true);
      action->setChecked(objectRenderer.getDragPlane() == plane);
      connect(action, &QAction::triggered, this, [this, plane]{ const_cast<SimObjectWidget*>(this)->setDragPlane(plane); });
    };
    addPlaneAction("X/Y Plane", Qt::Key_Z, SimRobotCore3::Renderer::xyPlane);
    addPlaneAction("X/Z Plane", Qt::Key_Y, SimRobotCore3::Renderer::xzPlane);
    addPlaneAction("Y/Z Plane", Qt::Key_X, SimRobotCore3::Renderer::yzPlane);
    subMenu->addSeparator();
    actionGroup = new QActionGroup(subMenu);
    auto addModeAction = [this, subMenu, actionGroup](const char* label, Qt::Key key, SimRobotCore3::Renderer::DragAndDropMode mode)
    {
      auto* action = subMenu->addAction(tr(label));
      actionGroup->addAction(action);
      action->setShortcut(QKeySequence(key));
      action->setCheckable(true);
      action->setChecked(objectRenderer.getDragMode() == mode);
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
    auto addFovYAction = [this, subMenu, actionGroup](const char* label, Qt::Key key, int fovY)
    {
      auto* action = subMenu->addAction(tr(label));
      actionGroup->addAction(action);
      action->setShortcut(QKeySequence(key));
      action->setCheckable(true);
      action->setChecked(objectRenderer.getFovY() == fovY);
      connect(action, &QAction::triggered, this, [this, fovY]{ const_cast<SimObjectWidget*>(this)->setFovY(fovY); });
    };
    addFovYAction("&20°", Qt::Key_1, 20);
    addFovYAction("&40°", Qt::Key_2, 40);
    addFovYAction("&60°", Qt::Key_3, 60);
    addFovYAction("&80°", Qt::Key_4, 80);
    addFovYAction("100°", Qt::Key_5, 100);
    addFovYAction("120°", Qt::Key_6, 120);
  }

  {
    QMenu* subMenu = menu->addMenu(tr("&Origin"));
    QActionGroup* actionGroup = new QActionGroup(subMenu);
    QAction* action = subMenu->menuAction();
    QIcon icon(":/Icons/icons8-collect-50.png");
    icon.setIsMask(true);
    action->setIcon(icon);
    auto addOriginAction = [this, subMenu, actionGroup](const char* label, Qt::Key key, SimRobotCore3::Renderer::RenderFlags flag)
    {
      auto* action = subMenu->addAction(tr(label));
      actionGroup->addAction(action);
      action->setShortcut(QKeySequence(key));
      action->setCheckable(true);
      const int mask = SimRobotCore3::Renderer::showAsGlobalView | SimRobotCore3::Renderer::showAsGlobalOrientation;
      action->setChecked((objectRenderer.getRenderFlags() & mask) == flag);
      connect(action, &QAction::triggered, this, [this, flag]
      {
        const unsigned flags = objectRenderer.getRenderFlags() & ~mask;
        const_cast<SimObjectWidget*>(this)->objectRenderer.setRenderFlags(flags | flag);
      });
    };
    addOriginAction("S&cene", Qt::Key_C, SimRobotCore3::Renderer::showAsGlobalView); // cspell:disable-line
    addOriginAction("&Object Position", Qt::Key_O, SimRobotCore3::Renderer::showAsGlobalOrientation);
    addOriginAction("Object &Pose", Qt::Key_P, SimRobotCore3::Renderer::RenderFlags(0));
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
      action->setChecked(objectRenderer.getSurfaceShadeMode() == shading);
      connect(action, &QAction::triggered, this, [this, shading]{ const_cast<SimObjectWidget*>(this)->setSurfaceShadeMode(shading); });
    };
    addShadingAction("&Off", Qt::Key(0), SimRobotCore3::Renderer::noShading);
    addShadingAction("&Wire Frame", Qt::Key_W, SimRobotCore3::Renderer::wireframeShading);
    addShadingAction("&Flat Shading", Qt::Key_F, SimRobotCore3::Renderer::flatShading);
    addShadingAction("&Smooth Shading", Qt::Key_M, SimRobotCore3::Renderer::smoothShading);
  }

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
      action->setChecked(objectRenderer.getPhysicsShadeMode() == shading);
      connect(action, &QAction::triggered, this, [this, shading]{ const_cast<SimObjectWidget*>(this)->setPhysicsShadeMode(shading); });
    };
    addShadingAction("&Off", SimRobotCore3::Renderer::noShading);
    addShadingAction("&Wire Frame", SimRobotCore3::Renderer::wireframeShading);
    addShadingAction("&Flat Shading", SimRobotCore3::Renderer::flatShading);
    addShadingAction("&Smooth Shading", SimRobotCore3::Renderer::smoothShading);
  }

  {
    QMenu* subMenu = menu->addMenu(tr("&Drawings Rendering"));
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
      action->setChecked(objectRenderer.getDrawingsShadeMode() == shading);
      connect(action, &QAction::triggered, this, [this, shading]{ const_cast<SimObjectWidget*>(this)->setDrawingsShadeMode(shading); });
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
      action->setChecked((objectRenderer.getRenderFlags() & (SimRobotCore3::Renderer::enableDrawingsOcclusion | SimRobotCore3::Renderer::enableDrawingsTransparentOcclusion)) == flag);
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
    action->setChecked(objectRenderer.getRenderFlags() & flag);
    connect(action, &QAction::triggered, this, [this, flag]{ const_cast<SimObjectWidget*>(this)->toggleRenderFlag(flag); });
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
  QString fileName = QFileDialog::getSaveFileName(this,
                                                  tr("Export as Image"), settings.value("ExportDirectory", "").toString(), tr("Portable Network Graphic (*.png)")
#if defined LINUX && defined QT_VERSION && QT_VERSION < QT_VERSION_CHECK(6, 6, 0)
                                                  , nullptr, QFileDialog::DontUseNativeDialog
#endif
                                                  );
  if(fileName.isEmpty())
    return;
  settings.setValue("ExportDirectory", QFileInfo(fileName).dir().path());

  QImage image;
  {
    unsigned int winWidth, winHeight;
    objectRenderer.getSize(winWidth, winHeight);
    makeCurrent();

    // render object using a temporary framebuffer
    //TODO
    QOpenGLFramebufferObject framebuffer(width, height, QOpenGLFramebufferObject::Depth);
    framebuffer.bind();
    objectRenderer.resize(fovY, width, height);
    objectRenderer.draw();
    image = framebuffer.toImage();
    framebuffer.release();

    objectRenderer.resize(fovY, winWidth, winHeight);
  }

  image.save(fileName);
}

void SimObjectWidget::setSurfaceShadeMode(int style)
{
  objectRenderer.setSurfaceShadeMode(SimRobotCore3::Renderer::ShadeMode(style));
  update();
}

void SimObjectWidget::setPhysicsShadeMode(int style)
{
  objectRenderer.setPhysicsShadeMode(SimRobotCore3::Renderer::ShadeMode(style));
  update();
}

void SimObjectWidget::setDrawingsShadeMode(int style)
{
  objectRenderer.setDrawingsShadeMode(SimRobotCore3::Renderer::ShadeMode(style));
  update();
}

void SimObjectWidget::setDrawingsOcclusion(int flag)
{
  unsigned int flags = objectRenderer.getRenderFlags();
  flags &= ~(SimRobotCore3::Renderer::enableDrawingsOcclusion | SimRobotCore3::Renderer::enableDrawingsTransparentOcclusion);
  flags |= flag;
  objectRenderer.setRenderFlags(flags);
  update();
}

void SimObjectWidget::setCameraMode(int mode)
{
  objectRenderer.setCameraMode(SimRobotCore3::Renderer::CameraMode(mode));
  update();
}

void SimObjectWidget::setFovY(int fovY)
{
  unsigned int width, height;
  this->fovY = fovY;
  objectRenderer.getSize(width, height);
  makeCurrent();
  objectRenderer.resize(fovY, width, height);
  update();
}

void SimObjectWidget::setDragPlane(int plane)
{
  objectRenderer.setDragPlane(SimRobotCore3::Renderer::DragAndDropPlane(plane));
  update();
}

void SimObjectWidget::setDragMode(int mode)
{
  objectRenderer.setDragMode(SimRobotCore3::Renderer::DragAndDropMode(mode));
  update();
}

void SimObjectWidget::resetCamera()
{
  objectRenderer.resetCamera();
  update();
}

void SimObjectWidget::toggleCameraMode()
{
  objectRenderer.toggleCameraMode();
  update();
}

void SimObjectWidget::toggleRenderFlag(int flag)
{
  unsigned int flags = objectRenderer.getRenderFlags();
  if(flags & flag)
    flags &= ~flag;
  else
    flags |= flag;
  objectRenderer.setRenderFlags(flags);

  update();
}
