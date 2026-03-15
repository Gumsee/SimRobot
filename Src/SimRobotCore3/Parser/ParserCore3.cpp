/**
 * @file ParserCore3.cpp
 * Implementation of class ParserCore3
 * @author Colin Graf
 */

#include "ParserCore3.h"
#include "Graphics/SimLight.h"
#include "Platform/Assert.h"
#include "Simulation/Actuators/Hinge.h"
#include "Simulation/Actuators/Slider.h"
#include "Simulation/Appearances/BoxAppearance.h"
#include "Simulation/Appearances/CapsuleAppearance.h"
#include "Simulation/Appearances/ComplexAppearance.h"
#include "Simulation/Appearances/CylinderAppearance.h"
#include "Simulation/Appearances/SphereAppearance.h"
#include "Simulation/Axis.h"
#include "Simulation/Body.h"
#include "Simulation/Compound.h"
#include "Simulation/Geometries/BoxGeometry.h"
#include "Simulation/Geometries/CapsuleGeometry.h"
#include "Simulation/Geometries/CylinderGeometry.h"
#include "Simulation/Geometries/SphereGeometry.h"
#include "Simulation/Masses/BoxMass.h"
#include "Simulation/Masses/CapsuleMass.h"
#include "Simulation/Masses/CylinderMass.h"
#include "Simulation/Masses/InertiaMatrixMass.h"
#include "Simulation/Masses/SphereMass.h"
#include "Simulation/Motors/PT2Motor.h"
#include "Simulation/Motors/ServoMotor.h"
#include "Simulation/Motors/VelocityMotor.h"
#include "Simulation/Scene.h"
#include "Simulation/Sensors/Accelerometer.h"
#include "Simulation/Sensors/CameraSensor.h"
#include "Simulation/Sensors/CollisionSensor.h"
#include "Simulation/Sensors/DepthImageSensor.h"
#include "Simulation/Sensors/Gyroscope.h"
#include "Simulation/Sensors/ObjectSegmentedImageSensor.h"
#include "Simulation/Sensors/SingleDistanceSensor.h"
#include "Simulation/Simulation.h"
#include "Simulation/UserInput.h"

