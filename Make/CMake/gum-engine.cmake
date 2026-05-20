#add_definitions(-DGUM_ENGINE_NO_SHADOWMAP)
add_definitions(-DGUM_PRIMITIVES_MESH_UP_Z)
add_definitions(-DGUM_SHADOWMAP_WITH_TRANSPARENT_TEXTURES)
if(${CMAKE_BUILD_TYPE} MATCHES "Debug" OR ${CMAKE_BUILD_TYPE} MATCHES "Develop")
  add_definitions(-DCHECK_GL_ERRORS)
endif()

message("Buildtype ")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-DGUM_OS_LINUX)
    add_definitions(-DGUM_OS_UNIX)
    set(GUM_OS_LINUX true)
    set(GUM_OS_UNIX true)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions(-DGUM_OS_WINDOWS)
    add_definitions(-DGUM_OS_DOSBASED)
    set(GUM_OS_WINDOWS true)
    set(GUM_OS_DOSBASED true)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-DGUM_OS_MACOS)
    add_definitions(-DGUM_OS_UNIX)
    set(GUM_OS_MACOS true)
    set(GUM_OS_UNIX true)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    add_definitions(-DGUM_OS_ANDROID)
    add_definitions(-DGUM_OS_UNIX)
    set(GUM_OS_ANDROID true)
    set(GUM_OS_UNIX true)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    add_definitions(-DGUM_OS_BSD)
    add_definitions(-DGUM_OS_UNIX)
    set(GUM_OS_BSD true)
    set(GUM_OS_UNIX true)
