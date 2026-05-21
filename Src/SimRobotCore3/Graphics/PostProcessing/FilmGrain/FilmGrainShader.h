#pragma once
#include <Graphics/Shader.h>

static const std::string FilmGrainFragmentShader = GLSL(
  in vec2 Texcoord;
  
  out vec4 FragColor;

  uniform sampler2D texture0;
  uniform float fIntensity;
  uniform float fTime;

  const float speed = 2.0;
  const float mean = 0.5;
  const float variance = 0.5;

  float gaussian(float z, float u, float o) 
  {
    return (1.0 / (o * sqrt(2.0 * 3.1415))) * exp(-(((z - u) * (z - u)) / (2.0 * (o * o))));
  }

  void main()
  {
    vec4 color = texture(texture0, Texcoord);
    
    float t = fTime * speed;
    float seed = dot(Texcoord, vec2(12.9898, 78.233));
    float noise = gaussian(fract(sin(seed) * 43758.5453 + t), mean, variance * variance);
    vec4 grain = vec4(vec3(noise) * (1.0 - color.rgb), 0.0f) * fIntensity;
    
    FragColor = color - grain;
  }
);