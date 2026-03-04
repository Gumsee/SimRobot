#include <Desktop/Window.h>
#include <Desktop/Display.h>
#include <System/Output.h>
#include <System/MemoryManagement.h>
#include <QMainWindow>

static QMainWindow* toNativeWindow(void* ptr) { return (QMainWindow*)ptr; }

namespace Gum
{
    void Window::initNativeWindow()
    {
        /*GLFWwindow* sharedContext = nullptr; //Set to main context to share
        if(pContextWindow != nullptr)
            sharedContext = toNativeWindow(pContextWindow->getNativeWindow());

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        pNativeWindowHandle = glfwCreateWindow(v2Size.x, v2Size.y, sTitle.c_str(), NULL, sharedContext);


        glfwSetWindowUserPointer(toNativeWindow(pNativeWindowHandle), this);


        glfwSetWindowSizeCallback(toNativeWindow(pNativeWindowHandle), [](GLFWwindow* window, int x, int y) {
            Gum::Window* win = (Gum::Window*)glfwGetWindowUserPointer(window);
            Event evnt;
            evnt.type = GUM_EVENT_WINDOW_RESIZE;
            evnt.data.windowsize = ivec2(x, y);

            win->handleEvent(evnt);
        });

        glfwSetWindowPosCallback(toNativeWindow(pNativeWindowHandle), [](GLFWwindow* window, int x, int y) {
            Gum::Window* win = (Gum::Window*)glfwGetWindowUserPointer(window);
            Event evnt;
            evnt.type = GUM_EVENT_WINDOW_REPOSITION;
            evnt.data.windowpos = ivec2(x, y);

            if(win->pParentWindow != nullptr)
                evnt.data.windowpos -= win->pParentWindow->getPosition();


            win->handleEvent(evnt);
        });

        glfwSetWindowCloseCallback(toNativeWindow(pNativeWindowHandle), [](GLFWwindow* window) {
            Gum::Window* win = (Gum::Window*)glfwGetWindowUserPointer(window);
            Event evnt;
            evnt.type = GUM_EVENT_WINDOW_CLOSE;

            win->handleEvent(evnt);
        });

        glfwSetWindowFocusCallback(toNativeWindow(pNativeWindowHandle), [](GLFWwindow* window, int i) {
            Gum::Window* win = (Gum::Window*)glfwGetWindowUserPointer(window);
            Event evnt;
            evnt.type = i == 1 ? GUM_EVENT_FOCUS_IN : GUM_EVENT_FOCUS_OUT;

            win->handleEvent(evnt);
        });*/
    }   
    
    void Window::destroyNativeWindow()
    {
        //glfwDestroyWindow(toNativeWindow(pNativeWindowHandle));
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