#pragma once
#include <Graphics/Shader.h>

static const std::string ForwardRenderingVertexShader = GLSL(
  layout (location = 0) in vec3 vertexPosition;
  layout (location = 1) in vec2 TextureCoords;
  layout (location = 2) in vec3 Normals;
  layout (location = 3) in mat4 TransMatrix;

  out vec3 FragPos;
  smooth out vec3 Normal;
  out vec2 TexCoords;

  uniform mat4 projectionMatrix;
  uniform mat4 viewMatrix;

  void main()
  {
    FragPos = vec3(TransMatrix * vec4(vertexPosition, 1.0));
    Normal = mat3(TransMatrix) * Normals;
    TexCoords = TextureCoords;
    gl_Position = projectionMatrix * viewMatrix * vec4(FragPos, 1.0);
  }
);

static const std::string ForwardRenderingFragmentShader = GLSL(
  in vec3 FragPos;
  smooth in vec3 Normal;
  in vec2 TexCoords;

  uniform vec4 color;
  uniform sampler2D texture0;
  uniform int hasTexture;

  out vec4 FragColor;

  void main()
  {
    FragColor = color;
    
    if(hasTexture > 0)
      FragColor = FragColor * texture(texture0, TexCoords);

    if(FragColor.a < 0.01)
      discard;
  }
);