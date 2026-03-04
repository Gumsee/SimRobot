/**
 * @file Simulation/Appearances/ComplexAppearance.h
 * Declaration of class ComplexAppearance
 * @author Colin Graf
 */

#pragma once

#include "Simulation/Appearances/Appearance.h"
#include "Tools/Math/Eigen.h"
#include <list>
#include <vector>

/**
 * @class ComplexAppearance
 * The graphical representation of a complex shape
 */
class ComplexAppearance : public Appearance
{
public:
  /**
   * @enum Mode
   * Possible primitive group types (\c triangles, \c quads, ...)
   */
  enum Mode
  {
    triangles,
    quads
  };

  class Vertices        : public ElementCore3 { public: std::vector<vec3> data; float unit; 
    void addParent(Element& element) override {
      ComplexAppearance* complexAppearance = dynamic_cast<ComplexAppearance*>(&element);
      ASSERT(!complexAppearance->vertices);
      complexAppearance->vertices = this;
    } 
  };
  class Normals         : public ElementCore3 { public: std::vector<vec3> data; 
    void addParent(Element& element) override {
      ComplexAppearance* complexAppearance = dynamic_cast<ComplexAppearance*>(&element);
      ASSERT(!complexAppearance->normals);
      complexAppearance->normals = this;
    }
  };
  class TexCoords       : public ElementCore3 { public: std::vector<vec2> data; 
    void addParent(Element& element) override {
      ComplexAppearance* complexAppearance = dynamic_cast<ComplexAppearance*>(&element);
      ASSERT(!complexAppearance->texCoords);
      complexAppearance->texCoords = this;
    }
  };
  class PrimitiveGroup  : public ElementCore3 { public: std::list<unsigned int> indices; Mode mode; };

  Vertices* vertices = nullptr;
  Normals* normals = nullptr;
  TexCoords* texCoords = nullptr;
  std::list<PrimitiveGroup*> primitiveGroups; /**< The primitives that define the complex shape */

private:
  /**
   * Creates a mesh for this appearance in the given graphics context
   * @param graphicsContext The graphics context to create the mesh in
   * @return The resulting mesh
   */
  Mesh* createMesh() override;

  /**
   * Creates the mesh if it is not already cached
   * @tparam VertexType The vertex type from the \c bGraphicsContext that is used for this mesh
   * @tparam withTextureCoordinates Whether the vertex type has texture coordinates
   * @param graphicsContext The graphics context to create the mesh in
   * @return The resulting mesh
   */
  Mesh* createMeshImpl();
};
