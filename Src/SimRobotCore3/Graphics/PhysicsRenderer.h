#pragma once
#include <Graphics/Renderable.h>
#include "Simulation/PhysicalObject.h"

class PhysicsRenderer : public Renderable
{
private:
    const PhysicalObject* physicalObject;
    inline static bool renderphysics = false;

public:
    PhysicsRenderer(const PhysicalObject* physicalobject);

    void render() override;
    static void enable(const bool& enabled);
};