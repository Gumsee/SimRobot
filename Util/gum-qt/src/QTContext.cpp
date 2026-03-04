#include <Desktop/GraphicsContext.h>
#include <Desktop/Display.h>
#include <Desktop/Window.h>
#include <System/Output.h>
#include <QSurfaceFormat>
#include <QApplication>
#include <QMainWindow>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QOpenGLWidget>

inline static bool CONTEXT_FORMAT_ALREADY_SET = false;

#if QT_VERSION < QT_VERSION_CHECK(6, 4, 0)
extern void qt_registerDefaultPlatformBackingStoreOpenGLSupport();
#endif

static QOpenGLContext* toNativeContext(void* ptr) { return (QOpenGLContext*)ptr; }
static QOffscreenSurface* toNativeContextSurface(void* ptr) { return (QOffscreenSurface*)ptr; }


Gum::ContextFBConfig GraphicsContext::initNative(Gum::ContextFBConfig& wantedconfig)
{
  if(!CONTEXT_FORMAT_ALREADY_SET)
  {
    QSurfaceFormat format;
    format.setVersion(wantedconfig.major_gl_version, wantedconfig.minor_gl_version);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(wantedconfig.numSamples);
    format.setStencilBufferSize(wantedconfig.stencilBits);
    format.setDepthBufferSize(wantedconfig.depthBits);
    format.setRedBufferSize(wantedconfig.rgbaBits.r);
    format.setGreenBufferSize(wantedconfig.rgbaBits.g);
    format.setBlueBufferSize(wantedconfig.rgbaBits.b);
    format.setAlphaBufferSize(wantedconfig.rgbaBits.a);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    QSurfaceFormat::setDefaultFormat(format);



    #if QT_VERSION < QT_VERSION_CHECK(6, 4, 0)
      // Workaround: For OpenGL to be used in windows, support must be registered before the window is created.
      // The following function is declared as a constructor in QtOpenGL (i.e. executed at library loading time),
      // but since the SimRobot application doesn't reference QtOpenGL it isn't sufficient to link QtOpenGL
      // due to lazy loading. Therefore, we call this function here (probably resulting in the function being
      // called twice, but this is handled by the function).
      qt_registerDefaultPlatformBackingStoreOpenGLSupport();
    #endif


    QOffscreenSurface* offscreenSurface = new QOffscreenSurface();
    offscreenSurface->create();
    pNativeContextSurface = offscreenSurface;
    
    toNativeContextSurface(pNativeContextSurface)->setFormat(format);

    QOpenGLContext* offscreenContext = new QOpenGLContext();
    offscreenContext->setShareContext(QOpenGLContext::globalShareContext());
    if(!offscreenContext->create())
        Gum::Output::error("Failed to create OpenGL Context");
    offscreenContext->makeCurrent(offscreenSurface);

    pNativeContext = offscreenContext;

    CONTEXT_FORMAT_ALREADY_SET = true;
  }

    //pNativeContext = new QOpenGLContext((QMainWindow*)pWindow->getNativeWindow());
    //toNativeContext(pNativeContext)->setFormat(format);
    //toNativeContext(pNativeContext)->setShareContext(QOpenGLContext::globalShareContext());
    //if(toNativeContext(pNativeContext)->create())
    //  Gum::Output::error("Failed to create OpenGL Context");
    


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
  toNativeContext(pNativeContext)->makeCurrent(toNativeContextSurface(pNativeContextSurface));
}

void GraphicsContext::unbind()                
{
    toNativeContext(pNativeContext)->doneCurrent();
}