#include <Desktop/Window.h>
#include <QMainWindow>

static QMainWindow* toNativeWindow(void* ptr) { return (QMainWindow*)ptr; }

namespace Gum
{
    void Window::initNativeWindow()
    {
    }   
    
    void Window::destroyNativeWindow()
    {
    }
    
    void Window::makeResizable(bool isresizable)
    {
        //glfwSetWindowAttrib(toNativeWindow(pNativeWindowHandle), GLFW_RESIZABLE, isresizable);
        bIsResizable = isresizable;
    }

    void Window::makeFullscreen(bool fullscreen)
    {
        bIsFullscreen = fullscreen;
    }

    void Window::makeFloating(bool isfloating)
    {
        //glfwSetWindowAttrib(toNativeWindow(pNativeWindowHandle), GLFW_FLOATING, isfloating);
        this->bIsFloating = isfloating;
    }

    void Window::showBorder(bool show)
    {
        //glfwSetWindowAttrib(toNativeWindow(pNativeWindowHandle), GLFW_DECORATED, show);
        bHasBorder = show;
    }

    
    void Window::restore()
    {
        //glfwRestoreWindow(toNativeWindow(pNativeWindowHandle));
    }
    
	void Window::maximize(bool domaximize) 
	{ 
    (void)domaximize;
	}

	void Window::minimize(bool dominimize)     
    { 
    (void)dominimize;
    }
	void Window::finishRender() 
    { 
        pContext->swapBuffers();
    }  

	void Window::hide(bool hiddenstat)
	{ 
    (void)hiddenstat;
        
	}
    void Window::focus()
    {
        //glfwFocusWindow(toNativeWindow(pNativeWindowHandle));
    }


    //Setter
	void Window::setVerticalSync(bool vsync)		
    { 
      (void)vsync;
    }
    void Window::setSize(const ivec2& size)    
    { 
      (void)size;
    }
    void Window::setPosition(const ivec2& pos) 
    {
      (void)pos;
    }
	void Window::setTitle(const std::string& title) 
    { 
        toNativeWindow(pNativeWindowHandle)->setWindowTitle(title.c_str());
        this->sTitle = title; 
    }
    void Window::setIcon(std::vector<IconImageData> images, bool isgrayscale, vec4 color)
    {
      (void)images;
      (void)isgrayscale;
      (void)color;
    }

    void Window::removeIcon()
    {    
    }

    void Window::setParent(Window* parent) 
    {
      (void)parent;
    }

    void Window::pollPosition()
    {
        //glfwGetWindowPos(toNativeWindow(pNativeWindowHandle), &v2Pos.x, &v2Pos.y);
        if(pParentWindow != nullptr)
            v2Pos -= pParentWindow->getPosition();
    }
    
    void Window::pollSize()
    {
        //glfwGetWindowSize(toNativeWindow(pNativeWindowHandle), &v2Size.x, &v2Size.y);
    }

}