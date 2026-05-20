/**
 * @file Simulation/Body.cpp
 * Implementation of class Body
 * @author Colin Graf
 */

#include "Body.h"
#include "Platform/Assert.h"
#include "Simulation/Actuators/Joint.h"
#include "Simulation/Geometries/Geometry.h"
#include "Simulation/Masses/Mass.h"
#include "Simulation/Scene.h"
#include "Simulation/Simulation.h"
#include <mujoco/mujoco.h>
#include <iostream>

Body::Body(const std::string& name)
  : ::PhysicalObject(mjOBJ_BODY, findAvailableName(name, "Body"))
{
}

void Body::addParent(Element& element)
{
  ASSERT(!parent);
  ::PhysicalObject* physicalparent = dynamic_cast<::PhysicalObject*>(&element);
  physicalparent->physicalChildren.push_back(this);
  parent = physicalparent;
  SimObject::addParent(element);
}

void Body::createPhysicsInternal()
{
  ASSERT(!body);

  // register body at parent body
  //std::cout << parentBody << std::endl;
  if(parentBody)
  {
    parentBody->bodyChildren.push_back(this);
    body = mjs_addBody(parentBody->body, nullptr);
    rootBody = parentBody->rootBody;
    collisionGroup = rootBody->collisionGroup;
  }
  else
  {
    Simulation::simulation->scene->bodies.push_back(this);
    body = mjs_addBody(Simulation::simulation->worldBody, nullptr);
    if(!dynamic_cast<Joint*>(parent))
    {
      mjs_addFreeJoint(body);
      rootBody = this;
      collisionGroup = Simulation::simulation->scene->detectBodyCollisions ? Simulation::simulation->nextCollisionGroup++ : 1;
    }
    else
    {
      rootBody = nullptr;
      collisionGroup = 0; // belongs to the static world, doesn't collide with it
    }
  }

  mjs_setName(body->element, mujocoName.c_str());

  // add masses
  for(SimObject* iter : children)
  {
    auto* mass = dynamic_cast<Mass*>(iter);
    if(mass)
      addMass(*mass);
  }

  // set position
  Transformable3D transformInParentBody;
  transformInParentBody.setMatrix(parentBody != nullptr 
    ? Gum::Maths::inverseTransformationMatrix(parentBody->getMatrix()) * getMatrix() 
    : getMatrix()
  );
  mju_f2n(body->pos, transformInParentBody.getPosition().data(), 3);
  mju_f2n(body->quat, transformInParentBody.getRotation().data(), 4);
  mju_negQuat(body->quat, body->quat); // column major -> row major
}

void Body::addMass(Mass& mass)
{
  if(body->mass == 0.f)
  {
    vec3 com;
    float inertia[6];
    body->mass = mass.createMass(com, inertia);
    mju_f2n(body->fullinertia, inertia, 6);
    mju_f2n(body->ipos, com.data(), 3);
    /*
    if(mass.rotation)
      rotate mass; -> rotate tensor of inertia and center of mass
    if(mass.translation)
      mju_f2n(body->ipos, mass.translation->data(), 3); // by moving the inertia frame, no adjustment to the tensor of inertia is needed
    */
    mju_addTo3(body->ipos, body->pos);
  }
  /*
  if(this->mass.mass == 0.f)
  {
    this->mass = mass.createMass();
    if(mass.rotation)
    {
      dMatrix3 matrix;
      ODETools::convertMatrix(*mass.rotation, matrix);
      dMassRotate(&this->mass, matrix);
    }
    if(mass.translation)
      centerOfMass = *mass.translation;
  }
  else
  {
    if(centerOfMass != Vector3f::Zero())
    {
      dMassTranslate(&this->mass, centerOfMass.x(), centerOfMass.y(), centerOfMass.z());
      centerOfMass = Vector3f::Zero();
    }

    const dMass& constAdditionalMass = mass.createMass();
    if(mass.rotation || mass.translation)
    {
      dMass additionalMass = constAdditionalMass;
      if(mass.rotation)
      {
        dMatrix3 matrix;
        ODETools::convertMatrix(*mass.rotation, matrix);
        dMassRotate(&additionalMass, matrix);
      }
      if(mass.translation)
        dMassTranslate(&additionalMass, mass.translation->x(), mass.translation->y(), mass.translation->z());
      dMassAdd(&this->mass, &additionalMass);
    }
    else
      dMassAdd(&this->mass, &constAdditionalMass);
  }
   */
}

void Body::createGraphics()
{
  GraphicalObject::createGraphics();
  for(Body* child : bodyChildren)
    child->createGraphics();
}

void Body::createIDs()
{
  ::PhysicalObject::createIDs();

  if(body == nullptr || id < 0)
    return;

  registeredBodies[id] = this;
}

void Body::updateTransformation()
{
  // get pose from MuJoCo
  //mju_n2f(getPosition().data(), Simulation::simulation->data->xpos + id * 3, 3);
  mju_n2f(vPosition.data(), Simulation::simulation->data->xpos + id * 3, 3);
  mju_n2f(qRotation.data(), Simulation::simulation->data->xquat + id * 4, 4);
  updateMatrix(false);

  SimObject::updateTransformation();
  GraphicalObject::updateAppearances();
  for(Body* child : bodyChildren)
    child->updateAppearances();
}

