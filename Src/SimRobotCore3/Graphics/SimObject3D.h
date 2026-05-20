#pragma once
#include <Graphics/Object3D.h>
#include <Graphics/Material.h>

class SimObject3D : public Object3D
{
private:
  Material* material;
  bool shadowOmitted = false;

public:
  SimObject3D(std::string name);
  SimObject3D(Mesh *mesh, std::string name);
  ~SimObject3D();

  void render() override;
  void renderForShadowmap() override;

  void setMaterial(Material* material);
  void omitShadow(bool omit);

  Material* getMaterial();
};