#include "Light.h"
#include "Tools.h"
#include <iostream>

Light::Light()
{
	color = glm::vec3(1.0);
	intensity = 1;

	enableShadowMap = false;
	shadowMapResolution = 4096;
	shadowMapBias = 0.001;
	shadowMapDepthTexture = 0;
	shadowMapFBO = 0;
	shadowMapResolution = 4096;
}

Light::Light(glm::vec3 color, float intensity) : color(color), intensity(intensity)
{
	enableShadowMap = false;
	shadowMapResolution = 4096;
	shadowMapBias = 0.001;
	shadowMapDepthTexture = 0;
	shadowMapFBO = 0;
	shadowMapResolution = 4096;
}

Light::~Light()
{
	glDeleteFramebuffers(1, &shadowMapFBO);
	glDeleteTextures(1, &shadowMapDepthTexture);
}

void Light::CastShadow(bool enable)
{
	enableShadowMap = enable;

	if (enable)
	{
		if (shadowMapDepthTexture == 0)
		{
			glGenTextures(1, &shadowMapDepthTexture);
		}

		glBindTexture(GL_TEXTURE_2D, shadowMapDepthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, shadowMapResolution, shadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		if (shadowMapFBO == 0)
		{
			glGenFramebuffers(1, &shadowMapFBO);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMapDepthTexture, 0);

		GLenum status = Tools::CheckFramebufferStatus(shadowMapFBO);

		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Error in " << name << " shadow generation" << std::endl;
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
