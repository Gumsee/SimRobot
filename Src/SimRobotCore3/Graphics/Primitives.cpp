/**
 * @file Graphics/Primitives.cpp
 *
 * This file implements functions to create meshes for standard geometry objects.
 *
 * @author Arne Hasselbring
 */

#include "Primitives.h"
#include "Tools/Math/Constants.h"

bGraphicsContext::Mesh* Primitives::createLine(bGraphicsContext& bGraphicsContext, const Vector3f& start, const Vector3f& end)
{
  bGraphicsContext::VertexBuffer<bGraphicsContext::VertexPN>* vertexBuffer = bGraphicsContext.requestVertexBuffer<bGraphicsContext::VertexPN>();
  vertexBuffer->vertices.emplace_back(start, Vector3f(0.f, 0.f, 1.f));
  vertexBuffer->vertices.emplace_back(end, Vector3f(0.f, 0.f, 1.f));
  vertexBuffer->finish();

  return bGraphicsContext.requestMesh(vertexBuffer, nullptr, bGraphicsContext::lineList);
}

bGraphicsContext::Mesh* Primitives::createDisk(bGraphicsContext& bGraphicsContext, float inner, float outer, unsigned int slices)
{
  if(slices < 3 || inner < 0.f || outer < 0.f)
    return nullptr;

  bGraphicsContext::VertexBuffer<bGraphicsContext::VertexPN>* vertexBuffer = bGraphicsContext.requestVertexBuffer<bGraphicsContext::VertexPN>();
  auto& vertices = vertexBuffer->vertices;
  vertices.reserve(slices * 4);
  for(unsigned int i = 0; i < slices; ++i)
  {
    const float c = std::cos(static_cast<float>(i) * 2.f * pi / static_cast<float>(slices));
    const float s = std::sin(static_cast<float>(i) * 2.f * pi / static_cast<float>(slices));
    const float xInner = c * inner;
    const float yInner = s * inner;
    const float xOuter = c * outer;
    const float yOuter = s * outer;
    vertices.emplace_back(Vector3f(xInner, yInner, 0.f), Vector3f(0.f, 0.f, 1.f));
    vertices.emplace_back(Vector3f(xOuter, yOuter, 0.f), Vector3f(0.f, 0.f, 1.f));
    vertices.emplace_back(Vector3f(xInner, yInner, 0.f), Vector3f(0.f, 0.f, -1.f));
    vertices.emplace_back(Vector3f(xOuter, yOuter, 0.f), Vector3f(0.f, 0.f, -1.f));
  }
  vertexBuffer->finish();

  bGraphicsContext::IndexBuffer* indexBuffer = bGraphicsContext.requestIndexBuffer();
  auto& indices = indexBuffer->indices;
  indices.reserve(slices * 12);
  for(unsigned int side = 0; side < 2; ++side)
  {
    for(unsigned int i = 0; i < slices; ++i)
    {
      indices.push_back(4 * i + side + 2 * side);
      indices.push_back(4 * i + (1 - side) + 2 * side);
      indices.push_back(4 * ((i + 1) % slices) + side + 2 * side);
      indices.push_back(4 * ((i + 1) % slices) + side + 2 * side);
      indices.push_back(4 * ((i + 1) % slices) + (1 - side) + 2 * side);
      indices.push_back(4 * i + side + 2 * side);
    }
  }

  return bGraphicsContext.requestMesh(vertexBuffer, indexBuffer, bGraphicsContext::triangleList);
}

bGraphicsContext::Mesh* Primitives::createPyramid(bGraphicsContext& bGraphicsContext, float width, float height, float depth)
{
  bGraphicsContext::VertexBuffer<bGraphicsContext::VertexPN>* vertexBuffer = bGraphicsContext.requestVertexBuffer<bGraphicsContext::VertexPN>();
  auto& vertices = vertexBuffer->vertices;
  vertices.reserve(5);
  vertices.emplace_back(Vector3f(0.f, 0.f, 0.f), Vector3f(0.f, 0.f, 1.f));
  vertices.emplace_back(Vector3f(depth, -width * 0.5f, -height * 0.5f), Vector3f(0.f, 0.f, 1.f));
  vertices.emplace_back(Vector3f(depth, -width * 0.5f, height * 0.5f), Vector3f(0.f, 0.f, 1.f));
  vertices.emplace_back(Vector3f(depth, width * 0.5f, height * 0.5f), Vector3f(0.f, 0.f, 1.f));
  vertices.emplace_back(Vector3f(depth, width * 0.5f, -height * 0.5f), Vector3f(0.f, 0.f, 1.f));
  vertexBuffer->finish();

  bGraphicsContext::IndexBuffer* indexBuffer = bGraphicsContext.requestIndexBuffer();
  auto& indices = indexBuffer->indices;
  indices.reserve(16);
  for(unsigned int i = 1; i <= 4; ++i)
  {
    indices.push_back(0);
    indices.push_back(i);
  }
  for(unsigned int i = 1; i <= 4; ++i)
  {
    indices.push_back(i);
    indices.push_back((i % 4) + 1);
  }

  return bGraphicsContext.requestMesh(vertexBuffer, indexBuffer, bGraphicsContext::lineList);
}
