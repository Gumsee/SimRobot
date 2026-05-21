#include "JpegCompression.h"
#include "JpegCompressionShader.h"
#include <Engine/PostProcessing/PostProcessing.h>
#include <Essentials/Time.h>

JpegCompression::JpegCompression(Canvas* canvas)
{
	init(canvas);

  this->pShader = ShaderProgram::requestShaderProgram("JpegCompressionShaderStage1", true);
  this->pShader->addShader(Gum::PostProcessing::VertexShader);
  this->pShader->addShader(Shader::requestShader("JpegCompressionShaderStage1", JpegCompressionStage1FragmentShader, Shader::TYPES::FRAGMENT_SHADER));
  this->pShader->build();

  this->pShader2 = ShaderProgram::requestShaderProgram("JpegCompressionShaderStage2", true);
  this->pShader2->addShader(Gum::PostProcessing::VertexShader);
  this->pShader2->addShader(Shader::requestShader("JpegCompressionShaderStage2", JpegCompressionStage2FragmentShader, Shader::TYPES::FRAGMENT_SHADER));
  this->pShader2->build();
}


JpegCompression::~JpegCompression() { }

Texture* JpegCompression::render(Texture* texture)
{
	pFramebuffer->bind();
	pFramebuffer->clear(Framebuffer::ClearFlags::COLOR);
	pShader->use();
  pShader->loadUniform("fLevel", fLevel);
  pShader->loadUniform("resolution", pFramebuffer->getSize());
	texture->bind(0);
	pRenderCanvas->render();
	texture->unbind(0);
	pShader->unuse();
  
	pShader2->use();
  pShader2->loadUniform("resolution", pFramebuffer->getSize());
	pFramebuffer->getTextureAttachment()->bind(0);
	pRenderCanvas->render();
	pFramebuffer->getTextureAttachment()->unbind(0);
	pShader2->unuse();
	pFramebuffer->unbind();
    
  return pFramebuffer->getTextureAttachment(0);
}

void JpegCompression::setLevel(const float& level)
{
  this->fLevel = level;
}