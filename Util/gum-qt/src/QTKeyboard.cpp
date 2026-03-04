#include <Desktop/IO/Keyboard.h>
#include <Desktop/Window.h>

#include <codecvt>
#include <locale>

//static GLFWwindow* toNativeWindow(void* ptr) { return (GLFWwindow*)ptr; }

namespace Gum {
namespace IO
{
    void Keyboard::initNativeKeyboard()
    {
        /*glfwSetKeyCallback(toNativeWindow(pContextWindow->getNativeWindow()), [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            Gum::Window* win = (Gum::Window*)glfwGetWindowUserPointer(window);
            Event evnt;
            evnt.type = (action == GLFW_PRESS || action == GLFW_REPEAT) ? GUM_EVENT_KEYBOARD_PRESSED : GUM_EVENT_KEYBOARD_RELEASED;
            evnt.data.keyboardkey = key;

            win->getKeyboard()->handleEvent(evnt);
        });

        glfwSetCharCallback(toNativeWindow(pContextWindow->getNativeWindow()), [](GLFWwindow* window, unsigned int codepoint) {
            Gum::Window* win = (Gum::Window*)glfwGetWindowUserPointer(window);

            Event evnt;
            evnt.type = GUM_EVENT_KEYBOARD_TEXT_ENTERED;
            evnt.data.keyboardkey = codepoint;
            
            win->getKeyboard()->handleEvent(evnt);
        });*/
    }
    
    void Keyboard::destroyNativeKeyboard()
    {
    }

	bool Keyboard::checkKeyPressed(const int& key) const
	{ 
		//return glfwGetKey(toNativeWindow(pContextWindow->getNativeWindow()), key) == GLFW_PRESS; 
        return false;
	}

	bool Keyboard::checkKeyReleased(const int& key) const
	{ 
		//return glfwGetKey(toNativeWindow(pContextWindow->getNativeWindow()), key) == GLFW_RELEASE; 
        return false;
	}
}}