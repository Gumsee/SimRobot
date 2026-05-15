#include <Desktop/GraphicsContext.h>
#include <Desktop/Display.h>
#include <Desktop/Window.h>
#include <System/Output.h>
#include <QSurfaceFormat>
#include <QApplication>
#include <QMainWindow>
#include "QTContext.h"

inline static bool CONTEXT_FORMAT_ALREADY_SET = false;

#if QT_VERSION < QT_VERSION_CHECK(6, 4, 0)
extern void qt_registerDefaultPlatformBackingStoreOpenGLSupport();
#endif

static QOpenGLContext* toNativeContext(void* ptr) { return (QOpenGLContext*)ptr; }
static QTContextData* toQTContextData(void* data) { return (QTContextData*)data; }


Gum::ContextFBConfig GraphicsContext::initNative(Gum::ContextFBConfig& wantedconfig)
{
  return wantedconfig;
}

void GraphicsContext::createNativeContext()
{   
}

void GraphicsContext::createNativeContextWindow()
{
}

void GraphicsContext::destroyNative()
{   
}

void GraphicsContext::swapBuffers()
{
}

void GraphicsContext::bind()
{
  CurrentlyBoundContext = this;
  toQTContextData(pCustomData)->bindFunc();
}

void GraphicsContext::unbind()
{
  toQTContextData(pCustomData)->unbindFunc();
}