ParserCore3::ParserCore3()
{
  using namespace std::placeholders;

  elements =
  {
    // { element, class, handler, text handler, flags
    //   required children, optional children, repeatable children, path attributes }
    {"Include", infrastructureClass, std::bind(&ParserCore3::includeElement, this), nullptr, 0,
      0, 0, 0, {}},
    {"Simulation", infrastructureClass, std::bind(&ParserCore3::simulationElement, this), nullptr, 0,
      sceneClass, 0, 0, {}},

    {"Set", setClass, std::bind(&ParserCore3::setElement, this), nullptr, 0,
      0, 0, 0, {}},

    {"Scene", sceneClass, std::bind(&ParserCore3::sceneElement, this), nullptr, 0,
      0, 0, setClass | bodyClass | compoundClass | lightClass | userInputClass, {}},
    {"DirLight", lightClass, std::bind(&ParserCore3::dirLightElement, this), nullptr, 0,
      0, 0, 0, {}},
    {"PointLight", lightClass, std::bind(&ParserCore3::pointLightElement, this), nullptr, 0,
      0, 0, 0, {}},
    {"SpotLight", lightClass, std::bind(&ParserCore3::spotLightElement, this), nullptr, 0,
      0, 0, 0, {}},

    {"Compound", compoundClass, std::bind(&ParserCore3::compoundElement, this), nullptr, 0,
      0, translationClass | rotationClass, setClass | jointClass | compoundClass | bodyClass | appearanceClass | geometryClass | extSensorClass | userInputClass, {}},
    {"Body", bodyClass, std::bind(&ParserCore3::bodyElement, this), nullptr, 0,
      massClass, translationClass | rotationClass, setClass | jointClass | appearanceClass | geometryClass | massClass | intSensorClass | extSensorClass | userInputClass, {}},

    {"Translation", translationClass, std::bind(&ParserCore3::translationElement, this), nullptr, 0,
      0, 0, 0, {}},
    {"Rotation", rotationClass, std::bind(&ParserCore3::rotationElement, this), nullptr, 0,
      0, 0, 0, {}},

    {"Mass", massClass, std::bind(&ParserCore3::massElement, this), nullptr, constantFlag,
      0, translationClass | rotationClass, setClass | massClass, {}},
    {"BoxMass", massClass, std::bind(&ParserCore3::boxMassElement, this), nullptr, constantFlag,
      0, translationClass | rotationClass, setClass | massClass, {}},
    {"SphereMass", massClass, std::bind(&ParserCore3::sphereMassElement, this), nullptr, constantFlag,
      0, translationClass | rotationClass, setClass | massClass, {}},
    {"InertiaMatrixMass", massClass, std::bind(&ParserCore3::inertiaMatrixMassElement, this), nullptr, constantFlag,
      0, translationClass | rotationClass, setClass | massClass, {}},
    {"CapsuleMass", massClass, std::bind(&ParserCore3::capsuleMassElement, this), nullptr, constantFlag,
      0, translationClass | rotationClass, setClass | massClass, {}},
    {"CylinderMass", massClass, std::bind(&ParserCore3::cylinderMassElement, this), nullptr, constantFlag,
      0, translationClass | rotationClass, setClass | massClass, {}},

    {"Geometry", geometryClass, std::bind(&ParserCore3::geometryElement, this), nullptr, 0,
      0, translationClass | rotationClass | materialClass, setClass | geometryClass, {}},
    {"BoxGeometry", geometryClass, std::bind(&ParserCore3::boxGeometryElement, this), nullptr, 0,
      0, translationClass | rotationClass | materialClass, setClass | geometryClass, {}},
    {"SphereGeometry", geometryClass, std::bind(&ParserCore3::sphereGeometryElement, this), nullptr, 0,
      0, translationClass | rotationClass | materialClass, setClass | geometryClass, {}},
    {"CylinderGeometry", geometryClass, std::bind(&ParserCore3::cylinderGeometryElement, this), nullptr, 0,
      0, translationClass | rotationClass | materialClass, setClass | geometryClass, {}},
    {"CapsuleGeometry", geometryClass, std::bind(&ParserCore3::capsuleGeometryElement, this), nullptr, 0,
      0, translationClass | rotationClass | materialClass, setClass | geometryClass, {}},

    {"Material", materialClass, std::bind(&ParserCore3::materialElement, this), nullptr, constantFlag,
      0, 0, 0, {}},

    {"Appearance", appearanceClass, std::bind(&ParserCore3::appearanceElement, this), nullptr, 0,
      0, translationClass | rotationClass, setClass | appearanceClass, {}},
    {"BoxAppearance", appearanceClass, std::bind(&ParserCore3::boxAppearanceElement, this), nullptr, 0,
      surfaceClass, translationClass | rotationClass, setClass | appearanceClass, {}},
    {"SphereAppearance", appearanceClass, std::bind(&ParserCore3::sphereAppearanceElement, this), nullptr, 0,
      surfaceClass, translationClass | rotationClass, setClass | appearanceClass, {}},
    {"CylinderAppearance", appearanceClass, std::bind(&ParserCore3::cylinderAppearanceElement, this), nullptr, 0,
      surfaceClass, translationClass | rotationClass, setClass | appearanceClass, {}},
    {"CapsuleAppearance", appearanceClass, std::bind(&ParserCore3::capsuleAppearanceElement, this), nullptr, 0,
      surfaceClass, translationClass | rotationClass, setClass | appearanceClass, {}},
    {"ComplexAppearance", appearanceClass, std::bind(&ParserCore3::complexAppearanceElement, this), nullptr, 0,
      surfaceClass | verticesClass | primitiveGroupClass, translationClass | rotationClass | normalsClass | texCoordsClass, setClass | primitiveGroupClass | appearanceClass, {}},

    {"Vertices", verticesClass, std::bind(&ParserCore3::verticesElement, this), std::bind(&ParserCore3::verticesText, this, _1, _2), textFlag | constantFlag,
      0, 0, 0, {}},
    {"Normals", normalsClass, std::bind(&ParserCore3::normalsElement, this), std::bind(&ParserCore3::normalsText, this, _1, _2), textFlag | constantFlag,
      0, 0, 0, {}},
    {"TexCoords", texCoordsClass, std::bind(&ParserCore3::texCoordsElement, this), std::bind(&ParserCore3::texCoordsText, this, _1, _2), textFlag | constantFlag,
      0, 0, 0, {}},
    {"Triangles", primitiveGroupClass, std::bind(&ParserCore3::trianglesElement, this), std::bind(&ParserCore3::trianglesAndQuadsText, this, _1, _2), textFlag | constantFlag,
      0, 0, 0, {}},
    {"Quads", primitiveGroupClass, std::bind(&ParserCore3::quadsElement, this), std::bind(&ParserCore3::trianglesAndQuadsText, this, _1, _2), textFlag | constantFlag,
      0, 0, 0, {}},

    {"Surface", surfaceClass, std::bind(&ParserCore3::surfaceElement, this), nullptr, constantFlag,
      0, 0, 0, {"diffuseTexture"}},

    {"Hinge", jointClass, std::bind(&ParserCore3::hingeElement, this), nullptr, 0,
      bodyClass | axisClass, translationClass | rotationClass, setClass, {}},
    {"Slider", jointClass, std::bind(&ParserCore3::sliderElement, this), nullptr, 0,
      bodyClass | axisClass, translationClass | rotationClass, setClass, {}},
    {"Axis", axisClass, std::bind(&ParserCore3::axisElement, this), nullptr, 0,
      0, motorClass | deflectionClass, setClass, {}},
    {"Deflection", deflectionClass, std::bind(&ParserCore3::deflectionElement, this), nullptr, 0,
      0, 0, 0, {}},
    {"PT2Motor", motorClass, std::bind(&ParserCore3::PT2MotorElement, this), nullptr, 0,
      0, 0, 0, {}},
    {"ServoMotor", motorClass, std::bind(&ParserCore3::servoMotorElement, this), nullptr, 0,
      0, 0, 0, {}},
    {"VelocityMotor", motorClass, std::bind(&ParserCore3::velocityMotorElement, this), nullptr, 0,
      0, 0, 0, {}},

    {"Gyroscope", intSensorClass, std::bind(&ParserCore3::gyroscopeElement, this), nullptr, 0,
      0, translationClass | rotationClass, 0, {}},
    {"Accelerometer", intSensorClass, std::bind(&ParserCore3::accelerometerElement, this), nullptr, 0,
      0, translationClass | rotationClass, 0, {}},
    {"Camera", extSensorClass, std::bind(&ParserCore3::cameraElement, this), nullptr, 0,
      0, translationClass | rotationClass, 0, {}},
    {"CollisionSensor", intSensorClass, std::bind(&ParserCore3::collisionSensorElement, this), nullptr, 0,
      0, translationClass | rotationClass, geometryClass, {}},
    {"ObjectSegmentedImageSensor", extSensorClass, std::bind(&ParserCore3::objectSegmentedImageSensorElement, this), nullptr, 0,
      0, translationClass | rotationClass, 0, {}},
    {"SingleDistanceSensor", extSensorClass, std::bind(&ParserCore3::singleDistanceSensorElement, this), nullptr, 0,
      0, translationClass | rotationClass, 0, {}},
    {"DepthImageSensor", extSensorClass, std::bind(&ParserCore3::depthImageSensorElement, this), nullptr, 0,
      0, translationClass | rotationClass, 0, {}},

    {"UserInput", userInputClass, std::bind(&ParserCore3::userInputElement, this), nullptr, 0,
      0, 0, 0, {}},
  };

  for(const ElementInfo& element : elements)
    elementInfos[element.name] = &element;
}

