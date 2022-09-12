#include "Renderer.h"
#include "GeometryNode.h"
#include "Tools.h"
#include <algorithm>
#include "ShaderProgram.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"
#include "OBJLoader.h"
#include "SDL2/SDL.h"
#include <iostream>
#include <chrono>
#include "GLEW\glew.h"
#include "TextureManager.h"
#include "TileType.h"
#include "Font.h"
#include "Enemy.h"
#include "CannonsTower.h"
#include "MovingProjectile.h"
#include "CastProjectile.h"
#include "MagicsTower.h"

// RENDERER
Renderer::Renderer() : camera(glm::vec3(29, 9, 0), glm::vec3(-8, 3, 0), glm::vec3(0, 1, 0)), skybox(100), wave(0), lives(4), score(0), cash(150)
{
	m_vbo_fbo_vertices = 0;
	m_vao_fbo = 0;

	m_fbo = 0;
	m_fbo_texture = 0;

	reflectionFBO = 0;
	reflectionFBOColor = 0;
	reflectionFBODepth = 0;

	refractionFBO = 0;
	refractionFBOColor = 0;
	refractionFBODepth = 0;

	renderingMode = RENDERING_MODE::TRIANGLES;
	m_continous_time = 0.0;
}

Renderer::~Renderer()
{
	glDeleteTextures(1, &reflectionFBOColor);
	glDeleteTextures(1, &reflectionFBODepth);
	glDeleteFramebuffers(1, &reflectionFBO);

	glDeleteTextures(1, &refractionFBOColor);
	glDeleteTextures(1, &refractionFBODepth);
	glDeleteFramebuffers(1, &refractionFBO);

	// delete g_buffer
	glDeleteTextures(1, &m_fbo_texture);
	glDeleteFramebuffers(1, &m_fbo);

	// delete common data
	glDeleteVertexArrays(1, &m_vao_fbo);
	glDeleteBuffers(1, &m_vbo_fbo_vertices);
}

bool Renderer::Init(int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
	this->screenWidth = SCREEN_WIDTH;
	this->screenHeight = SCREEN_HEIGHT;

	// Initialize OpenGL functions

	//Set clear color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//This enables depth and stencil testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);
	// glClearDepth sets the value the depth buffer is set at, when we clear it

	// Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// open the viewport
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); //we set up our viewport

	bool techniques_initialization = InitRenderingTechniques();
	bool buffers_initialization = InitIntermediateShaderBuffers();
	bool items_initialization = InitCommonItems();
	bool lights_sources_initialization = InitLightSources();
	bool meshes_initialization = InitGeometricMeshes();
	ResizeBuffers(screenWidth, screenHeight);
	InitScene();

	//If there was any errors
	if (Tools::CheckGLError() != GL_NO_ERROR)
	{
		printf("Exiting with error at Renderer::Init\n");
		return false;
	}

	//If everything initialized
	return techniques_initialization && items_initialization && buffers_initialization;
}

void Renderer::Update(float dt, glm::vec2 mouse_coord)
{
	camera.Update(dt);
	m_continous_time += dt;

	sea->Update(dt);

	// Spawn pirate if wave is active
	spawner->Spawn(dt);

	for (int i = 0; i < Object::GetObjectsWithTag(Tag::Enemys).size(); ++i)
	{
		Object::GetObjectsWithTag(Tag::Enemys).at(i)->Update(dt);
	}

	for (std::shared_ptr<Object>& tower : Object::GetObjectsWithTag(Tag::CannonTower))
	{
		if (tower == unconstructedTower) continue;

		tower->Update(dt);
	}

	for (std::shared_ptr<Object>& tower : Object::GetObjectsWithTag(Tag::MagicTower))
	{
		tower->Update(dt);
	}

	for (int i = 0; i < Object::GetObjectsWithTag(Tag::Cannonball).size();)
	{
		std::shared_ptr<MovingProjectile> cannonball = std::static_pointer_cast<MovingProjectile>(Object::GetObjectsWithTag(Tag::Cannonball).at(i));
		cannonball->Update(dt);

		int x = cannonball->GetPosition().x + map->GetWidth() / 2;
		int y = cannonball->GetPosition().z + map->GetHeight() / 2;

		if (abs(cannonball->GetPosition().x) > 20 || abs(cannonball->GetPosition().z) > 20)
		{
			Object::GetObjectsWithTag(Tag::Cannonball).erase(Object::GetObjectsWithTag(Tag::Cannonball).begin() + i);
		}
		else if (x >= 0 && x < map->GetWidth() && y >= 0 && y < map->GetHeight())
		{
			if (map->GetTileState(std::pair<int, int>(y, x)) == LandscapeState::Unable)
			{
				Object::GetObjectsWithTag(Tag::Cannonball).erase(Object::GetObjectsWithTag(Tag::Cannonball).begin() + i);
			}
			else
			{
				++i;
			}
		}
		else
		{
			++i;
		}
	}


	for (int i = 0; i < Object::GetObjectsWithTag(Tag::Lightning).size();)
	{
		std::shared_ptr<CastProjectile> lightning = std::static_pointer_cast<CastProjectile>(Object::GetObjectsWithTag(Tag::Lightning).at(i));
		lightning->Update(dt);

		if (lightning->GetRemainingLifetime() < 0)
		{
			Object::GetObjectsWithTag(Tag::Lightning).erase(Object::GetObjectsWithTag(Tag::Lightning).begin() + i);
		}
		else
		{
			++i;
		}
	}

	for (int i = 0; i < Object::GetObjectsWithTag(Tag::Enemys).size();)
	{
		std::shared_ptr<Enemy> enemy = std::static_pointer_cast<Enemy>(Object::GetObjectsWithTag(Tag::Enemys).at(i));

		if (enemy->GetRemainingHitPoints() > 0)
		{
			for (int j = 0; j < Object::GetObjectsWithTag(Tag::Cannonball).size();)
			{
				std::shared_ptr<MovingProjectile> cannonball = std::static_pointer_cast<MovingProjectile>(Object::GetObjectsWithTag(Tag::Cannonball).at(j));

				if (enemy->Collide(*cannonball))
				{
					enemy->Damage(cannonball->GetDamage());
					Object::GetObjectsWithTag(Tag::Cannonball).erase(Object::GetObjectsWithTag(Tag::Cannonball).begin() + j);

					if (enemy->GetRemainingHitPoints() <= 0)
					{
						break;
					}
				}
				else
				{
					++j;
				}
			}

			++i;
		}
		else
		{
			if (enemy->GetRemainingLifetime() <= 0)
			{
				score += enemy->GetValue();
				scoreText.SetText("Score: " + std::to_string(score));
				scoreText.GenerateTextMesh();

				cash += enemy->GetValue();
				cashText.SetText("Cash: " + std::to_string(cash) + "$");
				cashText.GenerateTextMesh();

				Object::GetObjectsWithTag(Tag::Enemys).erase(Object::GetObjectsWithTag(Tag::Enemys).begin() + i);
			}
			else
			{
				++i;
			}
		}
	}

	//Check pirates collision with chest
	for (int i = 0; i < boxes.size();)
	{
		bool collided = false;

		for (int j = 0; j < Object::GetObjectsWithTag(Tag::Enemys).size(); ++j)
		{
			std::shared_ptr<Enemy> enemy = std::static_pointer_cast<Enemy>(Object::GetObjectsWithTag(Tag::Enemys).at(j));

			if (boxes.at(i)->Collide(*enemy) && enemy->GetRemainingHitPoints() > 0)
			{
				Object::GetObjectsWithTag(Tag::Enemys).erase(Object::GetObjectsWithTag(Tag::Enemys).begin() + j);
				collided = true;
				lives -= 1;

				if (lives == 0) CurrentState = State::GAME_OVER;

				livesText.SetText("Lives: " + std::to_string(lives));
				livesText.GenerateTextMesh();

				break;
			}
			else
			{
				++j;
			}
		}

		if (collided)
		{
			boxes.erase(boxes.begin() + i);
		}
		else
		{
			++i;
		}

		if (lives == 0)
		{
			break;
		}

	}



	switch (CurrentState)
	{
	case State::PLACE_TOWER:
	case State::PLACE_MAGIC_TOWER:
		unconstructedTower->MoveAt(glm::vec3(minX, m_mouse_wcs.y, minZ));
		break;
	case State::GAME_OVER:
		dim = glm::clamp(dim - dt * 0.5f, 0.4f, 1.0f);
	default:
		break;
	}


	//Get selected tile color depending on build capability
	m_mouse_scs = glm::vec3(mouse_coord, 0);

	currentPosition = m_mouse_wcs;

	int x = currentPosition.x + map->GetWidth() / 2;
	int y = currentPosition.z + map->GetHeight() / 2;

	if (x >= 0 && x < map->GetWidth() && y >= 0 && y < map->GetHeight())
	{
		if (map->GetTileState(std::pair<int, int>(y, x)) == LandscapeState::Empty && (CurrentState == State::PLACE_TOWER || CurrentState == State::PLACE_MAGIC_TOWER))
		{
			m_selection_color = glm::vec3(0, 1, 0);
		}
		else if ((map->GetTileState(std::pair<int, int>(y, x)) == LandscapeState::Magic || map->GetTileState(std::pair<int, int>(y, x)) == LandscapeState::Cannon) && CurrentState == State::REMOVE_TOWER)
		{
			m_selection_color = glm::vec3(0, 1, 0);
		}
		else
		{
			m_selection_color = glm::vec3(1, 0, 0);
		}
	}

	currentPosition = m_mouse_wcs;
}

