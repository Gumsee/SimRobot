#include <System/System.h>
#include <signal.h>

#if(GUM_OS_WINDOWS)
    //TODO
#elif(GUM_OS_LINUX)
    #include <dlfcn.h>
#endif

namespace Gum {
namespace System {

    std::function<void(int)> interruptCallback;

    void interruptHandler(int signal)
    {
        interruptCallback(signal);
    }

    void init()
    {
        #if(GUM_OS_WINDOWS)
            //TODO
        #elif(GUM_OS_LINUX)
            struct sigaction sigIntHandler;
            sigIntHandler.sa_handler = interruptHandler;
            sigemptyset(&sigIntHandler.sa_mask);
            sigIntHandler.sa_flags = 0;
            sigaction(SIGINT, &sigIntHandler, NULL);
        #endif
    }

    void onInterruptSignal(std::function<void(int)> callback)
    {
        interruptCallback = callback;
    }   


    void* loadDynamicLibrary(std::string filepath)
    {
        #if(GUM_OS_WINDOWS)
            //TODO
        #elif(GUM_OS_LINUX)
            return dlopen(filepath.c_str(), RTLD_LAZY | RTLD_LOCAL);
        #endif
        
        return nullptr;
    }

    void freeDynamicLibrary(void* module)
    {
        #if(GUM_OS_WINDOWS)
            //TODO
        #elif(GUM_OS_LINUX)
            dlclose(module);
        #endif
    }

}};