bool ParserCore3::getColor(const char* key, bool required, color& colors)
{
  unsigned char colorsUC[4];
  if(!Parser::getColor(key, required, colorsUC))
    return false;
  colors.r = colorsUC[0];
  colors.g = colorsUC[1];
  colors.b = colorsUC[2];
  colors.a = colorsUC[3];
  return true;
}

Element* ParserCore3::setElement()
{
  ASSERT(element);
  const std::string& name = getString("name", true);
  const std::string& value = getString("value", true);
  auto& vars = elementData->parent->vars;
  if(vars.find(name) == vars.end())
    vars[name] = value;
  return nullptr;
}

Element* ParserCore3::sceneElement()
{
  Scene* scene = new Scene(getString("name", false));
  scene->controller = getString("controller", false);
  getColor("color", false, scene->color);
  scene->stepLength = getTimeNonZeroPositive("stepLength", false, 0.01f);
  scene->gravity = getAcceleration("gravity", false, -9.80665f);
  scene->detectBodyCollisions = getBool("bodyCollisions", false, true);

  ASSERT(!Simulation::simulation->scene);
  Simulation::simulation->scene = scene;
  return scene;
}

Element* ParserCore3::dirLightElement()
{
  SimDirLight* light = new SimDirLight();
  color diffuseColor, ambientColor, specularColor;
  getColor("diffuseColor", false, diffuseColor);
  getColor("ambientColor", false, ambientColor);
  getColor("specularColor", false, specularColor);
  light->setDirection(vec3(
    getFloatMinMax("x", false, light->getDirection()->x, -1.f, 1.f),
    getFloatMinMax("y", false, light->getDirection()->y, -1.f, 1.f),
    getFloatMinMax("z", false, light->getDirection()->z, -1.f, 1.f)
  ));
  return light;
}

Element* ParserCore3::pointLightElement()
{
  SimPointLight* light = new SimPointLight();
  color diffuseColor, ambientColor, specularColor;
  getColor("diffuseColor", false, diffuseColor);
  getColor("ambientColor", false, ambientColor);
  getColor("specularColor", false, specularColor);
  light->setPosition(vec3(
    getLength("x", false, light->getPosition().x, false),
    getLength("y", false, light->getPosition().y, false),
    getLength("z", false, light->getPosition().z, false)
  ));
  //light->constantAttenuation = getFloatPositive("constantAttenuation", false, light->constantAttenuation);
  //light->linearAttenuation = getFloatPositive("linearAttenuation", false, light->linearAttenuation);
  //light->quadraticAttenuation = getFloatPositive("quadraticAttenuation", false, light->quadraticAttenuation);
  return light;
}

