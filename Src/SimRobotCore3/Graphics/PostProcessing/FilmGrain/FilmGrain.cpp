#include "FilmGrain.h"
#include "FilmGrainShader.h"
#include <Engine/PostProcessing/PostProcessing.h>
#include <Essentials/Time.h>

FilmGrain::FilmGrain(Canvas* canvas)
{
	init(canvas);

  this->pShader = ShaderProgram::requestShaderProgram("FilmGrainShader", true);
  this->pShader->addShader(Gum::PostProcessing::VertexShader);
  this->pShader->addShader(Shader::requestShader("FilmGrainShader", FilmGrainFragmentShader, Shader::TYPES::FRAGMENT_SHADER));
  this->pShader->build();

  this->pShader->addTexture("texture0", 0);
}


FilmGrain::~FilmGrain() { }

Texture* FilmGrain::render(Texture* texture)
{
	pFramebuffer->bind();
	pFramebuffer->clear(Framebuffer::ClearFlags::COLOR);
	pShader->use();
  pShader->loadUniform("fIntensity", fIntensity);
  pShader->loadUniform("fTime", Time::getTime());
	texture->bind(0);
	pRenderCanvas->render();
	texture->unbind(0);
	pShader->unuse();
	pFramebuffer->unbind();
    
  return pFramebuffer->getTextureAttachment(0);
}

void FilmGrain::setIntensity(const float& intensity)
{
  this->fIntensity = intensity;
}