bool Renderer::InitCommonItems()
{
	glGenVertexArrays(1, &m_vao_fbo);
	glBindVertexArray(m_vao_fbo);

	GLfloat fbo_vertices[] = {
		-1, -1,
		1, -1,
		-1, 1,
		1, 1,
	};

	glGenBuffers(1, &m_vbo_fbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_fbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);

	GLenum wrap_mode = GL_REPEAT;
	GLint max_anisotropy = 1;
	glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy);

	glGenSamplers(1, &m_trilinear_sampler);
	glSamplerParameteri(m_trilinear_sampler, GL_TEXTURE_WRAP_S, wrap_mode);
	glSamplerParameteri(m_trilinear_sampler, GL_TEXTURE_WRAP_T, wrap_mode);
	glSamplerParameteri(m_trilinear_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(m_trilinear_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(m_trilinear_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropy);

	return true;
}

bool Renderer::InitRenderingTechniques()
{
	bool initialized = true;

	std::string vertex_shader_path = "../Data/Shaders/terrainNormalMapping.vert";
	std::string fragment_shader_path = "../Data/Shaders/terrainNormalMapping.frag";
	terrainNormalMappingShader.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	terrainNormalMappingShader.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	terrainNormalMappingShader.CreateProgram();
	terrainNormalMappingShader.LoadUniform("uniform_projection_matrix");
	terrainNormalMappingShader.LoadUniform("uniform_view_matrix");
	terrainNormalMappingShader.LoadUniform("uniform_model_matrix");
	terrainNormalMappingShader.LoadUniform("uniform_normal_matrix");
	terrainNormalMappingShader.LoadUniform("uniform_diffuse");
	terrainNormalMappingShader.LoadUniform("uniform_specular");
	terrainNormalMappingShader.LoadUniform("uniform_shininess");
	terrainNormalMappingShader.LoadUniform("uniform_has_texture");
	terrainNormalMappingShader.LoadUniform("diffuse_texture");
	terrainNormalMappingShader.LoadUniform("uniform_camera_position");
	// Light Source Uniforms
	terrainNormalMappingShader.LoadUniform("uniform_light_projection_matrix");
	terrainNormalMappingShader.LoadUniform("uniform_light_view_matrix");
	terrainNormalMappingShader.LoadUniform("uniform_cast_shadows");
	terrainNormalMappingShader.LoadUniform("shadowmap_texture");
	//Textures
	terrainNormalMappingShader.LoadUniform("groundTexture");
	terrainNormalMappingShader.LoadUniform("groundNormalMap");
	terrainNormalMappingShader.LoadUniform("grassTexture");
	terrainNormalMappingShader.LoadUniform("grassNormalMap");
	terrainNormalMappingShader.LoadUniform("roadTexture");
	terrainNormalMappingShader.LoadUniform("roadNormalMap");
	terrainNormalMappingShader.LoadUniform("mountainTexture");
	terrainNormalMappingShader.LoadUniform("mountainNormalMap");
	terrainNormalMappingShader.LoadUniform("blendMap");
	terrainNormalMappingShader.LoadUniform("dudvMap");
	terrainNormalMappingShader.LoadUniform("caustics");
	terrainNormalMappingShader.LoadUniform("moveFactor");
	terrainNormalMappingShader.LoadUniform("directionalLight.direction");
	terrainNormalMappingShader.LoadUniform("directionalLight.color");


	vertex_shader_path = "../Data/Shaders/terrain.vert";
	fragment_shader_path = "../Data/Shaders/terrain.frag";
	terrainShader.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	terrainShader.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	terrainShader.CreateProgram();
	terrainShader.LoadUniform("uniform_projection_matrix");
	terrainShader.LoadUniform("uniform_view_matrix");
	terrainShader.LoadUniform("uniform_model_matrix");
	terrainShader.LoadUniform("uniform_normal_matrix");
	terrainShader.LoadUniform("uniform_diffuse");
	terrainShader.LoadUniform("uniform_specular");
	terrainShader.LoadUniform("uniform_shininess");
	terrainShader.LoadUniform("uniform_has_texture");
	terrainShader.LoadUniform("diffuse_texture");
	terrainShader.LoadUniform("uniform_camera_position");
	terrainShader.LoadUniform("plane");
	// Light Source Uniforms
	terrainShader.LoadUniform("uniform_light_projection_matrix");
	terrainShader.LoadUniform("uniform_light_view_matrix");
	terrainShader.LoadUniform("uniform_cast_shadows");
	terrainShader.LoadUniform("shadowmap_texture");
	//Textures
	terrainShader.LoadUniform("groundTexture");
	terrainShader.LoadUniform("grassTexture");
	terrainShader.LoadUniform("roadTexture");
	terrainShader.LoadUniform("mountainTexture");
	terrainShader.LoadUniform("blendMap");
	terrainShader.LoadUniform("directionalLight.direction");
	terrainShader.LoadUniform("directionalLight.color");
	
	vertex_shader_path = "../Data/Shaders/modelRendering.vert";
	fragment_shader_path = "../Data/Shaders/modelRendering.frag";
	modelShader.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	modelShader.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	modelShader.CreateProgram();
	modelShader.LoadUniform("uniform_projection_matrix");
	modelShader.LoadUniform("uniform_view_matrix");
	modelShader.LoadUniform("uniform_model_matrix");
	modelShader.LoadUniform("uniform_normal_matrix");
	modelShader.LoadUniform("uniform_light_projection_matrix");
	modelShader.LoadUniform("uniform_light_view_matrix");
	modelShader.LoadUniform("uniform_diffuse");
	modelShader.LoadUniform("uniform_specular");
	modelShader.LoadUniform("uniform_shininess");
	modelShader.LoadUniform("uniform_has_texture");
	modelShader.LoadUniform("diffuse_texture");
	modelShader.LoadUniform("uniform_camera_position");
	modelShader.LoadUniform("directionalLight.direction");
	modelShader.LoadUniform("directionalLight.color");
	modelShader.LoadUniform("uniform_cast_shadows");
	modelShader.LoadUniform("shadowmap_texture");
	modelShader.LoadUniform("opacity");

	vertex_shader_path = "../Data/Shaders/overlayRendering.vert";
	fragment_shader_path = "../Data/Shaders/overlayRendering.frag";
	overlayShader.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	overlayShader.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	overlayShader.CreateProgram();
	overlayShader.LoadUniform("uniform_projection_matrix");
	overlayShader.LoadUniform("uniform_view_matrix");
	overlayShader.LoadUniform("uniform_model_matrix");
	overlayShader.LoadUniform("uniform_normal_matrix");

	overlayShader.LoadUniform("minX");
	overlayShader.LoadUniform("maxX");
	overlayShader.LoadUniform("minZ");
	overlayShader.LoadUniform("maxZ");
	overlayShader.LoadUniform("selection_color");
	overlayShader.LoadUniform("radius");
	overlayShader.LoadUniform("position");
	overlayShader.LoadUniform("opacity");

	vertex_shader_path = "../Data/Shaders/water.vert";
	fragment_shader_path = "../Data/Shaders/water.frag";
	waterShader.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	waterShader.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	waterShader.CreateProgram();
	waterShader.LoadUniform("uniform_projection_matrix");
	waterShader.LoadUniform("uniform_view_matrix");
	waterShader.LoadUniform("uniform_model_matrix");
	waterShader.LoadUniform("reflectionTexture");
	waterShader.LoadUniform("refractionTexture");
	waterShader.LoadUniform("depthTexture");
	waterShader.LoadUniform("dudvMap");
	waterShader.LoadUniform("normalMap");
	waterShader.LoadUniform("moveFactor");
	waterShader.LoadUniform("cameraPosition");
	waterShader.LoadUniform("directionalLight.direction");
	waterShader.LoadUniform("directionalLight.color");
	waterShader.LoadUniform("uniform_light_projection_matrix");
	waterShader.LoadUniform("uniform_light_view_matrix");
	waterShader.LoadUniform("uniform_cast_shadows");
	waterShader.LoadUniform("shadowmap_texture");

	vertex_shader_path = "../Data/Shaders/skybox.vert";
	fragment_shader_path = "../Data/Shaders/skybox.frag";
	skyboxShader.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	skyboxShader.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	skyboxShader.CreateProgram();
	skyboxShader.LoadUniform("uniform_projection_matrix");
	skyboxShader.LoadUniform("uniform_view_matrix");
	skyboxShader.LoadUniform("cubeMap");

	vertex_shader_path = "../Data/Shaders/postproc.vert";
	fragment_shader_path = "../Data/Shaders/postproc.frag";
	m_postprocess_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_postprocess_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	m_postprocess_program.CreateProgram();
	m_postprocess_program.LoadUniform("uniform_texture");
	m_postprocess_program.LoadUniform("uniform_depth_texture");
	m_postprocess_program.LoadUniform("uniform_time");
	m_postprocess_program.LoadUniform("gameOver");
	m_postprocess_program.LoadUniform("dim");

	vertex_shader_path = "../Data/Shaders/shadow_map_rendering.vert";
	fragment_shader_path = "../Data/Shaders/shadow_map_rendering.frag";
	shadowMapShader.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	shadowMapShader.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	shadowMapShader.CreateProgram();
	shadowMapShader.LoadUniform("uniform_projection_matrix");
	shadowMapShader.LoadUniform("uniform_view_matrix");
	shadowMapShader.LoadUniform("uniform_model_matrix");

	vertex_shader_path = "../Data/Shaders/text.vert";
	fragment_shader_path = "../Data/Shaders/text.frag";
	textShader.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	textShader.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	textShader.CreateProgram();
	textShader.LoadUniform("color");
	textShader.LoadUniform("position");
	textShader.LoadUniform("scale");
	textShader.LoadUniform("fontAtlas");
	textShader.LoadUniform("fixHeight");

	vertex_shader_path = "../Data/Shaders/gui.vert";
	fragment_shader_path = "../Data/Shaders/gui.frag";
	guiShader.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	guiShader.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	guiShader.CreateProgram();
	guiShader.LoadUniform("background");

	vertex_shader_path = "../Data/Shaders/billboard.vert";
	fragment_shader_path = "../Data/Shaders/billboard.frag";
	billboardShader.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	billboardShader.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	billboardShader.CreateProgram();
	billboardShader.LoadUniform("uniform_projection_matrix");
	billboardShader.LoadUniform("uniform_view_matrix");
	billboardShader.LoadUniform("uniform_model_matrix");
	billboardShader.LoadUniform("axis");
	billboardShader.LoadUniform("time");
	billboardShader.LoadUniform("opacity");

	vertex_shader_path = "../Data/Shaders/tower_rendering.vert";
	fragment_shader_path = "../Data/Shaders/tower_rendering.frag";
	ghostShader.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	ghostShader.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	ghostShader.CreateProgram();
	ghostShader.LoadUniform("uniform_projection_matrix");
	ghostShader.LoadUniform("uniform_view_matrix");
	ghostShader.LoadUniform("uniform_model_matrix");
	ghostShader.LoadUniform("color");

	return initialized;
}

bool Renderer::ReloadShaders()
{
	bool reloaded = true;
	terrainNormalMappingShader.ReloadProgram();
	terrainShader.ReloadProgram();
	waterShader.ReloadProgram();
	overlayShader.ReloadProgram();
	m_postprocess_program.ReloadProgram();
	shadowMapShader.ReloadProgram();

	return reloaded;
}

bool Renderer::InitIntermediateShaderBuffers()
{
	glGenTextures(1, &m_fbo_texture);
	glGenTextures(1, &m_fbo_depth_texture);
	glGenFramebuffers(1, &m_fbo);

	glGenTextures(1, &reflectionFBOColor);
	glGenTextures(1, &reflectionFBODepth);
	glGenFramebuffers(1, &reflectionFBO);

	glGenTextures(1, &refractionFBOColor);
	glGenTextures(1, &refractionFBODepth);
	glGenFramebuffers(1, &refractionFBO);

	return true;
}

bool Renderer::ResizeBuffers(int width, int height)
{
	screenWidth = width;
	screenHeight = height;

	statusBar.Resize(screenWidth, screenHeight);
	scoreText.Resize(screenWidth, screenHeight);
	livesText.Resize(screenWidth, screenHeight);
	waveText.Resize(screenWidth, screenHeight);
	cashText.Resize(screenWidth, screenHeight);
	gameOver.Resize(screenWidth, screenHeight);

	// texture
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	// depth texture
	glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// framebuffer to link to everything together
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_fbo_depth_texture, 0);

	GLenum status = Tools::CheckFramebufferStatus(m_fbo);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, reflectionFBOColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, reflectionFBODepth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionFBOColor, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, reflectionFBODepth, 0);

	status = Tools::CheckFramebufferStatus(reflectionFBO);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Error creating reflection framebuffer object." << std::endl;
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, refractionFBOColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, refractionFBODepth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionFBOColor, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, refractionFBODepth, 0);

	status = Tools::CheckFramebufferStatus(refractionFBO);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Error creating refraction framebuffer object." << std::endl;
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	projectionMatrix = glm::perspective(glm::radians(60.f), width / (float)height, 0.1f, 1500.0f);

	return true;
}

