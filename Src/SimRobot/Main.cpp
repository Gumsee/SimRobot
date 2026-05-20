/**
 * @file SimRobot/Main.cpp
 * Implementation of the main function of SimRobot
 * @author Colin Graf
 */

#include <QApplication>
#include <QLocale>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QOffscreenSurface>

#ifdef WINDOWS
#include <crtdbg.h>
#else
#include <clocale>
#endif

#include "MainWindow.h"
#include <Desktop/Window.h>
#include <gum-engine.h>
#define GUMDESKTOP_FOUND
#include <Graphics/Graphics.h>
#include <QTContext.h>

#ifdef MACOS
#include <QFileOpenEvent>

/** The address of the main window object used by the following class. */
static MainWindow* mainWindow = nullptr;

/**
 * A helper for opening files when they were launched from the Finder and closing windows
 * in the correct order.
 * macOS triggers an event for them rather than passing them as a command line
 * parameter. This class handles that event.
 */
class SimRobotApp : public QApplication
{
public:
  SimRobotApp(int& argc, char** argv)
    : QApplication(argc, argv) {}

protected:
  bool event(QEvent* ev)
  {
    if(ev->type() == QEvent::FileOpen)
    {
      mainWindow->openFile(static_cast<QFileOpenEvent*>(ev)->file());
      return true;
    }
    return QApplication::event(ev);
  }
};

/** Use the new class rather than the default one. */
#define QApplication SimRobotApp
#endif // MACOS

int main(int argc, char* argv[])
{
    //Gum::MaterialManager::MATERIAL_ASSETS_PATH = Examples::assetPath + Gum::File("/materials/", Gum::Filesystem::DIRECTORY);
#ifdef WINDOWS
  _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
  //_CrtSetBreakAlloc(18969); // Use to track down memory leaks

  #if QT_VERSION < QT_VERSION_CHECK(6, 4, 0)
    // Workaround: For OpenGL to be used in windows, support must be registered before the window is created.
    // The following function is declared as a constructor in QtOpenGL (i.e. executed at library loading time),
    // but since the SimRobot application doesn't reference QtOpenGL it isn't sufficient to link QtOpenGL
    // due to lazy loading. Therefore, we call this function here (probably resulting in the function being
    // called twice, but this is handled by the function).
    qt_registerDefaultPlatformBackingStoreOpenGLSupport();
  #endif
#endif

  // Handle floating point values as programming languages would.
  QLocale::setDefault(QLocale::C);

  QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
  QApplication app(argc, argv);
#ifndef WINDOWS
  setlocale(LC_NUMERIC, "C");
#endif
  MainWindow mainWindow(argc, argv);
  Gum::Window* gumWindow = new Gum::Window(
    "SimRobot", ivec2(1920, 1080), 
    GUM_WINDOW_DEFAULTS | GUM_WINDOW_NO_CONTEXT,
    nullptr, Gum::DefaultContextConfig, nullptr,
    &mainWindow
  );
  Gum::IO::Mouse* winmouse = gumWindow->getMouse();
  Gum::IO::Keyboard* winkeyboard = gumWindow->getKeyboard();

  Gum::Graphics::addFramebufferToWindow(gumWindow);


  QSurfaceFormat format;
  format.setVersion(4, 1);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setSamples(Gum::DefaultContextConfig.numSamples);
  format.setStencilBufferSize(Gum::DefaultContextConfig.stencilBits);
  format.setDepthBufferSize(Gum::DefaultContextConfig.depthBits);
  format.setRedBufferSize(Gum::DefaultContextConfig.rgbaBits.r);
  format.setGreenBufferSize(Gum::DefaultContextConfig.rgbaBits.g);
  format.setBlueBufferSize(Gum::DefaultContextConfig.rgbaBits.b);
  format.setAlphaBufferSize(Gum::DefaultContextConfig.rgbaBits.a);
  format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
  format.setRenderableType(QSurfaceFormat::OpenGL);
  QSurfaceFormat::setDefaultFormat(format);

  QOffscreenSurface* offscreenSurface = new QOffscreenSurface();
  offscreenSurface->create();
  offscreenSurface->setFormat(format);
  

  QOpenGLContext* qoffscreenContext = new QOpenGLContext();
  qoffscreenContext->setShareContext(QOpenGLContext::globalShareContext());
  if(!qoffscreenContext->create())
      Gum::Output::error("Failed to create OpenGL Context");

  QTContextData contextData;
  contextData.bindFunc = [qoffscreenContext, offscreenSurface](){ qoffscreenContext->makeCurrent(offscreenSurface); };
  contextData.unbindFunc = [qoffscreenContext](){ qoffscreenContext->doneCurrent(); };
  GraphicsContext* offscreenContext = new GraphicsContext(qoffscreenContext, nullptr, &contextData, Gum::DefaultContextConfig);
  offscreenContext->bind();

  offscreenContext->printInfo();

#ifdef WINDOWS
  app.setStyle("fusion");
#elif defined MACOS
  ::mainWindow = &mainWindow;
#endif

  app.setApplicationName("SimRobot");

  bool noWindow = false;
  for(int i = 1; i < argc; i++)
    noWindow |= strcmp(argv[i], "-noWindow") == 0;

#ifdef MACOS
  if(!noWindow)
    mainWindow.show();
#endif

  // open file from commandline
  for(int i = 1; i < argc; i++)
    if(*argv[i] != '-' && strcmp(argv[i], "YES"))
    {
      mainWindow.openFile(argv[i]);
      break;
    }

#ifndef MACOS
  if(!noWindow)
    mainWindow.show();
#endif

  int ret = app.exec();
  
  if(gumWindow->getMouse() != winmouse)
  {
    delete winmouse;
    delete winkeyboard;
  }
  delete gumWindow;

  Gum::Engine::cleanup();

  return ret;
}