endif()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fPIC -ggdb -fno-omit-frame-pointer") #-O3
set (CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fPIC -ggdb -fno-omit-frame-pointer") #-O3
add_link_options(-fPIC)# -fuse-ld=mold)

set(CMAKE_INSTALL_LIBDIR "/usr/local/lib")
set(DISABLE_PACKAGE_CONFIGURATION "ON")
set(GUMGLFW_FOUND "YES")
macro(add_gumlibrary projname varname)
  set(CMAKE_PROJECT_NAME ${projname})
  set(PROJECT_NAME ${projname})
  include("${SIMROBOT_PREFIX}/Util/${projname}/src/CMakeLists.txt")
  set(${varname}_FOUND "YES")
  set(${varname}_INCLUDE_DIRS "${SIMROBOT_PREFIX}/Util/${projname}/src/")
  include_directories(SYSTEM "${SIMROBOT_PREFIX}/Util/${projname}/src/")
endmacro()

find_package(OpenGL)
add_gumlibrary(gum-maths GUMMATHS)
add_gumlibrary(gum-system GUMSYSTEM)
add_gumlibrary(gum-essentials GUMESSENTIALS)
add_gumlibrary(gum-primitives GUMPRIMITIVES)
add_gumlibrary(gum-codecs GUMCODECS)
add_gumlibrary(gum-desktop GUMDESKTOP)
add_gumlibrary(gum-graphics GUMGRAPHICS)
add_gumlibrary(gum-opengl GUMOPENGL)

include_directories(SYSTEM "${SIMROBOT_PREFIX}/Util/gum-desktop/external/tinyfd")
include_directories(SYSTEM "${SIMROBOT_PREFIX}/Util/gum-codecs/external")
include_directories(SYSTEM "${SIMROBOT_PREFIX}/Util/gum-engine/src")
include_directories(SYSTEM "${SIMROBOT_PREFIX}/Util/gum-qt/src")

if(APPLE)
  add_definitions(-DGUM_OS_MACOS)
elseif(WIN32)
  add_definitions(-DGUM_OS_WINDOWS)
else()
  add_definitions(-DGUM_OS_LINUX)
endif()


set(GUM_ENGINE_SRC 
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/gum-engine.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Rendering/Camera.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Rendering/CurveRenderer.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Rendering/Canvas.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Rendering/Renderer.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Rendering/Gizmo.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Rendering/World.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Rendering/IDRenderer.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Object/Skeletal/BoneRenderer/BoneRenderer.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Object/Skeletal/Skeleton.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Object/Skeletal/AnimatedModel.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Object/Skeletal/RagdollObjectInstance.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Object/PhysicsObjectInstance.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Object/Terrain/Terrain.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Object/Vehicle/Vehicle.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Object/CollisionObject.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Object/NormalRenderer/NormalRenderer.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Object/ObjectManager.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Object/SoftBody.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Object/SceneObject.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Object/Skybox.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Camera3D.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Lightning/Lightning.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Lightning/ShadowMapping/ShadowMapping.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/World3D.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Renderer3D.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/3D/Grid.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Lightning/LightManager.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Lightning/G_Buffer.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Lightning/DirectionalLight.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Lightning/SpotLight.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Lightning/Light.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Lightning/PointLight.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/General/Animation.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/PostProcessing/PostProcessing.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/PostProcessing/Effects/GaussianBlur/GaussianBlur.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/PostProcessing/Effects/CombineFramebuffer.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/PostProcessing/Effects/BloomEffect/BloomEffect.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/PostProcessing/Effects/Difference/Difference.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/PostProcessing/Effects/CombineTextures/CombineTextures.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/PostProcessing/Effects/PostProcessingEffect.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/PostProcessing/Effects/Brightfilter/Brightfilter.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/PostProcessing/Effects/ContrastChanger/ContrastChanger.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/PostProcessing/ImageCorrection/HighDynamicRange.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/PostProcessing/OutlineRenderer/OutlineRenderer.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/PostProcessing/SSAO.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/2D/Camera2D.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/2D/World2D.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/2D/Layer/Background.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/2D/Layer/Layer.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/2D/Layer/TileMap.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/2D/Renderer2D.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Physics/Physics.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Particle/Particle.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Particle/ShaderInitializer.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Particle/ParticleSystem.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Particle/Billboard.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Texture/ReflectiveTextureCube.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Texture/PerlinNoise.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Texture/PerlinNoise3D.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Texture/EnvironmentMap.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Texture/ClipPlaneTexture2D.cpp
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Texture/ReflectiveTexture2D.cpp

    ${SIMROBOT_PREFIX}/Util/gum-qt/src/QTContext.cpp
    ${SIMROBOT_PREFIX}/Util/gum-qt/src/QTDisplay.cpp
    ${SIMROBOT_PREFIX}/Util/gum-qt/src/QTKeyboard.cpp
    ${SIMROBOT_PREFIX}/Util/gum-qt/src/QTTouch.cpp
    ${SIMROBOT_PREFIX}/Util/gum-qt/src/QTMouse.cpp
    ${SIMROBOT_PREFIX}/Util/gum-qt/src/QTWindow.cpp
    ${SIMROBOT_PREFIX}/Util/gum-qt/src/QTKeybinds.cpp
    ${SIMROBOT_PREFIX}/Util/gum-qt/src/UnixSystem.cpp
)
#set(CMAKE_CXX_FLAGS "-g")
#set(CMAKE_BUILD_TYPE RelWithDebInfo)
add_library(gum-engine SHARED ${GUM_ENGINE_SRC})
if(APPLE)
  target_link_libraries(gum-engine PRIVATE 
    Qt6::Core Qt6::Gui Qt6::OpenGL Qt6::OpenGLWidgets Qt6::Widgets
    gum-opengl
    gum-graphics
    gum-essentials 
    gum-codecs 
    gum-primitives 
    gum-desktop 
    gum-system 
    gum-maths
  )
else()
  target_link_libraries(gum-engine PRIVATE 
    Qt6::Core Qt6::Gui Qt6::OpenGL Qt6::OpenGLWidgets Qt6::Widgets OpenGL GLU minizip GLEW 
    $<LINK_GROUP:RESCAN,gum-opengl,gum-graphics> 
    gum-essentials 
    gum-codecs 
    gum-primitives 
    gum-desktop 
    gum-system 
    gum-maths
  )
endif()


set_property(TARGET gum-engine PROPERTY LIBRARY_OUTPUT_DIRECTORY "${SIMROBOT_LIBRARY_DIR}")

include_directories(${CMAKE_CURRENT_BINARY_DIR}/)
