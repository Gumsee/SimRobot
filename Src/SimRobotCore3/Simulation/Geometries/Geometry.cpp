/**
 * @file Simulation/Geometries/Geometry.cpp
 * Implementation of class Geometry
 * @author Colin Graf
 */

#include "Geometry.h"
#include "Platform/Assert.h"
#include <mujoco/mujoco.h>
#include "Simulation/Simulation.h"
#include "Simulation/Body.h"
#include "Simulation/Sensors/CollisionSensor.h"

Geometry::Geometry(const std::string& name)
  : ::PhysicalObject(mjOBJ_GEOM, findAvailableName(name, "Geometry"))
{
  //color = rgba(204,204,204, 255);
  color = rgba::random(rgba(0,0,0,255), rgba(255,255,255,255));
}

Geometry::~Geometry()
{
  delete collisionCallbacks;
}

void Geometry::createPhysicsInternal()
{
  ASSERT(parentBody);
  ASSERT(parentBody->body);
  mjsBody* body = Simulation::simulation->worldBody;
  int collisionGroup = 0;
  bool immaterial = dynamic_cast<CollisionSensor*>(parent) != nullptr;
  if(immaterial)
    std::cout << name << " parent collisionsensor" << std::endl;

  if(parentBody != nullptr)
  {
    body = parentBody->body;
    collisionGroup = parentBody->collisionGroup;
  }

  geom = assembleGeometry(body);
  if(obj != nullptr)
  {
    Object3DInstance* instance = obj->addInstance();
    instance->setMatrix(worldTransformation.getMatrix());
    obj->applyTransformationMatrix(instance);
  }
  if(geom)
  {
    ASSERT(collisionGroup < 32);
    geom->contype = 1 << collisionGroup;
    geom->conaffinity = ~geom->contype;

    Transformable3D transformInParentBody;
    transformInParentBody.setMatrix(parentBody != nullptr 
      ? mat4::inverse(parentBody->worldTransformation.getMatrix()) * worldTransformation.getMatrix() 
      : worldTransformation.getMatrix()
    );
    // set offset
    mju_f2n(geom->pos, relativeTransformation.getPosition().data(), 3);
    mju_f2n(geom->quat, relativeTransformation.getRotation().data(), 4);
    //mju_negQuat(geom->quat, geom->quat); // column major -> row major

    geom->condim = 3;
    geom->friction[0] = 1.f;
    geom->friction[1] = 0.f;
    geom->friction[2] = 0.f;

    if(material)
    {
      geom->priority = 1;
      if(material->friction > 0.f)
        geom->friction[0] = material->friction;
      else
      {
        geom->friction[0] = 0.f;
        geom->condim = 1;
      }
      if(material->rollingFriction > 0.f)
      {
        geom->friction[1] = material->rollingFriction;
        geom->friction[2] = material->rollingFriction;
        geom->condim = 6;
      }
    }

    if(immaterial)
      geom->gap = std::numeric_limits<decltype(geom->gap)>::max();
  }
}

void Geometry::createIDs()
{
  if(geom == nullptr)
    return;

  id = mj_name2id(Simulation::simulation->model, type, name.c_str());
  registeredGeometries[id] = this;
}

bool Geometry::registerCollisionCallback(SimRobotCore3::CollisionCallback& collisionCallback)
{
  if(!collisionCallbacks)
    collisionCallbacks = new std::list<SimRobotCore3::CollisionCallback*>();
  collisionCallbacks->push_back(&collisionCallback);
  return false;
}

bool Geometry::unregisterCollisionCallback(SimRobotCore3::CollisionCallback& collisionCallback)
{
  for(auto iter = collisionCallbacks->begin(), end = collisionCallbacks->end(); iter != end; ++iter)
    if(*iter == &collisionCallback)
    {
      collisionCallbacks->erase(iter);
      if(collisionCallbacks->empty())
      {
        delete collisionCallbacks;
        collisionCallbacks = nullptr;
      }
      return true;
    }
  return false;
}

void Geometry::Material::addParent(Element& element)
{
  Geometry* geometry = dynamic_cast<Geometry*>(&element);
  ASSERT(geometry);
  ASSERT(!geometry->material);
  geometry->material = this;
}

void Geometry::updateTransformation()
{
  // get pose from MuJoCo
  if(obj != nullptr)
  {
    mju_n2f(obj->getInstance()->getPosition().data(), Simulation::simulation->data->geom_xpos + id * 3, 3);

    mjtNum quat[4];
    mju_mat2Quat(quat, Simulation::simulation->data->geom_xmat + id * 9);
    mju_n2f(obj->getInstance()->getRotation().data(), quat, 4);

    obj->getInstance()->updateMatrix();
    obj->applyTransformationMatrix(obj->getInstance());
    worldTransformation.setMatrix(obj->getInstance()->getMatrix());
  }
  else
  {
    calcTransformationMatrix();
  }

  ::PhysicalObject::updateTransformation();
}


void Geometry::drawPhysics() const
{
  Simulation::simulation->forwardRenderingShader->loadUniform("color", color.getGLColor());
  if(obj != nullptr)
    obj->render();
  ::PhysicalObject::drawPhysics();
}

void Geometry::checkCollisions()
{
  mjData* data = Simulation::simulation->data;
  Simulation::simulation->collisions = 0;
  contactPoints = data->ncon;
  for(int i = 0; i < data->ncon; ++i)
  {
    const int geom1 = data->contact[i].geom[0];
    const int geom2 = data->contact[i].geom[1];
    // Only report the first contact of each collision. (This assumes that the array is ordered.)
    if(i && geom1 == data->contact[i - 1].geom[0] && geom2 == data->contact[i - 1].geom[1])
      continue;
    Simulation::simulation->collisions++;
    Geometry* geometry1 = registeredGeometries[geom1];
    Geometry* geometry2 = registeredGeometries[geom2];
    if(geometry1 && geometry2)
    {
      if(geometry1->collisionCallbacks)
        for(auto* callback : *geometry1->collisionCallbacks)
          callback->collided(*geometry1, *geometry2);
      if(geometry2->collisionCallbacks)
        for(auto* callback : *geometry2->collisionCallbacks)
          callback->collided(*geometry2, *geometry1);
    }
  }
}