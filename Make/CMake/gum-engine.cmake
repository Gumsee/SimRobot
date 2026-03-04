#add_definitions(-DGUM_ENGINE_NO_SHADOWMAP)
add_definitions(-DGUM_PRIMITIVES_MESH_UP_Z)


add_library(gum-maths SHARED IMPORTED)
set_target_properties(gum-maths PROPERTIES IMPORTED_LOCATION "${SIMROBOT_PREFIX}/Build/Linux/gum-maths/libGumMaths.a")

add_library(gum-system SHARED IMPORTED)
set_target_properties(gum-system PROPERTIES IMPORTED_LOCATION "${SIMROBOT_PREFIX}/Build/Linux/gum-system/libGumSystem.a")

add_library(gum-essentials SHARED IMPORTED)
set_target_properties(gum-essentials PROPERTIES IMPORTED_LOCATION "${SIMROBOT_PREFIX}/Build/Linux/gum-essentials/libGumEssentials.a")

add_library(gum-primitives SHARED IMPORTED)
set_target_properties(gum-primitives PROPERTIES IMPORTED_LOCATION "${SIMROBOT_PREFIX}/Build/Linux/gum-primitives/src/libGumPrimitives.a")

add_library(gum-codecs SHARED IMPORTED)
set_target_properties(gum-codecs PROPERTIES IMPORTED_LOCATION "${SIMROBOT_PREFIX}/Build/Linux/gum-codecs/libGumCodecs.a")

add_library(gum-graphics SHARED IMPORTED)
set_target_properties(gum-graphics PROPERTIES IMPORTED_LOCATION "${SIMROBOT_PREFIX}/Build/Linux/gum-graphics/libGumGraphics.a")

add_library(gum-opengl SHARED IMPORTED)
set_target_properties(gum-opengl PROPERTIES IMPORTED_LOCATION "${SIMROBOT_PREFIX}/Build/Linux/gum-opengl/libGumOpenGL.a")

add_library(gum-desktop SHARED IMPORTED)
set_target_properties(gum-desktop PROPERTIES IMPORTED_LOCATION "${SIMROBOT_PREFIX}/Build/Linux/gum-desktop/libGumDesktop.a")


if(APPLE)
  add_definitions(-DGUM_OS_MACOS)
elseif(WIN32)
  add_definitions(-DGUM_OS_WINDOWS)
else()
  add_definitions(-DGUM_OS_LINUX)
endif()

add_link_options(-fPIC)

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
    ${SIMROBOT_PREFIX}/Util/gum-engine/src/Engine/Material/MaterialManager.cpp
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
set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_BUILD_TYPE RelWithDebInfo)
add_library(gum-engine SHARED ${GUM_ENGINE_SRC})
target_link_libraries(gum-engine PRIVATE 
  Qt6::Core Qt6::Gui Qt6::OpenGL Qt6::OpenGLWidgets Qt6::Widgets 
  gum-primitives gum-graphics gum-opengl gum-codecs gum-desktop gum-maths gum-system gum-essentials
)
set_property(TARGET gum-engine PROPERTY LIBRARY_OUTPUT_DIRECTORY "${SIMROBOT_LIBRARY_DIR}")

include_directories(${CMAKE_CURRENT_BINARY_DIR}/)

include_directories(${SIMROBOT_PREFIX}/Util/gum-graphics/src/)
include_directories(${SIMROBOT_PREFIX}/Util/gum-primitives/src/)
include_directories(${SIMROBOT_PREFIX}/Util/gum-maths/src/)
include_directories(${SIMROBOT_PREFIX}/Util/gum-essentials/src/)
include_directories(${SIMROBOT_PREFIX}/Util/gum-system/src/)
include_directories(${SIMROBOT_PREFIX}/Util/gum-codecs/src/)
include_directories(${SIMROBOT_PREFIX}/Util/gum-codecs/external/assimp/include)
include_directories(${SIMROBOT_PREFIX}/Util/gum-codecs/external/assimp/code)
include_directories(${SIMROBOT_PREFIX}/Util/gum-codecs/external/libxml2/include)
include_directories(${SIMROBOT_PREFIX}/Util/gum-codecs/external/zlib/)
include_directories(${SIMROBOT_PREFIX}/Util/gum-codecs/external/)
include_directories(${SIMROBOT_PREFIX}/Util/gum-desktop/src/)
include_directories(${SIMROBOT_PREFIX}/Util/gum-desktop/external/tinyfd/)
include_directories(${SIMROBOT_PREFIX}/Util/gum-engine/src/)