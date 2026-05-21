#pragma once
#include <Engine/PostProcessing/Effects/PostProcessingEffect.h>

class FilmGrain : public PostProcessingEffect
{
private:
	static inline ShaderProgram *pShader = nullptr;

  float fIntensity = 0.075f;

public:
	FilmGrain(Canvas* canvas);
	~FilmGrain();

	Texture* render(Texture* texture);
  void setIntensity(const float& intensity);
};