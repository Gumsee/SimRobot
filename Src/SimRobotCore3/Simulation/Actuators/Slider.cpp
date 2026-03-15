/**
 * @file Simulation/Actuators/Slider.cpp
 * Implementation of class Slider
 * @author <A href="mailto:tlaue@uni-bremen.de">Tim Laue</A>
 * @author <A href="mailto:kspiess@informatik.uni-bremen.de">Kai Spiess</A>
 * @author Colin Graf
 * @author Thomas Röfer
 */

#include "Slider.h"
#include "CoreModule.h"
#include "Simulation/Axis.h"
#include "Simulation/Body.h"
#include "Simulation/Motors/ServoMotor.h"
#include "Simulation/Simulation.h"
#include "Platform/Assert.h"
#include <mujoco/mujoco.h>

Slider::Slider(const std::string& name)
  : Joint(findAvailableName(name, "Slider"))
{}

void Slider::createPhysicsInternal()
{
  Joint::createPhysicsInternal();
  ASSERT(axis);

  axis->create();

  if(axis->deflection && axis->deflection->offset != 0.f)
    worldTransformation.setPosition(vec3(axis->x, axis->y, axis->z) * axis->deflection->offset);

  // find bodies to connect
  [[maybe_unused]] Body* parentBody = dynamic_cast<Body*>(parent);
  ASSERT(!parentBody || parentBody->body);
  ASSERT(!children.empty());
  Body* childBody = dynamic_cast<Body*>(children.front());
  ASSERT(childBody);
  ASSERT(childBody->body);

  childBody->createPhysics();

  mjsJoint* joint = mjs_addJoint(childBody->body, nullptr);
  mjs_setName(joint->element, name.c_str());
  joint->type = mjJNT_SLIDE;

  vec3 positionInChild = mat4::inverse(childBody->worldTransformation.getMatrix()) * vec4(worldTransformation.getPosition(), 1.0f);
  mju_f2n(joint->pos, positionInChild.data(), 3);
  vec3 axisInChild = mat4::inverse(Gum::Maths::rotateMatrix(childBody->worldTransformation.getRotation())) * Gum::Maths::rotateMatrix(worldTransformation.getRotation()) * vec4(axis->x, axis->y, axis->z, 0.0f);
  mju_f2n(joint->axis, axisInChild.data(), 3);

  if(axis->deflection)
  {
    joint->limited = mjLIMITED_TRUE;
    joint->range[0] = axis->deflection->min;
    joint->range[1] = axis->deflection->max;
    joint->ref = axis->deflection->offset;
  }

  // create motor
  if(axis->motor)
    axis->motor->create(this);
}

const QIcon* Slider::getIcon() const
{
  return &CoreModule::module->sliderIcon;
}
