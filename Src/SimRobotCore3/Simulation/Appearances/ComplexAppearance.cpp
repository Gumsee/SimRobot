/**
 * @file Simulation/Appearances/ComplexAppearance.cpp
 * Implementation of class ComplexAppearance
 * @author Colin Graf
 */

#include "ComplexAppearance.h"
#include "Platform/Assert.h"
#include "Simulation/Simulation.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <unordered_map>

Mesh* ComplexAppearance::createMesh()
{

  std::string hash = 
    std::to_string((unsigned long long)(void**)vertices) + 
    std::to_string((unsigned long long)(void**)normals) + 
    std::to_string((unsigned long long)(void**)texCoords);

  if(Tools::mapHasKeyNotNull(Mesh::mLoadedMeshes, hash))
    return Mesh::mLoadedMeshes[hash];

  this->pMesh = new Mesh(hash);

  return createMeshImpl();
}

Mesh* ComplexAppearance::createMeshImpl()
{
  ASSERT(texCoords->data.size() == vertices->data.size());

  std::unordered_map<std::uint64_t, unsigned int> indexMap;

  auto getVertex = [this, &indexMap](std::list<unsigned int>::const_iterator& iter) -> unsigned int
  {
    const unsigned int vertexIndex = *(iter++);
    const unsigned int normalIndex = normals ? *(iter++) : vertexIndex;
    if(vertexIndex >= vertices->data.size() || (normals && normalIndex >= normals->data.size()))
      return 0; // Same as above: This does not make sense, but is better than crashing.
    const std::uint64_t combinedIndex = normals ? (vertexIndex | (static_cast<std::uint64_t>(normalIndex) << 32)) : static_cast<std::uint64_t>(vertexIndex);
    // Has this vertex already been added to the buffer?
    unsigned int& index = indexMap[combinedIndex];
    if(index)
      return index - 1;
    // Append a new vertex.
    const vec3& vertex = vertices->data[vertexIndex];
    const vec3& normal = normals ? normals->data[normalIndex] : vec3(0.0f); // If no normals are defined, they will be calculated later (as the average of the normals of all faces that this vertex is part of).
    const vec2& texCoord = texCoords ? texCoords->data[vertexIndex] : vec2(0.0f);
    pMesh->addVertex(Vertex(vertex, texCoord, normal));
    // Write the new index back to the map.
    index = pMesh->numVertices();
    return index - 1;
  };

  for(const PrimitiveGroup* primitiveGroup : primitiveGroups)
  {
    ASSERT(primitiveGroup->mode == triangles || primitiveGroup->mode == quads);
    ASSERT(primitiveGroup->indices.size() % (normals ? (primitiveGroup->mode == triangles ? 6 : 8) : (primitiveGroup->mode == triangles ? 3 : 4)) == 0);
    for(auto iter = primitiveGroup->indices.begin(), end = primitiveGroup->indices.end(); iter != end;)
    {
      const auto i1 = getVertex(iter);
      const auto i2 = getVertex(iter);
      const auto i3 = getVertex(iter);

      pMesh->addIndex(i1);
      pMesh->addIndex(i2);
      pMesh->addIndex(i3);

      vec3 n;
      if(!normals)
      {
        const auto& p1 = pMesh->getVertex(i1).position;
        const auto& p2 = pMesh->getVertex(i2).position;
        const auto& p3 = pMesh->getVertex(i3).position;

        const vec3 u = p2 - p1;
        const vec3 v = p3 - p1;
        n = vec3::normalize(vec3::cross(u, v));

        pMesh->getVertex(i1).normal += n;
        pMesh->getVertex(i2).normal += n;
        pMesh->getVertex(i3).normal += n;
      }

      if(primitiveGroup->mode == quads)
      {
        const auto i4 = getVertex(iter);
        pMesh->addIndex(i3);
        pMesh->addIndex(i4);
        pMesh->addIndex(i1);
        if(!normals)
          pMesh->getVertex(i4).normal += n;
      }
    }
  }

  if(!normals)
    for(auto& vertex : pMesh->getVertexBuffer())
      vertex.normal = vec3::normalize(vertex.normal);


  return pMesh;
}