Element* ParserCore3::spotLightElement()
{
  SimSpotLight* light = new SimSpotLight();
  color diffuseColor, ambientColor, specularColor;
  getColor("diffuseColor", false, diffuseColor);
  getColor("ambientColor", false, ambientColor);
  getColor("specularColor", false, specularColor);
  light->setPosition(vec3(
    getLength("x", false, light->getPosition().x, false),
    getLength("y", false, light->getPosition().y, false),
    getLength("z", false, light->getPosition().z, false)
  ));

  light->setDirection(vec3(
    getLength("dirX", false, light->getDirection().x, false),
    getLength("dirY", false, light->getDirection().y, false),
    getLength("dirZ", false, light->getDirection().z, false)
  ));
  //light->constantAttenuation = getFloatPositive("constantAttenuation", false, light->constantAttenuation);
  //light->linearAttenuation = getFloatPositive("linearAttenuation", false, light->linearAttenuation);
  //light->quadraticAttenuation = getFloatPositive("quadraticAttenuation", false, light->quadraticAttenuation);
  //light->cutoff = getFloatMinMax("cutoff", false, light->cutoff, 0.f, 1.f);
  return light;
}

Element* ParserCore3::bodyElement()
{
  return new Body(getString("name", false));
}

Element* ParserCore3::compoundElement()
{
  return new Compound(getString("name", false));
}

Element* ParserCore3::hingeElement()
{
  return new Hinge(getString("name", false));
}

Element* ParserCore3::sliderElement()
{
  return new Slider(getString("name", false));
}

Element* ParserCore3::massElement()
{
  return new Mass(getString("name", false));
}

Element* ParserCore3::boxMassElement()
{
  BoxMass* boxMass = new BoxMass(getString("name", false));
  boxMass->value = getMass("value", true, 0.f);
  boxMass->width = getLength("width", true, 0.f, true);
  boxMass->height = getLength("height", true, 0.f, true);
  boxMass->depth = getLength("depth", true, 0.f, true);
  return boxMass;
}

Element* ParserCore3::sphereMassElement()
{
  SphereMass* sphereMass = new SphereMass(getString("name", false));
  sphereMass->value = getMass("value", true, 0.f);
  sphereMass->radius = getLength("radius", true, 0.f, true);
  return sphereMass;
}

Element* ParserCore3::inertiaMatrixMassElement()
{
  InertiaMatrixMass* inertiaMatrixMass = new InertiaMatrixMass(getString("name", false));
  inertiaMatrixMass->value = getMass("value", true, 0.f);
  inertiaMatrixMass->x = getLength("x", false, 0.f, false);
  inertiaMatrixMass->y = getLength("y", false, 0.f, false);
  inertiaMatrixMass->z = getLength("z", false, 0.f, false);
  inertiaMatrixMass->ixx = getMassLengthLength("ixx", true, 0.f);
  inertiaMatrixMass->ixy = getMassLengthLength("ixy", false, 0.f);
  inertiaMatrixMass->ixz = getMassLengthLength("ixz", false, 0.f);
  inertiaMatrixMass->iyy = getMassLengthLength("iyy", true, 0.f);
  inertiaMatrixMass->iyz = getMassLengthLength("iyz", false, 0.f);
  inertiaMatrixMass->izz = getMassLengthLength("izz", true, 0.f);
  return inertiaMatrixMass;
}

Element* ParserCore3::capsuleMassElement()
{
  CapsuleMass* capsuleMass = new CapsuleMass(getString("name", false));
  capsuleMass->value = getMass("value", true, 0.f);
  capsuleMass->radius = getLength("radius", true, 0.f, true);
  capsuleMass->height = getLength("height", true, 0.f, true);
  return capsuleMass;
}

Element* ParserCore3::cylinderMassElement()
{
  CylinderMass* cylinderMass = new CylinderMass(getString("name", false));
  cylinderMass->value = getMass("value", true, 0.f);
  cylinderMass->radius = getLength("radius", true, 0.f, true);
  cylinderMass->height = getLength("height", true, 0.f, true);
  return cylinderMass;
}

Element* ParserCore3::geometryElement()
{
  return new Geometry(getString("name", false));
}

Element* ParserCore3::boxGeometryElement()
{
  BoxGeometry* boxGeometry = new BoxGeometry(getString("name", false));
  getColor("color", false, boxGeometry->color);
  boxGeometry->width = getLength("width", true, 0.f, true);
  boxGeometry->height = getLength("height", true, 0.f, true);
  boxGeometry->depth = getLength("depth", true, 0.f, true);
  return boxGeometry;
}

