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
  Joint::createPhysicsInternal();
  ASSERT(axis);

  axis->create();

  // find bodies to connect
  [[maybe_unused]] Body* parentBody = dynamic_cast<Body*>(parent);
  ASSERT(!parentBody || parentBody->body);
  ASSERT(!children.empty());
  Body* childBody = dynamic_cast<Body*>(children.front());
  ASSERT(childBody);
  ASSERT(childBody->body);

  //std::cout << "Attaching hinge between " << parentBody->name << " and " << childBody->name << " with offset " << (*axis * Gum::Maths::toDegree(axis->deflection->offset)).toString() << std::endl;

  //axis->deflection->offset = 0;
  //if(axis->deflection && axis->deflection->offset != 0.f)
  //{
  //  relativeTransformation.setRotation(vec3(axis->x, axis->y, axis->z) * Gum::Maths::toDegree(axis->deflection->offset));
  //  calcTransformationMatrix();
  //}

  std::cout << name << " after: \n" << fquat::toEuler(worldTransformation.getRotation()).toString() << std::endl;

  childBody->parentBody = parentBody;
  childBody->createPhysics(true);


  //std::cout << name << " relpos: " << childBody->relativeTransformation.getPosition().toString() << std::endl;
  
  mjsJoint* joint = mjs_addJoint(childBody->body, nullptr);
  mjs_setName(joint->element, name.c_str());
  joint->type = mjJNT_HINGE;

  //vec3 positionInChild = childBody->relativeTransformation.getPosition();
  vec3 positionInChild = Gum::Maths::inverseTransformationMatrix(childBody->worldTransformation.getMatrix()) * vec4(worldTransformation.getPosition(), 1.0f);
  vec3 axisInChild = mat3::transpose(mat3(childBody->worldTransformation.getMatrix())) * mat3(worldTransformation.getMatrix()) * (vec3)*axis;
  //vec3 axisInChild = Gum::Maths::rotateMatrix(childBody->relativeTransformation.getRotation()) * vec4(*axis, 1.0f);
  std::cout << name << " after: " << fquat::toEuler(worldTransformation.getRotation()).toString() << " " << axisInChild.toString() << std::endl;

  mju_f2n(joint->pos, positionInChild.data(), 3);
  mju_f2n(joint->axis, axisInChild.data(), 3);

  //joint->damping = 0.9f;

  if(axis->deflection)
  {
    //std::cout << name << " offset: " << Gum::Maths::toDegree(axis->deflection->offset) << std::endl;
    joint->limited = mjLIMITED_TRUE;
    joint->range[0] = axis->deflection->min;
    joint->range[1] = axis->deflection->max;
    //joint->ref = axis->deflection->offset;
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
