/**
 * @file SimObjectWidgetEvent.cpp
 * Implementation Event methods for SimObjectWidget
 * @author Tom Beuke
 */

#include "SimObjectWidget.h"
#include <QMouseEvent>
#include <QApplication>
#include <QPinchGesture>

void SimObjectWidget::mouseMoveEvent(QMouseEvent* event)
{
  QOpenGLWidget::mouseMoveEvent(event);
  bindFramebuffer();
  oMouse.handleEvent(Gum::Event(Gum::Event::EventData(ivec2(event->position().x(), event->position().y())), GUM_EVENT_MOUSE_MOVED));
  event->accept();
}

void SimObjectWidget::mousePressEvent(QMouseEvent* event)
{
  QOpenGLWidget::mousePressEvent(event);
  bindFramebuffer();

  int btn = GUM_MOUSE_BUTTON_NONE;
  switch(event->button())
  {
      case Qt::LeftButton:    btn = GUM_MOUSE_BUTTON_LEFT;     break;
      case Qt::RightButton:   btn = GUM_MOUSE_BUTTON_RIGHT;    break;
      case Qt::MiddleButton:  btn = GUM_MOUSE_BUTTON_MIDDLE;   break;
      case Qt::BackButton:    btn = GUM_MOUSE_BUTTON_PREVIOUS; break;
      case Qt::ForwardButton: btn = GUM_MOUSE_BUTTON_NEXT;     break;
      default:                btn = GUM_MOUSE_BUTTON_NONE;     break;
  }

  ;
  oMouse.handleEvent(Gum::Event(Gum::Event::EventData(btn), GUM_EVENT_MOUSE_PRESSED));
  oMouse.handleEvent(Gum::Event(Gum::Event::EventData(ivec2(event->position().x(), event->position().y())), GUM_EVENT_MOUSE_MOVED));

  event->accept();
}

void SimObjectWidget::mouseReleaseEvent(QMouseEvent* event)
{
  QOpenGLWidget::mouseReleaseEvent(event);
  bindFramebuffer();

  int btn = GUM_MOUSE_BUTTON_NONE;
  switch(event->button())
  {
      case Qt::LeftButton:    btn = GUM_MOUSE_BUTTON_LEFT;
      case Qt::RightButton:   btn = GUM_MOUSE_BUTTON_RIGHT;
      case Qt::MiddleButton:  btn = GUM_MOUSE_BUTTON_MIDDLE;
      case Qt::BackButton:    btn = GUM_MOUSE_BUTTON_PREVIOUS;
      case Qt::ForwardButton: btn = GUM_MOUSE_BUTTON_NEXT;
      default:                btn = GUM_MOUSE_BUTTON_NONE;
  }

  oMouse.handleEvent(Gum::Event(Gum::Event::EventData(btn), GUM_EVENT_MOUSE_RELEASED));
  event->accept();
}

void SimObjectWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
  QOpenGLWidget::mouseDoubleClickEvent(event);
  bindFramebuffer();

  int btn = GUM_MOUSE_BUTTON_NONE;
  switch(event->button())
  {
      case Qt::LeftButton:    btn = GUM_MOUSE_BUTTON_LEFT;     break;
      case Qt::RightButton:   btn = GUM_MOUSE_BUTTON_RIGHT;    break;
      case Qt::MiddleButton:  btn = GUM_MOUSE_BUTTON_MIDDLE;   break;
      case Qt::BackButton:    btn = GUM_MOUSE_BUTTON_PREVIOUS; break;
      case Qt::ForwardButton: btn = GUM_MOUSE_BUTTON_NEXT;     break;
      default:                btn = GUM_MOUSE_BUTTON_NONE;     break;
  }

  Gum::Event gevent(Gum::Event::EventData(btn), GUM_EVENT_MOUSE_PRESSED_DOUBLE);
  oMouse.handleEvent(gevent);

  event->accept();

}

void SimObjectWidget::keyPressEvent(QKeyEvent* event)
{
  if(event->modifiers() != 0)
  {
    QOpenGLWidget::keyPressEvent(event);
    return;
  }

  switch(event->key())
  {
    case Qt::Key_PageUp:
    case Qt::Key_Plus:
      event->accept();
      objectRenderer.zoom(-100.f, -1.f, -1.f);
      update();
      break;

    case Qt::Key_PageDown:
    case Qt::Key_Minus:
      event->accept();
      objectRenderer.zoom(100.f, -1.f, -1.f);
      update();
      break;

    case Qt::Key_W:
    case Qt::Key_A:
    case Qt::Key_S:
    case Qt::Key_D:
      event->accept();
      switch(event->key())
      {
        case Qt::Key_W:
          wKey = true;
          break;
        case Qt::Key_A:
          aKey = true;
          break;
        case Qt::Key_S:
          sKey = true;
          break;
        case Qt::Key_D:
          dKey = true;
          break;
      }
      objectRenderer.setCameraMove(aKey, dKey, wKey, sKey);
      update();
      break;

    default:
      QOpenGLWidget::keyPressEvent(event);
      break;
  }
}

void SimObjectWidget::keyReleaseEvent(QKeyEvent* event)
{
  if(event->modifiers() != 0)
  {
    QOpenGLWidget::keyReleaseEvent(event);
    return;
  }

  switch(event->key())
  {
    case Qt::Key_W:
    case Qt::Key_A:
    case Qt::Key_S:
    case Qt::Key_D:
      event->accept();
      update();
      if(!event->isAutoRepeat())
      {
        switch(event->key())
        {
          case Qt::Key_W:
            wKey = false;
            break;
          case Qt::Key_A:
            aKey = false;
            break;
          case Qt::Key_S:
            sKey = false;
            break;
          case Qt::Key_D:
            dKey = false;
            break;
        }
        objectRenderer.setCameraMove(aKey, dKey, wKey, sKey);
      }
      break;

    default:
      QOpenGLWidget::keyReleaseEvent(event);
      break;
  }
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
      objectRenderer.zoom(change * 100.f, -1, -1);
      update();
      return true;
    }
  }
  return QOpenGLWidget::event(event);
}

void SimObjectWidget::wheelEvent(QWheelEvent* event)
{
  bindFramebuffer();
  oMouse.handleEvent(Gum::Event(Gum::Event::EventData(vec2(event->angleDelta().x() / 120.0f, event->angleDelta().y() / 120.0f)), GUM_EVENT_MOUSE_SCROLL));
  event->accept();

  QOpenGLWidget::wheelEvent(event);
}