/**
 * @file SimObjectWidgetEvent.cpp
 * Implementation Event methods for SimObjectWidget
 * @author Tom Beuke
 */

#include "SimObjectWidget.h"
#include <QMouseEvent>
#include <QApplication>
#include <QPinchGesture>

uint16_t convertModifiers()
{
  unsigned long mods = QApplication::keyboardModifiers();
  uint16_t retmods = 0;
    
  if(mods & Qt::ShiftModifier)   retmods |= GUM_KEYBOARD_MOD_SHIFT;
  if(mods & Qt::ControlModifier) retmods |= GUM_KEYBOARD_MOD_CONTROL;
  if(mods & Qt::AltModifier)     retmods |= GUM_KEYBOARD_MOD_ALT;
  if(mods & Qt::MetaModifier)    retmods |= GUM_KEYBOARD_MOD_SUPER;
  if(mods & Qt::KeypadModifier)  retmods |= GUM_KEYBOARD_MOD_NUM_LOCK;
  
  return retmods;
}

void SimObjectWidget::mouseMoveEvent(QMouseEvent* event)
{
  QOpenGLWidget::mouseMoveEvent(event);
  bindFramebuffer();
  oMouse.handleEvent(Gum::Event(Gum::Event::EventData(ivec2(event->position().x()*this->devicePixelRatio(), event->position().y()*this->devicePixelRatio())), GUM_EVENT_MOUSE_MOVED, 0, convertModifiers()));
  event->accept();
}

void SimObjectWidget::mousePressEvent(QMouseEvent* event)
{
  QOpenGLWidget::mousePressEvent(event);
  bindFramebuffer();

  uint16_t btn = GUM_MOUSE_BUTTON_NONE;
  switch(event->button())
  {
      case Qt::LeftButton:    btn = GUM_MOUSE_BUTTON_LEFT;     break;
      case Qt::RightButton:   btn = GUM_MOUSE_BUTTON_RIGHT;    break;
      case Qt::MiddleButton:  btn = GUM_MOUSE_BUTTON_MIDDLE;   break;
      case Qt::BackButton:    btn = GUM_MOUSE_BUTTON_PREVIOUS; break;
      case Qt::ForwardButton: btn = GUM_MOUSE_BUTTON_NEXT;     break;
      default:                btn = GUM_MOUSE_BUTTON_NONE;     break;
  }

  oMouse.setPosition(ivec2(event->position().x()*this->devicePixelRatio(), event->position().y()*this->devicePixelRatio()));
  oMouse.resetDelta();
  oMouse.handleEvent(Gum::Event(Gum::Event::EventData(btn), GUM_EVENT_MOUSE_PRESSED, 0, convertModifiers()));
  event->accept();
}

void SimObjectWidget::mouseReleaseEvent(QMouseEvent* event)
{
  QOpenGLWidget::mouseReleaseEvent(event);
  bindFramebuffer();

  uint16_t btn = GUM_MOUSE_BUTTON_NONE;
  switch(event->button())
  {
      case Qt::LeftButton:    btn = GUM_MOUSE_BUTTON_LEFT;
      case Qt::RightButton:   btn = GUM_MOUSE_BUTTON_RIGHT;
      case Qt::MiddleButton:  btn = GUM_MOUSE_BUTTON_MIDDLE;
      case Qt::BackButton:    btn = GUM_MOUSE_BUTTON_PREVIOUS;
      case Qt::ForwardButton: btn = GUM_MOUSE_BUTTON_NEXT;
      default:                btn = GUM_MOUSE_BUTTON_NONE;
  }

  oMouse.handleEvent(Gum::Event(Gum::Event::EventData(btn), GUM_EVENT_MOUSE_RELEASED, 0, convertModifiers()));
  event->accept();
}

void SimObjectWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
  QOpenGLWidget::mouseDoubleClickEvent(event);
  bindFramebuffer();

  uint16_t btn = GUM_MOUSE_BUTTON_NONE;
  switch(event->button())
  {
      case Qt::LeftButton:    btn = GUM_MOUSE_BUTTON_LEFT;     break;
      case Qt::RightButton:   btn = GUM_MOUSE_BUTTON_RIGHT;    break;
      case Qt::MiddleButton:  btn = GUM_MOUSE_BUTTON_MIDDLE;   break;
      case Qt::BackButton:    btn = GUM_MOUSE_BUTTON_PREVIOUS; break;
      case Qt::ForwardButton: btn = GUM_MOUSE_BUTTON_NEXT;     break;
      default:                btn = GUM_MOUSE_BUTTON_NONE;     break;
  }

  oMouse.handleEvent(Gum::Event(Gum::Event::EventData(btn), GUM_EVENT_MOUSE_PRESSED_DOUBLE, 0, convertModifiers()));
  event->accept();
}

void SimObjectWidget::wheelEvent(QWheelEvent* event)
{
  bindFramebuffer();
  oMouse.handleEvent(Gum::Event(Gum::Event::EventData(vec2(event->angleDelta().x() / 120.0f, event->angleDelta().y() / 120.0f)), GUM_EVENT_MOUSE_SCROLL, 0, convertModifiers()));
  event->accept();

  QOpenGLWidget::wheelEvent(event);
}

void SimObjectWidget::keyPressEvent(QKeyEvent* event)
{
  bindFramebuffer();
  oKeyboard.handleEvent(Gum::Event(Gum::Event::EventData(static_cast<unsigned int>(event->key())), GUM_EVENT_KEYBOARD_PRESSED, 0, convertModifiers()));
  event->accept();
  
  QOpenGLWidget::keyPressEvent(event);
}

void SimObjectWidget::keyReleaseEvent(QKeyEvent* event)
{
  bindFramebuffer();
  oKeyboard.handleEvent(Gum::Event(Gum::Event::EventData(static_cast<unsigned int>(event->key())), GUM_EVENT_KEYBOARD_RELEASED, 0, convertModifiers()));
  event->accept();
  
  QOpenGLWidget::keyReleaseEvent(event);
}

bool SimObjectWidget::event(QEvent* event)
{
  if(event->type() == QEvent::Gesture)
  {
    QPinchGesture* pinch = static_cast<QPinchGesture*>(static_cast<QGestureEvent*>(event)->gesture(Qt::PinchGesture));
    if(pinch && (pinch->changeFlags() & QPinchGesture::ScaleFactorChanged))
    {
#ifdef FIX_MACOS_PINCH_SCALE_RELATIVE_BUG
      pinch->setLastScaleFactor(1.f);
#endif
      float change = static_cast<float>(pinch->scaleFactor() > pinch->lastScaleFactor()
                                        ? -pinch->scaleFactor() / pinch->lastScaleFactor()
                                        : pinch->lastScaleFactor() / pinch->scaleFactor());
      camera->increaseZoom(camera->getZoomSpeed() * change);
      updateZoomInNextFrame = camera->updateZoom();
      return true;
    }
  }
  return QOpenGLWidget::event(event);
}