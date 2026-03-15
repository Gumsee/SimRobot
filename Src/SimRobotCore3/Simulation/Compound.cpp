/**
 * @file Simulation/Compound.cpp
 * Implementation of class Compound
 * @author Colin Graf
 */

#include "Compound.h"
#include "Platform/Assert.h"
#include "Simulation/Geometries/Geometry.h"
#include "Simulation/Simulation.h"

Compound::Compound(const std::string& name)
  : ::PhysicalObject(mjOBJ_UNKNOWN, findAvailableName(name, "Compound"))
{}

void Compound::addParent(Element& element)
{
  ::PhysicalObject::addParent(element);
  GraphicalObject::addParent(element);
}