bool Renderer::InitLightSources()
{
	directionalLight = DirectionalLight(glm::vec3(-0.3, -1.0, -1.0), glm::vec3(1.0), 40);

	return true;
}

bool Renderer::InitGeometricMeshes()
{	
	font = Font("../Data/harrington.fnt");
	font.AddTextureAtlas("../Data/harrington.png");
	scoreText = Text("Score: " + std::to_string(score), font, 2.3, glm::vec3(1, 1, 1), glm::vec2(0.0, 0.02), 0.25, true, true);
	livesText = Text("Lives: " + std::to_string(lives), font, 2.3, glm::vec3(1, 1, 1), glm::vec2(0.25, 0.02), 0.25, true, true);
	waveText = Text("Wave: " + std::to_string(wave), font, 2.3, glm::vec3(1, 1, 1), glm::vec2(0.5, 0.02), 0.25, true, true);
	cashText = Text("Cash: " + std::to_string(cash) + "$", font, 2.3, glm::vec3(1, 1, 1), glm::vec2(0.75, 0.02), 0.25, true, true);
	gameOver = Text("Game Over", font, 4, glm::vec3(1, 1, 1), glm::vec2(0, 0.48), 1, true, false);

	statusBar = StatusBar(70);
	statusBar.AddBackgroundTexture("../Data/woodenPlank.png");

	library.AddObject("MagicTowerTier1", "../Data/MagicTowers/MagicTowerTier1.obj");

	library.AddObject("CannonTowerTier1", "../Data/CannonTowers/CannonTowerTier1.obj");
	library.AddObject("Cannonball", "../Data/Projectiles/Cannonball.obj");

	library.AddObject("PirateBody", "../Data/Pirate/pirate_body.obj");
	library.AddObject("PirateLeftFoot", "../Data/Pirate/pirate_left_foot.obj");
	library.AddObject("PirateRightFoot", "../Data/Pirate/pirate_right_foot.obj");
	library.AddObject("PirateRightHand", "../Data/Pirate/pirate_arm.obj");

	library.AddObject("Tree2", "../Data/Trees/Tree2.obj");
	library.AddObject("Terrain", "../Data/Terrain/mapLowPoly.obj");
	library.AddObject("Plane", "../Data/Terrain/plane.obj");

	library.AddObject("BellHouse", "../Data/Castle/BellHouse.obj");
	library.AddObject("Box", "../Data/Castle/Box.obj");
	library.AddObject("Door", "../Data/Castle/Door.obj");
	library.AddObject("TowerSquareSmall", "../Data/Castle/TowerSquareSmall.obj");
	library.AddObject("TowerSquareTall", "../Data/Castle/TowerSquareTall.obj");
	library.AddObject("Wall", "../Data/Castle/Wall.obj");
	library.AddObject("WallCorner", "../Data/Castle/WallCorner.obj");
	library.AddObject("WallSmall", "../Data/Castle/WallSmall.obj");
	library.AddObject("Warehouse", "../Data/Castle/Warehouse.obj");
	library.AddObject("Window", "../Data/Castle/Window.obj");

	lightningModel = std::make_shared<GeometryNode>(GeometryNode());
	lightningModel->InitLightning();

	return true;
}

