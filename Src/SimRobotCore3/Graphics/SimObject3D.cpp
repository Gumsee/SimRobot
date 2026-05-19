#include "SimObject3D.h"
#include "Graphics/Object3D.h"
#include "Graphics/ShaderProgram.h"
#include "System/MemoryManagement.h"
#include "Simulation/Simulation.h"
#include <Engine/Rendering/Camera.h>

SimObject3D::SimObject3D(Mesh *mesh, std::string name) : Object3D(mesh, name)
{
  pMaterial = Material::getDefaultMaterial();
}

SimObject3D::SimObject3D(std::string name) : Object3D(false)
{
  pMaterial = Material::getDefaultMaterial();
  this->sName = name;
}

SimObject3D::~SimObject3D()
{

}

void SimObject3D::render()
{
  if(pShader != Simulation::simulation->forwardRenderingShader)
  {
    ShaderProgram::getCurrentlyBoundShader()->loadUniform("reflectivity", pMaterial->getReflectivity());
    ShaderProgram::getCurrentlyBoundShader()->loadUniform("refractivity", pMaterial->getRefractivity());
    ShaderProgram::getCurrentlyBoundShader()->loadUniform("specularity", pMaterial->getSpecularity());
    ShaderProgram::getCurrentlyBoundShader()->loadUniform("roughness", pMaterial->getRoughness());
    ShaderProgram::getCurrentlyBoundShader()->loadUniform("TextureMultiplier", pMaterial->getTextureMultiplier());
    ShaderProgram::getCurrentlyBoundShader()->loadUniform("viewPos", Camera::getActiveCamera()->getPosition());
  }

  ShaderProgram::getCurrentlyBoundShader()->loadUniform("color", pMaterial->getColor());
  ShaderProgram::getCurrentlyBoundShader()->loadUniform("hasTexture", pMaterial->hasTexture());

  pMaterial->bindTextures();
  renderMesh();
  pMaterial->unbindTextures();
}

void SimObject3D::renderForShadowmap()
{
  #ifdef GUM_SHADOWMAP_WITH_TRANSPARENT_TEXTURES
  if(pMaterial->hasTexture() && pMaterial->getTexture(0)->hasTransparency())
  {
    pMaterial->getTexture(0)->bind();
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
  this->pMaterial = material;
}

Material* SimObject3D::getMaterial()
{ 
  return this->pMaterial; 
}