Element* ParserCore3::sphereGeometryElement()
{
  SphereGeometry* sphereGeometry = new SphereGeometry(getString("name", false));
  getColor("color", false, sphereGeometry->color);
  sphereGeometry->radius = getLength("radius", true, 0.f, true);
  return sphereGeometry;
}

Element* ParserCore3::cylinderGeometryElement()
{
  CylinderGeometry* cylinderGeometry = new CylinderGeometry(getString("name", false));
  getColor("color", false, cylinderGeometry->color);
  cylinderGeometry->radius = getLength("radius", true, 0.f, true);
  cylinderGeometry->height = getLength("height", true, 0.f, true);
  return cylinderGeometry;
}

Element* ParserCore3::capsuleGeometryElement()
{
  CapsuleGeometry* capsuleGeometry = new CapsuleGeometry(getString("name", false));
  getColor("color", false, capsuleGeometry->color);
  capsuleGeometry->radius = getLength("radius", true, 0.f, true);
  capsuleGeometry->height = getLength("height", true, 0.f, true);
  return capsuleGeometry;
}

Element* ParserCore3::materialElement()
{
  Geometry::Material* material = new Geometry::Material();
  material->friction = getFloatPositive("friction", false, 0.f);
  material->rollingFriction = getFloatPositive("rollingFriction", false, 0.f);
  return material;
}

Element* ParserCore3::appearanceElement()
{
  return new Appearance(getString("name", false));
}

Element* ParserCore3::boxAppearanceElement()
{
  BoxAppearance* boxAppearance = new BoxAppearance(getString("name", false));
  boxAppearance->width = getLength("width", true, 0.f, true);
  boxAppearance->height = getLength("height", true, 0.f, true);
  boxAppearance->depth = getLength("depth", true, 0.f, true);
  boxAppearance->renderForward = getString("render", false) == "forward";
  return boxAppearance;
}

Element* ParserCore3::sphereAppearanceElement()
{
  SphereAppearance* sphereAppearance = new SphereAppearance(getString("name", false));
  sphereAppearance->radius = getLength("radius", true, 0.f, true);
  sphereAppearance->renderForward = getString("render", false) == "forward";
  return sphereAppearance;
}

Element* ParserCore3::cylinderAppearanceElement()
{
  CylinderAppearance* cylinderAppearance = new CylinderAppearance(getString("name", false));
  cylinderAppearance->height = getLength("height", true, 0.f, true);
  cylinderAppearance->radius = getLength("radius", true, 0.f, true);
  cylinderAppearance->renderForward = getString("render", false) == "forward";
  return cylinderAppearance;
}

Element* ParserCore3::capsuleAppearanceElement()
{
  CapsuleAppearance* capsuleAppearance = new CapsuleAppearance(getString("name", false));
  capsuleAppearance->height = getLength("height", true, 0.f, true);
  capsuleAppearance->radius = getLength("radius", true, 0.f, true);
  capsuleAppearance->renderForward = getString("render", false) == "forward";
  return capsuleAppearance;
}

Element* ParserCore3::complexAppearanceElement()
{
  ComplexAppearance* complexAppearance = new ComplexAppearance(getString("name", false));
  complexAppearance->renderForward = getString("render", false) == "forward";
  return complexAppearance;
}

Element* ParserCore3::trianglesElement()
{
  ComplexAppearance* parent = dynamic_cast<ComplexAppearance*>(element);
  ComplexAppearance::PrimitiveGroup* primitiveGroup = new ComplexAppearance::PrimitiveGroup();
  primitiveGroup->mode = ComplexAppearance::triangles;
  parent->primitiveGroups.push_back(primitiveGroup);
  return primitiveGroup;
}

void ParserCore3::trianglesAndQuadsText(std::string& text, Location location)
{
  ComplexAppearance::PrimitiveGroup* primitiveGroup = dynamic_cast<ComplexAppearance::PrimitiveGroup*>(element);
  
  const char* str = text.c_str();
  char* nextStr;
  unsigned int l;
  skipWhitespace(str, location);
  while(*str)
  {
    while(*str == '#') { while(*str && *str != '\n' && *str != '\r') { ++str; ++location.column; } skipWhitespace(str, location); if(!*str) return; }
    l = static_cast<unsigned int>(std::strtol(str, &nextStr, 10));
    if(str == nextStr)
    {
      handleError("Invalid index text (must be a space separated list of integers)", location);
      return;
    }
    location.column += nextStr - str;
    str = nextStr;
    skipWhitespace(str, location);
    primitiveGroup->indices.push_back(l);
  }
}