void Renderer::InitScene()
{
	CurrentState = State::NONE;

	map = Landscape::CreateInstance(library.GetObject("Terrain"));
	map->LoadLandscapeState("../Data/Terrain/mapLowPoly.info");
	map->LoadPath("../Data/Terrain/mapLowPoly.txt");

	map->AddTexture("groundTexture", "../Data/Maps/rock_ground_1/4K-rock_ground_1/4K-rock_ground_1-diffuse.jpg", true);
	map->AddTexture("groundNormalMap", "../Data/Maps/rock_ground_1/2K-rock_ground_1/2K-rock_ground_1-normal.jpg", true);
	map->AddTexture("grassTexture", "../Data/Maps/grass/grass01.jpg", true);
	map->AddTexture("grassNormalMap", "../Data/Maps/grass/grass01_n.jpg", true);
	map->AddTexture("roadTexture", "../Data/Maps/dirt_3/4K-dirt_3/4K-ground_dirt_3-diffuse.jpg", true);
	map->AddTexture("roadNormalMap", "../Data/Maps/dirt_3/2K-dirt_3/2K-ground_dirt_3-normal.jpg", true);
	map->AddTexture("mountainTexture", "../Data/Maps/ground_2/4K-ground_2/4K-ground_2-diffuse.jpg", true);
	map->AddTexture("mountainNormalMap", "../Data/Maps/ground_2/2K-ground_2/2K-ground_2-normal.jpg", true);
	map->AddTexture("blendMap", "../Data/Maps/blendMap.png", false);
	map->AddTexture("dudvMap", "../Data/Maps/waterDUDV.png", true);
	map->AddTexture("caustics", "../Data/Maps/caustics.jpg", true);

	sea = std::make_shared<Sea>(Sea(library.GetObject("Plane"), 0.03));

	sea->AddTexture("dudvMap", "../Data/Maps/waterDUDV.png", true);
	sea->AddTexture("normalMap", "../Data/Maps/waterNormalMap.png", true);
	sea->AddTexture("reflectionTexture", reflectionFBOColor);
	sea->AddTexture("refractionTexture", refractionFBOColor);
	sea->AddTexture("depthTexture", refractionFBODepth);


	std::vector<std::string> cubeMap;
	cubeMap.push_back("../Data/Maps/skybox/left.png");
	cubeMap.push_back("../Data/Maps/skybox/right.png");
	cubeMap.push_back("../Data/Maps/skybox/top.png");
	cubeMap.push_back("../Data/Maps/skybox/bottom.png");
	cubeMap.push_back("../Data/Maps/skybox/back.png");
	cubeMap.push_back("../Data/Maps/skybox/front.png");
	skybox.AddTexture(cubeMap);

	castleParts.push_back(std::make_shared<CastlePart>(CastlePart(library.GetObject("TowerSquareSmall"), glm::vec3(-16.5705, 0, 10.2664), glm::vec3(0.0, 1.0, 0.0), 0, glm::vec3(0.3885))));
	castleParts.push_back(std::make_shared<CastlePart>(CastlePart(library.GetObject("TowerSquareSmall"), glm::vec3(-12.9337, 0, 18.4110), glm::vec3(0.0, 1.0, 0.0), 0, glm::vec3(0.3885))));
	castleParts.push_back(std::make_shared<CastlePart>(CastlePart(library.GetObject("TowerSquareTall"), glm::vec3(-12.9337, 0, 16.1525), glm::vec3(0.0, 1.0, 0.0), 0, glm::vec3(0.3885))));
	castleParts.push_back(std::make_shared<CastlePart>(CastlePart(library.GetObject("TowerSquareTall"), glm::vec3(-12.9337, 0, 13.8664), glm::vec3(0.0, 1.0, 0.0), 0, glm::vec3(0.3885))));
	castleParts.push_back(std::make_shared<CastlePart>(CastlePart(library.GetObject("Wall"), glm::vec3(-12.9337, 0, 17.2897), glm::vec3(0.0, 1.0, 0.0), -glm::half_pi<float>(), glm::vec3(0.3885))));
	castleParts.push_back(std::make_shared<CastlePart>(CastlePart(library.GetObject("Wall"), glm::vec3(-12.9337, 0, 12.7485), glm::vec3(0.0, 1.0, 0.0), -glm::half_pi<float>(), glm::vec3(0.3885))));
	castleParts.push_back(std::make_shared<CastlePart>(CastlePart(library.GetObject("Wall"), glm::vec3(-15.4339, 0, 10.2665), glm::vec3(0.0, 1.0, 0.0), 0, glm::vec3(0.3885))));
	castleParts.push_back(std::make_shared<CastlePart>(CastlePart(library.GetObject("WallCorner"), glm::vec3(-13.9086, 0, 10.2665), glm::vec3(0.0, 1.0, 0.0), -glm::half_pi<float>(), glm::vec3(0.3885))));
	castleParts.push_back(std::make_shared<CastlePart>(CastlePart(library.GetObject("WallSmall"), glm::vec3(-12.9337, 1.0342, 15.0234), glm::vec3(0.0, 1.0, 0.0), -glm::half_pi<float>(), glm::vec3(0.3885))));
	castleParts.push_back(std::make_shared<CastlePart>(CastlePart(library.GetObject("Warehouse"), glm::vec3(-17.8940, 0, 15.8919), glm::vec3(0.0, 1.0, 0.0), -glm::half_pi<float>(), glm::vec3(0.3885))));
	castleParts.push_back(std::make_shared<CastlePart>(CastlePart(library.GetObject("BellHouse"), glm::vec3(-19.1192, 0, 17.9523), glm::vec3(0.0, 1.0, 0.0), 0, glm::vec3(0.3885))));
	castleParts.push_back(std::make_shared<CastlePart>(CastlePart(library.GetObject("Door"), glm::vec3(-16.5000, 0, 15.8261), glm::vec3(0.0, 1.0, 0.0), glm::half_pi<float>(), glm::vec3(0.3885))));
	castleParts.push_back(std::make_shared<CastlePart>(CastlePart(library.GetObject("Door"), glm::vec3(-16.5000, 0, 15.2816), glm::vec3(0.0, 1.0, 0.0), glm::half_pi<float>(), glm::vec3(0.3885))));
	castleParts.push_back(std::make_shared<CastlePart>(CastlePart(library.GetObject("Door"), glm::vec3(-18.4193, 0, 17.9723), glm::vec3(0.0, 1.0, 0.0), glm::half_pi<float>(), glm::vec3(0.3885))));
	castleParts.push_back(std::make_shared<CastlePart>(CastlePart(library.GetObject("Window"), glm::vec3(-16.54, 1.7928, 15.8261), glm::vec3(0.0, 1.0, 0.0), -glm::half_pi<float>(), glm::vec3(0.3885))));

	boxes.push_back(std::make_shared<Box>(Box(library.GetObject("Box"), glm::vec3(-14.4780, 0.3885, 16.3527), glm::vec3(0.3885), glm::vec3(-14.0, 0.0, 15.0), 1.0)));
	boxes.push_back(std::make_shared<Box>(Box(library.GetObject("Box"), glm::vec3(-14.2704, 0, 16.4625), glm::vec3(0.3885), glm::vec3(-14.0, 0.0, 15.0), 1.0)));
	boxes.push_back(std::make_shared<Box>(Box(library.GetObject("Box"), glm::vec3(-14.7838, 0, 16.2667), glm::vec3(0.3885), glm::vec3(-14.0, 0.0, 15.0), 1.0)));
	boxes.push_back(std::make_shared<Box>(Box(library.GetObject("Box"), glm::vec3(-14.7198, 0, 16.7195), glm::vec3(0.3885), glm::vec3(-14.0, 0.0, 15.0), 1.0)));

	////Setup spawner
	spawner = std::make_shared<Spawner>(Spawner(library, glm::vec3(19, 0, -17), glm::half_pi<float>()));

	std::default_random_engine generator;

	std::uniform_real_distribution<double> distribution(-0.5, 0.5);

	//Setup trees
	trees.push_back(std::make_shared<Tree>(Tree(library.GetObject("Tree2"), glm::vec3(-11.5 + distribution(generator), 0, -0.5 + distribution(generator)), glm::vec3(0.0, 1.0, 0.0), distribution(generator) * 2 * glm::pi<double>(), glm::vec3(0.4 + distribution(generator) * 0.1))));
	trees.push_back(std::make_shared<Tree>(Tree(library.GetObject("Tree2"), glm::vec3(-13.5 + distribution(generator), 0, -18.5 + distribution(generator)), glm::vec3(0.0, 1.0, 0.0), distribution(generator) * 2 * glm::pi<double>(), glm::vec3(0.4 + distribution(generator) * 0.1))));
	trees.push_back(std::make_shared<Tree>(Tree(library.GetObject("Tree2"), glm::vec3(-7.5 + distribution(generator), 0, -16.5 + distribution(generator)), glm::vec3(0.0, 1.0, 0.0), distribution(generator) * 2 * glm::pi<double>(), glm::vec3(0.4 + distribution(generator) * 0.1))));
	trees.push_back(std::make_shared<Tree>(Tree(library.GetObject("Tree2"), glm::vec3(0.5 + distribution(generator), 0, -5.5 + distribution(generator)), glm::vec3(0.0, 1.0, 0.0), distribution(generator) * 2 * glm::pi<double>(), glm::vec3(0.4 + distribution(generator) * 0.1))));
	trees.push_back(std::make_shared<Tree>(Tree(library.GetObject("Tree2"), glm::vec3(9.5 + distribution(generator), 0, -3.5 + distribution(generator)), glm::vec3(0.0, 1.0, 0.0), distribution(generator) * 2 * glm::pi<double>(), glm::vec3(0.4 + distribution(generator) * 0.1))));
	trees.push_back(std::make_shared<Tree>(Tree(library.GetObject("Tree2"), glm::vec3(5.5 + distribution(generator), 0, 8.5 + distribution(generator)), glm::vec3(0.0, 1.0, 0.0), distribution(generator) * 2 * glm::pi<double>(), glm::vec3(0.4 + distribution(generator) * 0.1))));
	trees.push_back(std::make_shared<Tree>(Tree(library.GetObject("Tree2"), glm::vec3(-1.5 + distribution(generator), 0, -7.5 + distribution(generator)), glm::vec3(0.0, 1.0, 0.0), distribution(generator) * 2 * glm::pi<double>(), glm::vec3(0.4 + distribution(generator) * 0.1))));
	trees.push_back(std::make_shared<Tree>(Tree(library.GetObject("Tree2"), glm::vec3(15.5 + distribution(generator), 0, -4.5 + distribution(generator)), glm::vec3(0.0, 1.0, 0.0), distribution(generator) * 2 * glm::pi<double>(), glm::vec3(0.4 + distribution(generator) * 0.1))));
	trees.push_back(std::make_shared<Tree>(Tree(library.GetObject("Tree2"), glm::vec3(17.5 + distribution(generator), 0, -2.5 + distribution(generator)), glm::vec3(0.0, 1.0, 0.0), distribution(generator) * 2 * glm::pi<double>(), glm::vec3(0.4 + distribution(generator) * 0.1))));
	trees.push_back(std::make_shared<Tree>(Tree(library.GetObject("Tree2"), glm::vec3(0.5 + distribution(generator), 0, -10.5 + distribution(generator)), glm::vec3(0.0, 1.0, 0.0), distribution(generator) * 2 * glm::pi<double>(), glm::vec3(0.4 + distribution(generator) * 0.1))));
	trees.push_back(std::make_shared<Tree>(Tree(library.GetObject("Tree2"), glm::vec3(18.5 + distribution(generator), 0, -2.5 + distribution(generator)), glm::vec3(0.0, 1.0, 0.0), distribution(generator) * 2 * glm::pi<double>(), glm::vec3(0.4 + distribution(generator) * 0.1))));
	trees.push_back(std::make_shared<Tree>(Tree(library.GetObject("Tree2"), glm::vec3(17.5 + distribution(generator), 0, -14.5 + distribution(generator)), glm::vec3(0.0, 1.0, 0.0), distribution(generator) * 2 * glm::pi<double>(), glm::vec3(0.4 + distribution(generator) * 0.1))));
	trees.push_back(std::make_shared<Tree>(Tree(library.GetObject("Tree2"), glm::vec3(14.5 + distribution(generator), 0, 4.5 + distribution(generator)), glm::vec3(0.0, 1.0, 0.0), distribution(generator) * 2 * glm::pi<double>(), glm::vec3(0.4 + distribution(generator) * 0.1))));

	directionalLight.CastShadow(true);
}

