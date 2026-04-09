/**
 * @file Simulation/Actuators/Hinge.cpp
 * Implementation of class Hinge
 * @author <A href="mailto:tlaue@uni-bremen.de">Tim Laue</A>
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 * @author Colin Graf
 */

#include "Hinge.h"
#include "CoreModule.h"
#include "Simulation/Axis.h"
#include "Simulation/Body.h"
#include "Simulation/Motors/ServoMotor.h"
#include "Simulation/Motors/VelocityMotor.h"
#include "Simulation/Simulation.h"
#include "Platform/Assert.h"
#include "Tools/Math/Rotation.h"
#include <mujoco/mujoco.h>

Hinge::Hinge(const std::string& name)
  : Joint(findAvailableName(name, "Hinge"))
{
}

void Hinge::createPhysicsInternal()
{
  ASSERT(axis);

  axis->create();

  // find bodies to connect
  [[maybe_unused]] Body* parentBody = dynamic_cast<Body*>(parent);
  ASSERT(!parentBody || parentBody->body);
  ASSERT(!children.empty());
  Body* childBody = dynamic_cast<Body*>(children.front());
  ASSERT(childBody);

  //CHANGED
  //ASSERT(childBody->body);

  //if(axis->deflection && axis->deflection->offset != 0.f)
  //{
  //  increaseRotation(fquat::toQuaternion(vec3(axis->x, axis->y, axis->z) * Gum::Maths::toDegree(axis->deflection->offset)));
  //  //calcTransformationMatrix();
  //}
  childBody->parentBody = parentBody;
  childBody->createPhysics(true);
  
  Joint::createPhysicsInternal();


  
  mjsJoint* joint = mjs_addJoint(childBody->body, nullptr);
  mjs_setName(joint->element, mujocoName.c_str());
  joint->type = mjJNT_HINGE;

  //vec3 positionInChild = childBody->relativeTransformation.getPosition();
  vec3 positionInChild = Gum::Maths::inverseTransformationMatrix(childBody->getMatrix()) * vec4(getPosition(), 1.0f);
  vec3 axisInChild = mat3::transpose(mat3(childBody->getMatrix())) * mat3(getMatrix()) * (vec3)*axis;
  //vec3 axisInChild = Gum::Maths::rotateMatrix(childBody->relativeTransformation.getRotation()) * vec4(*axis, 1.0f);

  mju_f2n(joint->pos, positionInChild.data(), 3);
  mju_f2n(joint->axis, axisInChild.data(), 3);

  //joint->damping = 0.9f;

  if(axis->deflection)
  {
    joint->limited = mjLIMITED_TRUE;
    joint->range[0] = axis->deflection->min;
    joint->range[1] = axis->deflection->max;
    //joint->ref = axis->deflection->offset; //CHANGED
  }

  // create motor
  if(axis->motor)
  {
    axis->motor->create(this);
    if(!dynamic_cast<VelocityMotor*>(axis->motor) && axis->deflection) // Move setpoint to a position inside the deflection range
      axis->motor->setpoint = axis->deflection->offset;
  }

}

const QIcon* Hinge::getIcon() const
{
  return &CoreModule::module->hingeIcon;
}
