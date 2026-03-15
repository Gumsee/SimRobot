#pragma once
#include <Graphics/Renderable.h>
#include "Simulation/PhysicalObject.h"

class PhysicsRenderer : public Renderable
{
private:
    const  PhysicalObject* physicalObject;

public:
    PhysicsRenderer(const PhysicalObject* physicalobject);

    void render() override;
};