void Renderer::SetRenderingMode(RENDERING_MODE mode)
{
	renderingMode = mode;
}

void Renderer::Render()
{
	RenderShadowMaps();

	RenderSkybox();

	RenderReflection();

	RenderRefraction();

	RenderGeometry();

	if (CurrentState == PLACE_TOWER || CurrentState == REMOVE_TOWER || CurrentState == PLACE_MAGIC_TOWER)
	{
		RenderOverlay();
	}

	RenderToOutFB();

	GLenum error = Tools::CheckGLError();
	if (error != GL_NO_ERROR)
	{
		printf("Reanderer:Draw GL Error\n");
		system("pause");
	}
}

void Renderer::RenderSkybox()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, screenWidth, screenHeight);
	GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawbuffers);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	skyboxShader.Bind();
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	viewMatrix[3][0] = 0;
	viewMatrix[3][1] = 0;
	viewMatrix[3][2] = 0;
	viewMatrix = glm::rotate(viewMatrix, float(m_continous_time * 0.02), glm::vec3(0.0, 1.0, 0.0));
	glUniformMatrix4fv(skyboxShader["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(skyboxShader["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(viewMatrix));
	skybox.Render(skyboxShader);
	skyboxShader.Unbind();

	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderShadowMaps()
{
	shadowMapShader.Bind();
	directionalLight.SetupShadowMap(shadowMapShader);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	map->Render(shadowMapShader);

	for (int i = 0; i < Object::GetObjectsWithTag(Tag::CannonTower).size(); ++i)
	{
		Object::GetObjectsWithTag(Tag::CannonTower).at(i)->Render(shadowMapShader);
	}

	for (int i = 0; i < Object::GetObjectsWithTag(Tag::MagicTower).size(); ++i)
	{
		Object::GetObjectsWithTag(Tag::MagicTower).at(i)->Render(shadowMapShader);
	}

	for (int i = 0; i < Object::GetObjectsWithTag(Tag::Cannonball).size(); ++i)
	{
		Object::GetObjectsWithTag(Tag::Cannonball).at(i)->Render(shadowMapShader);
	}

	for (int i = 0; i < trees.size(); ++i)
	{
		trees.at(i)->Render(shadowMapShader);
	}

	for (int i = 0; i < castleParts.size(); ++i)
	{
		castleParts.at(i)->Render(shadowMapShader);
	}

	for (int i = 0; i < boxes.size(); ++i)
	{
		boxes.at(i)->Render(shadowMapShader);
	}

	for (std::shared_ptr<Object>& enemy : Object::GetObjectsWithTag(Enemys))
	{
		enemy->Render(shadowMapShader);
	}

	glBindVertexArray(0);
	shadowMapShader.Unbind();

	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderGeometry()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, screenWidth, screenHeight);
	GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawbuffers);

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	terrainNormalMappingShader.Bind();
	glUniformMatrix4fv(terrainNormalMappingShader["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(terrainNormalMappingShader["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
	glUniform3fv(terrainNormalMappingShader["uniform_camera_position"], 1, glm::value_ptr(camera.GetPosition()));
	glUniform1f(terrainNormalMappingShader["moveFactor"], sea->GetMoveFactor());
	directionalLight.SetupLight(terrainNormalMappingShader);

	map->Render(terrainNormalMappingShader);

	terrainNormalMappingShader.Unbind();

	CalculateMouseCoordToWCS();

	modelShader.Bind();
	glUniformMatrix4fv(modelShader["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(modelShader["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
	glUniform3fv(modelShader["uniform_camera_position"], 1, glm::value_ptr(camera.GetPosition()));
	directionalLight.SetupLight(modelShader);

	for (std::shared_ptr<Object>& tower : Object::GetObjectsWithTag(Tag::CannonTower))
	{
		tower->Render(modelShader);
	}

	for (std::shared_ptr<Object>& tower : Object::GetObjectsWithTag(Tag::MagicTower))
	{
		tower->Render(modelShader);
	}

	for (std::shared_ptr<Object>& cannonball : Object::GetObjectsWithTag(Tag::Cannonball))
	{
		cannonball->Render(modelShader);
	}

	for (int i = 0; i < trees.size(); ++i)
	{
		trees.at(i)->Render(modelShader);
	}

	for (int i = 0; i < castleParts.size(); ++i)
	{
		castleParts.at(i)->Render(modelShader);
	}

	for (int i = 0; i < boxes.size(); ++i)
	{
		boxes.at(i)->Render(modelShader);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (std::shared_ptr<Object>& enemy : Object::GetObjectsWithTag(Enemys))
	{
		enemy->Render(modelShader);
	}

	modelShader.Unbind();

	waterShader.Bind();
	directionalLight.SetupLight(waterShader);

	glUniformMatrix4fv(waterShader["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(waterShader["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
	glUniform3fv(waterShader["cameraPosition"], 1, glm::value_ptr(camera.GetPosition()));
	sea->Render(waterShader);
	waterShader.Unbind();

	billboardShader.Bind();
	glUniformMatrix4fv(billboardShader["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(billboardShader["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));

	for (int i = 0; i < Object::GetObjectsWithTag(Tag::Lightning).size(); ++i)
	{
		Object::GetObjectsWithTag(Tag::Lightning).at(i)->Render(billboardShader);
	}

	billboardShader.Unbind();

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	if (renderingMode != RENDERING_MODE::TRIANGLES)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPointSize(1.0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderReflection()
{
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
	glViewport(0, 0, screenWidth, screenHeight);
	GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawbuffers);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CLIP_DISTANCE0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	terrainShader.Bind();
	glUniformMatrix4fv(terrainShader["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(terrainShader["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(camera.GetReflectedCameraViewMatrix()));
	glUniform3fv(terrainShader["uniform_camera_position"], 1, glm::value_ptr(camera.GetPosition()));
	glUniform4f(terrainShader["plane"], 0, 1, 0, 0.005);
	directionalLight.SetupLight(terrainShader);
	
	map->Render(terrainShader);

	terrainShader.Unbind();

	skyboxShader.Bind();
	glm::mat4 viewMatrix = camera.GetReflectedCameraViewMatrix();
	viewMatrix[3][0] = 0;
	viewMatrix[3][1] = 0;
	viewMatrix[3][2] = 0;
	viewMatrix = glm::rotate(viewMatrix, float(m_continous_time * 0.02), glm::vec3(0.0, 1.0, 0.0));
	glUniformMatrix4fv(skyboxShader["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(skyboxShader["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(viewMatrix));
	skybox.Render(skyboxShader);
	skyboxShader.Unbind();

	modelShader.Bind();
	glUniformMatrix4fv(modelShader["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(modelShader["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(camera.GetReflectedCameraViewMatrix()));
	glUniform3fv(modelShader["uniform_camera_position"], 1, glm::value_ptr(camera.GetPosition()));
	directionalLight.SetupLight(modelShader);

	for (std::shared_ptr<Object>& tower : Object::GetObjectsWithTag(Tag::CannonTower))
	{
		tower->Render(modelShader);
	}

	for (std::shared_ptr<Object>& tower : Object::GetObjectsWithTag(Tag::MagicTower))
	{
		tower->Render(modelShader);
	}

	for (std::shared_ptr<Object>& cannonball : Object::GetObjectsWithTag(Tag::Cannonball))
	{
		cannonball->Render(modelShader);
	}

	for (int i = 0; i < trees.size(); ++i)
	{
		trees.at(i)->Render(modelShader);
	}

	for (int i = 0; i < castleParts.size(); ++i)
	{
		castleParts.at(i)->Render(modelShader);
	}

	for (int i = 0; i < boxes.size(); ++i)
	{
		boxes.at(i)->Render(modelShader);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (std::shared_ptr<Object>& enemy : Object::GetObjectsWithTag(Enemys))
	{
		enemy->Render(modelShader);
	}

	modelShader.Unbind();

	glDisable(GL_CLIP_DISTANCE0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	if (renderingMode != RENDERING_MODE::TRIANGLES)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPointSize(1.0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderRefraction()
{
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
	glViewport(0, 0, screenWidth, screenHeight);
	GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawbuffers);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CLIP_DISTANCE0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	terrainShader.Bind();
	glUniformMatrix4fv(terrainShader["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(terrainShader["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
	glUniform3fv(terrainShader["uniform_camera_position"], 1, glm::value_ptr(camera.GetPosition()));
	glUniform4f(terrainShader["plane"], 0, -1, 0, -0.005);
	directionalLight.SetupLight(terrainShader);

	map->Render(terrainShader);

	terrainShader.Unbind();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CLIP_DISTANCE0);

	if (renderingMode != RENDERING_MODE::TRIANGLES)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPointSize(1.0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderOverlay()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, screenWidth, screenHeight);
	GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawbuffers);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	overlayShader.Bind();
	glUniformMatrix4fv(overlayShader["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(overlayShader["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
	glUniform3fv(overlayShader["uniform_camera_position"], 1, glm::value_ptr(camera.GetPosition()));

	// if tower set flag and radius else zero

	glUniform1f(overlayShader["minX"], minX);
	glUniform1f(overlayShader["maxX"], maxX);
	glUniform1f(overlayShader["minZ"], minZ);
	glUniform1f(overlayShader["maxZ"], maxZ);
	glUniform3fv(overlayShader["selection_color"], 1, glm::value_ptr(m_selection_color));
	glUniform1f(overlayShader["radius"], 0);
	glUniform3fv(overlayShader["position"], 1, glm::value_ptr(glm::vec3(0.0, 0, 0.0)));

	switch (CurrentState)
	{
	case State::PLACE_TOWER:
	case State::PLACE_MAGIC_TOWER:
		glUniform1f(overlayShader["radius"], unconstructedTower->GetAttackRange());
		glUniform3fv(overlayShader["position"], 1, glm::value_ptr(unconstructedTower->GetPosition() + glm::vec3(0.5, 0, 0.5)));
		break;
	default:
		break;
	}

	map->Render(overlayShader);

	glBindVertexArray(0);
	overlayShader.Unbind();

	ghostShader.Bind();
	glUniformMatrix4fv(ghostShader["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(ghostShader["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
	glUniform3fv(ghostShader["color"], 1, glm::value_ptr(m_selection_color));
	glUniform1f(ghostShader["opacity"], 0.6);
	switch (CurrentState)
	{
	case State::PLACE_TOWER:
	case State::PLACE_MAGIC_TOWER:
		unconstructedTower->Render(ghostShader);
		break;
	default:
		break;
	}
	ghostShader.Unbind();

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderToOutFB()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, screenWidth, screenHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	m_postprocess_program.Bind();

	glBindVertexArray(m_vao_fbo);
	glUniform1i(m_postprocess_program["uniform_texture"], 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);
	glUniform1i(m_postprocess_program["uniform_depth_texture"], 0);
	glUniform1f(m_postprocess_program["uniform_time"], m_continous_time);
	glUniform1i(m_postprocess_program["gameOver"], (CurrentState == State::GAME_OVER) ? 1 : 0);
	glUniform1f(m_postprocess_program["dim"], dim);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	m_postprocess_program.Unbind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	guiShader.Bind();
	statusBar.Render(guiShader);
	guiShader.Unbind();

	textShader.Bind();
	scoreText.Render(textShader);
	livesText.Render(textShader);
	waveText.Render(textShader);
	cashText.Render(textShader);

	if (CurrentState == State::GAME_OVER)
	{
		gameOver.Render(textShader);
	}

	textShader.Unbind();
	glDisable(GL_BLEND);
}

void Renderer::CalculateMouseCoordToWCS()
{
	GLfloat winX, winY, winZ;

	winX = (float)m_mouse_scs.x;
	winY = (float)screenHeight - (float)m_mouse_scs.y;
	glReadPixels(m_mouse_scs.x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	m_mouse_wcs = pos = glm::unProject(glm::vec3(winX, winY, winZ), camera.GetViewMatrix(), projectionMatrix, glm::vec4(0, 0, screenWidth, screenHeight));

	glm::modf(pos.x, realX);
	glm::modf(pos.z, realZ);

	if (pos.x < 0)
	{
		m_mouse_wcs.x = minX = realX - 1;
		maxX = realX; 
	}
	else
	{
		m_mouse_wcs.x = minX = realX;
		maxX = realX + 1;
	}

	if (pos.z < 0)
	{
		m_mouse_wcs.z = minZ = realZ - 1;
		maxZ = realZ;
	}
	else
	{
		m_mouse_wcs.z = minZ = realZ;
		maxZ = realZ + 1;
	}

	m_mouse_wcs.y = 0.0f;
}

void Renderer::StartWave()
{
	if (CurrentState == State::GAME_OVER) return;

	if (spawner->IsActive()) return;

	++wave;
	waveText.SetText("Wave: " + std::to_string(wave));
	waveText.GenerateTextMesh();
	spawner->UpgradeWave(wave);
	spawner->StartWave();
}

void Renderer::ToggleTowerMode()
{
	if (CurrentState == State::GAME_OVER) return;

	if (CurrentState != State::PLACE_TOWER)
	{
		CurrentState = PLACE_TOWER;
		unconstructedTower.reset();
		unconstructedTower = std::make_shared<CannonsTower>(CannonsTower(library.GetObject("CannonTowerTier1"), glm::vec3(), glm::vec3(0.0, 1.0, 0.0), 0, glm::vec3(0.3885), 5, 2, 5, library.GetObject("Cannonball"), glm::vec3(0.0, 0.89335, 0.25641)));
	}
	else
	{
		CurrentState = NONE;
	}
}

void Renderer::ToggleLightningTowerMode()
{
	if (CurrentState == State::GAME_OVER) return;

	if (CurrentState != State::PLACE_MAGIC_TOWER)
	{
		unconstructedTower.reset();
		unconstructedTower = std::make_shared<MagicsTower>(MagicsTower(library.GetObject("MagicTowerTier1"), glm::vec3(), glm::vec3(0.0, 1.0, 0.0), 0, glm::vec3(0.3885), 6, 2, 6, lightningModel));
		CurrentState = PLACE_MAGIC_TOWER;
	}
	else
	{
		CurrentState = NONE;
	}
}

void Renderer::ToogleDemolishMode()
{
	if (CurrentState == State::GAME_OVER) return;

	if (CurrentState != State::REMOVE_TOWER)
	{
		CurrentState = REMOVE_TOWER;
	}
	else
	{
		CurrentState = NONE;
	}
}

void Renderer::AddTower()
{
	if (CurrentState == State::PLACE_TOWER)
	{
		int z = minZ + map->GetHeight() / 2;
		int x = minX + map->GetWidth() / 2;

		if (x < 0 || x > 39 || z < 0 || z > 39) return;

		if (map->GetTileState(std::pair<int, int>(z, x)) == LandscapeState::Empty)
		{
			if (cash >= 50)
			{
				Object::gameObjects.at(Tag::CannonTower).push_back(unconstructedTower);
				unconstructedTower.reset();
				unconstructedTower = std::make_shared<CannonsTower>(CannonsTower(library.GetObject("CannonTowerTier1"), glm::vec3(), glm::vec3(0.0, 1.0, 0.0), 0, glm::vec3(0.3885), 5, 2, 5, library.GetObject("Cannonball"), glm::vec3(0.0, 0.89335, 0.25641)));

				map->SetTileState(std::pair<int, int>(z, x), LandscapeState::Cannon);
				cash -= 50;
				cashText.SetText("Cash: " + std::to_string(cash) + "$");
				cashText.GenerateTextMesh();
			}
		}
	}
	else if (CurrentState == State::PLACE_MAGIC_TOWER)
	{
		int z = minZ + map->GetHeight() / 2;
		int x = minX + map->GetWidth() / 2;

		if (x < 0 || x > 39 || z < 0 || z > 39) return;

		if (map->GetTileState(std::pair<int, int>(z, x)) == LandscapeState::Empty)
		{
			if (cash >= 60)
			{
				Object::gameObjects.at(Tag::MagicTower).push_back(unconstructedTower);
				unconstructedTower.reset();
				unconstructedTower = std::make_shared<MagicsTower>(MagicsTower(library.GetObject("MagicTowerTier1"), glm::vec3(), glm::vec3(0.0, 1.0, 0.0), 0, glm::vec3(0.3885), 6, 2, 6, lightningModel));

				map->SetTileState(std::pair<int, int>(z, x), LandscapeState::Magic);
				cash -= 60;
				cashText.SetText("Cash: " + std::to_string(cash) + "$");
				cashText.GenerateTextMesh();
			}
		}
	}
}

void Renderer::RemoveTower()
{
	if (CurrentState == State::REMOVE_TOWER)
	{
		int z = minZ + map->GetHeight() / 2;
		int x = minX + map->GetWidth() / 2;

		if (x < 0 || x > 39 || z < 0 || z > 39) return;

		if (map->GetTileState(std::pair<int, int>(z, x)) == LandscapeState::Cannon)
		{
			for (int i = 0; i < Object::GetObjectsWithTag(Tag::CannonTower).size(); ++i)
			{
				glm::vec3 position = std::static_pointer_cast<CannonsTower>(Object::GetObjectsWithTag(Tag::CannonTower).at(i))->GetPosition();

				if (int(position.x) == int(minX) && int(position.z) == int(minZ))
				{
					Object::GetObjectsWithTag(Tag::CannonTower).erase(Object::GetObjectsWithTag(Tag::CannonTower).begin() + i);
					map->SetTileState(std::pair<int, int>(z, x), LandscapeState::Empty);
					cash += 25;
					cashText.SetText("Cash: " + std::to_string(cash) + "$");
					cashText.GenerateTextMesh();
					break;
				}
			}
		}
		else if (map->GetTileState(std::pair<int, int>(z, x)) == LandscapeState::Magic)
		{
			for (int i = 0; i < Object::GetObjectsWithTag(Tag::MagicTower).size(); ++i)
			{
				glm::vec3 position = std::static_pointer_cast<CannonsTower>(Object::GetObjectsWithTag(Tag::MagicTower).at(i))->GetPosition();

				if (int(position.x) == int(minX) && int(position.z) == int(minZ))
				{
					Object::GetObjectsWithTag(Tag::MagicTower).erase(Object::GetObjectsWithTag(Tag::MagicTower).begin() + i);
					map->SetTileState(std::pair<int, int>(z, x), LandscapeState::Empty);
					cash += 30;
					cashText.SetText("Cash: " + std::to_string(cash) + "$");
					cashText.GenerateTextMesh();
					break;
				}
			}
		}
	}
}

void Renderer::CameraMoveForward(bool enable)
{
	camera.MoveForward(enable);
}
void Renderer::CameraMoveBackWard(bool enable)
{
	camera.MoveBackward(enable);
}

void Renderer::CameraMoveLeft(bool enable)
{
	camera.MoveLeft(enable);
}
void Renderer::CameraMoveRight(bool enable)
{
	camera.MoveRight(enable);
}

void Renderer::CameraLook(glm::vec2 lookDir)
{
	camera.Look(lookDir);
}
