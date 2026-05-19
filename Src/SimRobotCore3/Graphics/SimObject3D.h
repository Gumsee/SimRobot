#pragma once
#include <Graphics/Object3D.h>
#include <Graphics/Material.h>

class SimObject3D : public Object3D
{
protected:
  Material* pMaterial;

public:
  SimObject3D(std::string name);
  SimObject3D(Mesh *mesh, std::string name);
  ~SimObject3D();

  void render() override;
  void renderForShadowmap() override;

  void setMaterial(Material* material);

  Material* getMaterial();
};