Element* ParserCore3::quadsElement()
{
  ComplexAppearance* parent = dynamic_cast<ComplexAppearance*>(element);
  ComplexAppearance::PrimitiveGroup* primitiveGroup = new ComplexAppearance::PrimitiveGroup();
  primitiveGroup->mode = ComplexAppearance::quads;
  parent->primitiveGroups.push_back(primitiveGroup);
  return primitiveGroup;
}

Element* ParserCore3::verticesElement()
{
  ComplexAppearance::Vertices* vertices = new ComplexAppearance::Vertices();
  vertices->unit = getUnit("unit", false, 1);
  return vertices;
}

void ParserCore3::verticesText(std::string& text, Location location)
{
  ComplexAppearance::Vertices* vertices = dynamic_cast<ComplexAppearance::Vertices*>(element);
  const char* str = text.c_str();
  char* nextStr;
  vec3 components;
  skipWhitespace(str, location);
  while(*str)
  {
    for(unsigned int i = 0; i < 3; ++i)
    {
      while(*str == '#') { while(*str && *str != '\n' && *str != '\r') { ++str; ++location.column; } skipWhitespace(str, location); if(!*str) return; }
      components[i] = std::strtof(str, &nextStr);
      if(str == nextStr)
      {
        handleError("Invalid vertex text (must be a space separated list of floats)", location);
        return;
      }
      location.column += nextStr - str;
      str = nextStr;
      skipWhitespace(str, location);
    }
    components.x *= vertices->unit;
    components.y *= vertices->unit;
    components.z *= vertices->unit;
    vertices->data.emplace_back(components);
  }
}


Element* ParserCore3::normalsElement()
{
  return new ComplexAppearance::Normals();
}

void ParserCore3::normalsText(std::string& text, Location location)
{
  ComplexAppearance::Normals* normals = dynamic_cast<ComplexAppearance::Normals*>(element);
  
  const char* str = text.c_str();
  char* nextStr;
  vec3 components;
  skipWhitespace(str, location);
  while(*str)
  {
    for(unsigned int i = 0; i < 3; ++i)
    {
      while(*str == '#') { while(*str && *str != '\n' && *str != '\r') { ++str; ++location.column; } skipWhitespace(str, location); if(!*str) return; }
      components[i] = std::strtof(str, &nextStr);
      if(str == nextStr)
      {
        handleError("Invalid normal text (must be a space separated list of floats)", location);
        return;
      }
      location.column += nextStr - str;
      str = nextStr;
      skipWhitespace(str, location);
    }
    normals->data.emplace_back(components);
  }
}

Element* ParserCore3::texCoordsElement()
{
  return new ComplexAppearance::TexCoords();
}

void ParserCore3::texCoordsText(std::string& text, Location location)
{
  ComplexAppearance::TexCoords* texCoords = dynamic_cast<ComplexAppearance::TexCoords*>(element);

  const char* str = text.c_str();
  char* nextStr;
  vec2 components;
  skipWhitespace(str, location);
  while(*str)
  {
    for(unsigned int i = 0; i < 2; ++i)
    {
      while(*str == '#') { while(*str && *str != '\n' && *str != '\r') { ++str; ++location.column; } skipWhitespace(str, location); if(!*str) return; }
      components[i] = std::strtof(str, &nextStr);
      if(str == nextStr)
      {
        handleError("Invalid texture coordinate text (must be a space separated list of floats)", location);
        return;
      }
      location.column += nextStr - str;
      str = nextStr;
      skipWhitespace(str, location);
    }
    texCoords->data.emplace_back(components);
  }
}

Element* ParserCore3::translationElement()
{
  vec3 translation(getLength("x", false, 0.f, false), getLength("y", false, 0.f, false), getLength("z", false, 0.f, false));

  SimObject* simObject = dynamic_cast<SimObject*>(elementData->parent->returnedElement);
  if(simObject)
  {
    simObject->relativeTransformation.increasePosition(translation);
  }
  else
  {
    Mass* mass = dynamic_cast<Mass*>(elementData->parent->returnedElement);
    ASSERT(mass);
    mass->relativeTransformation.increasePosition(translation);
  }
  return nullptr;
}

Element* ParserCore3::rotationElement()
{
  vec3 rotation(
    Gum::Maths::toDegree(getAngle("x", false, 0.f, false)), 
    Gum::Maths::toDegree(getAngle("y", false, 0.f, false)), 
    Gum::Maths::toDegree(getAngle("z", false, 0.f, false))
  );


  SimObject* simObject = dynamic_cast<SimObject*>(element);
  if(simObject)
  {
    simObject->relativeTransformation.setRotation(simObject->relativeTransformation.getRotation() * fquat::toQuaternion(rotation));    
  }
  else
  {
    Mass* mass = dynamic_cast<Mass*>(element);
    ASSERT(mass);
    mass->relativeTransformation.increaseRotation(rotation);
  }
  return nullptr;
}