void Body::visitGraphicalControllerDrawings(const std::function<void(GraphicalObject&)>& accept)
{
  GraphicalObject::visitGraphicalControllerDrawings(accept);
  for(Body* child : bodyChildren)
    accept(*child);
}

void Body::drawPhysics() const
{
  Simulation::simulation->bodyComSphereMesh->render();

  // draw children
  ::PhysicalObject::drawPhysics();

  for(const Body* child : bodyChildren)
    child->drawPhysics();
}

void Body::visitPhysicalControllerDrawings(const std::function<void(::PhysicalObject&)>& accept)
{
  ::PhysicalObject::visitPhysicalControllerDrawings(accept);
  for(Body* child : bodyChildren)
    accept(*child);
}

void Body::onTransformUpdate()
{
  if(rootBody != this)
    return;
  ASSERT(Simulation::simulation->model->body_jntnum[id] == 1);
  const int jointIndex = Simulation::simulation->model->body_jntadr[id];
  ASSERT(Simulation::simulation->model->jnt_type[jointIndex] == mjJNT_FREE);
  const int poseIndex = Simulation::simulation->model->jnt_qposadr[jointIndex];
  mju_f2n(Simulation::simulation->data->qpos + poseIndex, vPosition.data(), 3);
  mju_f2n(Simulation::simulation->data->qpos + poseIndex + 3, qRotation.data(), 4);

  // Unfortunately it seems that forward kinematics have to be done for the entire model again.
  mj_kinematics(Simulation::simulation->model, Simulation::simulation->data);

  SimObject::updateTransformation();
  GraphicalObject::updateAppearances();
  for(Body* child : bodyChildren)
    child->updateAppearances();
}

const float* Body::getPositionF() const
{
  vec3 pos = const_cast<Body*>(this)->getPosition();
  mju_n2f(pos.data(), Simulation::simulation->data->xpos + id * 3, 3);
  return pos.data();
}

bool Body::getPose(float* pos, float (*rot)[3]) const
{
  pos[0] = const_cast<Body*>(this)->getPosition().x;
  pos[1] = const_cast<Body*>(this)->getPosition().y;
  pos[2] = const_cast<Body*>(this)->getPosition().z;

  mat4 rotMatrix = Gum::Maths::rotateMatrix(const_cast<Body*>(this)->getRotation());
  rot[0][0] = rotMatrix[0][0];
  rot[0][1] = rotMatrix[1][0];
  rot[0][2] = rotMatrix[2][0];
  rot[1][0] = rotMatrix[0][1];
  rot[1][1] = rotMatrix[1][1];
  rot[1][2] = rotMatrix[2][1];
  rot[2][0] = rotMatrix[0][2];
  rot[2][1] = rotMatrix[1][2];
  rot[2][2] = rotMatrix[2][2];
  return true;
}

const float* Body::getVelocity() const
{
  if(rootBody != this)
    return nullptr;
  // This is only possible for bodies that are connected to the worldbody via a freejoint.
  vec3& velocity = const_cast<Body*>(this)->velocityInWorld;

  ASSERT(Simulation::simulation->model->body_jntnum[id] == 1);
  const int jointIndex = Simulation::simulation->model->body_jntadr[id];
  ASSERT(Simulation::simulation->model->jnt_type[jointIndex] == mjJNT_FREE);
  const int velocityIndex = Simulation::simulation->model->jnt_dofadr[jointIndex];
  mju_n2f(velocity.data(), Simulation::simulation->data->qvel + velocityIndex, 3);
  return velocity.data();
}

void Body::setVelocity(const float* velocity)
{
  if(rootBody != this)
    return;
  // TODO: Is this world or body coordinates?
  ASSERT(Simulation::simulation->model->body_jntnum[id] == 1);
  const int jointIndex = Simulation::simulation->model->body_jntadr[id];
  ASSERT(Simulation::simulation->model->jnt_type[jointIndex] == mjJNT_FREE);
  const int velocityIndex = Simulation::simulation->model->jnt_dofadr[jointIndex];
  mju_f2n(Simulation::simulation->data->qvel + velocityIndex, velocity, 3);
}

void Body::move(const float* pos)
{
  increasePosition(vec3(pos[0], pos[1], pos[2]));
}

void Body::move(const float* pos, const float* rot)
{
  increasePosition(vec3(pos[0], pos[1], pos[2]));
  increaseRotation(vec3(rot[0], rot[1], rot[2]));
}

void Body::resetDynamics()
{
  mju_zero(Simulation::simulation->data->qvel + Simulation::simulation->model->body_dofadr[id], Simulation::simulation->model->body_dofnum[id]);
  for(Body* child : bodyChildren)
    child->resetDynamics();
}

void Body::enablePhysics(bool enable)
{
  // enable/disable dynamics
  if(enable)
    --Simulation::simulation->model->ngravcomp;
  else
    ++Simulation::simulation->model->ngravcomp;
  Simulation::simulation->model->body_gravcomp[id] = enable ? 0.f : 1.f;

  // enable/disable collisions with associated geoms
  Simulation::simulation->model->body_contype[id] = Simulation::simulation->model->body_conaffinity[id] = enable ? 1 : 0;

  for(Body* child : bodyChildren)
    child->enablePhysics(enable);
}

void Body::enableGravity(bool enable)
{
  Simulation::simulation->model->body_gravcomp[id] = enable ? 0.f : 1.f; // TODO
  for(Body* child : bodyChildren)
    child->enableGravity(enable);
}
