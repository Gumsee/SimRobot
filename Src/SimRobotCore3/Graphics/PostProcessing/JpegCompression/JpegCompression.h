#pragma once
#include <Engine/PostProcessing/Effects/PostProcessingEffect.h>

class JpegCompression : public PostProcessingEffect
{
private:
	static inline ShaderProgram *pShader = nullptr;
	static inline ShaderProgram *pShader2 = nullptr;

  float fLevel = 25.0f;

public:
	JpegCompression(Canvas* canvas);
	~JpegCompression();

	Texture* render(Texture* texture);
  void setLevel(const float& level);
};