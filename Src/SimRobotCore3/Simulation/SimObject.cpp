/**
 * @file Simulation/SimObject.cpp
 * Implementation of class SimObject
 * @author Colin Graf
 */


#include "SimObject.h"
#include "CoreModule.h"
#include "SimObjectWidget.h"
#ifdef __GNUC__
#include <cctype>
#else
#include <cstring>
#endif
#include <typeinfo>
#include <iostream>

SimObject::SimObject(const std::string& name)
  : name(name)
{
  unsigned int counter = 0;
  std::string suffix = "";
  while(Tools::mapHasKey(loadedObjects, name+suffix))
    suffix = std::to_string(counter++);
    
  mujocoName = name+suffix;
  loadedObjects[mujocoName] = this;
}

std::string SimObject::findAvailableName(std::string name, const std::string& defaultvalue)
{
  return name.empty() ? defaultvalue : name;
}

void SimObject::addParent(Element& parent)
{
  this->parent = dynamic_cast<SimObject*>(&parent);
  if(this->parent)
    this->parent->children.push_back(this);
}

void SimObject::registerObjects(int level)
{
  for(SimObject* simObject : children)
  {
    if(simObject->name.empty())
    {
      const char* typeName = typeid(*simObject).name();
#ifdef __GNUC__
      while(std::isdigit(*typeName))
        ++typeName;
#else
      const char* str = std::strchr(typeName, ' ');
      if(str)
        typeName = str + 1;
#endif
      simObject->fullName = fullName + "." + typeName;
    }
    else
      simObject->fullName = fullName + "." + simObject->name.c_str();

    //std::cout << getLevel(level) << simObject->mujocoName << " " << simObject->fullName.toStdString() << std::endl;
    //std::cout << getLevel(level) << simObject->mujocoName << " " << simObject->getRotation().toString() << std::endl;
    CoreModule::application->registerObject(*CoreModule::module, dynamic_cast<SimRobot::Object&>(*simObject), dynamic_cast<SimRobot::Object*>(this));
    simObject->registerObjects(level + 1);
  }
}

SimRobot::Widget* SimObject::createWidget()
{
  return new SimObjectWidget(*this);
}

const QIcon* SimObject::getIcon() const
{
  return &CoreModule::module->objectIcon;
}


void SimObject::calcTransformationMatrix()
{
  setMatrix(parent != nullptr
    ? parent->getMatrix() * this->relativeTransformation.getMatrix()
    : this->relativeTransformation.getMatrix()
  );
}

void SimObject::updateTransformation()
{
  for(SimObject* child : children)
    child->updateTransformation();
}