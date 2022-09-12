#pragma once

#include "GLEW\glew.h"
#include "glm\glm.hpp"
#include <vector>
#include <random>
#include "ShaderProgram.h"
#include "ObjectLibrary.h"
#include "Spawner.h"
#include "Tree.h"
#include "DirectionalLight.h"
#include "Camera.h"
#include "Skybox.h"
#include "Box.h"
#include "Font.h"
#include "Text.h"
#include "StatusBar.h"
#include "Landscape.h"
#include "Sea.h"
#include "CastlePart.h"
#include "TowerBase.h"

class Renderer
{
public:
	enum RENDERING_MODE
	{
		TRIANGLES,
		LINES,
		POINTS
	};

	enum State
	{
		NONE,
		PLACE_TOWER,
		PLACE_MAGIC_TOWER,
		REMOVE_TOWER,
		GAME_OVER
	};

	std::shared_ptr<Spawner> spawner;
	ObjectLibrary library;

protected:

	int												screenWidth, screenHeight;
	glm::mat4										projectionMatrix;
	glm::vec3										m_mouse_wcs;
	glm::vec3										m_mouse_scs;
	glm::vec3										m_selection_color;

	glm::vec3 pos;
	float maxX, minX, maxZ, minZ, realX, realZ;

	GLuint m_fbo;
	GLuint m_fbo_depth_texture;
	GLuint m_fbo_texture;

	GLuint reflectionFBO;
	GLuint reflectionFBODepth;
	GLuint reflectionFBOColor;

	GLuint refractionFBO;
	GLuint refractionFBODepth;
	GLuint refractionFBOColor;

	GLuint m_vao_fbo, m_vbo_fbo_vertices;

	float m_continous_time;
	float dim = 1;


	//Game logic variables
	int lives;
	int score;
	int cash;
	int wave;

	// Rendering Mode
	RENDERING_MODE renderingMode;

	// Camera
	Camera camera;

	// Lights
	DirectionalLight directionalLight;

	// Meshes

	std::shared_ptr<Landscape> map;
	std::shared_ptr<Sea> sea;
	std::shared_ptr<TowerBase> unconstructedTower;

	Skybox skybox;
	Font font;
	Text scoreText;
	Text waveText;
	Text cashText;
	Text livesText;
	Text gameOver;
	StatusBar statusBar;
	

	glm::vec3 currentPosition;

	std::shared_ptr<GeometryNode> lightningModel;
	std::vector<std::shared_ptr<Tree>> trees;
	std::vector<std::shared_ptr<CastlePart>> castleParts;
	std::vector<std::shared_ptr<Box>> boxes;



	// Protected Functions
	bool InitRenderingTechniques();
	bool InitIntermediateShaderBuffers();
	bool InitCommonItems();
	bool InitLightSources();
	bool InitGeometricMeshes();
	void InitScene();
	void CalculateMouseCoordToWCS();

	ShaderProgram terrainNormalMappingShader;
	ShaderProgram terrainShader;
	ShaderProgram m_postprocess_program;
	ShaderProgram shadowMapShader;
	ShaderProgram overlayShader;
	ShaderProgram waterShader;
	ShaderProgram modelShader;
	ShaderProgram skyboxShader;
	ShaderProgram textShader;
	ShaderProgram guiShader;
	ShaderProgram billboardShader;
	ShaderProgram ghostShader;

	GLuint m_trilinear_sampler;

	State CurrentState;
	std::pair<int, int> selectedTile;

public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	Renderer();

	/// <summary>
	/// Destructor.
	/// </summary>
	~Renderer();

	/// <summary>
	/// Initialize the renderer with the given screen dimensions.
	/// </summary>
	/// <param name='screenWidth'> The width of the screen. </param>
	/// <param name='screenHeight'> The height of the screen. </param>
	bool Init(int screenWidth, int screenHeight);

	/// <summary>
	/// Update the scene.
	/// </summary>
	/// <param name='delta'> The time that passed since the last update. </param>
	void Update(float delta, glm::vec2 mouse_coord);

	/// <summary>
	/// Resize the bufers to the new screen size.
	/// </summary>
	/// <param name='screenWidth'> The new width of the screen. </param>
	/// <param name='screenHeight'> The new height of the screen. </param>
	bool ResizeBuffers(int screenWidth, int screenHeight);

	/// <summary>
	/// Reload the shader.
	/// </summary>
	bool ReloadShaders();

	/// <summary>
	/// Render the scene.
	/// </summary>
	void Render();

	/// <summary>
	/// Render the skybox of the scene.
	/// </summary>
	void RenderSkybox();

	/// <summary>
	/// Render the shadow map of the scene.
	/// </summary>
	void RenderShadowMaps();

	/// <summary>
	/// Render the geometry of the scene.
	/// </summary>
	void RenderGeometry();

	/// <summary>
	/// Render the reflections of the scene.
	/// </summary>
	void RenderReflection();

	/// <summary>
	/// Render the reflections of the scene.
	/// </summary>
	void RenderRefraction();

	/// <summary>
	/// Render the the highlights of the scene.
	/// </summary>
	void RenderOverlay();

	/// <summary>
	/// Render the scenes framebuffer to the screen.
	/// </summary>
	void RenderToOutFB();

	/// <summary>
	/// Set the rendering mode of the renderer.
	/// </summary>
	void SetRenderingMode(RENDERING_MODE mode);

	/// <summary>
	/// Enable the cannon tower edit mode.
	/// </summary>
	void ToggleTowerMode();

	void ToggleLightningTowerMode();

	void ToogleDemolishMode();

	/// <summary>
	/// Add a tower at the mouse position.
	/// </summary>
	void AddTower();

	/// <summary>
	/// Remove the tower at the mouse position.
	/// </summary>
	void RemoveTower();

	/// <summary>
	/// Start the next wave.
	/// </summary>
	void StartWave();

	// Camera Function
	void CameraMoveForward(bool enable);
	void CameraMoveBackWard(bool enable);
	void CameraMoveLeft(bool enable);
	void CameraMoveRight(bool enable);
	void CameraLook(glm::vec2 lookDir);
};