Element* ParserCore3::axisElement()
{
  Axis* axis = new Axis();
  axis->x = getFloat("x", false, 0.f);
  axis->y = getFloat("y", false, 0.f);
  axis->z = getFloat("z", false, 0.f);
  axis->cfm = getFloatMinMax("cfm", false, -1.f, 0.f, 1.f);
  Joint* joint = dynamic_cast<Joint*>(element);
  ASSERT(joint);
  axis->joint = joint;
  return axis;
}

Element* ParserCore3::deflectionElement()
{
  Axis::Deflection* deflection = new Axis::Deflection();
  Axis* axis = dynamic_cast<Axis*>(element);
  ASSERT(axis);
  ASSERT(axis->joint);

  if(dynamic_cast<Hinge*>(axis->joint))
  {
    deflection->min = getAngle("min", true, 0.f, false);
    deflection->max = getAngle("max", true, 0.f, false);
    deflection->offset = getAngle("init", false, 0.f, false);
  }
  else if(dynamic_cast<Slider*>(axis->joint))
  {
    deflection->min = getLength("min", true, 0.f, false);
    deflection->max = getLength("max", true, 0.f, false);
    deflection->offset = getLength("init", false, 0.f, false);
  }
  else
    ASSERT(false);

  getBool("isActive", false, true);

  ASSERT(!axis->deflection);
  axis->deflection = deflection;
  return nullptr;
}

Element* ParserCore3::PT2MotorElement()
{
  PT2Motor* pt2motor = new PT2Motor();
  Axis* axis = dynamic_cast<Axis*>(element);
  ASSERT(axis);
  ASSERT(!axis->motor);

  pt2motor->T = getFloat("T", true, 0.f);
  pt2motor->D = getFloat("D", true, 0.f);
  pt2motor->K = getFloat("K", true, 0.f);
  pt2motor->V = getFloat("V", true, 0.f);
  pt2motor->F = getForce("F", true, 0.f);

  axis->motor = pt2motor;
  return nullptr;
}

Element* ParserCore3::servoMotorElement()
{
  ServoMotor* servoMotor = new ServoMotor();
  Axis* axis = dynamic_cast<Axis*>(element);
  ASSERT(axis);
  ASSERT(!axis->motor);

  if(dynamic_cast<Hinge*>(axis->joint))
    servoMotor->maxVelocity = getAngularVelocity("maxVelocity", true, 0.f);
  else if(dynamic_cast<Slider*>(axis->joint))
    servoMotor->maxVelocity = getVelocity("maxVelocity", true, 0.f);
  else
    ASSERT(false);

  servoMotor->maxForce = getForce("maxForce", true, 0.f);
  servoMotor->controller.p = getFloat("p", true, 0.f);
  servoMotor->controller.i = getFloat("i", false, 0.f);
  servoMotor->controller.d = getFloat("d", false, 0.f);
  servoMotor->delay = getFloatPositive("delay", false, 0.f);

  axis->motor = servoMotor;
  return nullptr;
}

Element* ParserCore3::velocityMotorElement()
{
  VelocityMotor* velocityMotor = new VelocityMotor();
  Axis* axis = dynamic_cast<Axis*>(element);
  ASSERT(axis);
  ASSERT(!axis->motor);

  if(dynamic_cast<Hinge*>(axis->joint))
    velocityMotor->maxVelocity = getAngularVelocity("maxVelocity", true, 0.f);
  else if(dynamic_cast<Slider*>(axis->joint))
    velocityMotor->maxVelocity = getVelocity("maxVelocity", true, 0.f);
  else
    ASSERT(false);

  velocityMotor->maxForce = getForce("maxForce", true, 0.f);

  axis->motor = velocityMotor;
  return nullptr;
}

Element* ParserCore3::surfaceElement()
{
  Appearance::Material* material = new Appearance::Material();
  
  color diffuseColor, ambientColor, specularColor, emissionColor;
  getColor("diffuseColor", true, diffuseColor);
  getColor("ambientColor", false, ambientColor);
  getColor("specularColor", false, specularColor);
  getColor("emissionColor", false, emissionColor);
  material->setColor(rgba::min(rgba(diffuseColor) + rgba(ambientColor) + rgba(specularColor), rgba(255)));

  material->setSpecularity(getFloatMinMax("shininess", false, material->getSpecularity(), 0.f, 128.f) / 255.0f);
  std::string textureFile = getString("diffuseTexture", false);
  if(!textureFile.empty())
    material->setTexture(Texture::autoLoad(textureFile), 0);
  
  return material;
}

Element* ParserCore3::gyroscopeElement()
{
  return new Gyroscope(getString("name", false));
}

Element* ParserCore3::accelerometerElement()
{
  return new Accelerometer(getString("name", false));
}

