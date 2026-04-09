/**
 * @file SimObjectWidget.h
 * Declaration of class SimObjectWidget
 * @author Colin Graf
 */

#pragma once

#include <QOpenGLWidget>

#include "SimRobotCore3.h"
#include <Desktop/GraphicsContext.h>
#include <Desktop/Window.h>
#include <Engine/3D/Camera3D.h>
#include <Engine/3D/Renderer3D.h>
#include <Engine/3D/World3D.h>
#include "Graphics/PhysicsRenderer.h"
#include "Simulation/Body.h"


class SimObject;
class Simulation;

/**
 * @class SimObjectWidget
 * A class that implements the 3D-view for simulated objects
 */
class SimObjectWidget : public QOpenGLWidget, public SimRobot::Widget, public SimRobotCore3::Renderer
{
  Q_OBJECT

public:
  /**
   * Constructor
   * @param simObject The object that should be displayed
   */
  SimObjectWidget(SimObject& simObject);

  /** Destructor */
  ~SimObjectWidget();

private:
  SimObject& simObject;
  const SimRobot::Object& object; /**< The object that should be displayed */
  GraphicsContext* pGLContext;
  Framebuffer* pContextFramebuffer = nullptr;
  Canvas* renderCanvas = nullptr;
  Camera3D* camera = nullptr;
  Renderer3D* renderer = nullptr;
  World3D* pWorld = nullptr;
  Gum::IO::Mouse oMouse;
  Gum::IO::Keyboard oKeyboard;
  PhysicsRenderer* physicsRenderer = nullptr;
  ShadeMode appearanceShadeMode = ShadeMode::smoothShading;
  ShadeMode controllerdrawingsShadeMode = ShadeMode::smoothShading;
  unsigned int dragStartTime = 0;
  bool dragRotate = false;
  vec3 dragPlane = vec3(0,0,1);
  vec3 dragStartPos;
  DragAndDropMode dragMode = keepDynamics;
	static inline ShaderProgram *pShader = nullptr;

  vec3 defaultCameraPos;
  Body* clickedBody = nullptr;
  Object3DInstance* clickedBodyRing = nullptr;
  bool updateCamera = false;
  bool updateZoomInNextFrame = false;
  bool isSceneWidget;
  bool registeredAtManager = false;
  unsigned int renderFlags = SimRobotCore3::Renderer::enableLights | SimRobotCore3::Renderer::enableTextures | SimRobotCore3::Renderer::enableMultisample;

  QWidget* getWidget() override {return this;}
  void update() override;
  QMenu* createEditMenu() const override;
  QMenu* createUserMenu() const override;

  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseDoubleClickEvent(QMouseEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  bool event(QEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void bindFramebuffer();
  Body* selectObject(vec3 startpos, vec3 raydir);
  QSize sizeHint() const override {return QSize(320, 240);}

private slots:
  void copy();
  void setDrawingsOcclusion(int flag);
  void setDragPlane(vec3 plane);
  void setDragMode(DragAndDropMode mode);
  void resetCamera();
  void toggleRenderFlag(int flag);
  void exportAsImage(int width, int height);
};
