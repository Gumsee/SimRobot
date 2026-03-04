#include <System/Output.h>
#include <Desktop/Display.h>
#include <Desktop/Window.h>
#include <System/MemoryManagement.h>
#include <sstream>

namespace Gum {
namespace Display
{
    void initNativeDisplay(void* nativeptr)
	{
		/*glfwInit();


		GLFWmonitor* primarymonitor = glfwGetPrimaryMonitor();

		int count;
		GLFWmonitor** monitors = glfwGetMonitors(&count);
		for(int i = 0; i < count; i++)
		{
			GLFWmonitor* monitor = monitors[i];

			ivec2 size_mm;
			glfwGetMonitorPhysicalSize(monitor, &size_mm.x, &size_mm.y);

			ivec2 pos;
			glfwGetMonitorPos(monitor, &pos.x, &pos.y);

			std::string name = glfwGetMonitorName(monitor);
			const GLFWvidmode* videomode = glfwGetVideoMode(monitor);

			Monitor* mon = new Monitor(
				i, 
				ivec2(videomode->width, videomode->height),
				size_mm,
				pos,
				videomode->redBits + videomode->greenBits + videomode->blueBits,
				videomode->refreshRate,
				0,
				0,
				name,
				monitor == primarymonitor
			);
			
			if(mon->isPrimary())
				setPrimaryMonitor(mon);

			addMonitors(mon);
		}*/
	}

    void pollNativeEvents()
	{
   		//glfwPollEvents();
	}


    void destroyNativeDisplay()
	{
        //glfwTerminate();
	}
}}