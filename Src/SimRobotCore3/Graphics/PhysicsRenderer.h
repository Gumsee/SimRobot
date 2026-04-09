#pragma once
#include <Graphics/Renderable.h>
#include "Simulation/PhysicalObject.h"

class PhysicsRenderer : public Renderable
{
private:
    const PhysicalObject* physicalObject;
    SimRobotCore3::Renderer::ShadeMode shademode = SimRobotCore3::Renderer::ShadeMode::noShading;

public:
    PhysicsRenderer(const PhysicalObject* physicalobject);

    void render() override;
    void setShadeMode(SimRobotCore3::Renderer::ShadeMode shademode);
    SimRobotCore3::Renderer::ShadeMode getShadeMode();
};