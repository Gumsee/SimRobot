/**
 * @file SimRobot/Main.cpp
 * Implementation of the main function of SimRobot
 * @author Colin Graf
 */

#include <QApplication>
#include <QLocale>
#include <QSurfaceFormat>

#ifdef WINDOWS
#include <crtdbg.h>
#else
#include <clocale>
#endif

#include "MainWindow.h"
#include <System/Output.h>
#include <Engine/3D/Object/ObjectManager.h>
#include <gum-engine.h>
#include <Desktop/Window.h>

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
    Gum::Output::init();
    ObjectManager::MODEL_ASSETS_PATH = Gum::File("/home/gumse/Projects/gumengine/gum-engine/examples/assets/objects/", Gum::Filesystem::DIRECTORY);;

    //Gum::MaterialManager::MATERIAL_ASSETS_PATH = Examples::assetPath + Gum::File("/materials/", Gum::Filesystem::DIRECTORY);
#ifdef WINDOWS
  _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
  //_CrtSetBreakAlloc(18969); // Use to track down memory leaks
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
    GUM_WINDOW_DEFAULTS, 
    nullptr, Gum::DefaultContextConfig, nullptr, 
    &mainWindow
  );


  Gum::Engine::init();
  gumWindow->getContext()->initOpenGL();
  gumWindow->getContext()->setDefaults();
  gumWindow->getContext()->printInfo();
  Lightning::initShader();

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
  
  Gum::_delete(gumWindow);
  Gum::Engine::cleanup();

  return ret;
}
