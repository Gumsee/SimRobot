#pragma once
#include <Graphics/Shader.h>

//Credit https://www.shadertoy.com/view/DtlfWH

static const std::string JpegCompressionStage1FragmentShader = GLSL(
  in vec2 Texcoord;
  
  out vec4 FragColor;

  uniform sampler2D texture0;
  uniform float fLevel;
  uniform ivec2 resolution;

  float dct(vec2 k, vec2 x) {
    return cos(GUM_PI*k.x*x.x)*cos((GUM_PI*k.y*x.y));
  }

  mat3 rgb_to_ycbcr = mat3(
    0.299, -0.168736, 0.5,
    0.587, -0.331264, -0.418688,
    0.144, 0.5, -0.081312
  );

  vec3 rgb_to_ycbcr_offset = vec3(0., .5, .5);

  void main()
  {
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 k = mod(fragCoord, 8.)- .5;
    vec2 block = fragCoord - .5 - k;
    
    vec3 col = vec3(0);
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            col += ((texture(texture0, (block + vec2(x, y) - 0.5) / resolution).xyz - 0.5)) * dct(k, (vec2(x, y) + 0.5) / 8.) * (k.x<.5?GUM_SQRT1_2:1.) * (k.y<.5?GUM_SQRT1_2:1.);
        }
    }
    
    FragColor = vec4(col/4.,1.0);
    FragColor = vec4(round(FragColor.rgb / 8. * fLevel)/fLevel*8., 1);
  }
);

static const std::string JpegCompressionStage2FragmentShader = GLSL(
  in vec2 Texcoord;
  
  out vec4 FragColor;

  uniform sampler2D texture0;
  uniform ivec2 resolution;

  const int num_freq = 8;


  float dct(vec2 k, vec2 x) 
  {
    return cos(GUM_PI*k.x*x.x)*cos((GUM_PI*k.y*x.y));
  }

  void main()
  {    
    vec2 fragCoord = gl_FragCoord.xy;
      vec2 k = mod(fragCoord, 8.)- .5;
      vec2 block = fragCoord - k - .5 ;
      
      vec3 col = vec3(0);
      for (int x = 0; x < num_freq; x++) {
          for (int y = 0; y < num_freq; y++) {
              col += texture(texture0, (block + vec2(x, y) + .5) / resolution).xyz * dct(vec2(x, y), (k+.5)/8.) * (x==0?GUM_SQRT1_2:1.) * (y==0?GUM_SQRT1_2:1.);
          }
      }
      
      FragColor = vec4((((col)/4.) + 0.5),1.0);
  }
);