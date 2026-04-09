/**
 * @file Simulation/PhysicalObject.h
 * Implementation of class PhysicalObject
 * @author Colin Graf
 */

#include "PhysicalObject.h"
#include "Platform/Assert.h"
#include "Simulation/Body.h"
#include "Simulation/Simulation.h"
#include <mujoco/mujoco.h>

PhysicalObject::PhysicalObject(const int& type, const std::string& name)
  : SimObject(name),
    type(type)
    
{}

void PhysicalObject::addParent(Element& element)
{
  ASSERT(!parent);
  PhysicalObject* physicalparent = dynamic_cast<PhysicalObject*>(&element);
  physicalparent->physicalChildren.push_back(this);
  physicalparent->physicalDrawings.push_back(this);
  parent = physicalparent;
  SimObject::addParent(element);
}

void PhysicalObject::createPhysics(bool withchildren)
{
  std::string parentname = "";
  if(parent != nullptr)
    parentname = parent->name;

  // find parent body for child objects
  Body* body = dynamic_cast<Body*>(this);
  if(!body)
    body = parentBody;

  if(!isinitialized)
  {
    calcTransformationMatrix();
    //std::cout << levelSpaces(level) << name << " " << getPosition().toString() << " " << fquat::toEuler(getRotation()).toString() << std::endl;
    createPhysicsInternal();
    isinitialized = true;
  }

  // initialize and call createPhysics() for each child object
  if(withchildren)
  for(PhysicalObject* object : physicalChildren)
  {
    object->parentBody = body;
    object->createPhysics();
  }
}

void PhysicalObject::createIDs()
{
  if(type != mjOBJ_UNKNOWN)
    id = mj_name2id(Simulation::simulation->model, type, mujocoName.c_str());

  //std::cout << name << " " << id << std::endl;
  for(PhysicalObject* object : physicalChildren)
    object->createIDs();
}

void PhysicalObject::drawPhysics() const
{
  for(const PhysicalObject* drawing : physicalDrawings)
    drawing->drawPhysics();
}

void PhysicalObject::drawControllerDrawings() const
{
  for(SimRobotCore3::Controller3DDrawing* drawing : controllerDrawings)
    drawing->draw();
  const_cast<PhysicalObject*>(this)->visitPhysicalControllerDrawings([](PhysicalObject& child){child.drawControllerDrawings();});
}

void PhysicalObject::beforeControllerDrawings(const float* projection, const float* view)
{
  for(SimRobotCore3::Controller3DDrawing* drawing : controllerDrawings)
    drawing->beforeFrame(projection, view, &getMatrix()[0][0]);
  const_cast<PhysicalObject*>(this)->visitPhysicalControllerDrawings([projection, view](PhysicalObject& child){child.beforeControllerDrawings(projection, view);});
}

void PhysicalObject::afterControllerDrawings() const
{
  for(SimRobotCore3::Controller3DDrawing* drawing : controllerDrawings)
    drawing->afterFrame();
  const_cast<PhysicalObject*>(this)->visitPhysicalControllerDrawings([](PhysicalObject& child){child.afterControllerDrawings();});
}

void PhysicalObject::visitPhysicalControllerDrawings(const std::function<void(PhysicalObject&)>& accept)
{
  for(PhysicalObject* drawing : physicalDrawings)
    accept(*drawing);
}

bool PhysicalObject::registerDrawing(SimRobotCore3::Controller3DDrawing& drawing)
{
  controllerDrawings.push_back(&drawing);
  return true;
}

bool PhysicalObject::unregisterDrawing(SimRobotCore3::Controller3DDrawing& drawing)
{
  for(auto iter = controllerDrawings.begin(), end = controllerDrawings.end(); iter != end; ++iter)
    if(*iter == &drawing)
    {
      controllerDrawings.erase(iter);
      return true;
    }
  return false;
}

SimRobotCore3::Body* PhysicalObject::getParentBody()
{
  return parentBody;
}
