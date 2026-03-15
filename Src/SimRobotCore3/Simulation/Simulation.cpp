/**
 * @file Simulation/Simulation.cpp
 * Implementation of class Simulation
 * @author Colin Graf
 */

#include "Simulation.h"
#include "CoreModule.h"
#include "Parser/ElementCore3.h"
#include "Parser/ParserCore3.h"
#include "Platform/Assert.h"
#include "Platform/System.h"
#include "Simulation/Body.h"
#include "Simulation/Geometries/Geometry.h"
#include "Simulation/Scene.h"
#include <mujoco/mujoco.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <Desktop/Window.h>
#include "Graphics/ForwardRenderingShader.h"

Simulation* Simulation::simulation = nullptr;

Simulation::Simulation()
{
  ASSERT(!simulation);
  simulation = this;
  mju_user_error = &Simulation::mjError;
  mju_user_warning = &Simulation::mjWarning;
}

Simulation::~Simulation()
{
  for(ElementCore3* element : elements)
    delete element;

  if(data)
    mj_deleteData(data);
  if(model)
    mj_deleteModel(model);

  mju_user_error = nullptr;
  mju_user_warning = nullptr;
  ASSERT(simulation == this);
  simulation = nullptr;
}

bool Simulation::loadFile(const std::string& filename, std::list<std::string>& errors)
{
  ASSERT(!scene);
  ASSERT(elements.empty());

  ParserCore3 parser;
  if(!parser.parse(filename, errors))
  {
    if(scene)
    {
      for(ElementCore3* element : elements)
        delete element;
      elements.clear();
      scene = nullptr;
    }
    return false;
  }

  ASSERT(scene);

  spec = mj_makeSpec();
  ASSERT(spec);

  spec->compiler.degree = 0;
  spec->compiler.inertiafromgeom = mjINERTIAFROMGEOM_FALSE;
  spec->compiler.saveinertial = 1;

  spec->option.timestep = scene->stepLength;
  spec->option.apirate = scene->stepLength;
  spec->option.integrator = mjINT_EULER;
  spec->option.iterations = 50;
  spec->option.tolerance = 1e-6;
  spec->option.solver = mjSOL_NEWTON;
  spec->option.jacobian = mjJAC_AUTO;
  spec->option.gravity[0] = mjtNum(0);
  spec->option.gravity[1] = mjtNum(0);
  spec->option.gravity[2] = mjtNum(scene->gravity);

  worldBody = mjs_findBody(spec, "world");

  scene->createPhysics();

  worldBody = nullptr;

  model = mj_compile(spec, nullptr);

  if(!model)
    errors.push_back(mjs_getError(spec));

  mj_deleteSpec(spec);
  spec = nullptr;

  if(!model)
    return false;

  data = mj_makeData(model);
  ASSERT(data);

  mj_kinematics(model, data);

  Geometry::registeredGeometries.resize(model->ngeom);
  Body::registeredBodies.resize(model->nbody);
  scene->createIDs();

  forwardRenderingShader = new ShaderProgram("ForwardRenderingShader", false);
  forwardRenderingShader->addShader(new Shader(ForwardRenderingVertexShader, Shader::TYPES::VERTEX_SHADER));
  forwardRenderingShader->addShader(new Shader(ForwardRenderingFragmentShader, Shader::TYPES::FRAGMENT_SHADER));
  forwardRenderingShader->build({{"vertexPosition", 0}, {"TextureCoords", 1}, {"Normals", 2}, {"TransMatrix", 3}});


  scene->createGraphics();

  xAxisMesh = new Object3D(Mesh::generateLine(vec3(0,0,0), vec3(1.f, 0.f, 0.f)), "xAxis");
  yAxisMesh = new Object3D(Mesh::generateLine(vec3(0,0,0), vec3(0.f, 1.f, 0.f)), "yAxis");
  zAxisMesh = new Object3D(Mesh::generateLine(vec3(0,0,0), vec3(0.f, 0.f, 1.f)), "zAxis");
  dragPlaneMesh = new Object3D(Mesh::generateDisk(0.003f, 0.5f, 30), "dragPlane");
  bodyComSphereMesh = new Object3D(Mesh::generateSphere(0.003f, 10, 10), "bodyComSphereMesh");
  static const float redColor[] = {1.f, 0.f, 0.f, 1.f};
  static const float greenColor[] = {0.f, 1.f, 0.f, 1.f};
  static const float blueColor[] = {0.f, 0.f, 1.f, 1.f};
  static const float dragPlaneColor[] = {0.5f, 0.5f, 0.5f, 0.5f};
  static const float bodyComSphereColor[] = {0.8f, 0.f, 0.f, 1.f};
  xAxisSurface = graphicsContext.requestSurface(redColor, redColor);
  yAxisSurface = graphicsContext.requestSurface(greenColor, greenColor);
  zAxisSurface = graphicsContext.requestSurface(blueColor, blueColor);
  dragPlaneSurface = graphicsContext.requestSurface(dragPlaneColor, dragPlaneColor);
  bodyComSphereSurface = graphicsContext.requestSurface(bodyComSphereColor, bodyComSphereColor);

  graphicsContext.pushModelMatrixStack();
  graphicsContext.pushModelMatrixByReference(originPose);
  originModelMatrix = graphicsContext.requestModelMatrix(bGraphicsContext::ModelMatrix::origin);
  graphicsContext.popModelMatrix();
  graphicsContext.pushModelMatrixByReference(dragPlanePose);
  dragPlaneModelMatrix = graphicsContext.requestModelMatrix(bGraphicsContext::ModelMatrix::dragPlane);
  graphicsContext.popModelMatrix();
  graphicsContext.popModelMatrixStack();

  graphicsContext.compile();

  return true;
}

void Simulation::doSimulationStep()
{
  ++simulationStep;
  simulatedTime += scene->stepLength;

  mj_step1(model, data);
  scene->updateActuators();
  mj_step2(model, data);

  std::memset(data->xfrc_applied, 0, model->nbody * 6 * sizeof(mjtNum));

  Geometry::checkCollisions();

  scene->updateTransformations();

  updateFrameRate();
}

void Simulation::updateFrameRate()
{
  const unsigned int currentTime = System::getTime();
  const unsigned int timeDiff = currentTime - lastFrameRateComputationTime;
  //Only update frame rate once in two seconds
  if(timeDiff > 2000)
  {
    const float frameRate = static_cast<float>(simulationStep - lastFrameRateComputationStep) / (static_cast<float>(timeDiff) * 0.001f);
    currentFrameRate = static_cast<int>(frameRate + 0.5f);
    lastFrameRateComputationTime = currentTime;
    lastFrameRateComputationStep = simulationStep;
  }
}

void Simulation::mjError(const char* message)
{
  fprintf(stderr, "MuJoCo error: %s\n", message);
}

void Simulation::mjWarning(const char* message)
{
  fprintf(stderr, "MuJoCo warning: %s\n", message);
}

void Simulation::registerObjects()
{
  scene->fullName = scene->name.c_str();
  CoreModule::application->registerObject(*CoreModule::module, *scene, nullptr);
  scene->registerObjects();
}
