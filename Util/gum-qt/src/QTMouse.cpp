#include <Desktop/IO/Mouse.h>
#include <Desktop/Display.h>
#include <Desktop/Window.h>
#include <gum-system.h>
#include <map>

//static GLFWwindow* toNativeWindow(void* ptr) { return (GLFWwindow*)ptr; }

/*struct CursorTypeTranlation
{
    int glfw;
    GLFWcursor* cursor;
};

static std::map<uint8_t, CursorTypeTranlation> cursors = {
    {GUM_CURSOR_DEFAULT,                       {GLFW_ARROW_CURSOR,         nullptr}},
    {GUM_CURSOR_HORIZONTAL_RESIZE,             {GLFW_RESIZE_EW_CURSOR,     nullptr}},
    {GUM_CURSOR_VERTICAL_RESIZE,               {GLFW_RESIZE_NS_CURSOR,     nullptr}},
    {GUM_CURSOR_TOPLEFT_TO_BOTTOMRIGHT_RESIZE, {GLFW_RESIZE_NWSE_CURSOR,   nullptr}},
    {GUM_CURSOR_TOPRIGHT_TO_BOTTOMLEFT_RESIZE, {GLFW_RESIZE_NESW_CURSOR,   nullptr}},
    {GUM_CURSOR_ALL_SIDES_RESIZE,              {GLFW_RESIZE_ALL_CURSOR,    nullptr}},
    {GUM_CURSOR_CROSSHAIR,                     {GLFW_CROSSHAIR_CURSOR,     nullptr}},
    {GUM_CURSOR_HAND,                          {GLFW_POINTING_HAND_CURSOR, nullptr}},
    {GUM_CURSOR_IBEAM,                         {GLFW_IBEAM_CURSOR,         nullptr}},
    {GUM_CURSOR_NOT_ALLOWED,                   {GLFW_NOT_ALLOWED_CURSOR,   nullptr}},
};
static GLFWcursor *pActiveCursor = nullptr;
//static GLFWcursor *pEmptyCursor;*/

namespace Gum {
namespace IO
{
    void Mouse::initNativeMouse()
    {
        /*if (glfwRawMouseMotionSupported())
            glfwSetInputMode(toNativeWindow(pContextWindow->getNativeWindow()), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        glfwSetCursorPosCallback(toNativeWindow(pContextWindow->getNativeWindow()), [](GLFWwindow* window, double x, double y) {
            Gum::Window* win = (Gum::Window*)glfwGetWindowUserPointer(window);
            Event evnt;
            evnt.type = GUM_EVENT_MOUSE_MOVED;
            evnt.data.mousepos = ivec2(x, y);

            win->getMouse()->handleEvent(evnt);
        });


        glfwSetMouseButtonCallback(toNativeWindow(pContextWindow->getNativeWindow()), [](GLFWwindow* window, int button, int action, int mods) {
            Gum::Window* win = (Gum::Window*)glfwGetWindowUserPointer(window);
            Event evnt;
            evnt.type = action == GLFW_PRESS ? GUM_EVENT_MOUSE_PRESSED : GUM_EVENT_MOUSE_RELEASED;
            switch(button)
            {
                case GLFW_MOUSE_BUTTON_1: evnt.data.mousebutton = GUM_MOUSE_BUTTON_LEFT;     break;
                case GLFW_MOUSE_BUTTON_3: evnt.data.mousebutton = GUM_MOUSE_BUTTON_MIDDLE;   break;
                case GLFW_MOUSE_BUTTON_2: evnt.data.mousebutton = GUM_MOUSE_BUTTON_RIGHT;    break;
                case GLFW_MOUSE_BUTTON_4: evnt.data.mousebutton = GUM_MOUSE_BUTTON_PREVIOUS; break;
                case GLFW_MOUSE_BUTTON_5: evnt.data.mousebutton = GUM_MOUSE_BUTTON_NEXT;     break;
            }

            win->getMouse()->handleEvent(evnt);
        });

        glfwSetScrollCallback(toNativeWindow(pContextWindow->getNativeWindow()), [](GLFWwindow* window, double x, double y) {
            Gum::Window* win = (Gum::Window*)glfwGetWindowUserPointer(window);
            Event evnt;
            evnt.type = GUM_EVENT_MOUSE_SCROLL;
            evnt.data.mousescroll = ivec2(x, y);

            win->getMouse()->handleEvent(evnt);
        });

        glfwSetCursorEnterCallback(toNativeWindow(pContextWindow->getNativeWindow()), [](GLFWwindow* window, int entered) {
            Gum::Window* win = (Gum::Window*)glfwGetWindowUserPointer(window);
            Event evnt;
            evnt.type = entered ? GUM_EVENT_MOUSE_ENTERED : GUM_EVENT_MOUSE_LEFT;

            win->getMouse()->handleEvent(evnt);
        });*/
    }
    
    void Mouse::destroyNativeMouse()
    {
        /*for(auto it : cursors)
        {
            if(it.second.cursor != nullptr)
            {
                glfwDestroyCursor(it.second.cursor);
                cursors[it.first] = {it.second.glfw, nullptr};
            }
        }*/
    }


    void Mouse::hide(const bool& doHide) 			                
    { 
        //glfwSetInputMode(toNativeWindow(pContextWindow->getNativeWindow()), GLFW_CURSOR, doHide ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
    }

    void Mouse::trap(const bool& doTrap) 			                
    { 
        //glfwSetInputMode(toNativeWindow(pContextWindow->getNativeWindow()), GLFW_CURSOR, doTrap ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
    
    void Mouse::setPosition(const ivec2& pos) 
    { 
        this->v2PreviousPosition = v2Position; 
        //glfwSetCursorPos(toNativeWindow(pContextWindow->getNativeWindow()), pos.x, pos.y);
        v2Position = pos; 
    }

    void Mouse::setGlobalPosition(const ivec2& pos) 
    { 
        //Warp
    }

    void Mouse::setCursor(uint8_t shape)
    {
        /*if(cursors[shape].cursor == nullptr)
            cursors[shape].cursor = glfwCreateStandardCursor(cursors[shape].glfw);
        pActiveCursor = cursors[shape].cursor;

        glfwSetCursor(toNativeWindow(pContextWindow->getNativeWindow()), pActiveCursor);*/

        //XDefineCursor(Display::getSystemHandle(), pContextWindow->getNativeWindow(), *pActiveCursor);
        //XDefineCursor(Display::getSystemHandle(), pContextWindow->getNativeWindow(), XC_gumby);
    }

    void Mouse::pollPosition()
    {
        double x = 0, y = 0;
        //glfwGetCursorPos(toNativeWindow(pContextWindow->getNativeWindow()), &x, &y);
        v2Position = ivec2(x, y);
    }
}}