Element* ParserCore3::cameraElement()
{
  CameraSensor* camera = new CameraSensor(getString("name", false));
  camera->setImageDimensions(ivec2(
    getInteger("imageWidth", true, 0, true),
    getInteger("imageHeight", true, 0, true)
  ));
  camera->angleX = getAngle("angleX", true, 0.f, true);
  camera->angleY = getAngle("angleY", true, 0.f, true);
  return camera;
}

Element* ParserCore3::collisionSensorElement()
{
  return new CollisionSensor(getString("name", false));
}

Element* ParserCore3::objectSegmentedImageSensorElement()
{
  ObjectSegmentedImageSensor* camera = new ObjectSegmentedImageSensor(getString("name", false));
  camera->imageWidth = getInteger("imageWidth", true, 0, true);
  camera->imageHeight = getInteger("imageHeight", true, 0, true);
  camera->angleX = getAngle("angleX", true, 0.f, true);
  camera->angleY = getAngle("angleY", true, 0.f, true);
  return camera;
}

Element* ParserCore3::singleDistanceSensorElement()
{
  SingleDistanceSensor* singleDistanceSensor = new SingleDistanceSensor(getString("name", false));
  singleDistanceSensor->min = getLength("min", false, 0.f, false);
  singleDistanceSensor->max = getLength("max", false, 999999.f, false);
  return singleDistanceSensor;
}

Element* ParserCore3::depthImageSensorElement()
{
  DepthImageSensor* depthImageSensor = new DepthImageSensor(getString("name", false));
  depthImageSensor->imageWidth = getInteger("imageWidth", true, 0, true);
  depthImageSensor->imageHeight = getInteger("imageHeight", false, 1, true);
  depthImageSensor->angleX = getAngle("angleX", true, 0.f, true);
  depthImageSensor->angleY = getAngle("angleY", true, 0.f, true);
  depthImageSensor->min = getLength("min", false, 0.f, false);
  depthImageSensor->max = getLength("max", false, 999999.f, false);

  const std::string& projection = getString("projection", false);
  if(projection == "" || projection == "perspective")
    depthImageSensor->projection = DepthImageSensor::perspectiveProjection;
  else if(projection == "spherical")
  {
    if(depthImageSensor->imageHeight > 1)
      handleError("Spherical projection is currently only supported for 1-D sensors (i.e. with imageHeight=\"1\")",
                  attributes->find("projection")->second.valueLocation);
    else
      depthImageSensor->projection = DepthImageSensor::sphericalProjection;
  }
  else
    handleError("Unexpected projection type \"" + projection + "\" (expected one of \"perspective, spherical\")",
                attributes->find("projection")->second.valueLocation);

  return depthImageSensor;
}

Element* ParserCore3::userInputElement()
{
  UserInput* userInput = new UserInput(getString("name", false));
  std::string type = getString("type", false);
  if(type == "angle")
  {
    userInput->inputPort.unit = QString::fromUtf8("°");
    userInput->inputPort.min = getAngle("min", true, 0.f, false);
    userInput->inputPort.max = getAngle("max", true, 0.f, false);
    userInput->inputPort.defaultValue = getAngle("default", false, 0.f, false);
  }
  else if(type == "angularVelocity")
  {
    userInput->inputPort.unit = QString::fromUtf8("°/s");
    userInput->inputPort.min = getAngularVelocity("min", true, 0.f);
    userInput->inputPort.max = getAngularVelocity("max", true, 0.f);
    userInput->inputPort.defaultValue = getAngularVelocity("default", false, 0.f);
  }
  else if(type == "length" || type == "")
  {
    userInput->inputPort.unit = QString::fromUtf8("m");
    userInput->inputPort.min = getLength("min", true, 0.f, false);
    userInput->inputPort.max = getLength("max", true, 0.f, false);
    userInput->inputPort.defaultValue = getLength("default", false, 0.f, false);
  }
  else if(type == "velocity")
  {
    userInput->inputPort.unit = QString::fromUtf8("m/s");
    userInput->inputPort.min = getVelocity("min", true, 0.f);
    userInput->inputPort.max = getVelocity("max", true, 0.f);
    userInput->inputPort.defaultValue = getVelocity("default", false, 0.f);
  }
  else if(type == "acceleration")
  {
    userInput->inputPort.unit = QString::fromUtf8("m/s^2");
    userInput->inputPort.min = getAcceleration("min", true, 0.f);
    userInput->inputPort.max = getAcceleration("max", true, 0.f);
    userInput->inputPort.defaultValue = getAcceleration("default", false, 0.f);
  }
  else
    handleError("Unexpected user input type \"" + type + "\" (expected one of \"length, velocity, acceleration, angle, angularVelocity\")",
                attributes->find("type")->second.valueLocation);

  return userInput;
}
