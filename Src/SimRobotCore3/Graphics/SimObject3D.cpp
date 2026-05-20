#include "SimObject3D.h"
#include "Graphics/Object3D.h"
#include "Graphics/ShaderProgram.h"
#include "System/MemoryManagement.h"
#include "Simulation/Simulation.h"
#include <Engine/Rendering/Camera.h>

SimObject3D::SimObject3D(Mesh *mesh, std::string name) : Object3D(mesh, name)
{
  material = Material::requestMaterial(name);
}

SimObject3D::SimObject3D(std::string name) : Object3D(false)
{
  material = Material::requestMaterial(name);
  this->sName = name;
}

SimObject3D::~SimObject3D()
{

}

void SimObject3D::render()
{
  if(pShader != Simulation::simulation->forwardRenderingShader)
  {
    ShaderProgram::getCurrentlyBoundShader()->loadUniform("reflectivity", material->getReflectivity());
    ShaderProgram::getCurrentlyBoundShader()->loadUniform("refractivity", material->getRefractivity());
    ShaderProgram::getCurrentlyBoundShader()->loadUniform("specularity", material->getSpecularity());
    ShaderProgram::getCurrentlyBoundShader()->loadUniform("roughness", material->getRoughness());
    ShaderProgram::getCurrentlyBoundShader()->loadUniform("TextureMultiplier", material->getTextureMultiplier());
    ShaderProgram::getCurrentlyBoundShader()->loadUniform("viewPos", Camera::getActiveCamera()->getPosition());
  }

  ShaderProgram::getCurrentlyBoundShader()->loadUniform("color", material->getColor());
  ShaderProgram::getCurrentlyBoundShader()->loadUniform("hasTexture", material->hasTexture());

  material->bindTextures();
  renderMesh();
  material->unbindTextures();
}

void SimObject3D::renderForShadowmap()
{
  if(shadowOmitted)
    return;
  #ifdef GUM_SHADOWMAP_WITH_TRANSPARENT_TEXTURES
  if(material->hasTexture() && material->getTexture(0)->hasTransparency())
  {
    material->getTexture(0)->bind();
    ShaderProgram::getCurrentlyBoundShader()->loadUniform("withTexture", true);
  }
  #endif
  renderMesh();
  #ifdef GUM_SHADOWMAP_WITH_TRANSPARENT_TEXTURES
  ShaderProgram::getCurrentlyBoundShader()->loadUniform("withTexture", false);
  #endif
}

void SimObject3D::setMaterial(Material* material) 
{ 
  this->material = material;
}

void SimObject3D::omitShadow(bool omit)
{
  shadowOmitted = omit;
}

Material* SimObject3D::getMaterial()
{ 
  return this->material; 
}