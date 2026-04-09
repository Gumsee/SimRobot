/**
 * @file SimObjectRenderer.cpp
 * Declaration of class SimObjectRenderer
 * @author Colin Graf
 */

#include "SimObjectRenderer.h"
#include "Platform/Assert.h"
#include "Platform/System.h"
#include "Simulation/Body.h"
#include "Simulation/Scene.h"
#include "Simulation/Simulation.h"
#include "Tools/Math.h"
#include "Tools/Math/Constants.h"
#include "Tools/Math/Rotation.h"
#include "Tools/OpenGLTools.h"
#include <mujoco/mujoco.h>
#include <iostream>

bool releaseDrag(int x, int y)
{
  /*else // object control
  {
    if(dragMode == adoptDynamics)
      moveDrag(x, y, dragType);
    else if(dragMode == applyDynamics)
    {
      Vector3f projectedClick = projectClick(x, y);
      Vector3f currentPos;
      if(intersectRayAndPlane(cameraPos, projectedClick - cameraPos, dragSelection->poseInWorld.translation, dragPlaneVector, currentPos))
      {
        if(dragType == dragRotate || dragType == dragRotateWorld)
        {
          Vector3f oldV = dragStartPos - dragSelection->poseInWorld.translation;
          Vector3f newV = currentPos - dragSelection->poseInWorld.translation;

          if(dragType != dragRotateWorld)
          {
            const RotationMatrix invRotation = dragSelection->poseInWorld.rotation.inverse();
            oldV = invRotation * oldV;
            newV = invRotation * newV;
          }

          float angle = 0.f;
          if(dragPlane == yzPlane)
            angle = normalize(std::atan2(newV.z(), newV.y()) - std::atan2(oldV.z(), oldV.y()));
          else if(dragPlane == xzPlane)
            angle = normalize(std::atan2(newV.x(), newV.z()) - std::atan2(oldV.x(), oldV.z()));
          else
            angle = normalize(std::atan2(newV.y(), newV.x()) - std::atan2(oldV.y(), oldV.x()));

          const Vector3f offset = dragPlaneVector * angle;
          const Vector3f torque = offset * static_cast<float>(Simulation::simulation->model->body_mass[dragSelection->bodyIndex]) * 50.f;
          mju_f2n(Simulation::simulation->data->xfrc_applied + dragSelection->bodyIndex * 6 + 3, torque.data(), 3);
        }
        else
        {
          const Vector3f offset = currentPos - dragStartPos;
          const Vector3f force = offset * static_cast<float>(Simulation::simulation->model->body_mass[dragSelection->bodyIndex]) * 500.f;
          mju_f2n(Simulation::simulation->data->xfrc_applied + dragSelection->bodyIndex * 6, force.data(), 3);
        }
      }
    }
  }*/
 return false;
}
