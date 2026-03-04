/**
 * @file Light.h
 *
 * This file declares scene light elements.
 *
 * @author Colin Graf
 * @author Arne Hasselbring
 */

#pragma once

#include "Parser/ElementCore3.h"
#include <Engine/Lightning/PointLight.h>
#include <Engine/Lightning/DirectionalLight.h>
#include <Engine/Lightning/SpotLight.h>

/**
 * @class SimLight
 * A scene light definition
 */
class SimLight : public ElementCore3
{
public:
  /** Default constructor */
  SimLight();

  /**
   * Registers an element as parent
   * @param element The element to register
   */
  void addParent(Element& element) override;
};
/**
 * @class DirLight
 * A directional light definition
 */
class SimDirLight : public SimLight, public DirectionalLight
{
public:

  /** Default constructor */
  SimDirLight();
};

/**
 * @class PointLight
 * A point light definition
 */
class SimPointLight : public SimLight, public PointLight
{
public:
  /** Default constructor */
  SimPointLight();
};

/**
 * @class SpotLight
 * A spot light definition
 */
class SimSpotLight : public SimLight, public SpotLight
{
public:
  /** Default constructor */
  SimSpotLight();
};
